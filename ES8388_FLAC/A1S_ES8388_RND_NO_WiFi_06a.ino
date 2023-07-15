/*
sudo chmod a+rw /dev/ttyUSB0
sudo chmod a+rw /dev/ttyACM0
Switches 01100    0=down; 1=up;

Instrumental_flac_7
Jazzy_flac_284
Modal_flac_220
Vocal_flac_10
MP3_mp3_280

*/

#define CODEDIR "/home/znak/Arduino/CODY/ES8388/A1S_ES8388_RND_NO_WiFi/A1S_ES8388_RND_NO_WiFi_06.ino"
#include "Arduino.h"
//#include "WiFi.h"
#include "SPI.h"
//#include "SD.h"
#include "SD_MMC.h"
#include "FS.h"
#include "Wire.h"
#include "ES8388.h"  // https://github.com/maditnerd/es8388
#include "Audio.h"   // https://github.com/schreibfaul1/ESP32-audioI2S
#include "z.h"
#include <vector>
#include <esp_task_wdt.h>                
#define WDT_TIMEOUT 3

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
#define DS   "/"

int volume         = 90;    // 0...100
int cur_volume     = 8;

String musicFolder = "/mp3/";
int    musicMax    = 100;
String musicExt    = ".mp3";
String zapis[5] = { "/mp3/", "100", "mp3", "ssid", "pass"};

int countFolders = 0;
int actualFolder = 0;
int lastFolder   = 0;
//String actualTitle = "";
//String actualName  = ""

unsigned long timeMillis = 0;
int counterLoop = 0;

vector<String> nameFolders;
vector<infoFOLDER> allFolders;

infoSongType infoSong;
/*
  char *TITLE;
  char *ALBUM;
  char *ARTIST;
  int   DURATION;
 */
//infoSTRUCT infoBit={0,0,0,0,"","","","","",""};
 
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

String humanTime(int sek){
      int minut = sek/60;
      sek = sek - (minut*60);
      String zeresek="";
      if (sek<10) zeresek="0";
      return String(minut)+":"+zeresek+String(sek);
}
void printInfoSong(){
  //Serial.println("++++++++++ printInfoSong +++++++++++++ ");
  String title = nameFolders[actualFolder];
  Serial.print("PATH:  ");Serial.print(title);Serial.print("; ");
       Serial.print("ALBUM:  ");Serial.print(infoSong.ALBUM);Serial.print("; ");     
       Serial.print("TITLE:  ");Serial.print(infoSong.TITLE);Serial.print("; ");
       Serial.print("ARTIST: ");Serial.print(infoSong.ARTIST);Serial.print("; ");
       Serial.print("DURAT:  ");Serial.print(humanTime(infoSong.DURATION));Serial.print("; ");
       Serial.println();
}
void printInfo(){
  Serial.println("************* random MUSIC info ************* ");
    Serial.print("@musicFolder=");          Serial.println(musicFolder);
    Serial.print("@musicMax=");             Serial.println(musicMax);

          Serial.print("#countFolders=");   Serial.println(countFolders); 
          Serial.print("#lastFolder=");     Serial.println(lastFolder); 
          Serial.print("#actualFolder=");   Serial.println(actualFolder);
          for(int i = 0; i < countFolders; i++){
              Serial.print(i);Serial.print(". fnam="); Serial.println(nameFolders[i]);
          }
          
          infoFOLDER info = parseFolderName(actualFolder);              
          Serial.print("info.Path=");       Serial.println(info.Path);
          Serial.print("info.Title=");      Serial.println(info.Title);
          Serial.print("info.Ext=");        Serial.println(info.Extention);
          Serial.print("info.LastNumber="); Serial.println(info.LastNumber);
  Serial.println("************* random MUSIC info ************* ");
}

//----------------------------------------------------------------------------------------------------------------------

void playRandomMusic(){
  Serial.println("\nRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR");
      infoSong.DURATION = 0;
      infoSong.TITLE    = "";
      infoSong.ALBUM    = "";
      infoSong.ARTIST   = "";
    //printInfo();
          actualFolder = random(1,lastFolder);
          infoFOLDER info = parseFolderName(actualFolder);
          //Serial.print("actualFolder=");       Serial.println(actualFolder);
          //Serial.print("rnd info.Path=");       Serial.println(info.Path);
          //Serial.print("rnd info.Title=");      Serial.println(info.Title);
          //Serial.print("rnd info.Ext=");        Serial.println(info.Extention);
          //Serial.print("rnd info.LastNumber="); Serial.println(info.LastNumber);
         
    int randomFolder = random(1,lastFolder);
    int randomNumber = random(1,info.LastNumber);
    String filename = DS+info.Path +DS+String(randomNumber) + "."+info.Extention;
    //log_i("actualFolder= %d, file=%s\n",actualFolder,filename.c_str());
    //int randomNumber = random(1,musicMax);
    //String filename = musicFolder + String(randomNumber) + musicExt;
    Serial.print("***.........rndMusic="); Serial.println(filename);
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
  Serial.println("\nRead from file:::::::::::::::::::::::: \n");
  while(file.available()){
    String list = file.readStringUntil('\n');
    zapis[c] = String(list);
    c++;
  }
  file.close();
  Serial.println("info.txt odczyt ======");
  for (int c=0; c<5; c++){
    Serial.print(c); Serial.print(" === ");Serial.println(zapis[c]);
  }
  //QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
  musicFolder = zapis[0];
  musicMax    = zapis[1].toInt();
  musicExt    = zapis[2];
}



infoFOLDER parseFolderName(int folderNR){
  String fname = nameFolders[folderNR];  
  int first = fname.indexOf("_");
  int last  = fname.lastIndexOf("_");
  int len   = fname.length();  
  String count = fname.substring(last+1 , len);
  int maxCount = count.toInt();
  infoFOLDER info;
  info.Path       = fname;
  info.Title     = fname.substring(0      , first);
  info.Extention  = fname.substring(first+1, last);
  info.LastNumber = maxCount;  
  return info;
}

/*dddddddddddddddddddddddddddddddddddddddddddddddddddddddd*/

int getDirectory(fs::FS &fs) {
  File root = fs.open("/"); 
  nameFolders.clear();
  allFolders.clear();
  while (true) {
    File entry =  root.openNextFile();
    if (! entry) {break;}    
    if (entry.isDirectory()){
      String nnn = entry.name();
      if (nnn.charAt(0) != '.') {nameFolders.push_back(entry.name());}
    }
    entry.close();
  }
  root.close();
  return nameFolders.size();
}

/*dddddddddddddddddddddddddddddddddddddddddddddddddddddddd*/

void setup(){
    Serial.begin(115200);
    
                Serial.println("Configuring WDT...");
                esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
                esp_task_wdt_add(NULL); //add current thread to WDT watch
                
  timeMillis = millis();
  pinMode(KEY3, INPUT_PULLUP);
  pinMode(KEY4, INPUT_PULLUP);
  pinMode(KEY5, INPUT_PULLUP);
  pinMode(KEY6, INPUT_PULLUP);
  

    Serial.println("\r\n");    Serial.println(CODEDIR);
    Serial.printf_P(PSTR("Free mem=%d\n"), ESP.getFreeHeap());
    readPreferences();

    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    
    SPI.setFrequency(1000000);
    
    SD.begin(SD_CS);
    delay(100);

    Serial.println("getDirectory*************************");
    countFolders = getDirectory(SD);
    lastFolder   = countFolders-1;
    actualFolder = 3;
    
    //infoFOLDER info;
    /*
    for(int i = 0; i < countFolders; i++){
      Serial.print(i);Serial.print(" .... "); Serial.println(nameFolders[i]);
      infoFOLDER info = parseFolderName(i);
      Serial.println(info.Path);
      Serial.println(info.Title);
      Serial.println(info.Extention);
      Serial.println(info.LastNumber);
    }*/
    
    
    //Serial.println("ddddddd*************************");
    readFile(SD, "/info.txt");
    printInfo();
    
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
    //audio.setTone(4,-9,-6);
    playRandomMusic();
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
    if (k3 == LOW) {
        //musicFolder = "/Modal_flac_220/";
        //musicMax    = 220;
        //musicExt    = ".flac";
        playRandomMusic();
    }
    if (k4 == LOW) {
        //musicFolder = "/Jazzy_flac_284/";
        //musicMax    = 284;
        //musicExt    = ".flac";
        playRandomMusic();
    }
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
        counterLoop++;
        if (counterLoop % 150 == 0){
          int nowTime = (millis() - timeMillis) / (60*1000);
          bool run = audio.isRunning();
          String title = nameFolders[actualFolder];
          Serial.printf("### run=%d nowTime=%d title=%s \n", run, nowTime, title.c_str());
          printInfoSong();
        }
        esp_task_wdt_reset();
        if (!audio.isRunning()){
          for (int i=0; i<3; i++) Serial.println("??????????????????????????????????????");
          playRandomMusic();
        }
    }
   SerialListener();
    
}
//----------------------------------------------------------------------------------------------------------------------
String rep(String tx){
  tx.replace(":","-");
  tx.replace("{","-");
  tx.replace("}","-");
  tx.replace("'","");
  tx.replace("\"","-");
  tx.replace("\/","%");
  return tx;
}

void notifyKeyVal(String key, String val){
  
    if (key=="error"){      
      log_i("error --------------\n");
      val.replace("FLAC decode error ","");
      String err = rep(val.c_str());
      log_i("\nERROR---------- = %s\n",err);
      playRandomMusic();    
    }
   if (key=="syncword_not_found"){      
      log_i("syncword_not_found --------------\n");  
      val.replace("syncword not found ","");
      String sync = rep(val.c_str());
      log_i("\nSYNC NOT---------- = %s\n",sync);
      playRandomMusic();    
    }
    if (key=="Reading_file"){
      //log_i("\nReading_file = %s\n",val);      
    }
    if (key=="StreamTitle"){
      val.replace("StreamTitle ","");
      String StreamTitle = rep(val.c_str());
      log_i("StreamTitle = %s\n",StreamTitle);      
   }
   if (key=="SampleRate")    {
      val.replace("SampleRate: ","");
      //log_i("SampleRate = %d\n",val.toInt());
      int bitRate = val.toInt();
      if (bitRate>0) Serial.printf("$_SampleRate= %d kHz\n", bitRate);
   }
   if (key=="BitRate")    {
      val.replace("BitRate: ","");
      int bitRate = val.toInt()/1000;
      Serial.printf("$_BitRate= %d kHz\n", bitRate);
      printInfoSong();
    }
   if (key=="audio_file_duration")    {
      val.replace("audio file duration: ","");
      //int sek = val.toInt();
      //int min = sek/60;
      //sek = sek - (min*60);
      String sTime = humanTime(val.toInt());
      infoSong.DURATION = val.toInt();
      Serial.printf("$_audioDuration = %s\n",sTime.c_str());
   }
    
}

// optional
void audio_info(const char *info){
    Serial.print("info        "); 
    Serial.println(info);
    if(strncmp(info, "StreamTitle"       , 7) ==0)  {notifyKeyVal("StreamTitle",String(info));}   
    if(strncmp(info, "SampleRate"        , 7) ==0)  {notifyKeyVal("SampleRate",String(info));}
    if(strncmp(info, "audio file duration"        , 5) ==0)  {notifyKeyVal("audio_file_duration",String(info));}
    if(strncmp(info, "BitRate",7) ==0)       {notifyKeyVal("BitRate",String(info));}
    if(strncmp(info, "syncword not found",12) ==0)  {notifyKeyVal("syncword_not_found",String(info));}
    if(strncmp(info, "FLAC decode error" ,15) ==0)  {notifyKeyVal("error",String(info));}
    if(strncmp(info, "Reading file"      ,10) ==0)  {
      //Serial.print(" r= ");Serial.println(rep(String(info)));
      String r = String(info);
      String rr = rep(r);
      Serial.print("Reading file= ");Serial.println(rr);
      //notifyKeyVal("Reading_file",String(rr));
    }
    
    //FLAC decode error -1 : BLOCKSIZE TOO BIG
}
void audio_id3data(const char *info){  //id3 metadata
    Serial.print("id3data     ");Serial.println(info);
    if(strncmp(info, "TITLE"       , 5) ==0){
        String val = String(info);
        val.replace("TITLE: ",""); 
        infoSong.TITLE    = val;
        Serial.println(infoSong.TITLE);
    }
    if(strncmp(info, "ARTIST"       , 5) ==0){
        String val = String(info);         
        val.replace("ARTIST: ","");
        infoSong.ARTIST    = val;
        Serial.println(infoSong.ARTIST);
    }
    if(strncmp(info, "ALBUM"       , 5) ==0){
        String val = String(info);         
        val.replace("ALBUM: ","");
        infoSong.ALBUM    = val;
        Serial.println(infoSong.ALBUM);
    }

   
}
void audio_eof_mp3(const char *info){  //end of file
    Serial.print("eof_mp3     ");Serial.println(info);
    playRandomMusic();    
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



void SerialListener(){
    if (Serial.available() > 0) {        
        String r=Serial.readString(); r.trim();
        log_e("Serial.read=%s", r);
        if (r == "-9") {ESP.restart();}
        if (r == "0") {actualFolder=0;playRandomMusic();}
        if (r == "1") {actualFolder=1;playRandomMusic();}
        if (r == "2") {actualFolder=2;playRandomMusic();}
        if (r == "4") {actualFolder=4;playRandomMusic();}
        if (r == "5") {actualFolder=5;playRandomMusic();}
    }
}  


  
/*
Szkic używa 891665 bajtów (68%) pamięci programu. Maksimum to 1310720 bajtów.
Zmienne globalne używają 31192 bajtów (9%) pamięci dynamicznej, pozostawiając 296488 bajtów dla zmiennych lokalnych. Maksimum to 327680 bajtów.

Szkic używa 892001 bajtów (68%) pamięci programu. Maksimum to 1310720 bajtów.
Zmienne globalne używają 31224 bajtów (9%) pamięci dynamicznej, pozostawiając 296456 bajtów dla zmiennych lokalnych. Maksimum to 327680 bajtów.

Szkic używa 900913 bajtów (68%) pamięci programu. Maksimum to 1310720 bajtów.
Zmienne globalne używają 31272 bajtów (9%) pamięci dynamicznej, pozostawiając 296408 bajtów dla zmiennych lokalnych. Maksimum to 327680 bajtów.



*/
