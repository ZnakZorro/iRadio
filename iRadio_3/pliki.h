// Digital I/O used for SD
/*#define SS   13 // SD_CS SD chip select
#define SCK  14
#define MOSI 15
#define MISO 2*/

int lastFileNr = 0;
int countFile  = 0;


void getFileCount(fs::FS &fs, const char * dirname, uint8_t levels){
    File root = fs.open(dirname);
    if(!root)              { Serial.println("Failed to open directory");}
    if(!root.isDirectory()){ Serial.println("Not a directory");         }
    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            if(levels){getFileCount(fs, file.name(), levels -1);}
        } else {
          countFile++;
          Serial.println(countFile);
          }
        file = root.openNextFile();
    }
    Serial.print("countFile X=");Serial.println(countFile);
}



String listDir(fs::FS &fs, const char * dirname, uint8_t levels, int ktory){
  int numer = 0;
  if (ktory>countFile) ktory = 0;
   // Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root)              { Serial.println("Failed to open directory"); return " ";}
    if(!root.isDirectory()){ Serial.println("Not a directory");          return " ";}

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1, ktory);
            }
        } else {
            //Serial.print("  FILE: ");
            //Serial.print(numer);Serial.print(" / ");Serial.print(ktory);Serial.print(" = ");
            //Serial.print(file.name());
            //Serial.print("  SIZE: "); Serial.println(file.size());
            if (ktory > -1 && numer == ktory) return file.name();
            numer++;
        }
        file = root.openNextFile();
    }
    Serial.println("%%%%%%%%%%%%%%%%%%%%");
    Serial.println(numer);
     return " ";
    /* 
    if (ktory<0) {
        countFile = numer;
        Serial.println(countFile);
        //return String(countFile);
       
    }*/
}
