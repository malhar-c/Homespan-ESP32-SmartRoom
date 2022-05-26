#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display (SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//for all time related operations
#include "time_management.h"

//function prototypes
void splash();
bool display_time_large();
bool dipslay_time_small();

void OLED_init()
{
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    display.clearDisplay();
    splash();
}

// void test_OLED() //this is for debugging purpose only
// {
    // display.setTextSize(4);             // Normal 1:1 pixel scale
    // display.setTextColor(SSD1306_WHITE);        // Draw white text
    // display.setCursor(0,0);             
    // display.print(F("24"));
    // display.setTextSize(1);
    // display.print(F("o")); //degree symbol
    // display.setTextSize(2);
    // display.println(F(" C"));

// //  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
// //  display.println(3.141592);
// //
// //  display.setTextSize(2);             // Draw 2X-scale text
// //  display.setTextColor(SSD1306_WHITE);
// //  display.print(F("0x")); display.println(0xDEADBEEF, HEX);

//     display.display();
// }

//Splash screen when the device turns on / reboot
void splash()
{
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(20,5);             
    display.print(F("* Smart Home *"));
    display.setCursor(37,15); 
    display.setTextSize(1);
    display.print(F("ESP - 32")); //degree symbol
    display.setTextSize(1);
    // display.setTextColor(SSD1306_WHITE);
    display.setCursor(25,25);
    display.print(F("Apple Homekit"));
    display.display();
}

void display_climate(int temp, bool AC_status)
{
  //set and update the OLED display with temp, humidity, etc
  // if the AC is on, display the AC set temp, else display time and the current temp

  if(AC_status) //when the AC is on
  {
      display.clearDisplay();
      display.setTextSize(4);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(0,0);             
      display.print(temp);
      display.setTextSize(1);
      display.print(F("o")); //degree symbol
      display.setTextSize(2);
      display.println(F(" C"));   
  }
  else{
      display.clearDisplay();
      display.setTextSize(3);             // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(0,0);             
      display.print(F("OFF"));
  }
  display.display();
}

void update_mode(int M)
{
    // update the AC mode display
}

void display_time()
{

}