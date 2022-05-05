//#include <Preferences.h>
//Preferences preferences;
//const char* preferencesName = "eink2022";


#include "credentials.h"
#include "str.h"
#ifdef EINK
  void piszEpapPogda(String oneLine);
  void serverPOGODA();
#endif

void TimersSetup();
String getRealTime();
long getTimeStamp();
//void saveONEpref(String key, String val);
//void savePreferences();
//void readPreferences();

static void connectWIFI_ini();
static void connectWIFI_start();
void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
void Wifi_connected(WiFiEvent_t event, WiFiEventInfo_t info);
void Wifi_disconnected(WiFiEvent_t event, WiFiEventInfo_t info);
void installServer();
String wifiScan();

/****Function declaration EOF*******/

//void OLEDopisz(const char *info);
//void OLEDlinie();
//void setExternInfo(char const *strCHAR);
//void setExternIP(char const *strCHAR);



void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println((info.got_ip.ip_info.ip.addr));
  Serial.println("\n___#27 nnet WiFi connected"); 
  Serial.print("###10 IP address: ");Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
        #ifdef EINK
            piszEpapPogda(ip1+"\n"+ip2+"\n"+ip3+"\n");
            delay(3000);          
            serverPOGODA();           
        #endif
        TimersSetup();
        installServer();
}

void Wifi_connected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("\n___Successfully connected to Access Point");
}

void Wifi_disconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("\n___Disconnected from WIFI access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.disconnected.reason);
  
  //WiFi.begin(ssid, password);
  if (info.disconnected.reason != 8){
      Serial.println("Reconnecting.................");
      delay(5000);
      connectWIFI_start();
  }
}

static void connectWIFI_ini(){
      //WiFi.mode(WIFI_OFF);
    //WiFi.disconnect();
    
  WiFi.onEvent(WiFiGotIP,         WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);
  WiFi.onEvent(Wifi_connected,    WiFiEvent_t::SYSTEM_EVENT_STA_CONNECTED);
  WiFi.onEvent(Wifi_disconnected, WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED); 

  
  WiFiEventId_t eventID = WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info){
    Serial.print("\n### nnet \n#62 WiFi lost connection. Reason: ");
    Serial.println(info.disconnected.reason);
    //connectWIFI_start();
  }, WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);
}

static void connectWIFI_start(){

    int licznik=0;
    //Serial.println(ESP.getFreeHeap());
    Serial.printf_P(PSTR("Free mem=%d\n"), ESP.getFreeHeap());


    WiFi.mode(WIFI_STA);
          WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
          WiFi.setHostname(HOST_NAME);
    //WiFi.begin(ssid, pass);
    
           preferences.begin(preferencesName, false);
                  String ssid1 = preferences.getString("ssid1", "");
                  String pass1 = preferences.getString("pass1", "");
                  String ssid2 = preferences.getString("ssid2", "");
                  String pass2 = preferences.getString("pass2", "");
                  Serial.printf("%s, %s; %s, $s\n",ssid1, pass1,ssid2, pass2);
           preferences.end();  
             
      if (ssid1 !="") wifiMulti.addAP(ssid1.c_str(), pass1.c_str());
      if (ssid2 !="") wifiMulti.addAP(ssid2.c_str(), pass2.c_str());
      for (int i = 0; i < 8; i++) {
         wifiMulti.addAP(credential[i].ssid, credential[i].pass);
      } 
    //lcd.setCursor(0, 0);
    //lcd.printf("WiFi Start ");      
    while(wifiMulti.run() != WL_CONNECTED) {
      Serial.print("*");
      Serial.println("WiFi::"+String(licznik));
      delay(333);
      if (licznik%50==49) ESP.restart();
      //lcd.print("*");
      licznik++;
      if (licznik>200){
        break;
      }
    } 
    //Serial.printf_P(PSTR("Connected\r\nRSSI: "));
    //Serial.print("**WiFi.status="); Serial.println(WiFi.status());
    //Serial.print("**WiFi.RSSI=");   Serial.println(WiFi.RSSI());
    //Serial.print("**IP: ");         Serial.println(WiFi.localIP());
    //Serial.print("**SSID: ");       Serial.println(WiFi.SSID());
    //String ip4 = WiFi.localIP().toString();
    //setExternInfo(ip4.c_str());
    //OLEDstring(" ");
    //OLEDopisz(ip4.c_str());
}


String wifiScan(){
  // WiFi.scanNetworks will return the number of networks found
  //WiFi.mode(WIFI_STA);
  //WiFi.disconnect();
    
  int n = WiFi.scanNetworks();
  //Serial.println(n);
  //Serial.println("scan start");
  String out="";
  if (n == 0) {
      Serial.println("no networks found");
  } else {
    //Serial.print(n);
    //Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      //Serial.print(i + 1);
      //Serial.print(": ");
      //Serial.print(WiFi.SSID(i));
      //Serial.print(" (");
      //Serial.print(WiFi.RSSI(i));
      //Serial.print(")");
      //Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
      delay(10);
      out += String(WiFi.SSID(i));
      out += "\n";
    }
  }
  
  //Serial.println("");
   //Serial.println("scan done");

  return out;
}
