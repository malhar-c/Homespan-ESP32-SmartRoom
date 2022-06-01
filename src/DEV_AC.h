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

//not sure if this is optional
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

struct DEV_Smart_AC : Service::HeaterCooler 
{
  SpanCharacteristic *active;
  SpanCharacteristic *curr_temp;
  SpanCharacteristic *curr_state;
  SpanCharacteristic *mode;
  SpanCharacteristic *set_cooling_temp;
  SpanCharacteristic *fan_speed;
  SpanCharacteristic *swing_on_off;

  int fan_speed_for_display = 0; //auto

  DEV_Smart_AC() : Service::HeaterCooler()
  {

    active=new Characteristic::Active(0, true);
      curr_temp=new Characteristic::CurrentTemperature(dht.readTemperature());
      curr_state=new Characteristic::CurrentHeaterCoolerState(3); //0-inactive 1-idle, 2-heating, 3-cooling
      mode=new Characteristic::TargetHeaterCoolerState(0, true); //0-Auto 1-Heat 2-Cool
      set_cooling_temp=(new Characteristic::CoolingThresholdTemperature(25, true))->setRange(16,31,1);
      fan_speed=new Characteristic::RotationSpeed(50);
      swing_on_off=new Characteristic::SwingMode(0, true); //0 - disable 1- enable
      new Characteristic::TemperatureDisplayUnits(); //0-Celsius 1-Fahrenheit

    pinMode(kIrLed, OUTPUT);
    update(); //to run at first boot
  }

  boolean update() //only triggered when something is changed/updated in Home App
  {

    // digitalWrite(relayPin,power->getNewVal());     
    // Serial.println(mode->getNewVal());

    if((active->getNewVal()) == 1)
    {
      //send data through IR and update the OLED display only when the AC is active
      ac.on();
      // ac.send();

      if(mode->getNewVal() == 0) //Mode - Auto
      {
        ac.setMode(kHitachiAc1Auto);
        ac.setFan(kHitachiAc1FanAuto);
        fan_speed_for_display = 0;
        set_cooling_temp->setVal(25); //in Auto Mode Hitachi temp setting is not allowed and is locked to 25C
        swing_on_off->setVal(1); //swing is on by default in auto mode
      }
  
      else if(mode->getNewVal() == 1)  //Mode - Heat (will trigger Fan mode)
      {
        ac.setMode(kHitachiAc1Fan);
        //Setting Fan Speed according to the controller
        if(fan_speed->getNewVal() > 80) //Fan High
        {
          ac.setFan(kHitachiAc1FanHigh);
          fan_speed_for_display = 3;
        }
        else if(fan_speed->getNewVal() < 80 && fan_speed->getNewVal() > 40)
        {
          ac.setFan(kHitachiAc1FanMed);
          fan_speed_for_display = 2;
        }
        else
        {
          ac.setFan(kHitachiAc1FanLow);
          fan_speed_for_display = 1;
        }

        //setting V Swing
        if(swing_on_off->getNewVal() ==1)
        {
          //swing on
          ac.setSwingV(true);
        }
        else{
          ac.setSwingV(false);
        }
      }

      else if(mode->getNewVal() == 2)  //Mode - Cool (will trigger Cool mode)
      {
        ac.setMode(kHitachiAc1Cool);
        ac.setTemp(set_cooling_temp->getNewVal()); //set the temp as per controller

        //Setting Fan Speed according to the controller
        if(fan_speed->getNewVal() > 80) //Fan High
        {
          ac.setFan(kHitachiAc1FanHigh);
          fan_speed_for_display = 3;
        }
        else if(fan_speed->getNewVal() < 80 && fan_speed->getNewVal() > 40)
        {
          ac.setFan(kHitachiAc1FanMed);
          fan_speed_for_display = 2;
        }
        else
        {
          ac.setFan(kHitachiAc1FanLow);
          fan_speed_for_display = 1;
        }

        //setting V Swing
        if(swing_on_off->getNewVal() ==1)
        {
          //swing on
          ac.setSwingV(true);
        }
        else{
          ac.setSwingV(false);
        }
      }
      
    }
    else{
      ac.off();
    }

    ac.send(); //fire the IR blaster/commit AC control operation
    // delay(500);

    //for debugging
    // Serial.println("Hitachi A/C remote is in the following state:");
    // Serial.println(set_cooling_temp->getNewVal());
    // Serial.println(active->getNewVal());
    display_stuff(set_cooling_temp->getNewVal(), active->getNewVal(), mode->getNewVal(), fan_speed_for_display);
    Serial.printf("  %s\n", ac.toString().c_str());

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
