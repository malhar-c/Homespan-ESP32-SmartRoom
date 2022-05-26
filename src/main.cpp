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
 
////////////////////////////////////////////////////////////
//                                                        //
//    HomeSpan: A HomeKit implementation for the ESP32    //
//    ------------------------------------------------    //
//                                                        //
// Example 8: HomeKit Bridges and Bridge Accessories      //
//                                                        //
////////////////////////////////////////////////////////////

#include "HomeSpan.h" 
#include "DEV_Switches.h"
#include "DEV_Fan_Regulator.h"
#include "DEV_AC.h"   

void setup() {

  // If the only Service defined in the FIRST Accessory of a mult-Accessory device is the required Accessory Information Service,
  // the device is said to be configured as a "Bridge".  Historically there may have been a number of functional differences between bridge
  // devices and non-bridge devices, but since iOS 15, it's not obvious there are any differences in functionality, with two exceptions:
  
  //  1. Recall from Example 7 that the use of Characteristic::Name() to change the default name of an Accessory Tile
  //     does not work for the first Accessory defined.  The Home App always displays the default name of the first Accessory Tile
  //     as the name of the device specified in homeSpan.begin().  However, this is not an issue when implementing a device
  //     as a Bridge, since the first Accessory is nothing but the Bridge itself - having the default name match the name
  //     of the device in this case makes much more sense.  More importantly, you can now use Characteristic::Name() to change the 
  //     default name of BOTH the LED Accessory Tiles.

  //  2. Devices configured as a Bridge appear in the Home App under the main settings page that displays all Hubs and Bridges.

  // The sketch below is functionally identical to Example 7, except that instead of defining two Accessories (one for the Simple On/Off
  // LED and one for the Dimmable LED), we define three Accessories, where the first acts as the Bridge.
  
  // As usual, all previous comments have been deleted and only new changes from the previous example are shown.

  // NOTE: To see how this works in practice, you'll need to unpair your device and re-pair it once the new code is loaded.
  
  // pinMode(15, OUTPUT);
  // pinMode(2, OUTPUT);
  // pinMode(5, OUTPUT);
  // pinMode(27, OUTPUT);
  // pinMode(26, OUTPUT);
  // pinMode(25, OUTPUT);
  // pinMode(33, OUTPUT);
  // pinMode(32, OUTPUT);

  //defining pinMode pulls the pins to low for a brief period of time, and that's enough for the relays to tick,
  //thus disabling pinMode, and directly assigning the pins to HIGH

  digitalWrite(15, HIGH);
  digitalWrite(2, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(27, HIGH);
  digitalWrite(26, HIGH);
  digitalWrite(25, HIGH);
  digitalWrite(33, HIGH);
  digitalWrite(32, HIGH);

  Serial.begin(115200);

  OLED_init(); //Initialize the OLED display... defined in OLED_config.h

  homeSpan.begin(Category::Bridges,"Smart Room");
  
  new SpanAccessory();                            // This first Accessory is the new "Bridge" Accessory.  It contains no functional Services, just the Accessory Information Service
    new Service::AccessoryInformation();
      new Characteristic::Identify();            
 
  new SpanAccessory();                            // This second Accessory is the same as the first Accessory in Example 7, with the exception that Characteristic::Name() now does something
    new Service::AccessoryInformation();
      new Characteristic::Identify();            
      new Characteristic::Name("Ceiling P1");     // Note that unlike in Example 7, this use of Name() is now utilized by the Home App since it is not the first Accessory (the Bridge above is the first)
    new DEV_Light_pair_1(15);

  new SpanAccessory();                            
    new Service::AccessoryInformation();    
      new Characteristic::Identify();               
      new Characteristic::Name("Ceiling P2");  
    new DEV_Light_pair_2(2);
  
  new SpanAccessory();                            
    new Service::AccessoryInformation();    
      new Characteristic::Identify();               
      new Characteristic::Name("Tube Light");  
    new DEV_Tube_Light(5);

  new SpanAccessory();                            
    new Service::AccessoryInformation();    
      new Characteristic::Identify();               
      new Characteristic::Name("Night Lamp");  
    new DEV_Night_Light(27);

  new SpanAccessory();                            
    new Service::AccessoryInformation();    
      new Characteristic::Identify();               
      new Characteristic::Name("Main Power Socket");  
    new DEV_Socket(26);

  new SpanAccessory();                            
    new Service::AccessoryInformation();    
      new Characteristic::Identify();               
      new Characteristic::Name("Ceiling Fan");  
    new DEV_Ceiling_Fan;

  new SpanAccessory();                            
    new Service::AccessoryInformation();    
      new Characteristic::Identify();               
      new Characteristic::Name("Air Conditioner");  
    new DEV_Smart_AC;

  new SpanAccessory();                            
    new Service::AccessoryInformation();    
      new Characteristic::Identify();               
      new Characteristic::Name("Humidity Sensor");  
    new DEV_Humidity;

} // end of setup()

//////////////////////////////////////

void loop(){
  
  homeSpan.poll();
} // end of loop()