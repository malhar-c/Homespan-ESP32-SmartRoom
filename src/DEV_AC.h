// DHT settings starts
#include "DHT.h"
#define DHTPIN 23     // DHT sensor pin connected to GPIO23
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
// ****** DHT settings end

//IR Transmission Config starts
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Hitachi.h>

const uint16_t kIrLed = 4;  //IR LED connected to GPIO4
IRHitachiAc1 ac(kIrLed);
//IR Transmission config ends

//OLED config
#include "OLED_config.h"

//this is required to be called in main.cpp setup()
void setup_init()
{
  dht.begin();
  ac.begin();
  //initial state of the AC
  ac.off();
  ac.setFan(kHitachiAc1FanAuto);
  ac.setMode(kHitachiAc1Auto);
  ac.setTemp(25);
  ac.setSwingV(true);
  Serial.begin(115200);
  OLED_init();
  // test_OLED();
}

short AC_Fan_speed = 0; //30 - low, 60- medium, 90-high
bool AC_Fan_state; //autop or manual
bool AC_Swing_mode = 0; //0-off, 1-on

short fan_speed_for_display = 0; //auto

short AC_mode;
short AC_prev_mode = 0;
bool AC_on_off = 0;
short AC_set_temp;

bool AC_fan_updation = false;

struct DEV_Smart_AC : Service::HeaterCooler 
{
  SpanCharacteristic *active;
  SpanCharacteristic *curr_temp;
  SpanCharacteristic *curr_state;
  SpanCharacteristic *mode;
  SpanCharacteristic *set_cooling_temp;
  // SpanCharacteristic *fan_speed;
  // SpanCharacteristic *swing_on_off;

  DEV_Smart_AC() : Service::HeaterCooler()
  {

    active=new Characteristic::Active(0, true);
      curr_temp=new Characteristic::CurrentTemperature(dht.readTemperature());
      curr_state=new Characteristic::CurrentHeaterCoolerState(3); //0-inactive 1-idle, 2-heating, 3-cooling
      mode=new Characteristic::TargetHeaterCoolerState(0, true); //0-Auto 1-Heat 2-Cool
      set_cooling_temp=(new Characteristic::CoolingThresholdTemperature(25, true))->setRange(16,31,1);
      // fan_speed=new Characteristic::RotationSpeed(50);
      // swing_on_off=new Characteristic::SwingMode(0, true); //0 - disable 1- enable

    pinMode(kIrLed, OUTPUT);
    update(); //to run at first boot
  }

  boolean update() //only triggered when something is changed/updated in Home App
  {

    // digitalWrite(relayPin,power->getNewVal());     
    // Serial.println(mode->getNewVal());

    AC_fan_updation = true; //to also run the dedicated fan update function
    AC_mode = mode->getNewVal();

    //detect auto mode switch
    if(set_cooling_temp->getNewVal() != 25 && mode->getNewVal() == 0 && AC_prev_mode != 0)
    {
      //for debugg
      // Serial.println("DETECT AUTO MODE CHANGE and set the temp as 25 Forcefully");
      // Serial.print("Current Switched Mode: ");
      // Serial.println(mode->getNewVal());
      // Serial.print("Previous Mode: ");
      // Serial.println(AC_prev_mode);
      set_cooling_temp->setVal(25);
    }
    
    if(set_cooling_temp->getNewVal() != 25 && AC_prev_mode == 0)
    {
      //for debugg
      mode->setVal(2); //force COOL mode
      // Serial.println("FORCE COOL MODE CHANGE when Temp changed from 25 and prev mode was auto");
      // Serial.print("Current Switched Mode: ");
      // Serial.println(mode->getNewVal());
      // Serial.print("Previous Mode: ");
      // Serial.println(AC_prev_mode);
    }


    // if(mode->getNewVal() == 0)
    // {
    //   set_cooling_temp->setVal(25);
    // }
    
    // if((set_cooling_temp->getNewVal()) != 25)
    // {
    //   mode->setVal(2); //force COOL mode
    // }

    if((active->getNewVal()) == 1)
    {
      //send data through IR and update the OLED display only when the AC is active
      ac.on();
      AC_on_off = 1;
      // ac.send();
      switch (mode->getNewVal())
      {
      case 0: //mode- auto
        Serial.println("### Case 0: AUTO MODE BABY ###");
        Serial.print("AC_Fan_state : ");
        Serial.println(AC_Fan_state);
        AC_Fan_state = 0;
        AC_Swing_mode = 1; //auto mode has V swing turned on, so settingt his variable to access it in the fan fn
        ac.setMode(kHitachiAc1Auto);
        fan_speed_for_display = 0;
        // set_cooling_temp->setVal(25);
        break;

      case 1: //heating in Home app, but will trigger AC fan mode
        Serial.println("### Case 1: FAN MODE - HEATING MODE IN HOME ###");
        Serial.print("AC_Fan_state : ");
        Serial.println(AC_Fan_state);
        ac.setMode(kHitachiAc1Fan);
        if(AC_Fan_state == 0) //if auto
        {
          ac.setFan(kHitachiAc1FanAuto);
        }
        if (AC_Fan_state == 1) //manual
        {
          switch(AC_Fan_speed)  //change the fan speed according to set speed from dedicated fan service
          {
          case 90:
            ac.setFan(kHitachiAc1FanHigh);
            fan_speed_for_display = 3;
            break;
          case 60:
            ac.setFan(kHitachiAc1FanMed);
            fan_speed_for_display = 2;
            break;
          case 30:
            ac.setFan(kHitachiAc1FanLow);
            fan_speed_for_display = 1;
            break;
          }
        }
        break;

      case 2: //cool mode
        Serial.println("### Case 2: COOL MODE ###");
        Serial.print("AC_Fan_state : ");
        Serial.println(AC_Fan_state);
        ac.setMode(kHitachiAc1Cool);
        ac.setTemp(set_cooling_temp->getNewVal());
        if(AC_Fan_state == 0) //if auto
        {
          ac.setFan(kHitachiAc1FanAuto);
        }
        else //manual
        {
          switch(AC_Fan_speed)  //change the fan speed according to set speed from dedicated fan service
          {
          case 90:
            ac.setFan(kHitachiAc1FanHigh);
            fan_speed_for_display = 3;
            break;
          case 60:
            ac.setFan(kHitachiAc1FanMed);
            fan_speed_for_display = 2;
            break;
          case 30:
            ac.setFan(kHitachiAc1FanLow);
            fan_speed_for_display = 1;
            break; 
          default:
            ac.setFan(kHitachiAc1FanAuto);
            fan_speed_for_display = 0;
            break;
          }
        }
        break;
      
      default:
        break;
      }
    }
    else{
      ac.off();
      AC_on_off = 0;
      ac.send();
      display_stuff(AC_set_temp, AC_on_off, AC_mode, fan_speed_for_display);
    }

    // ac.send(); //fire the IR blaster/commit AC control operation
    // delay(500);

    //for debugging
    // Serial.println("Hitachi A/C remote is in the following state:");
    // Serial.println(set_cooling_temp->getNewVal());
    // Serial.println(active->getNewVal());

    AC_set_temp = set_cooling_temp->getNewVal();
    AC_mode = mode->getNewVal();

    // display_stuff(AC_set_temp, AC_on_off, AC_mode, fan_speed_for_display);
    // Serial.printf("  %s\n", ac.toString().c_str());

    AC_prev_mode = mode->getNewVal();
    // AC_prev_mode = AC_mode;

    return(true);                               // return true
  
  } // update

  void loop()  //always runs as long as the homeSpan.poll() runs
  {
    // display_time();
    if(curr_temp->timeVal() > 2000)
    {
      if(active->getNewVal() == 0) //when the AC is off update the time every 2 sec
      {
        display_time_large();
      }
      // display_stuff(set_cooling_temp->getNewVal(), active->getNewVal()); // for time display (update every 1s)
      curr_temp->setVal(dht.readTemperature());
      // Serial.print("temp: ");
      // Serial.println(dht.readTemperature());
    }
  }
};

//Humidity sensor
struct DEV_Humidity : Service::HumiditySensor {

  // SpanCharacteristic *power;                        // reference to the On Characteristic
  // float humidity = dht.readHumidity();

  SpanCharacteristic *curr_hum;
  
  DEV_Humidity() : Service::HumiditySensor()
  {       // constructor() method

    curr_hum=new Characteristic::CurrentRelativeHumidity(dht.readHumidity());                      
    
  } // end constructor

  boolean update(){                              // update() method

    // digitalWrite(relayPin,power->getNewVal());      
   
    return(true);                               // return true
  
  } // update

  void loop()
  {
    // Serial.println(two_second_passed());
    // Serial.print(current_time_s);
    // Serial.print(" ");
    // Serial.println(previous_time_s);
    
    if(curr_hum->timeVal() > 2000)  //if the humidity is last updated more than 2s Update it
    {
      curr_hum->setVal(dht.readHumidity());
      // Serial.print("hum: ");
      // Serial.println(curr_hum->getNewVal());
      // Serial.println(dht.readHumidity());
    }

    //for debuggg
    // Serial.print("Last time Humidity was updated : ");
    // Serial.println(curr_hum->timeVal());
  }
};

//For AC Fan control
struct DEV_AC_Fan : Service::Fan
{
  SpanCharacteristic *fan_Active;
  SpanCharacteristic *fan_speed;
  SpanCharacteristic *fan_mode;
  SpanCharacteristic *swing_mode;

  DEV_AC_Fan() : Service::Fan()
  {
    fan_Active = new Characteristic::Active();
    // new Characteristic::CurrentFanState(2); // 0-inactive //1-idle //2-blowing air
    fan_mode = new Characteristic::TargetFanState(); //0-manual //1-auto
    swing_mode = new Characteristic::SwingMode(0, true); //0-disabled //1-enabled
    fan_speed = (new Characteristic::RotationSpeed(60, true))->setRange(0,90,30);

    // update();
  }

  boolean update()
  {
    if(AC_mode != 0)  //not Auto
    {
      AC_Fan_speed = fan_speed->getNewVal();
      if(AC_Fan_speed != 0)
      {
        fan_mode->setVal(0); //set it to manual
        Serial.print(" MANUAL forced : ");
        Serial.println(fan_mode->getNewVal());
      }
      if(fan_mode->getNewVal() == 1)
      {
        fan_speed->setVal(0);
        Serial.print(" MANUAL BUTTON pressed : ");
        Serial.println(fan_speed->getNewVal());
      }
      AC_Fan_state  = !(fan_mode->getNewVal()); //reverse logic
      AC_Swing_mode =  swing_mode->getNewVal();
      ac.setSwingV(AC_Swing_mode);
      switch(fan_speed->getNewVal())  //change the fan speed according to set speed from dedicated fan service
      {
      case 90:
        ac.setFan(kHitachiAc1FanHigh);
        fan_speed_for_display = 3;
        break;
      case 60:
        ac.setFan(kHitachiAc1FanMed);
        fan_speed_for_display = 2;
        break;
      case 30:
        ac.setFan(kHitachiAc1FanLow);
        fan_speed_for_display = 1;
        break;
      default:
        ac.setFan(kHitachiAc1FanAuto);
        fan_speed_for_display = 0;
        fan_mode->setVal(1); //set mode auto in ui
        break;
      }
    }
    //when the AC mode is AUTO
    else
    {
      fan_mode->setVal(AC_Fan_state);
      swing_mode->setVal(AC_Swing_mode);
      fan_speed->setVal(0);
      ac.setFan(kHitachiAc1FanAuto);
    }

    if(fan_speed->getNewVal() == 0)
    {
      fan_mode->setVal(1); //set the button to auto
    }

    Serial.print("AC_on_off = ");
    Serial.println(AC_on_off);

    if(AC_on_off == 1)
    {
      // fan_Active->setVal(1); //if the ac is on, turn on the ac fan too (only for Home app ui fn)
      Serial.printf("  %s\n", ac.toString().c_str());
      display_stuff(AC_set_temp, AC_on_off, AC_mode, fan_speed_for_display);
      ac.send(); //only fire IR for fan settings when the AC is on
    }   

    return (true);
  }

  void loop()
  {
    // Serial.println("TEST inside AC Fan function loop");
    // DEV_Smart_AC ac;
    // Serial.println(ac.active->getNewVal());
    // Serial.println(AC_fan_speed->getNewVal());

    if(AC_fan_updation == true)
    {
      update();
      AC_fan_updation = false;
    }

    if(AC_on_off == 1)
    {
      fan_Active->setVal(1); //if the ac is on, turn on the ac fan too (only for Home app ui fn)
    } 
    else{
      fan_Active->setVal(0); // when the AC is off turn the ac fann too
    } 
  }
};

