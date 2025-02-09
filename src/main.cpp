/*********************************************************************************
 *  MIT License
 *  
 *  Copyright (c) 2020-2022 Gregg E. Berman
 *  
 *  https://github.com/HomeSpan/HomeSpan
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *  
 ********************************************************************************/



#include "HomeSpan.h" 
// #include "DEV_Switches.h"
// #include "DEV_Fan_Regulator.h"
#include "DEV_AC.h"

// #define Relay_1 15
// #define Relay_2 26
// #define Relay_3 5
// #define Relay_4 27
// #define Relay_5 26
// #define Relay_6 25
// #define Relay_7 33
// #define Relay_8 32

#define Status_LED      2
#define Control_Button  25

void setup() {

  //defining pinMode pulls the pins to low for a brief period of time, and that's enough for the relays to tick,
  //thus disabling pinMode, and directly assigning the pins to HIGH (perfect solution? IDK... but it works :)

  // pin_ext_1.pinMode(P0, OUTPUT);
  // pin_ext_1.pinMode(P1, OUTPUT);
  // pin_ext_1.pinMode(P2, OUTPUT);
  // pin_ext_1.pinMode(P3, OUTPUT);
  // pin_ext_1.pinMode(P4, OUTPUT);
  // pin_ext_1.pinMode(P5, OUTPUT);
  // pin_ext_1.pinMode(P6, OUTPUT);
  // pin_ext_1.pinMode(P7, OUTPUT);
  // pin_ext_1.begin();

  // pin_ext_1.digitalWrite(P0, HIGH);

  // digitalWrite(Relay_1, HIGH);
  // digitalWrite(Relay_2, HIGH);
  // digitalWrite(Relay_3, HIGH);
  // digitalWrite(Relay_4, HIGH);
  // digitalWrite(Relay_5, HIGH);
  /*
  * RC Fan regulator relay config
  * Speed position 1 -> rc path 1
  * Speed position 2 -> rc path 2
  * Speed position 3 -> rc path 1 and 2 in parralel
  * Speed position 4 -> rc ckt bypass (Full Speed)
  */
  // digitalWrite(Relay_6, HIGH); //fan regulator speed1 (rc path 1)
  // digitalWrite(Relay_7, HIGH); //fan regulator speed2 (rc path 2)
  // digitalWrite(Relay_8, HIGH); //fan regulator speed3 (rc path bypass)

  pinMode(man_sw_1, INPUT_PULLUP);

  Serial.begin(115200);

  // OLED_init(); //Initialize the OLED display... defined in OLED_config.h
  setup_init();
  // time init (get time from ntp server) // written in time_management.h
  time_init();
  //pin_extender pcf8574 initiation
  setup_pin_extender();
  setup_IR_recv();

  homeSpan.setStatusPin(Status_LED);
  homeSpan.setControlPin(Control_Button);

  homeSpan.enableOTA(); //void enableOTA(boolean auth=true, boolean safeLoad=true)
  homeSpan.begin(Category::Bridges,"Smart Room");
  
  new SpanAccessory();                            // This first Accessory is the new "Bridge" Accessory.  It contains no functional Services, just the Accessory Information Service
    new Service::AccessoryInformation();
      new Characteristic::Identify();            
 
  new SpanAccessory();                            // This second Accessory is the same as the first Accessory in Example 7, with the exception that Characteristic::Name() now does something
    new Service::AccessoryInformation();
      new Characteristic::Identify();            
      new Characteristic::Name("Ceiling P1");     // Note that unlike in Example 7, this use of Name() is now utilized by the Home App since it is not the first Accessory (the Bridge above is the first)
    new DEV_Light_pair_1;

  new SpanAccessory();                            
    new Service::AccessoryInformation();    
      new Characteristic::Identify();               
      new Characteristic::Name("Ceiling P2");  
    new DEV_Light_pair_2;
  
  new SpanAccessory();                            
    new Service::AccessoryInformation();    
      new Characteristic::Identify();               
      new Characteristic::Name("Tube Light");  
    new DEV_Tube_Light;

  new SpanAccessory();                            
    new Service::AccessoryInformation();    
      new Characteristic::Identify();               
      new Characteristic::Name("Night Lamp");  
    new DEV_Night_Light;

  new SpanAccessory();                            
    new Service::AccessoryInformation();    
      new Characteristic::Identify();               
      new Characteristic::Name("Ceiling Fan");  
    new DEV_Ceiling_Fan;

  // new SpanAccessory();                            
  //   new Service::AccessoryInformation();    
  //     new Characteristic::Identify();               
  //     new Characteristic::Name("Air Conditioner");  
  //   new DEV_Smart_AC;

  // new SpanAccessory();                            
  //   new Service::AccessoryInformation();    
  //     new Characteristic::Identify();               
  //     new Characteristic::Name("Humidity Sensor");  
  //   new DEV_Humidity;

  // new SpanAccessory();
  //   new Service::AccessoryInformation();
  //     new Characteristic::Identify();
  //     new Characteristic::Name("AC_Fan");
  //   new DEV_AC_Fan;

  //Remaking of AC and climate control (will get advanced options and control authority)
  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify();
    // new Service::HeaterCooler();
      // new Characteristic::Active();
      // new Characteristic::CurrentTemperature();
      // new Characteristic::CurrentHeaterCoolerState();
      // new Characteristic::TargetHeaterCoolerState();
      // new Characteristic::Name("Air Conditioner");
    new DEV_Smart_AC; 
    // new Service::HumiditySensor();
      // new Characteristic::CurrentRelativeHumidity();
      // new Characteristic::Name("Humidity Sensor");
    new DEV_Humidity;
    // new Service::Fan();
      // new Characteristic::Active();
      // new Characteristic::Name("AC_Fan");
    new DEV_AC_Fan;

} // end of setup()

//////////////////////////////////////

void loop(){
  
  homeSpan.poll();

  // if(a_minute_passed() == true)
  // {
  //   printLocalTime(); //for debugging
  //   // update_time_hh_mm(); //for debgggg
  // }

  // Serial.print(current_time_s);
  // Serial.print(" ");
  // Serial.println(previous_time_s);

} // end of loop()
