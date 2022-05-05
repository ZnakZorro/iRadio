#include "time.h"

//const char* ntpServer = "3.pl.pool.ntp.org";
const long  gmtOffset_sec      = 3600;   //12600;
const int   daylightOffset_sec = 3600;
/*
      const char powitanie[] PROGMEM = {"Radio blacha"};
      
      const char dzien_0[] PROGMEM = "Niedziela";
      const char dzien_1[] PROGMEM = "Poniedziałek";
      const char dzien_2[] PROGMEM = "Wtorek";
      const char dzien_3[] PROGMEM = "środa";
      const char dzien_4[] PROGMEM = "Czwartek";
      const char dzien_5[] PROGMEM = "Piątek";
      const char dzien_6[] PROGMEM = "Sobota";
      
      const char *const daysOFweek[] PROGMEM = {dzien_0,dzien_1,dzien_2,dzien_3,dzien_4,dzien_5,dzien_6};
      char buffer[16];
*/

/*** Function declaration ***/
//String getRealTime();



void TimersSetup(){
  Serial.println("TimersSetup()TimersSetup()TimersSetup()");
  configTime(gmtOffset_sec, daylightOffset_sec, "0.pl.pool.ntp.org","1.pl.pool.ntp.org","3.pl.pool.ntp.org");
  //configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  //epochTime = getTimeStamp();
}

long getTimeStamp() { //millisek
    time_t now;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      return(0);
    }
    time(&now);
    return now;
}
  
String pad(int liczba){
  String p="";
  if (liczba<10) p="0";
  return p+String(liczba);
}

String getRealTime(){
      //Serial.println("#43-czas.h---showTime()---");
       struct tm timeinfo;
       if(!getLocalTime(&timeinfo)){
          Serial.println("\n$$$ Failed to obtain time");
          return "";
      }
      int godzina = timeinfo.tm_hour;
      int minuta  = timeinfo.tm_min;
      int sekunda = timeinfo.tm_sec;
      //Serial.print("time="); Serial.print(godzina);   Serial.print(":");  Serial.println(minuta);
      //return pad(godzina)+":"+pad(minuta)+":"+pad(sekunda);
      return pad(godzina)+":"+pad(minuta);
       //epochTime = getTimeStamp();
}
