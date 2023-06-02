#include <PCF8574.h>
PCF8574 pin_ext_1(0x38);
// PCF8574 pin_ext_1(0x20);
PCF8574 pin_ext_2(0x3A);

#define man_sw_1 18

bool previous_state;
bool state_change;
bool reboot = 1;  //This variable is to detect if the ESP has restarted or not.

void setup_pin_extender()
{
  //2nd extender
  pin_ext_2.pinMode(P0, INPUT);

  //begin
  pin_ext_1.begin();
  pin_ext_2.begin();
}

// prototypes
int invert_state(int);
bool Switch_pressed(short);

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
    // Logic for manual switch operation
    // Checking for manual switch state change and whether the device is rebooted
    if(Switch_pressed(man_sw_1) && !reboot)
    {
      delay(50); //debouce delay
      power->setVal(invert_state(digitalRead(man_sw_1))); //updating in the home app ui
      update(); //call the update function to make the relay go brrrrrr

      //test pin extender
      // pin_ext_2.digitalWrite(P0, invert_state(digitalRead(man_sw_1)));
      //test end for pin extender
    }
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

    if(power->getNewVal())
    {
      switch (FanSpeed->getNewVal())
      {
      case 0: //when speed is 0
        pin_ext_1.digitalWrite(P4, LOW);
        // pin_ext_1.digitalWrite(P3, power->getNewVal());
        pin_ext_1.digitalWrite(P5, LOW);
        pin_ext_1.digitalWrite(P6, LOW);
        break;
    
      case 25: //when the speed is 25% (Speed position 1)
        pin_ext_1.digitalWrite(P4, LOW);
        pin_ext_1.digitalWrite(P5, LOW);
        pin_ext_1.digitalWrite(P6, HIGH);
        break;

      case 50: //when the speed is 50% (Speed position 2)
        pin_ext_1.digitalWrite(P4, LOW);
        pin_ext_1.digitalWrite(P5, HIGH);
        pin_ext_1.digitalWrite(P6, LOW);
        break;

      case 75: //speed is 75% or speed position 3
        pin_ext_1.digitalWrite(P4, LOW);
        pin_ext_1.digitalWrite(P5, HIGH);
        pin_ext_1.digitalWrite(P6, HIGH);
        break;

      case 100: //speed is 100% full speed position 4
        pin_ext_1.digitalWrite(P4, HIGH);
        pin_ext_1.digitalWrite(P5, LOW);
        pin_ext_1.digitalWrite(P6, LOW);
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
    }

    
    return(true);                               // return true
  
  } // update
};


int invert_state(int in_state)
{
  return(!in_state);
}

bool Switch_pressed(short man_switch)
{
  if(digitalRead(man_switch) == previous_state){
    state_change = 0;
  }
  else{
    state_change = 1;
  }
  previous_state = digitalRead(man_switch);
  return state_change;
}
