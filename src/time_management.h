//ntp time getting config
#include <time.h>

const char* ntpServer = "in.pool.ntp.org";
const long  gmtOffset_sec = 19800;
const int   daylightOffset_sec = 0;
//ntp config ends

// millis() implementation config
unsigned long previous_time = 0;
unsigned long current_time =  millis();
unsigned short previous_time_s = 0;
unsigned short current_time_s =  millis();

short hours = 0;
short minutes = 0;

void time_init()
{
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

//test time print funtion
void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  // getLocalTime(&timeinfo);
  Serial.print ("TEST ---- ");
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

bool update_time_hh_mm()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    return 0;
  }
  hours = timeinfo.tm_hour;
  minutes = timeinfo.tm_min;
  //for debugging
  // Serial.print ("Stored values ---- ");
  // Serial.print(hours);
  // Serial.print(":");
  // Serial.println(minutes);
  return 1;
}

//This function will return true once every minute
//So those method needs to be called/updated once in a minute can utilize this
//**non-blocking implementation
//**this can used for preciesed timed events as long as the variable is not overflowed,
//i.e. 49 ish days, even after the overflow it can be used for precise timming after complete reset
//for another 49ish days
//NOTE: overflow will not break anything
bool a_minute_passed()
{
  current_time = millis();
  if(current_time - previous_time >= 60000 || current_time == 0)
  {
    previous_time = current_time;
    return true;
  }
  else{
    return false;
  }
}

//This function will return true once every 2 seconds
//Those method needs to be called/updated once in two second can utilize this
//This will overflow every minute-ish and reset itself
//**non-blocking implementation
//***DEPECRATED***
// bool two_second_passed()
// {
//   current_time_s = millis();
//   if(previous_time_s > current_time_s) 
//   {
//     previous_time_s = 0;
//   }
//   if(current_time_s - previous_time_s >= 2000)
//   {
//     previous_time_s = current_time_s;
//     return true;
//   }
//   else{
//     return false;
//   }
// }
