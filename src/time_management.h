//ntp time getting config
#include <time.h>

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 18030;
const int   daylightOffset_sec = 3600;
//ntp config ends

// millis() implementation config
unsigned long previous_time = 0;
unsigned long current_time =  millis();

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

//This function will return true once every second
//Those method needs to be called/updated once in a second can utilize this
//**non-blocking implementation
bool a_second_passed()
{
  if(current_time - previous_time >= 1000)
  {
    previous_time = current_time;
    return true;
  }else{
    return false;
  }
}