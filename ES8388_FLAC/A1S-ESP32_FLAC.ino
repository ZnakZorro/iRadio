/*
sudo chmod a+rw /dev/ttyUSB0
sudo chmod a+rw /dev/ttyACM0
Switches 01100    0=down; 1=up;
*/

#define CODEDIR "CODY/ES8388/A1S_ES8388_RND_NO_WiFi/A1S_ES8388_RND_NO_WiFi_02.ino"
#include "Arduino.h"
//#include "WiFi.h"
#include "SPI.h"
//#include "SD.h"
#include "SD_MMC.h"
#include "FS.h"
#include "Wire.h"
#include "ES8388.h"  // https://github.com/maditnerd/es8388
#include "Audio.h"   // https://github.com/schreibfaul1/ESP32-audioI2S


// GPIOs for SPI
#define SD_CS         13
#define SPI_MOSI      15
#define SPI_MISO       2
#define SPI_SCK       14

#define folderRndMp3     "/mp3_280/"
#define maxRndMp3        280

// I2S GPIOs, the names refer on ES8388, AS1 Audio Kit V2.2 3378
//model=ES8388,bck=27,ws=25,do=26,sda=33,scl=32,i2c=16
//set_GPIO: 21=amp,22=green:0,39=jack:0

#define I2S_SDOUT   26
#define I2S_DSIN    35
#define I2S_BCLK    27
#define I2S_LRCK    25
#define I2S_MCLK    0

// I2C GPIOs
#define IIC_CLK       32
#define IIC_DATA      33

#define GPIO_PA_EN    21

#define LED_BUILTIN 22
#define KEY6 5
#define KEY5 18
#define KEY4 23
#define KEY3 19

int volume         = 90;    // 0...100
int cur_volume     = 8;

String musicFolder = "/mp3/";
int    musicMax    = 100;
String musicExt    = ".mp3";

String zapis[5] = { "/mp3/", "100", "mp3", "ssid", "pass"};


ES8388 es;
Audio audio;

#include <Preferences.h>
Preferences preferences;
const char* preferencesName = "es8388RND3";


 void savePreferences(){
     preferences.begin(preferencesName, false);
     preferences.putUInt("cur_volume", cur_volume);
     preferences.putString("musicFolder", musicFolder); 
     preferences.end();
     Serial.printf("save cur_vol=%d musicFolder=%s\n",cur_volume,musicFolder.c_str());     
} 

 void readPreferences(){
     preferences.begin(preferencesName, false);  
     cur_volume   = preferences.getUInt("cur_volume", 8);
     musicFolder  = preferences.getString("musicFolder", "/mp3/"); 
     preferences.end();
     Serial.printf("read cur_vol=%d musicFolder=%s\n",cur_volume,musicFolder.c_str());     
} 


//----------------------------------------------------------------------------------------------------------------------

void playRandomMp3(){
  Serial.println("++++++++++++++ mp3 random * mp3 random * mp3 random * mp3 random");
  delay(100);
  Serial.print("musicFolder=");Serial.println(musicFolder);
  Serial.print("musicMax=");Serial.println(musicMax);
  Serial.println("-------------- mp3 random * mp3 random * mp3 random * mp3 random");
  
    //int randomNumber = random(1,maxRndMp3);
    //String filename = folderRndMp3 + String(randomNumber) + musicExt;
    int randomNumber = random(1,musicMax);
    String filename = musicFolder + String(randomNumber) + musicExt;
    Serial.print("rndMp3="); Serial.println(filename);
    audio.connecttoFS(SD, filename.c_str());
}

void readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }
  //String s;
  int c=0;
  Serial.println("\n\nRead from file:::::::::::::::::::::::: \n");
  while(file.available()){
    String list = file.readStringUntil('\n');
    zapis[c]=String(list);
    c++;
  }
  file.close();
  Serial.println("zapis=");
  for (int c=0; c<5; c++){
    Serial.print(c); Serial.print(" === ");Serial.println(zapis[c]);
  }
  //QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
  musicFolder = zapis[0];
  musicMax    = zapis[1].toInt();
  musicExt    = zapis[2];
}

void setup(){
  pinMode(KEY3, INPUT_PULLUP);
  pinMode(KEY4, INPUT_PULLUP);
  pinMode(KEY5, INPUT_PULLUP);
  pinMode(KEY6, INPUT_PULLUP);
  
    Serial.begin(115200);
    Serial.println("\r\n");    Serial.println(CODEDIR);
    Serial.printf_P(PSTR("Free mem=%d\n"), ESP.getFreeHeap());
    readPreferences();

    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    //delay(100);
    SPI.setFrequency(1000000);
    //delay(100);
    SD.begin(SD_CS);
    //delay(100);
    readFile(SD, "/info.txt");
             
    Serial.printf("Connect to ES8388 codec... ");
    //delay(100);
    while (not es.begin(IIC_DATA, IIC_CLK)) {
        Serial.printf("Failed!\n");
        delay(1000);
    }
    Serial.printf("OK\n");

    es.volume(ES8388::ES_MAIN, volume);
    es.volume(ES8388::ES_OUT1, volume);
    es.volume(ES8388::ES_OUT2, volume);
    es.mute(ES8388::ES_OUT1, false);
    es.mute(ES8388::ES_OUT2, false);
    es.mute(ES8388::ES_MAIN, false);

    // Enable amplifier
    pinMode(GPIO_PA_EN, OUTPUT);
    digitalWrite(GPIO_PA_EN, HIGH);

    audio.setPinout(I2S_BCLK, I2S_LRCK, I2S_SDOUT);
	  audio.i2s_mclk_pin_select(I2S_MCLK);
    audio.setVolume(cur_volume); // 0...21
    playRandomMp3();
    //randomSeed(analogRead(0));
}
//----------------------------------------------------------------------------------------------------------------------
void volumeUp(int step){
  cur_volume = cur_volume+step;
  if (cur_volume < 0)  cur_volume = 0;
  if (cur_volume > 21) cur_volume = 21;
  //Serial.println(cur_volume);
  audio.setVolume(cur_volume);
  savePreferences();
}

void buttons_loop(){
   int k3 = digitalRead(KEY3);
    int k4 = digitalRead(KEY4);
    int k5 = digitalRead(KEY5);
    int k6 = digitalRead(KEY6);
    /*Serial.println("---");
    Serial.print(k3);
    Serial.print(k4);
    Serial.print(k5);
    Serial.println(k6);*/
    if (k3 == LOW) playRandomMp3();
    if (k4 == LOW) playRandomMp3();
    if (k5 == LOW) volumeUp(-1);
    if (k6 == LOW) volumeUp(1);
    //if (k3 == ) 
}
unsigned long previousMillis = millis();
unsigned long intervalTime = 400;

void loop(){
    audio.loop();
    if (millis()-previousMillis > intervalTime){
        buttons_loop();
        previousMillis = millis();
    }
   
    
}
//----------------------------------------------------------------------------------------------------------------------

// optional
void audio_info(const char *info){
    Serial.print("info        "); Serial.println(info);
}
void audio_id3data(const char *info){  //id3 metadata
    Serial.print("id3data     ");Serial.println(info);
}
void audio_eof_mp3(const char *info){  //end of file
    Serial.print("eof_mp3     ");Serial.println(info);
    playRandomMp3();    
}
void audio_showstation(const char *info){
    Serial.print("station     ");Serial.println(info);
}
void audio_showstreaminfo(const char *info){
    Serial.print("streaminfo  ");Serial.println(info);
}
void audio_showstreamtitle(const char *info){
    Serial.print("streamtitle ");Serial.println(info);
}
void audio_bitrate(const char *info){
    Serial.print("bitrate     ");Serial.println(info);
}
void audio_commercial(const char *info){  //duration in sec
    Serial.print("commercial  ");Serial.println(info);
}
void audio_icyurl(const char *info){  //homepage
    Serial.print("icyurl      ");Serial.println(info);
}
void audio_lasthost(const char *info){  //stream URL played
    Serial.print("lasthost    ");Serial.println(info);
}
void audio_eof_speech(const char *info){
    Serial.print("eof_speech  ");Serial.println(info);
}

