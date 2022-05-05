//max_stations= sizeof(stations)/sizeof(stations[0]); 

#define LAST_STATION    4
int max_stations  = LAST_STATION+1; 


struct stacjeSTRUCT  {
  int  const  ampli;  
  char const *info;
  char const *stream;
};

    stacjeSTRUCT radia [LAST_STATION+1]={
      //{0, "TOK-FM",    "http://pl-play.adtonos.com/tok-fm"},
      {0, "TOK-FM",    "http://pl-play.adtonos.com/tok-fm"},
      {0, "RNS",       "http://stream.rcs.revma.com/ypqt40u0x1zuv"},
      {0, "357",       "http://stream.rcs.revma.com/ye5kghkgcm0uv"},
      {2, "Jazz",      "http://stream.srg-ssr.ch/m/rsj/aacp_96"},
      {2, "Classic",   "http://stream.srg-ssr.ch/m/rsc_de/aacp_96"}
    }; 

struct sQQQ  {
  int8_t l;  
  int8_t m;
  int8_t h;
};


sQQQ qqq[5]={
    {-3, 0, -3},
    { 3, 0, 3},
    { 6, -6, 6},
    { 6, -9, 9},
    { 0, 0, 0},
};

/*
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
   Serial.printf("Listing directory: %s\r\n", dirname);

   File root = fs.open(dirname);
   if(!root){
      Serial.println("− failed to open directory");
      return;
   }
   if(!root.isDirectory()){
      Serial.println(" − not a directory");
      return;
   }

   File file = root.openNextFile();
   while(file){
      if(file.isDirectory()){
         Serial.print("  DIR : ");
         Serial.println(file.name());
         if(levels){
            listDir(fs, file.name(), levels -1);
         }
      } else {
         Serial.print("  FILE: ");
         Serial.print(file.name());
         Serial.print("\tSIZE: ");
         Serial.println(file.size());
      }
      file = root.openNextFile();
   }
}
*/


void listFileFromSPIFFS(){
    unsigned int totalBytes = SPIFFS.totalBytes();
    unsigned int usedBytes = SPIFFS.usedBytes();
    Serial.println("\n===== File system info =====");
    Serial.print("Total space: "); Serial.print(totalBytes); Serial.println(" byte");
    Serial.print("Total used:  "); Serial.print(usedBytes); Serial.println(" byte");
    Serial.println();
  File root = SPIFFS.open("/");  
  File file = root.openNextFile();
  while(file){
      Serial.print("FILE: "); Serial.print(file.name());
      Serial.print("\t\t");   Serial.println(file.size(), DEC);      
      file = root.openNextFile();
  }
}

char * folders[]  =  {"/Ballads","/Vocal","/Bladerunner"};
int    maxFiles[] =  {422,      500,    12};



void opiszDefines(){
   Serial.println("===============");
   Serial.println("/*");
        Serial.print("#define SS   ");      Serial.println(SS);
        Serial.print("#define SCK  ");      Serial.println(SCK);
        Serial.print("#define MOSI ");      Serial.println(MOSI);
        Serial.print("#define MISO ");      Serial.println(MISO);
        Serial.println("\n//I2S");
         //Serial.print("#define ADC1_CHANNEL_4 ");      Serial.println(ADC1_CHANNEL_4);
       
        //Serial.print("#define  I2S_NUM_0");      Serial.println(I2S_NUM_0);
        Serial.print("#define SDA ");      Serial.println(SDA);
        Serial.print("#define SCL ");      Serial.println(SCL);
        Serial.print("#define DAC1 ");      Serial.println(DAC1);
        Serial.print("#define DAC2 ");      Serial.println(DAC2);
        Serial.print("#define A0 ");      Serial.println(A0);
        Serial.print("#define A3 ");      Serial.println(A3);
        Serial.print("#define A4 ");      Serial.println(A4);
 
        /*Serial.print("#define I2S_DOUT ");      Serial.println(I2S_DOUT);
        Serial.print("#define I2S_BCLK ");      Serial.println(I2S_BCLK);
        Serial.print("#define I2S_LRC ");       Serial.println(I2S_LRC);
        Serial.print("#define DOUT ");      Serial.println(DOUT);
        Serial.print("#define BCLK ");      Serial.println(BCLK);
        Serial.print("#define LRC ");       Serial.println(LRC);*/


   Serial.println("*/");
   Serial.println("===============");
}
