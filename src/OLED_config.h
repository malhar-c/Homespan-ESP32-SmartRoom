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

//for custom logos / icons
#include "custom_icons/cool.h"
#include "custom_icons/fan_auto.h"
#include "custom_icons/fan_min.h"
#include "custom_icons/fan_mid.h"
#include "custom_icons/fan_max.h"

//function prototypes
void splash();
void display_time_large();
void AC_mode_display(int);
void AC_fan_settings_display(int);

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

void display_stuff(int temp, bool active, int AC_mode, int fan_speed)
{
  //set and update the OLED display with temp, humidity, etc
  // if the AC is on, display the AC set temp, else display time and the current temp
  if(active)
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
      AC_mode_display(AC_mode);
      AC_fan_settings_display(fan_speed);
      display.display();
  }
  else{
      display_time_large();
  }
}

void AC_mode_display(int mode) //0-auto 1-heat/fan 2-cool
{
    switch (mode)
    {
    case 0:
        /* code */
        break;
    case 1:
        //fan mode logo show
        break;

    case 2:
        display.drawBitmap(109, 0, cool_bmp, cool_width, cool_height, 1);
        break;

    default:
        break;
    }
}

void AC_fan_settings_display(int speed)
{
    Serial.print("Fan Mode Value: ");
    Serial.println(speed);
    switch(speed) //0-auto 1-low 2-med 3-high
    {
        case 0: 
            display.drawBitmap(89, 17, fan_auto_bmp, fan_auto_width, fan_auto_height, 1);
            break;
        case 1: 
            display.drawBitmap(89, 17, fan_min_bmp, fan_min_width, fan_min_height, 1);
            break;
        case 2: 
            display.drawBitmap(89, 17, fan_mid_bmp, fan_mid_width, fan_mid_height, 1);
            break;
        case 3: 
            display.drawBitmap(89, 17, fan_max_bmp, fan_max_width, fan_max_height, 1);
            break;
    }
}

void display_time_large()
{
    display.clearDisplay();
    if(update_time_hh_mm())
    {
        display.setTextSize(2);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);        // Draw white text
        display.setCursor(0,10);             
        // display.print(F("OFF"));
        display.print(hours);
        display.print(F(":"));
        display.print(minutes);
        display.display();

        // display.drawBitmap(100, 0, cool_bmp, cool_width, cool_height, 1);
        // // display.drawBitmap(89, 20, fan_auto_bmp, fan_auto_width, fan_auto_height, 1);
        // display.display();
        
        //for debuggg
        // Serial.println("update time display called");
        // Serial.print(hours);
        // Serial.print(":");
        // Serial.println(minutes);
        // printLocalTime(); //for debugging
    }
}