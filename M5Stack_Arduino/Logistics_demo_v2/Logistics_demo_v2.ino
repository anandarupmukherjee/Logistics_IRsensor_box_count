#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino_JSON.h>
#include <M5Stack.h>
#include "SimpleBeep.h"



// Replace the next variables with your SSID/Password combination
const char* ssid = "UniOfCam-IoT";
const char* password =  "iH37Esye";


// Add your MQTT Broker IP address, example:
const char* mqtt_server = "10.252.21.118";
char out[256];

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 0;
unsigned long epochTime; 


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
int sensor1 = 2;
int sensor2 = 5;
IPAddress ip; 



void printLocalTime()
{    M5.Lcd.setTextSize(2);
     M5.Lcd.setCursor(0,0);
     M5.Lcd.setTextColor(WHITE, BLACK);
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    M5.Lcd.println("Failed to obtain time");
    return;
  }
  //M5.Lcd.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  M5.Lcd.println(&timeinfo);
}


unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}



void setup() {
  M5.begin();
  M5.Lcd.setTextColor(TFT_GREEN,TFT_BLACK);  
  M5.Lcd.setTextSize(2);
  sb.init(); 
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);  
  setup_wifi();
  configTime(0,0, ntpServer);
  printLocalTime();
  client.setServer(mqtt_server, 1883);
}


void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0,0);
  M5.Lcd.setTextColor(GREEN , BLACK);
  //Serial.println();
  M5.Lcd.print("Connecting to ");
  M5.Lcd.print(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.print(".");
  }

  //Serial.println("");
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0,42);
  M5.Lcd.setTextColor(BLUE , BLACK);
  M5.Lcd.print("WiFi connected");
  M5.Lcd.print("IP address: ");
  M5.Lcd.print(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0,84);
  M5.Lcd.setTextColor(RED, BLACK);
  while (!client.connected()) {
    M5.Lcd.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Anand_M5_Client")) {
      M5.Lcd.print("connected");
      // Subscribe
      //client.subscribe("esp32/output");
    } 
    else {
      M5.Lcd.setTextSize(2);
      M5.Lcd.setCursor(0,84);
      M5.Lcd.print("failed, rc=");
      M5.Lcd.print(client.state());
      M5.Lcd.print(" try again in 5 seconds");
      //sb.beep(5,A4,100);  // beep (volume 5, pitch A, duration 100ms)
      delay(5000);
    }
  }
}






void loop() {
  if (!client.connected()) {
    
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;

   struct tm timeinfo;
   epochTime = getTime();
   //printLocalTime();
   
   
   String line;
   int in_sen = !digitalRead(sensor1);
   int out_sen = !digitalRead(sensor2);
   line = String(in_sen+out_sen);
   if (in_sen+out_sen == 2){
    sb.beep(5,C4,100);
    delay(100);
    sb.beep(5,A4,100);
   }
   ip = WiFi.localIP();

//    String doc ="{Location: Line-1, IP:"+ip.toString() +","+line+"}";
    String doc ="{Location: Lightstalk, IP:"+ip.toString() +", Status{ I:"+String(in_sen)+", O:"+String(out_sen)+", D:"+String(line)+", tInS:"+String(epochTime)+"}}";
    doc.toCharArray(out, doc.length()+1);
//    serializeJson(doc,out);

 
     M5.Lcd.fillScreen(BLACK);
     M5.Lcd.setTextSize(2);

     
     M5.Lcd.setCursor(0,42);
     M5.Lcd.setTextColor(RED, BLACK);
     M5.Lcd.println("Connected....");
     M5.Lcd.println(WiFi.localIP());

     M5.Lcd.setTextSize(4);
     M5.Lcd.setCursor(0,84);
     M5.Lcd.setTextColor(WHITE, BLACK);
     M5.Lcd.println("ONLINE");
    
     M5.Lcd.setTextSize(2);
     M5.Lcd.setCursor(0,130);
     M5.Lcd.setTextColor(GREEN, BLACK);
     M5.Lcd.println(out);
     client.publish("log/belt1/status",out);

     M5.Lcd.setTextSize(2);
     M5.Lcd.setCursor(0,220);
     M5.Lcd.setTextColor(YELLOW, BLACK);
     M5.Lcd.println("ALL OK");

    printLocalTime();
  }

  
}
