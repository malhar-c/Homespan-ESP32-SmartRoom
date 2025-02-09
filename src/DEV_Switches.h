#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRtext.h>
#include <IRutils.h>

//OLED config

#include <PCF8574.h>
PCF8574 pin_ext_1(0x38);
PCF8574 pin_ext_2(0x3A);

#define man_sw_1 18

#define PIR_motion_sensor 13

const uint16_t kRecvPin = 14;
const uint32_t kBaudRate = 115200;
const uint16_t kCaptureBufferSize = 1024;
const uint8_t kTimeout = 50;
const uint16_t kMinUnknownSize = 12;
const uint8_t kTolerancePercentage = kTolerance;  // kTolerance is normally 25%

#define LEGACY_TIMING_INFO false

// Use turn on the save buffer feature for more complete capture coverage.
IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);
decode_results results;  // Somewhere to store the results

//input pins define
#define sw1_input     P7 //extender 1 pin 7
#define sw2_input     0 //extender 2 pin 0
#define sw3_input     1 //extender 2 pin 1
#define sw4_input     2 //extender 2 pin 2
#define sw5_input     3 //extender 2 pin 3
#define rot_1_input   4 //extender 2 pin 4
#define rot_2_input   5 //extender 2 pin 5
#define rot_3_input   6 //extender 2 pin 6
#define rot_4_input   7 //extender 2 pin 7

//on off states of switches
bool Switch_On_Off_State[5];
bool IR_switch_state[5];
bool actual_relay_state[7];
bool Rotary_state[4];

//Input switch filtering vlaues
#define Switch_input_noise_fix_delay 15
#define Switch_debounce_delay 100


bool PIR_ready = 1;  //This should be automatically set to 1 after 60 seconds of device switch on
bool PIR_automatic = 1; //This can be set to 0 if I want to go manual mode
bool pir_state_change = 0;
bool pir_previous_state;

bool previous_state;
bool state_change;
bool sw1_previous_state;
bool sw1_state_change = 0;
bool reboot = 1;  //This variable is to detect if the ESP has restarted or not.
unsigned short IR_triggered = 0;
unsigned short Fan_speed = 0;
bool rotary_triggered = 0;
unsigned short rotary_pos;

void setup_pin_extender()
{
  //input pins
  pin_ext_1.pinMode(sw1_input, INPUT_PULLUP);
  pin_ext_2.pinMode(sw2_input, INPUT_PULLUP);
  pin_ext_2.pinMode(sw3_input, INPUT_PULLUP);
  pin_ext_2.pinMode(sw4_input, INPUT_PULLUP);
  pin_ext_2.pinMode(sw5_input, INPUT_PULLUP);
  pin_ext_2.pinMode(rot_1_input, INPUT);
  pin_ext_2.pinMode(rot_2_input, INPUT);
  pin_ext_2.pinMode(rot_3_input, INPUT);
  pin_ext_2.pinMode(rot_4_input, INPUT); 

  // pin_ext_1.digitalWrite(sw1_input, HIGH); //doesnt work

  //begin
  pin_ext_1.begin();
  pin_ext_2.begin();
}

void setup_IR_recv()
{
  Serial.printf("\n" D_STR_IRRECVDUMP_STARTUP "\n", kRecvPin);

#if DECODE_HASH
  // Ignore messages with less than minimum on or off pulses.
  irrecv.setUnknownThreshold(kMinUnknownSize);
#endif  // DECODE_HASH
  irrecv.setTolerance(kTolerancePercentage);  // Override the default tolerance.
  irrecv.enableIRIn();  // Start the receiver
}

// prototypes
int invert_state(int);
bool Switch_pressed(short);
bool PIR_change_detected(short);
bool sw1_state_changed();
void IR_recv_update(); //function will capture any IR signal and update the states of relays
bool IR_trigger_handler(); //function to handle the IR triggred state as the actual IR recv updation loop cannot be called again and again
bool rotary_manual();

struct DEV_Light_pair_1 : Service::LightBulb {               // ON/OFF LED

  // int relayPin;                                       // pin number defined for this LED
  SpanCharacteristic *power;                        // reference to the On Characteristic
  
  DEV_Light_pair_1() : Service::LightBulb()
  {       // constructor() method

    power=new Characteristic::On(0, true);                 
    // this->relayPin=relayPin;                            
    pin_ext_1.pinMode(P0, OUTPUT);
    update(); //update the relays according to the NVS stored value (in case of accidental reboot)                       
    
  } // end constructor

  boolean update(){                              // update() method

    // digitalWrite(relayPin,power->getNewVal()); 
    pin_ext_1.digitalWrite(P0, power->getNewVal());
    // pin_ext_1.digitalWrite(P0, HIGH);
    // power->setVal(1);
    return(true);                               // return true
  
  } // update

  void loop()
  {
    if(sw1_state_changed() && !reboot)
    {
      Serial.print(" Sw 1 STATE CHANGED --> ");
      Serial.println(Switch_On_Off_State[0]);
      // delay(Switch_debounce_delay); //debouce delay
      power->setVal(invert_state(Switch_On_Off_State[0])); //updating in the home app ui
      update(); //call the update function to make the relay go brrrrrr
    }

    // Serial.print(PIR_ready);
    // Serial.print(PIR_automatic);
    // Serial.println(PIR_change_detected(PIR_motion_sensor));
    //PIR motion sensor for main light pair
    if(PIR_ready && PIR_automatic && PIR_change_detected(PIR_motion_sensor)){
      Serial.println(" PIR state change detected --- ");
      Serial.println(PIR_change_detected(PIR_motion_sensor));
      Serial.println(digitalRead(PIR_motion_sensor));
      delay(10); //delay to counteract noise if any
      power->setVal(digitalRead(PIR_motion_sensor));
      update();
    }
    
    IR_recv_update(); //only call this once to update all the states and update the IR_triggered bool value
    if(IR_trigger_handler() && IR_triggered == 1 && !reboot){
      power->setVal(IR_switch_state[0]);
      Serial.println("inside IR switch 1");
      IR_triggered = 0;
      update();
    }
    else{
      IR_switch_state[0] = power->getNewVal();
    }
    
    actual_relay_state[0] = power->getNewVal();

    reboot = 0;
  }
};

//Light pair 2 relay control
struct DEV_Light_pair_2 : Service::LightBulb {               // ON/OFF LED

  // int relayPin;                                       // pin number defined for this LED
  SpanCharacteristic *power;                        // reference to the On Characteristic
  
  DEV_Light_pair_2() : Service::LightBulb()
  {       // constructor() method

    power=new Characteristic::On(0, true);                 
    // this->relayPin=relayPin;                            
    // pinMode(relayPin,OUTPUT);  
    pin_ext_1.pinMode(P1, OUTPUT);
    update(); //update the relays according to the NVS stored value (in case of accidental reboot)                       
    
  } // end constructor

  boolean update(){                              // update() method

    // digitalWrite(relayPin,invert_state(power->getNewVal()));  
    pin_ext_1.digitalWrite(P1, power->getNewVal());    
   
    return(true);                               // return true
  
  } // update

  void loop()
  {
    if (Switch_pressed(sw2_input) && !reboot)
    {
      Serial.print(" Sw 2 STATE CHANGED --> ");
      Serial.println(Switch_On_Off_State[sw2_input+1]);
      // delay(Switch_debounce_delay); //debouce delay
      power->setVal(invert_state(Switch_On_Off_State[sw2_input+1])); //updating in the home app ui
      update(); //call the update function to make the relay go brrrrrr
    }

    // IR_recv_update();
    if(IR_trigger_handler() && IR_triggered == 2 && !reboot){
      power->setVal(IR_switch_state[sw2_input+1]);
      Serial.println("inside IR switch 2");
      IR_triggered = 0;
      update();
    }
    else{
      IR_switch_state[sw2_input+1] = power->getNewVal();
    }

    actual_relay_state[1] = power->getNewVal();

    reboot = 0;
  }
};

//Tube Light relay control
struct DEV_Tube_Light : Service::LightBulb {               // ON/OFF LED

  // int relayPin;                                       // pin number defined for this LED
  SpanCharacteristic *power;                        // reference to the On Characteristic
  
  DEV_Tube_Light() : Service::LightBulb()
  {       // constructor() method

    power=new Characteristic::On(0, true);                 
    // this->relayPin=relayPin;                            
    // pinMode(relayPin,OUTPUT); 
    pin_ext_1.pinMode(P2, OUTPUT); 
    update(); //update the relays according to the NVS stored value (in case of accidental reboot)                      
    
  } // end constructor

  boolean update(){                              // update() method

    // digitalWrite(relayPin,invert_state(power->getNewVal()));    
    pin_ext_1.digitalWrite(P2, power->getNewVal());  
   
    return(true);                               // return true
  
  } // update

  void loop()
  {
    if (Switch_pressed(sw3_input) && !reboot)
    {
      Serial.print(" Sw 3 STATE CHANGED --> ");
      Serial.println(Switch_On_Off_State[sw3_input+1]);
      // delay(Switch_debounce_delay); //debouce delay
      power->setVal(invert_state(Switch_On_Off_State[sw3_input+1])); //updating in the home app ui
      update(); //call the update function to make the relay go brrrrrr
    }

    // IR_recv_update();
    if(IR_trigger_handler() && IR_triggered == 3 && !reboot){
      power->setVal(IR_switch_state[sw3_input+1]);
      Serial.println("inside IR switch 3");
      IR_triggered = 0;
      update();
    }
    else{
      IR_switch_state[sw3_input+1] = power->getNewVal();
    }

    actual_relay_state[2] = power->getNewVal();

    reboot = 0;
  }

};

//Night lamp relay control
struct DEV_Night_Light : Service::LightBulb {               // ON/OFF LED

  // int relayPin;                                       // pin number defined for this LED
  SpanCharacteristic *power;                        // reference to the On Characteristic
  
  DEV_Night_Light() : Service::LightBulb()
  {       // constructor() method

    power=new Characteristic::On(0, true);
    // Serial.println(power->getNewVal());                
    // this->relayPin=relayPin;                            
    // pinMode(relayPin,OUTPUT);  
    pin_ext_1.pinMode(P3, OUTPUT);
    update(); //update the relays according to the NVS stored value (in case of accidental reboot)
                           
    
  } // end constructor

  boolean update(){                              // update() method

    // digitalWrite(relayPin,invert_state(power->getNewVal()));    
    pin_ext_1.digitalWrite(P3, power->getNewVal());  
   
    return(true);                               // return true
  
  } // update

  void loop()
  {
    if (Switch_pressed(sw4_input) && !reboot)
    {
      Serial.print(" Sw 4 STATE CHANGED --> ");
      Serial.println(Switch_On_Off_State[sw4_input+1]);
      // delay(Switch_debounce_delay); //debouce delay
      power->setVal(invert_state(Switch_On_Off_State[sw4_input+1])); //updating in the home app ui
      update(); //call the update function to make the relay go brrrrrr
    }

    // IR_recv_update();
    if(IR_trigger_handler() && IR_triggered == 4 && !reboot){
      power->setVal(IR_switch_state[sw4_input+1]);
      Serial.println("inside IR switch 4");
      IR_triggered = 0;
      update();
    }
    else{
      IR_switch_state[sw4_input+1] = power->getNewVal();
    }

    actual_relay_state[3] = power->getNewVal();

    reboot = 0;
  }
};

struct DEV_Ceiling_Fan : Service::Fan {               // ON/OFF LED

  // int relayPin_1speed = 25;
  // int relayPin_2speed = 33;
  // int relayPin_FullSpeed = 32;
  SpanCharacteristic *FanSpeed;                        // reference to the speed Characteristic
  SpanCharacteristic *power;
  
  DEV_Ceiling_Fan() : Service::Fan()
  {       // constructor() method

    power = new Characteristic::Active(0, true);                                   // NEW: This allows control of the Rotation Direction of the Fan
      FanSpeed = (new Characteristic::RotationSpeed(75, true))->setRange(0,100,25);
    
    // power=new Characteristic::On();                 
    // this->relayPin=relayPin;  

    // pinMode(relayPin_FullSpeed,OUTPUT);                         
    // pinMode(relayPin_1speed,OUTPUT);
    // pinMode(relayPin_2speed,OUTPUT);
    pin_ext_1.pinMode(P4, OUTPUT);
    pin_ext_1.pinMode(P5, OUTPUT);
    pin_ext_1.pinMode(P6, OUTPUT);
    update(); //update the relays according to the NVS stored value (in case of accidental reboot)

  } // end constructor

  boolean update(){                              // update() method

    // digitalWrite(relayPin,power->getNewVal());
    // P4 ---> relay 7
    // P5 ---> relay 6
    // P6 ---> relay 5

    if(power->getNewVal())
    {
      switch (FanSpeed->getNewVal())
      {
      case 0: //when speed is 0
        pin_ext_1.digitalWrite(P4, LOW);
        // pin_ext_1.digitalWrite(P3, power->getNewVal());
        pin_ext_1.digitalWrite(P5, LOW);
        pin_ext_1.digitalWrite(P6, LOW);
        actual_relay_state[4] = 0;
        actual_relay_state[5] = 0;
        actual_relay_state[6] = 0;
        break;
    
      case 25: //when the speed is 25% (Speed position 1)
        pin_ext_1.digitalWrite(P4, LOW);
        pin_ext_1.digitalWrite(P5, LOW);
        pin_ext_1.digitalWrite(P6, HIGH);
        actual_relay_state[4] = 1;
        actual_relay_state[5] = 0;
        actual_relay_state[6] = 0;
        break;

      case 50: //when the speed is 50% (Speed position 2)
        pin_ext_1.digitalWrite(P4, LOW);
        pin_ext_1.digitalWrite(P5, HIGH);
        pin_ext_1.digitalWrite(P6, LOW);
        actual_relay_state[4] = 0;
        actual_relay_state[5] = 1;
        actual_relay_state[6] = 0;
        break;

      case 75: //speed is 75% or speed position 3
        pin_ext_1.digitalWrite(P4, LOW);
        pin_ext_1.digitalWrite(P5, HIGH);
        pin_ext_1.digitalWrite(P6, HIGH);
        actual_relay_state[4] = 1;
        actual_relay_state[5] = 1;
        actual_relay_state[6] = 0;
        break;

      case 100: //speed is 100% full speed position 4
        pin_ext_1.digitalWrite(P4, HIGH);
        pin_ext_1.digitalWrite(P5, LOW);
        pin_ext_1.digitalWrite(P6, LOW);
        actual_relay_state[4] = 0;
        actual_relay_state[5] = 0;
        actual_relay_state[6] = 1;
        break;
    
      default:
        break;
      }
    }

    else //turn it off
    {
      pin_ext_1.digitalWrite(P4, LOW);
      pin_ext_1.digitalWrite(P5, LOW);
      pin_ext_1.digitalWrite(P6, LOW);
      actual_relay_state[4] = 0;
      actual_relay_state[5] = 0;
      actual_relay_state[6] = 0;
    }

    
    return(true);                               // return true
  
  } // update

  void loop(){
    //write something to control the behaviour of Fan speed and the relays
    if (Switch_pressed(sw5_input) && !reboot)
    {
      Serial.print(" Sw 5 STATE CHANGED --> ");
      Serial.println(Switch_On_Off_State[sw5_input+1]);
      // delay(Switch_debounce_delay); //debouce delay
      power->setVal(invert_state(Switch_On_Off_State[sw5_input+1])); //updating in the home app ui
      update(); //call the update function to make the relay go brrrrrr
    }

    // if rotary switch is triggered
    if(rotary_manual() && !reboot)
    {
      Serial.print("Rotary manual trigered!!! Pos value --- ");
      Serial.println(rotary_pos);
      switch (rotary_pos)
      {
      case 1:
        power->setVal(1);
        FanSpeed->setVal(25);
        break;
      case 2:
        power->setVal(1);
        FanSpeed->setVal(50);
        break;
      case 3:
        power->setVal(1);
        FanSpeed->setVal(75);
        break;
      case 4:
        power->setVal(1);
        FanSpeed->setVal(100);
        break;
      default:
        power->setVal(0);
        FanSpeed->setVal(0);
        break;
      }
      // rotary_triggered = 0;
      Serial.println("UPDATE the fan relays !!! ");
      update();
    }

    //IR remote handling
    if(IR_trigger_handler() && IR_triggered >= 10 && !reboot){
      if(Fan_speed != 0){
        power->setVal(1);
        FanSpeed->setVal(Fan_speed);
      }else{
        power->setVal(0);
      }
      
      Serial.print("inside IR Fan Control!!! Speed Set --- ");
      Serial.println(Fan_speed);
      IR_triggered = 0;
      update();
    }
    // else{
    //   IR_switch_state[sw2_input+1] = power->getNewVal();
    // }
    reboot = 0;
  }
};


int invert_state(int in_state)
{
  return(!in_state);
}

bool Switch_pressed(short man_switch)
{
  delay(20);
  if(pin_ext_2.digitalRead(man_switch) == Switch_On_Off_State[man_switch+1]){
    state_change = 0;
  } 
  else{
    state_change = 1;
  }
  delay(Switch_input_noise_fix_delay); //noisy input fix
  Switch_On_Off_State[man_switch+1] = pin_ext_2.digitalRead(man_switch);
  // Switch_On_Off_State[0] = sw1_previous_state;
  return state_change;
}

bool IR_trigger_handler()
{
  if(IR_triggered != 0){
    return 1;
  }
  else{
    return 0;
  }
}

bool sw1_state_changed()
{
  // Serial.println(pin_ext_1.digitalRead(sw1_input));
  delay(20);
  // Serial.print(pin_ext_1.digitalRead(sw1_input));
  // Serial.print(" ");
  // Serial.print(pin_ext_2.digitalRead(sw2_input));
  // Serial.print(" ");
  // Serial.print(pin_ext_2.digitalRead(sw3_input));
  // Serial.print(" ");
  // Serial.print(pin_ext_2.digitalRead(sw4_input));
  // Serial.print(" ");
  // Serial.print(pin_ext_2.digitalRead(sw5_input));
  // Serial.println(" ");

  if(pin_ext_1.digitalRead(sw1_input) == Switch_On_Off_State[0]){
    sw1_state_change = 0;
  } 
  else{
    sw1_state_change = 1;
  }
  delay(Switch_input_noise_fix_delay); //noisy input fix
  Switch_On_Off_State[0] = pin_ext_1.digitalRead(sw1_input);
  // Switch_On_Off_State[0] = sw1_previous_state;
  return sw1_state_change;
}

bool PIR_change_detected(short PIR_pin)
{
  if(digitalRead(PIR_pin) == pir_previous_state){
    pir_state_change = 0;
  }
  else{
    pir_state_change = 1;
  }
  pir_previous_state = digitalRead(PIR_pin);
  // Serial.println(pir_previous_state);
  return pir_state_change;
}

bool Rotary_touched(short pin)
{
  delay(30);
  if(pin_ext_2.digitalRead(pin) == Rotary_state[pin-4]){
    state_change = 0;
  } 
  else{
    state_change = 1;
  }
  delay(Switch_input_noise_fix_delay); //noisy input fix
  Rotary_state[pin-4] = pin_ext_2.digitalRead(pin);
  // Switch_On_Off_State[0] = sw1_previous_state;
  return state_change;
}

bool rotary_manual()
{
  if(Rotary_touched(rot_1_input)){
    //state update
    rotary_pos = 1;
    return 1;
  }
  else if(Rotary_touched(rot_2_input)){
    //state update
    rotary_pos = 2;
    return 1;
  }
  else if(Rotary_touched(rot_3_input)){
    // state update
    rotary_pos = 3;
    return 1;
  }
  else if(Rotary_touched(rot_4_input)){
    //state update
    rotary_pos = 4;
    return 1;
  }
  else{
    return 0;
  }
}

void IR_recv_update()
{
  if (irrecv.decode(&results)) 
  {
    String HEXvalue = resultToHexidecimal(&results);
    unsigned short bits_count = results.bits;
    Serial.println(HEXvalue);
    Serial.println(bits_count);
    if(bits_count == 32)
    {
      //do stuff update stuff
      // run a switch case
      if(strcmp(HEXvalue.c_str(), "0x1FEE01F") == 0)
      {
        //pressed value = 0
        Serial.println(" 0 pressed ");
        IR_triggered = 10;
        Fan_speed = 0;
        // IR_switch_state[0] = !IR_switch_state[0];
      }
      else if(strcmp(HEXvalue.c_str(), "0x1FE50AF") == 0)
      {
        //pressed value = 1
        Serial.println(" 1 pressed ");
        IR_triggered = 11;
        Fan_speed = 25;
      }
      else if(strcmp(HEXvalue.c_str(), "0x1FED827") == 0)
      {
        //pressed value = 2
        Serial.println(" 2 pressed ");
        IR_triggered = 12;
        Fan_speed = 50;
      }
      else if(strcmp(HEXvalue.c_str(), "0x1FEF807") == 0)
      {
        //pressed value = 3
        Serial.println(" 3 pressed ");
        IR_triggered = 13;
        Fan_speed = 75;
      }
      else if(strcmp(HEXvalue.c_str(), "0x1FE30CF") == 0)
      {
        //pressed value = 4
        Serial.println(" 4 pressed ");
        IR_triggered = 14;
        Fan_speed = 100;
      }
      else if(strcmp(HEXvalue.c_str(), "0x1FE48B7") == 0)
      {
        //pressed value = power
        Serial.println(" power pressed ");
        //turn on light 1
        IR_triggered = 1;
        IR_switch_state[0] = !IR_switch_state[0];
      }
      else if(strcmp(HEXvalue.c_str(), "0x1FE58A7") == 0)
      {
        //pressed value = mode
        Serial.println(" mode pressed ");
        // turn on light 2
        IR_triggered = 2;
        IR_switch_state[1] = !IR_switch_state[1];
      }
      else if(strcmp(HEXvalue.c_str(), "0x1FE7887") == 0)
      {
        //pressed value = mute
        Serial.println(" mute pressed ");
        // turn on light 3
        IR_triggered = 3;
        IR_switch_state[2] = !IR_switch_state[2];
      }
      else if(strcmp(HEXvalue.c_str(), "0x1FE807F") == 0)
      {
        //pressed value = play/pause
        Serial.println(" play/pause pressed ");
        // turn on light 4
        IR_triggered = 4;
        IR_switch_state[3] = !IR_switch_state[3];
      }
      else if(strcmp(HEXvalue.c_str(), "0x1FE00FF") == 0)
      {
        //pressed value = 7
        Serial.println(" 7 pressed ");
      }
      else if(strcmp(HEXvalue.c_str(), "0x1FEF00F") == 0)
      {
        //pressed value = 8
        Serial.println(" 8 pressed ");
      }
      else if(strcmp(HEXvalue.c_str(), "0x1FE9867") == 0)
      {
        //pressed value = 9
        Serial.println(" 9 pressed ");
      }
      else if(strcmp(HEXvalue.c_str(), "0x1FE20DF") == 0)
      {
        //pressed value = EQ
        Serial.println(" EQ pressed ");
        Serial.print(" Motion detection -- ");
        PIR_automatic = !PIR_automatic;
        Serial.println(PIR_automatic);
      }
      else{
        IR_triggered = 0;
      }
    }
    // IR_triggered = 0;
    yield();
  }
}