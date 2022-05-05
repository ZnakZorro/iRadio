#include "Arduino.h"

//#define BLURT printf ("#%d F:\"%s\" (function <%s>)\n",__LINE__, __FILE__, __func__)
#define BLURT printf ("#%d  (fun <%s>)\n",__LINE__, __func__)

#include <Preferences.h>
Preferences preferences;
const char* preferencesName = "eink2022";

#include "WiFi.h"

#include "DNSServer.h"
#include "AsyncTCP.h"             //https://github.com/me-no-dev/ESPAsyncTCP 
#include "ESPAsyncWebServer.h"    //https://github.com/me-no-dev/ESPAsyncWebServer
#include "HTTPClient.h"

#include <HTTPClient.h>
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
AsyncWebServer server(80);

#include "Audio.h"

//#include "SPI.h"
#include "SD.h"
//#include "FS.h"


#define OLED_U8G2 1
#define addres2lineI2C 0xC3
//#define addres3lineI2C 0xc3

//#define DEBUG 1
//#define EINK 1
//#define ADCPOT 1

//#define BLACK
#define TUNER
//#define PLASTICBOX

#ifdef PLASTICBOX
    // Plastikowe pudełko
    #define I2S_DOUT      27
    #define I2S_BCLK      26
    #define I2S_LRC       25
    #define HOST_NAME    "PLASTICBOX";
    String WEB_DIR  =     "/PLASTICBOX/";
    int cur_volume    = 5;
    int cur_equalizer = 1;
    int arrEQ[3]={6,-9,6};
      #define LED_BUILTIN_1 12
      #define LED_BUILTIN_2 14
      #define pinKEY1    16
      #define pinKEY2    17
      #define pinKEY3     5
      #define pinKEY4    19    
      //#define POGODA
#endif

#ifdef BLACK
    // Black cartoon
    #define I2S_DOUT      25
    #define I2S_BCLK      26
    #define I2S_LRC       27
    #define HOST_NAME    "BLACK"
    String WEB_DIR  =    "/BLACK/";    
    int cur_volume    = 5;
    int cur_equalizer = 1;
    int arrEQ[3]={4,-9,6};
      #define LED_BUILTIN_1 12
      #define LED_BUILTIN_2 14
      #define pinKEY1    16
      #define pinKEY2    17
      #define pinKEY3     5
      #define pinKEY4    19
      #define POGODA
#endif

// tuner wrover
//The pins GPIO16 and GPIO17 ESP32-WROVER modules have the pins reserved for internal use.
#ifdef TUNER
    #define I2S_DOUT      25
    #define I2S_BCLK      27
    #define I2S_LRC       26
    #define HOST_NAME    "TUNER"
    String WEB_DIR  =     "/TUNER/";
    int cur_volume    = 3;
    int cur_equalizer = 1;
    int arrEQ[3]={0,-4,3};
      #define LED_BUILTIN_1 14  // 33,34 zajęte
      #define LED_BUILTIN_2 12  // 33,34 zajęte
      #define pinKEY1    32 //??
      #define pinKEY2    33 //??
      #define pinKEY3     2
      #define pinKEY4    15
      #define POGODA
#endif

String  musicFolder = "";

#include "Stations.h"
#include "web.h"
#include "czas.h"
#include "nnet.h"
#include "str.h"

#ifdef OLED_U8G2
  #include <U8g2lib.h>
  #ifdef U8X8_HAVE_HW_SPI
    #include <SPI.h>
  #endif
  #ifdef U8X8_HAVE_HW_I2C
    #include <Wire.h>
  #endif

// 128x64  
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display
  
//128x32
//U8G2_SH1106_128X32_VISIONOX_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); 
//U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 21, /* data=*/ 20, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather M0 Basic Proto + FeatherWing OLED
//U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather ESP8266/32u4 Boards + FeatherWing OLED
//U8G2_SSD1306_64X32_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); 
  
#endif

// Digital I/O used for SD
/*
 * //I2S
#define SDA 21
#define SCL 22
#define DAC1 25
#define DAC2 26
#define A0 36
#define A3 39
#define A4 32

*/

// TTGO T1
/*
#define SS   13
#define SCK  14
#define MOSI 15
#define MISO 2
*/
/*
// Dev module, LOLIN D32, NANO 32
#define SS   5
#define SCK  18
#define MOSI 23
#define MISO 19
*/
#define SS   5
#define SCK  18
#define MOSI 23
#define MISO 19


Audio audio;

bool needSavePreferences = false;
unsigned long currentMillis  = 0;
unsigned long previousMillis = 0;
unsigned long intervalMillis = 250;

unsigned long timerSLEEP     = 1000 * 60 * 60 * 2;  // 1 godziny
//unsigned long timerSLEEP     = 1000 * 60 * 5;  // 5 minut test
unsigned long LastTimerSLEEP = 0;

int cur_station   = 0;
unsigned int cur_boot   = 0;
//char extraInfo[48];
String wifis="";

String hostURL = "hostURL";
//String hostNAME = "???";
const char* hostHOST ="http://stream.rcs.revma.com/ypqt40u0x1zuv";

String liniaRadio  = HOST_NAME;
String liniaTitle1 = "iRadio ESP32";
String liniaTitle2 = "znak";
String liniaIP = "0.0.0.0";
#ifdef POGODA
String liniaPogoda1 = "Pogoda+";
String liniaPogoda2 = "Pogoda++";
#endif
int licznik = 0;
const int ADCpodzial = 14;
const int deltaADC   = 100;
int    ADC           = 0;
int    lastADC       = 0;
int    potValue      = 0;
bool   ADCzero       = false;
String streamInfo    = " ";
#define  potPin        32
#define  ledPIN        34

constexpr byte  showRadio = 0;
constexpr byte  showIP    = 1;
constexpr byte  showTime  = 2;
constexpr byte  showPogoda= 3;

/*buttons*/
struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
};

//unsigned long millisBUTTON = millis();
unsigned long plusBUTTON   = 0;

Button button1 = {pinKEY1, 0, false};
Button button2 = {pinKEY2, 0, false};
Button button3 = {pinKEY3, 0, false};
Button button4 = {pinKEY4, 0, false};

void IRAM_ATTR isr1() {button1.numberKeyPresses += 1;button1.pressed = true;}
void IRAM_ATTR isr2() {button2.numberKeyPresses += 1;button2.pressed = true;}
void IRAM_ATTR isr3() {button3.numberKeyPresses += 1;button3.pressed = true;}
void IRAM_ATTR isr4() {button4.numberKeyPresses += 1;button4.pressed = true;}
/*buttons*/


/*** E-PAPER *** E-PAPER *** E-PAPER *** E-PAPER *** E-PAPER ***/
/*** E-PAPER *** E-PAPER *** E-PAPER *** E-PAPER *** E-PAPER ***/
/*** E-PAPER *** E-PAPER *** E-PAPER *** E-PAPER *** E-PAPER ***/

char   buftt[32];
String txt  = "";

#ifdef EINK
    #define ENABLE_GxEPD2_GFX 0
    #include <GxEPD2_BW.h>
    #include <GxEPD2_3C.h>
    #define EPD_CS SS

    //u8g2_font_ncenB12_tr
    //u8g2_font_ncenB10_tr

    //#include <Fonts/FreeMonoBold12pt7b.h>
    //#include <Fonts/FreeMonoBold18pt7b.h>
    //#include <Fonts/FreeMonoBold9pt7b.h>
    //#define DISABLE_DIAGNOSTIC_OUTPUT
    
    GxEPD2_BW<GxEPD2_213_B73, GxEPD2_213_B73::HEIGHT> display(GxEPD2_213_B73(/*CS=5*/ EPD_CS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)); // GDEH0213B73
    //250x128px
    
    /****Function declaration*******/
    void piszEpapPogda(String oneLine);
#endif
#ifdef POGODA
void serverPOGODA();
void OLEDPogda(String payload);
#endif
void TimersSetup();
String getRealTime();
long getTimeStamp();


void SerialListener();
void savePreferences();
void readPreferences();
//void setExternInfo(char const *strCHAR);
//void setExternIP(char const *strCHAR);
//void OLEDpisz(const char *info);

static void connectWIFI_ini();
static void connectWIFI_start();

void installServer();
//void saveONEpref(String key, String val);
/****Function declaration EOF*******/

void OLEDPogda(String payload){
  //Serial.println("OLED_Pogda");
  //Serial.println(payload);
      int r=0, t=0;
      String sa[5];
      for (int i=0; i < payload.length(); i++){ 
       if(payload.charAt(i) == '\n') { 
            sa[t] = payload.substring(r, i); 
            r=(i+1); 
            t++; 
        }
      }
  Serial.println("#296 OLEDPogda=");
  liniaPogoda1 = sa[0];
  liniaPogoda2 = sa[1];
  OLEDstring(showPogoda);
  Serial.println(sa[0]);
  Serial.println(sa[1]);
  Serial.println(sa[2]);
  Serial.println(sa[3]);
  Serial.println(sa[4]);
  
}
//ooooooooooooooooooooooooooooooooooooooooooooo
#define oy1 20
#define oy2 42
#define oy3 62

void OLEDstring(byte flaga){
          Serial.printf("isRun=(%d) cur_vol=(%d) cur_sta=(%d)\n",audio.isRunning(),cur_volume,cur_station);
          if (!audio.isRunning()){
            Serial.println("not Running");
          }  
    #ifdef DEBUG
      Serial.print("#315 liniaTitle len=");Serial.println(liniaTitle1.length());
      Serial.println(liniaTitle1);  
      Serial.println(liniaTitle2); 
      Serial.println(liniaIP); 
    #endif
      int len = liniaTitle1.length();
      if (len >2){ 
          if (len >15){
              if (len>30) len = 30;
              liniaTitle2 = liniaTitle1.substring(15,len);
              liniaTitle1 = liniaTitle1.substring(0,15);
              #ifdef DEBUG
                  Serial.println("#327 len=");
                  Serial.println(len); 
                  Serial.println(liniaTitle1); 
                  Serial.println(liniaTitle2);
              #endif
          }
      } else {
        liniaTitle1 = liniaIP;
      }
    
    #ifdef DEBUG
      Serial.println("\nlinie===========");
      Serial.print("liniaTitle1 = "); Serial.println(liniaTitle1);
      Serial.print("liniaTitle2 = "); Serial.println(liniaTitle1);
      Serial.print("liniaIP     = "); Serial.println(liniaIP);
      Serial.println("linie------");
    #endif


    #ifdef OLED_U8G2 
      // mniej pamięci RAM  //1 223 862 bajtów (38%)  //53 964 bajtów (16%)
      u8g2.firstPage();
      do {
        String volsta = String(cur_volume)+"\" "+String(cur_station+1)+". ";
        //Serial.printf("#312 volsta=(%s) \n",volsta);
        // linia 1
        u8g2.setFont(u8g2_font_ncenB12_tr);    
          u8g2.drawStr(2,oy1,volsta.c_str());
            if (flaga == showTime || flaga == showPogoda){
                  String czas = getRealTime();
                  Serial.printf("czas=(%s) \n",czas);
                  u8g2.drawStr(75,oy1,czas.c_str()); 
            } else {
                   if (flaga == showIP) {
                      String cur_but = " *"+String(cur_boot)+"  ";
                      u8g2.drawStr(44,oy1,cur_but.c_str());
                   } else {
                      liniaRadio = radia[cur_station].info;
                      u8g2.drawStr(44,oy1,liniaRadio.c_str());
                   }
            }
        // linie 2 i 3
        u8g2.setFont(u8g2_font_ncenB10_tr);
        
          if (flaga == showIP) {
            //String cur_but = " #"+String(cur_boot)+"      ";
            //u8g2.drawStr(44,oy1,cur_but.c_str());
            u8g2.drawStr(2,oy2,liniaPogoda1.c_str()); 
            u8g2.drawStr(2,oy3,liniaIP.c_str()); 
          }
          if (flaga == showPogoda){
              u8g2.drawStr(2,oy2,liniaPogoda1.c_str()); 
              u8g2.drawStr(2,oy3,liniaPogoda2.c_str()); 
          }
          if (flaga == showRadio || flaga == showTime){
              u8g2.drawStr(2,oy2,liniaTitle1.c_str());
              u8g2.drawStr(2,oy3,liniaTitle2.c_str());    
          }
       
      } while ( u8g2.nextPage() );  
      
    #endif

    //Serial.printf_P(PSTR("%d OLEDlinie Heap=%d\n"),licznik, ESP.getFreeHeap());
    //Serial.printf("%d OLEDlinie Heap=%d\n", licznik,ESP.getFreeHeap());
    Serial.printf("#385 %d. boot=%d, heap=%d\n", licznik,cur_boot, ESP.getFreeHeap());
    //Serial.printf("#385 %d. heap=%d, len1 =%d, len2 =%d\n", licznik,ESP.getFreeHeap(),liniaTitle1.length(),liniaTitle2.length());
    #ifdef DEBUG
      Serial.println(liniaTitle1);
      Serial.println(liniaTitle2);
    #endif
    //int len1 = liniaTitle1.length();
    //int len2 = liniaTitle2.length();
    
    //Serial.printf_P(PSTR("liniaTitle1=%s %d\n"), liniaTitle1,liniaTitle1.length());
    //Serial.printf_P(PSTR("liniaTitle2=%s %d\n"), liniaTitle2,liniaTitle2.length());
    
    //Serial.printf_P(("liniaTitle1=%s %d\n"), liniaTitle1,liniaTitle1.length());
    //Serial.printf_P(("liniaTitle2=%s %d\n"), liniaTitle2,liniaTitle2.length());

    //Serial.printf_P(liniaTitle1.c_str(),len1);
    //Serial.printf(liniaTitle2,len2);
}

void audio_setVolume(bool save=false){
    if (cur_volume < 0)  cur_volume = 0;
    if (cur_volume > 19) cur_volume = 19;
    //if (save) savePreferences();
    if (save) {
      needSavePreferences=true;
      OLEDstring(showRadio);
    }
    audio.setVolume(cur_volume);
    //OLEDopisz("");
    
}


void audio_SetStation(){
      Serial.println("\n#4195 CoJestGrane=");
      String CoJestGrane = "/"+String(radia[cur_station].info)+".mp3"; 
      Serial.println(SPIFFS.exists(CoJestGrane));
      if (!SPIFFS.exists(CoJestGrane)){
          CoJestGrane = "/r.mp3";
      }
      if (SPIFFS.exists(CoJestGrane)){
          const char* stacjaNazwa = CoJestGrane.c_str();
          audio.setVolume(cur_volume-1);
          audio.connecttoFS(SPIFFS, stacjaNazwa);
      } else {
          audio.connecttohost(radia[cur_station].stream);
      }
    Serial.println(CoJestGrane);
    
    //OLEDopisz(stacjaNazwa);
    hostURL = String(radia[cur_station].stream);
    OLEDstring(showRadio);
}

void esp_Sleep(){
      Serial.println("ssssssssssleep");
      Serial.println(timerSLEEP);
      Serial.println(LastTimerSLEEP);
                liniaIP="SLEEP";
                OLEDstring(showIP);

        audio.stopSong();
        audio.setVolume(0); 
        delay(500); 
        WiFi.disconnect();
        esp_sleep_enable_ext0_wakeup(GPIO_NUM_32,0);
        esp_deep_sleep_start();  

}

/*
void saveONEpref(String pkey, String pval){
  preferences.begin(preferencesName, false);
    preferences.putString(pkey.c_str(),pval.c_str());
  preferences.end();
}
*/
 void savePreferences(){
  BLURT;
     preferences.begin(preferencesName, false);
     preferences.putUInt("cur_volume", cur_volume);
     preferences.putUInt("cur_station", cur_station);
     preferences.putUInt("cur_equalizer", cur_equalizer);     
     preferences.end();
     log_w("cur_vol=%d cur_sta=%d cur_equ=%d",cur_volume,cur_station,cur_equalizer);
     OLEDstring(showIP);
} 

 void readPreferences(){
  // getpreferences
  BLURT;
     preferences.begin(preferencesName, false);  
     cur_volume   = preferences.getUInt("cur_volume", 3);
     cur_station  = preferences.getUInt("cur_station", 0);
     cur_equalizer= preferences.getUInt("cur_equalizer", 0);
       musicFolder = preferences.getString("musicFolder", "");
       cur_boot   = preferences.getUInt("cur_boot", 0);
       cur_boot++;
       preferences.putUInt("cur_boot", cur_boot);
       Serial.printf("cur_boot=%d\n",cur_boot);      
     preferences.end();
     log_w("cur_vol=%d cur_sta=%d cur_equ=%d",cur_volume,cur_station,cur_equalizer); 
} 




#ifdef EINK
void showClock(){
      String czas = getRealTime();
      long timeStamp = getTimeStamp();
      //Serial.printf(" czas/timeStamp=(%s; %d) \n",czas,timeStamp);

      display.setPartialWindow(0, 110, 100, 128);
      display.firstPage();
      do
        {
          display.fillScreen(GxEPD_WHITE);
          display.setCursor(0, 118);
          //txt = czas;
          //txt.toCharArray(buftt, 8);
          display.println(czas);
        }
      while (display.nextPage());
}


void piszEPAPRadio(){ 
      display.setPartialWindow(100, 110, 250, 128);
      display.firstPage();
       do
        {
          display.fillScreen(GxEPD_WHITE);
          display.setCursor(100, 118);
          txt = radia[cur_station].info;
          txt.toCharArray(buftt, 8);
          display.println(txt);
        }
       while (display.nextPage());
      //delay(200);
}
#endif
 
#ifdef EINK
void piszEpapPogda(String oneLine){
  Serial.println(oneLine);  
      int r=0, t=0;
      String sa[5];
      for (int i=0; i < oneLine.length(); i++){ 
       if(oneLine.charAt(i) == '\n') { 
            sa[t] = oneLine.substring(r, i); 
            r=(i+1); 
            t++; 
        }
      }
      display.setRotation(3);
      display.setPartialWindow(0, 0, 250, 100);
      display.fillScreen(GxEPD_WHITE);
      display.firstPage();
      do
      {
          //display.drawRect(0, 0, 200, 121,  GxEPD_BLACK);
          for (int i=0; i<5; i++){
            display.setCursor(0, ((i+1)*24)-4);
            (sa[i]).toCharArray(buftt, 19);
            display.println(buftt);
        }
      }
      while (display.nextPage());
      //delay(200);
}

/*** E-PAPER *** E-PAPER *** E-PAPER *** E-PAPER *** E-PAPER ***/
#endif

#ifdef POGODA
void serverPOGODA() {
    //Serial.println("2 === server ================");
    //Serial.print("wifiMulti.run()=");Serial.println(wifiMulti.run());  //0=not conect
    if ((wifiMulti.run() == WL_CONNECTED)) {
      WiFiClient client;
      HTTPClient http;
     if (http.begin(client, "http://zszczech.zut.edu.pl/cdn/pogoda/yrno2json.php?format=eink")) {
       Serial.println("3 === serverPOGODA================"); 
        int httpCode = http.GET();
        if (httpCode > 0) {
            if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                String payload = http.getString();
                //Serial.println(payload);
                #ifdef EINK
                  piszEpapPogda(payload);          
                #endif
                #ifdef TUNER
                  OLEDPogda(payload);          
                #endif
                #ifdef BLACK
                  OLEDPogda(payload);          
                #endif
            }
        } else { Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str()); }
        http.end();
      } else {Serial.printf("[HTTP} Unable to connect\n");}
    } // if connected 
  
}
#endif

static void blinki(){
  digitalWrite(LED_BUILTIN_1, !digitalRead(LED_BUILTIN_1));
}

void prntBits(byte b){
  if (b<10) Serial.print(" ");
  Serial.print(b);Serial.print(" ");
  //Serial.println(bitState+128, BIN);
  for(int i = 7; i >= 0; i--)
    Serial.print(bitRead(b,i));
  Serial.println();  
}


byte lastState = 0;
byte loop_BUTTONS() {
  byte bitState = 0;
  if (button1.pressed) {bitSet(bitState, 0); button1.pressed = false;}
  if (button2.pressed) {bitSet(bitState, 1); button2.pressed = false;}
  if (button3.pressed) {bitSet(bitState, 2); button3.pressed = false;}
  if (button4.pressed) {bitSet(bitState, 3); button4.pressed = false;}
  if (lastState==bitState) return 0;
  lastState=bitState;
  if (bitState != 0) {
     Serial.print(LED_BUILTIN_1); Serial.print("="); Serial.print(digitalRead(LED_BUILTIN_1));
     //digitalWrite(LED_BUILTIN_1, 1); 
     //digitalWrite(LED_BUILTIN_1, !digitalRead(LED_BUILTIN_1));    
     //Serial.println(digitalRead(LED_BUILTIN_1));
     blinki();
    //Serial.printf("bitState=(%d) \n",bitState);
    prntBits(bitState);
    plusBUTTON = 500;
    if (bitState == 1) {cur_volume--;audio_setVolume(1);}
    if (bitState == 2) {cur_volume++;audio_setVolume(1);}
    if (bitState == 4) {audio_SetIncreaseStation(-1);}
    if (bitState == 8) {audio_SetIncreaseStation(1);}
    if (bitState == 9) {blinki();Serial.println("restart");WiFi.disconnect(); delay(500);ESP.restart();}
    //if (bitState == 3) {Serial.println(3);cur_volume=0;audio_setVolume(1);}
    //if (bitState == 12){Serial.println(12);}
    
    //Serial.printf("%d, %d, %d, %d", button1.PIN,button2.PIN,button3.PIN,button4.PIN);
    //Serial.printf("%d, %d, %d, %d\n", digitalRead(button1.PIN),digitalRead(button2.PIN),digitalRead(button3.PIN),digitalRead(button4.PIN));
    needSavePreferences=false;
    OLEDstring(showRadio);
    return bitState;
  }
}


void setup() {
  /*cur_boot++;
  preferences.begin(preferencesName, false);
     preferences.putUInt("cur_boot", cur_boot);
  preferences.end();
  */
  Serial.begin(115200); 
        Serial.println(""); 
        Serial.println(__FILE__);
        //opiszDefines(); 
        pinMode(LED_BUILTIN_1, OUTPUT); 
        digitalWrite(LED_BUILTIN_1, HIGH);
        #ifdef OLED_U8G2
            u8g2.begin();
        #endif
        OLEDstring(showIP);
        //pinMode(ledPIN, OUTPUT);
        //display.init(115200); // enable diagnostic output on Serial
         #ifdef EINK
          display.init(); // disable diagnostic output on Serial
              display.setRotation(3);
              display.setFont(&FreeMonoBold12pt7b);
              display.setTextColor(GxEPD_BLACK);
              display.setFullWindow(); 
              //display.setPartialWindow(0, 0, display.width(), display.height());
               Serial.print(display.width());  Serial.print("x");Serial.println(display.height());
               Serial.printf("Eink =(%d x %d) px \n",display.width(),display.height());
               piszEpapPogda("ZnakZorro\niRadio-ESP-32\nYRNO\n");
          #endif

    #ifdef DEBUG
        Serial.println("");
    #endif
    readPreferences();
      wifis=wifiScan();
      //digitalWrite(LED_BUILTIN_1, LOW);
      blinki();
      //Serial.println(wifis);
    WiFi.disconnect();
    connectWIFI_ini();
    //digitalWrite(LED_BUILTIN_1, HIGH);
    blinki();
    connectWIFI_start();
    blinki();
    //digitalWrite(LED_BUILTIN_1, LOW);

    
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio_setVolume();
    audio.connecttohost(radia[cur_station].stream);
                delay(200);
                pinMode(SS, OUTPUT);      
                digitalWrite(SS, HIGH);
                SPI.begin(SCK, MISO, MOSI); 
                /*if(!SD.begin(SS)){
                    Serial.println("Card Mount Failed ??????????????????\n???????????????????????????????"); 
                } else {
                  Serial.println("Card Mount SUCCES.................."); 
                  Serial.println("connecttoSD hey.mp3");
                   audio.connecttoSD("/hey.mp3");
                }*/
                SPI.setFrequency(1000000);
                

                if (!SPIFFS.begin(true)) {
                      Serial.println("Error SPIFFS");
                } else {
                      Serial.println("SPIFFS OK");
                      #ifdef DEBUG
                          listFileFromSPIFFS();
                          //---List contents of SPIFFS
                          //---listDir(SPIFFS, WEB_DIR.c_str(), 0);
                      #endif
                }

        audio_setVolume();
        //audio.setTone(qqq[cur_equalizer].l, qqq[cur_equalizer].m, qqq[cur_equalizer].h);
        audio.setTone(arrEQ[0],arrEQ[1],arrEQ[2]);
        //audio_SetEQNr(cur_equalizer);
        audio.connecttoFS(SPIFFS, "/a.mp3");
        //audio.connecttoFS(SPIFFS, "/r.mp3");
        
     /*buttons*/
      //digitalWrite(LED_BUILTIN_1, HIGH);
      blinki();
      pinMode(button1.PIN, INPUT_PULLUP);
      pinMode(button2.PIN, INPUT_PULLUP);
      pinMode(button3.PIN, INPUT_PULLUP);
      pinMode(button4.PIN, INPUT_PULLUP);
      Serial.printf("%d, %d, %d, %d\n", digitalRead(button1.PIN),digitalRead(button2.PIN),digitalRead(button3.PIN),digitalRead(button4.PIN));
    
          //LOW, HIGH, CHANGE, FALLING, RISING
      attachInterrupt(button1.PIN, isr1, RISING);
      attachInterrupt(button2.PIN, isr2, RISING);
      attachInterrupt(button3.PIN, isr3, RISING);
      attachInterrupt(button4.PIN, isr4, RISING);
      Serial.printf("%d, %d, %d, %d\n", digitalRead(button1.PIN),digitalRead(button2.PIN),digitalRead(button3.PIN),digitalRead(button4.PIN));
    /*buttons*/ 
              liniaIP = WiFi.localIP().toString();
              OLEDstring(showRadio);
              serverPOGODA();
} // EOF setup





char incomingByte;
long unsigned modulo = 0;


/*** LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP LOOP ***/
void loop(){
    licznik++;
    audio.loop();
    
    currentMillis = millis();
    // every 250ms
    if(currentMillis - previousMillis > intervalMillis+plusBUTTON){
      byte b = loop_BUTTONS();
      plusBUTTON=0;
        previousMillis = currentMillis;
        

         // POGODA co 30 minut
         if (modulo % 7200 == 3600) {
              #ifdef POGODA
                serverPOGODA();
              #endif
         }
         // Clock every 1 minute
            if (modulo % 240 == 40) {
              Serial.println("\n1 minute %%%%%%%%%%%%%%%%%%%%%%%%%");
               digitalWrite(LED_BUILTIN_1,LOW);
               OLEDstring(showIP);
              #ifdef EINK
                showClock();
              #endif
              if (needSavePreferences) {
                savePreferences();
                needSavePreferences=false;
              }

              if(currentMillis - LastTimerSLEEP > timerSLEEP) {
                Serial.println("#702 * SLEEP * SLEEP * SLEEP * SLEEP"); 
                 //LastTimerSLEEP = currentMillis; 
                 esp_Sleep();
              }
            }
             if (modulo % 240 == 50 || modulo % 240 == 100) {
              Serial.println("\nRADIO %%%%%%%%%%%%%%%%%");
                OLEDstring(showRadio);
            }
            if (modulo % 240 == 60) {
              Serial.println("\nCZAS %%%%%%%%");
                OLEDstring(showTime);
            }
            if (modulo % 240 == 200) {
              Serial.println("\nPOGODA %%%%");
                OLEDstring(showPogoda);
            }
        
         modulo++;
         SerialListener();
          
     #ifdef EINK
        ADCListener();
     #endif
          
   } // EOF intervalMillis


} // EOF loop


void audio_eof_mp3(const char *info){
   //#ifdef DEBUG
      Serial.print("\n#838 audio_eof_mp3  ====="); 
      Serial.println(info);
   //#endif
  
  //Serial.println(cur_station);
  //Serial.println(cur_volume);

  audio_setVolume(0);
  audio.connecttohost(radia[cur_station].stream); 
  hostURL = String(radia[cur_station].stream);
  Serial.printf("eof_mp3 cur_volume=(%d)\n",cur_volume);
  licznik =0;
  }


void audio_showstation(const char *info){  
  //OLEDopisz(info);
  Serial.print("#735 showstation=");       Serial.println(info);
  //Serial.println(cur_station);
  //Serial.println(cur_volume);
  audio.setVolume(cur_volume+radia[cur_station].ampli);
   {
     txt = radia[cur_station].info;
     txt.toCharArray(buftt, 8);
     #ifdef EINK
      piszEPAPRadio();
     #endif
    licznik =0;
  }
}

void audio_showstreamtitle(const char *info){ 
  Serial.print("#792 showstreamtitle=");Serial.println(info);
  liniaTitle1 = String(info);
  liniaTitle1.trim();
  if (liniaTitle1.length()>2){
    OLEDstring(showRadio);
  } else {
    OLEDstring(showIP);
  }
  
}

void audio_showstreaminfo(const char *info){  
  Serial.print("#804 streaminfo =");   Serial.println(info);
  //snprintf(extraInfo, 48, info);
}

/*
void audio_info(const char *info){            Serial.print("info        "); Serial.println(info);}
void audio_id3data(const char *info){         Serial.print("id3data     "); Serial.println(info);}
*/


 /*   
    void audio_showstreamtitle(const char *info){ 
      return;
      String infoTXT = info;
      Serial.println("----------------");
      Serial.print("licznik=");      Serial.println(licznik);
       if (licznik==1){
           
           streamInfo = info;
      } 
       licznik++;
 
    }
 */   
//void audio_bitrate(const char *info){         Serial.print("bitrate     "); Serial.println(info);}
//void audio_commercial(const char *info){      Serial.print("commercial  "); Serial.println(info);}
//void audio_icyurl(const char *info){          Serial.print("icyurl      "); Serial.println(info);}
//void audio_lasthost(const char *info){        Serial.print("lasthost    "); Serial.println(info);}
//void audio_eof_speech(const char *info){      Serial.print("eof_speech  "); Serial.println(info);}






/************* SERVER *******************/
/************* SERVER *******************/
/************* SERVER *******************/
// radio info
String getRadioInfo(){
  //return "1!1!1!1!1!1";  

  const String      sep = "!"; 
  String v = String(cur_volume);
  String q = String(cur_equalizer);
  String qqq = String(arrEQ[0])+","+String(arrEQ[1])+","+String(arrEQ[2]);
  String n = String(cur_station);
  String radio = liniaRadio;
  //String hostNAME = String(radia[cur_station].info);
  String ri    = String(WiFi.RSSI());
    //LCD_Buff_CoJestGrane = extraInfo;
    //LCD_Buff_JakaStacja = "V="+String(cur_volume) + ", "+String(hostNAME);
    //LCD_Buff_JakaStacja = s;
    String czas = String((timerSLEEP/(1000*60)));
    //hostURL = "hostURL";
    log_w("s=%s; s=%s; ri=%s; hName=%s q=%s czas=%s hURL=%s",n,v,ri,radio,q,czas,hostURL);
    //OLEDpisz((v+" "+n).c_str());
              //log_v("Verbose");
              //log_d("Debug");
              //log_i("Info");
              //log_w("Warning"); 
              //log_e("Error");    
  String curB = String(cur_boot);
  String isR = String(audio.isRunning());
  
  return n+sep+v+sep+ri+sep+radio+sep+liniaTitle1+liniaTitle2+sep+qqq+sep+czas+sep+curB+sep+isR+sep+hostURL;

}

void audio_SetEQNr(String ParamValue){
      cur_equalizer = ParamValue.toInt();
      //savePreferences(); 
      needSavePreferences=true;
      audio.setTone(qqq[cur_equalizer].l, qqq[cur_equalizer].m, qqq[cur_equalizer].h);
}


void audio_SetIncreaseStation(int delta){
    cur_station += delta;
    if (cur_station<0) cur_station=LAST_STATION;
    if (cur_station>LAST_STATION) cur_station=0; 
    Serial.print("#532 cur_station=");Serial.println(cur_station);
    audio_ChangeStation(String(cur_station));
}





void audio_ChangeStation(String ParamValue){
  cur_station =  ParamValue.toInt();
      if (cur_station <= 0)  cur_station = 0;
    if (cur_station > 4) cur_station = 4;
    needSavePreferences=true;
    audio.setVolume(0);
    //audio.connecttohost(radia[cur_station].stream);
    audio_SetStation();
    Serial.print("#846 audio_ChangeStation ==="); Serial.println(ParamValue);
    //OLEDpisz(const char *info)
} 

void audio_SetStationUrl(String ParamValue){
      Serial.println("\n#903 xxxxxxxxxxxxxxxxxxx"); 
      LastTimerSLEEP = millis()-60000;
        liniaTitle1 = ParamValue;
        OLEDstring(showRadio);
          Serial.println("#907 audio_SetStationUrl hostURL==="); 
        //audio.setVolume(0); 
        audio.connecttohost(ParamValue.c_str());
}

//ajax
void audio_ChangeVolume(String ParamValue){
    Serial.println("audio_ChangeVolume vvvvvvvvvvv");
    Serial.println(ParamValue);  
    LastTimerSLEEP = millis()-60000;
    if (ParamValue=="p") cur_volume++;
    if (ParamValue=="m") cur_volume--;
    if (ParamValue=="*") cur_volume++;
    if (ParamValue=="-") cur_volume--;
    audio_setVolume(1);
    //setCurVolume();
}


void installServer(){
  Serial.println("#581 installServer...");
  Serial.println(WEB_DIR);

  /*
   // test only
    server.on("*", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("*******************");
    Serial.println(request->params());
      AsyncWebParameter* p0 = request->getParam(0); 
      AsyncWebParameter* p1 = request->getParam(1); 
      AsyncWebParameter* p2 = request->getParam(2); 
      String name0  = String(p0->name());
      String name1  = String(p1->name());
      String name2  = String(p2->name());
      String val0   = p0->value();
      String val1   = p1->value();
      String val2   = p2->value();
      
        Serial.print(name0);Serial.print(" 0=");Serial.println(val0);
        Serial.print(name1);Serial.print(" 1=");Serial.println(val1);
        Serial.print(name2);Serial.print(" 2=");Serial.println(val2);
       
    request->send(SPIFFS, INDEX_HTML, "text/html");
  });*/
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/www/index.html", "text/html");
  });
  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/www/index.html", "text/html");
  });
  server.on("/radio.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/www/radio.js", "text/javascript");
  });
  server.on("/eq.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/www/eq.js", "text/javascript");
  });
  server.on("/FileSaver.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/www/FileSaver.min.js", "text/javascript");
  });
  server.on("/radio.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/www/radio.css", "text/css");
  });
  /*server.on("/css.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, WEB_DIR+"css.css", "text/css");
  });*/
  server.on("/radio.web.json", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, WEB_DIR+"radio.web.json", "application/manifest+json");
  });
  server.on("/radio.svg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, WEB_DIR+"radio.svg", "image/svg+xml");
  });
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, WEB_DIR+"favicon.ico", "image/x-icon");
  });
  server.on("/start", HTTP_GET, [](AsyncWebServerRequest *request){
    bool gramy = audio.pauseResume();
    request->send(200, "text/plain",  getRadioInfo());
  });
  server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain",  wifis);
  });
 
  
  // AJAXY *************************
  // AJAXY *************************


  // AJAXY *************************
 
  server.on("/radio", HTTP_GET, [](AsyncWebServerRequest *request){
      String valParam = request->getParam(0)->value();
    
      if (request->hasParam("n")) {
          log_w("hasParam(n)=(%s) \n",valParam);
        
      }
      if (request->hasParam("s")) {
          log_w("hasParam(s)=(%s) \n",valParam);
          audio_ChangeStation(valParam); 
      }
      if (request->hasParam("x")) {
          log_w("hasParam(/radio?x=%s) \n",valParam);
          audio_SetStationUrl(valParam);    
      }
      if (request->hasParam("y")) {
        Serial.println(valParam);
        long unsigned y=valParam.toInt();
        y *=1000*60;
          LastTimerSLEEP = millis();
          timerSLEEP = y;
        Serial.println(timerSLEEP);
          log_w("hasParam(/radio?y=%s) \n",valParam);   
      }
      if (request->hasParam("r")) {   // reboot
          log_w("hasParam(/radio?r=%s) \n",valParam);
          ESP.restart();
      }
      if (request->hasParam("e")) {
          log_w("hasParam(/radio?e=%s) \n",valParam);
          esp_Sleep();
      }
      if (request->hasParam("q")) {
          log_w("hasParam(/radio?q=%s) \n",valParam);
          audio_SetEQNr(valParam);
      }
      if (request->hasParam("start")) {
          log_w("hasParam(/radio?start=%s) \n",valParam);
          audio.pauseResume();
      }     
      if (request->hasParam("vol")) {
          cur_volume += valParam.toInt();
          audio_setVolume(1);
          log_w("hasParam(/radio?vol=%s) \n",valParam);    
      }
      if (request->hasParam("v")) {
          cur_volume = valParam.toInt();
          if (cur_volume>19) cur_volume=19;
          audio_setVolume(1);
          log_w("hasParam(/radio?vol=%s) \n",valParam);    
      }
      if (request->hasParam("sta")) {
          audio_SetIncreaseStation(valParam.toInt());
          log_w("hasParam(/radio?sta=%s) \n",valParam);    
      }
        if (request->hasParam("eqlow")) {
            arrEQ[0]=(valParam.toInt());
            audio.setTone(arrEQ[0],arrEQ[1],arrEQ[2]);          
            log_w("$d; $d; $d",arrEQ[0],arrEQ[1],arrEQ[2]);    
        }
      if (request->hasParam("eqmid")) {
            arrEQ[1]=(valParam.toInt());
            audio.setTone(arrEQ[0],arrEQ[1],arrEQ[2]);          
            log_w("$d; $d; $d",arrEQ[0],arrEQ[1],arrEQ[2]);    
        }
      if (request->hasParam("eqhig")) {
            arrEQ[2]=(valParam.toInt());
            audio.setTone(arrEQ[0],arrEQ[1],arrEQ[2]);          
            log_w("$d; $d; $d",arrEQ[0],arrEQ[1],arrEQ[2]);    
        }
      request->send(200, "text/plain",getRadioInfo());
  });

  // from /ap.html 

/*
server.on("/zapissss", HTTP_POST, [](AsyncWebServerRequest *request){  
  Serial.println("zapisss");
      String inputMessage;
      //String inputParam;
  
      if (request->hasParam("ssid1")) {
        inputMessage = request->getParam("ssid1")->value();
        Serial.printf("ssid1=%s\n",inputMessage);        
        saveONEpref("ssid1",request->getParam("ssid1")->value());
      }
      if (request->hasParam("pass1")) {
        inputMessage = request->getParam("pass1")->value();
        Serial.printf("pass1=%s\n",inputMessage);        
        saveONEpref("pass1",request->getParam("pass1")->value());
      }
      if (request->hasParam("ssid2")) {
        inputMessage = request->getParam("ssid2")->value();
        Serial.printf("ssid2=%s\n",inputMessage);        
        saveONEpref("ssid2",request->getParam("ssid2")->value());
      }
      if (request->hasParam("pass2")) {
        inputMessage = request->getParam("pass2")->value();
        Serial.printf("pass2=%s\n",inputMessage);        
        saveONEpref("pass2",request->getParam("pass2")->value());
      }
      request->redirect("/");
  });
  */
  server.on("/key", HTTP_GET, [](AsyncWebServerRequest *request){
      int params = request->params();
      AsyncWebParameter* p0 = request->getParam(0); 
      AsyncWebParameter* p1 = request->getParam(1); 
      AsyncWebParameter* p2 = request->getParam(2); 
      String name0  = String(p0->name());
      String name1  = String(p1->name());
      String name2  = String(p2->name());
      String val0   = p0->value();
      String val1   = p1->value();
      String val2   = p2->value();
       
      //Serial.print("#710 name= ");Serial.print(name0.c_str()); Serial.print("="); Serial.println(val0.c_str());
      //Serial.print("#711 val = ");Serial.print(name1.c_str()); Serial.print("="); Serial.println(val1.c_str());
      //Serial.print("#712 nr  = ");Serial.print(name2.c_str()); Serial.print("="); Serial.println(val2.c_str());
      //Serial.println();

      String nr       = val2;
      String nameName = "sName"+nr;
      String nameVal  =  val0;
      String strmName = "sUrl"+nr;
      String strmVal  =  val1;
      Serial.print("#731  ");Serial.print(nameName); Serial.print(" = "); Serial.println(nameVal);
      Serial.print("#732  ");Serial.print(strmName); Serial.print(" = "); Serial.println(strmVal);
      Serial.println();
         //preferences.begin(preferencesName, false);
         //preferences.putString(nameName.c_str(), nameVal.c_str());     
         //preferences.putString(strmName.c_str(), strmVal.c_str());     
         //preferences.end();
      
      request->send(200, "text/plain",getRadioInfo());
  }); // on/key


/*ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ*/
/*ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ*/
/*ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ*/
  
   server.on("/zapis", HTTP_POST, [](AsyncWebServerRequest *request){
    Serial.println("...\n");
    int params = request->params();
     for(int i=0;i<params;i++){
          AsyncWebParameter* p = request->getParam(i);          
          String ParamName  = String(p->name());
          String ParamValue = String(p->value());
          ParamName.trim();
          ParamValue.trim();
          if (ParamValue !="") {
            //Serial.printf(" %d. name=%s, value=%s\n",i, ParamName, ParamValue);
            Serial.print(i); Serial.print(". "); 
            Serial.print(ParamName); Serial.print("="); Serial.println(ParamValue);
            // Zapis do preferencess
                 preferences.begin(preferencesName, false);
                    //preferences.putString(const char* key, const String value)
                    preferences.putString(ParamName.c_str(), ParamValue);              
                preferences.end();
          }
     }
      request->redirect("/");
  });
 

  
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  server.begin();
  //initWebSocket(); 
  Serial.println("#700 Start SERVER");
  //audio.connecttohost(hostURL.c_str());
  
  // po uruchomieniu servera
  //audioStart();
  server.serveStatic("/ap/", SPIFFS, "/ap/").setDefaultFile("index.html");
  server.serveStatic("/conf/", SPIFFS, "/ap/").setDefaultFile("index.html");
}




/*________________________________________________________________________________*/
/*________________________________________________________________________________*/
/*________________________________________________________________________________*/


void SerialListener(){
    if (Serial.available() > 0) {
        Serial.println("\n======SerialListener==========");
        int SerialVal = Serial.parseInt(); // czytamy
        int noNeedByte = Serial.read(); 
        Serial.println(SerialVal);
        if (SerialVal == -9) {ESP.restart();}
        //if (SerialVal == -1) {serverPOGODA();}
        Serial.print("SerialVal=");Serial.println(SerialVal);
                                  
            if(SerialVal >= 100 && SerialVal < 121) {
              cur_volume = SerialVal-100;
              audio_setVolume();
              Serial.print("volume="); Serial.println(SerialVal-100);
            }
            if(SerialVal == 200) audio.setTone(0,0,0);
            if(SerialVal == 201) audio.setTone(6,0,0);
            if(SerialVal == 202) audio.setTone(0,0,6);
            if(SerialVal == 203) audio.setTone(6,0,6);
            if(SerialVal == 204) audio.setTone(6,-6,6);
            if(SerialVal == 205) audio.setTone(6,-6,0);
            if(SerialVal == 206) audio.setTone(0,-6,6);
            
            if(SerialVal < max_stations) audio.connecttohost(radia[cur_station].stream);        
        
    }
}


void ADCListener(){
       #ifdef EINK

          /* ADC * ADC * ADC * ADC * ADC * ADC * ADC * ADC * */
          ADC = (analogRead(potPin) + lastADC) >> 1;
          //Serial.printf("ADC=%d lastADC=%d delta=%d", ADC, lastADC, ADC-lastADC); Serial.println("");
          if (abs(ADC-lastADC) > deltaADC){ 
            //Serial.print(ADC); Serial.print("="); Serial.println(ADC-lastADC);
              potValue = (ADC) >> 8; // 1024
              #ifdef DEBUG
                   //Serial.printf("ADC=%d lastADC=%d delta=%d potValue=%d", ADC, lastADC, ADC-lastADC,potValue); Serial.println("");   
              #endif
              
              if (potValue <= ADCpodzial){
                  cur_volume = potValue;
                  audio_setVolume();
                  //piszEPAPRadio();
                  if (ADCzero==true){
                      ADCzero=false;
                      audio.connecttohost(radia[cur_station].stream);
                      licznik=0;
                  }
              }
              if (potValue > ADCpodzial) {
                cur_station++;
                if(cur_station < max_stations) {
                    #ifdef DEBUG
                        Serial.print("cur_station=");Serial.println(cur_station);
                    #endif
                    audio.connecttohost(radia[cur_station].stream);
                    licznik=0;   
                }   else cur_station = 0; 
              } 
              if (potValue == 0) { 
                cur_station =0;
                audio.stopSong();
                piszEPAPRadio();
                ADCzero=true;
                #ifdef DEBUG 
                    Serial.print("STACJA=");Serial.println(cur_station);
                #endif
              }    
                      
          }  // eoif potValue
          lastADC = ADC;
          /*ADC end*/               
  #endif 
}





/*
  2022.05.05 
  DOM 
  "ESp32 DEV Module" - working eink
  "ESP32 TTGO T1" - not working eink
  e-ink TTGO T5 V2.0
  250x128px

  https://github.com/lewisxhe/TTGO-EPaper-Series
  https://github.com/schreibfaul1/ESP32-audioI2S
*/
