//Temp sensor
//default i2c
// -SDA = D2 
// -SCL = D1

#include <Wire.h>
#include "SHT20.h"
SHT20    sht20;
//Temp sensor


//telegram
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
char ssid[] = "AndroidAP";     // your network SSID (name)
char password[] = "12345777"; // your network key
#define BOTtoken "526357899:AAHBHTnCqcXLjfnrY9ITOVGEMdWdRgBmdW4"  // your Bot Token (Get from Botfather)
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done
bool Start = false;
//telegram

//CO2 
#include "MQ135.h"
MQ135 gasSensor = MQ135(A0);
#define RZERO 900

//CO2 

//PMS3003
//  PMS3003     Arduino(UNO)
//  PIN1        5V
//  PIN2        GND
//  PIN4 TX     Pin 5 //used 3.3v logic
//  PIN5 RX     Pin 4 //not used 3.3v logic

#define PMS3003_RX_PIN 0
#define PMS3003_TX_PIN 2
#include <SoftwareSerial.h>
SoftwareSerial PMS3003Serial(PMS3003_RX_PIN, PMS3003_TX_PIN);
int pmcf10 = 0; // PM1.0  In the Atmosphere
int pmcf25 = 0; // PM2.5  In the Atmosphere
int pmcf100 = 0; // PM10.0 In the Atmosphere
int pmat10 = 0; // PM1.0  Standard Particles
int pmat25 = 0; // PM2.5  Standard Particles
int pmat100 = 0; // PM10.0 Standard Particles
#define PMS3003_DATALEN 32
uint8_t PMS3003_buf[PMS3003_DATALEN];
#define FIRST_LOOP_DELAY 2000  // delay seconds to wait for PMS3003 sensor on first-loop.
boolean firstLoop = true;
//PMS3003

//LED
#define Red_LED 12
#define Blue_LED 13
#define Green_LED 14
//LED

int ChangeTrackingOld = 0;
int ChangeTrackingNew = 0;

String AutoSend;

String SensorData;

//String my_chat_id ="292126439";

/////////////////////////////void setup////////////////////////////////void setup
void setup() {
// SHT20
sht20.initSHT20();
delay(100);
sht20.checkSHT20();
// SHT20

//LED
pinMode(Red_LED, OUTPUT);
pinMode(Blue_LED, OUTPUT);
pinMode(Green_LED, OUTPUT);

LED ("Red");
delay(500);
LED ("Blue");
delay(500);  
LED ("Green");
delay(500);  

digitalWrite(Red_LED, LOW); //led off
digitalWrite(Blue_LED, LOW);  //led off
digitalWrite(Green_LED, LOW);  //led off
//LED

  Serial.begin(115200);
  
//telegram
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
  Serial.print(".");
  delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
//telegram

//PMS3003
PMS3003Serial.begin(9600); // initialize PMS3003
firstLoop = true;
//PMS3003
}

/////////////////////////////void loop////////////////////////////////void loop
void loop() 
{

float humd = sht20.readHumidity();
float temp = sht20.readTemperature();
int valueH = humd  + 0.5;
int valueT = temp  + 0.5;
  
//CO2 
float ppm = gasSensor.getPPM();
delay(500);  
//CO2 

//PMS3003
readPMS3003();
SensorData = "Dust sensor\n" "PM1 - " + String(pmcf10)   +  " mkg/m3 \n" + "PM2.5 - " + String(pmcf25) +  " mkg/m3 \n" + "PM10 - " + String(pmcf100);

SensorData = SensorData + " mkg/m3 \n PM2.5 ";
  if ( pmcf25 >= 0 && pmcf25 < 50) {    SensorData = SensorData + "Good";  }
  if ( pmcf25 >= 50 && pmcf25 < 100) {    SensorData = SensorData + "Moderate";  }
  if ( pmcf25 >= 100 && pmcf25 < 150) {    SensorData = SensorData + "Unhealthy for Sensitive Groups";  }
  if ( pmcf25 >= 150 && pmcf25 < 200) {    SensorData = SensorData + "Unhealthy";  }
  if ( pmcf25 >= 200 && pmcf25 < 300) {    SensorData = SensorData + "Very Unhealthy";  }
  if ( pmcf25 >= 300) {    SensorData = SensorData + "Hazardous";  }

  SensorData = SensorData + "\n PM10 ";
  if ( pmcf100 >= 0 && pmcf100 < 54) {    SensorData = SensorData + "Good";  }
  if ( pmcf100 >= 54 && pmcf100 < 154) {    SensorData = SensorData + "Moderate";  }
  if ( pmcf100 >= 154 && pmcf100 < 254) {    SensorData = SensorData + "Unhealthy for Sensitive Groups";  }
  if ( pmcf100 >= 254 && pmcf100 < 354) {    SensorData = SensorData + "Unhealthy";  }
  if ( pmcf100 >= 354 && pmcf100 < 424) {    SensorData = SensorData + "Very Unhealthy";  }
  if ( pmcf100 >= 424) {    SensorData = SensorData + "Hazardous";  }

SensorData = SensorData + "\n CO2 " + ppm;
  if( ppm >= 0 && ppm < 0.25) { SensorData = SensorData + " Normal";}
  if( ppm >= 0.26 && ppm < 1) { SensorData = SensorData + " Attention";}
  if( ppm >= 1 ){ SensorData = SensorData + " Hazard";};
SensorData = SensorData + "\n \n Temperature " + valueT  + "C \n Humidity " + valueH  + "%";

 delay(1000); //READING_SENSOR_INTERVAL
 firstLoop = false;
//PMS3003

//limit checking
//Normal
  if (( pmcf25 >= 0 && pmcf25 < 50)    || ( pmcf100 >= 0 && pmcf100 < 54)    || ( ppm >= 0 && ppm < 0.45) )  //Normal
  {
    LED ("Green");
    ChangeTrackingNew = 1;
    AutoSend = "It back to normal \n \n" + SensorData;
  } 
  
//Attention
  if (( pmcf25 >= 51 && pmcf25 < 150)  || ( pmcf100 >= 55 && pmcf100 < 254)  || ( ppm >= 0.45 && ppm < 1.5) ) //Attention
  {
    LED ("Blue");
    ChangeTrackingNew = 2;
    AutoSend = "Attention \n \n" + SensorData;
   }
   
//Hazard
  if (( pmcf25 >= 151) || ( pmcf100 >= 255 ) || ( ppm >= 1 )) //Hazard
  {
    LED ("Red");
    ChangeTrackingNew = 3;
    AutoSend = "Hazard \n \n" + SensorData;
   }
//limit checking

//change checking
if (ChangeTrackingNew != ChangeTrackingOld)
{
  Serial.println("Auto message");
  String chat_id = String(bot.messages[0].chat_id);
  bot.sendMessage(chat_id, AutoSend);
}
ChangeTrackingOld = ChangeTrackingNew;
//change checking

//telegram
    if (millis() > Bot_lasttime + Bot_mtbs)  {
      int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  
      while(numNewMessages) {
        Serial.println("got request for data");
        handleNewMessages(numNewMessages);
        numNewMessages = bot.getUpdates(bot.last_message_received + 1);
      }
      Bot_lasttime = millis();
    }
//telegram

}

/////////////////////////////Send Message////////////////////////////////Send Message
void handleNewMessages(int numNewMessages) 
{
  Serial.println("Making data response");

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    if (text == "/sensor") {
      bot.sendMessage(chat_id, SensorData);
    }

    if (text == "/start") {
      bot.sendMessage(chat_id,  "Hi I'm EcoAir \n Stay with me if you want to know about dust and CO2 concentration in my location \n Use /sensor \n for check it manually or keep calm and wait for automatic updates");       
    }
  }
}

////////////////////Read value from PMS3003////////////////////Read value from PMS3003
void readPMS3003() {
  pmcf10 = 0;
  pmcf25 = 0;
  pmcf100 = 0;
  pmat10 = 0;
  pmat25 = 0;
  pmat100 = 0;

  if (firstLoop) {
    delay(FIRST_LOOP_DELAY);
  }
  memset(PMS3003_buf, 0, PMS3003_DATALEN);
  int idx = 0;
  const int MAX_INDEX = 23;
  while (PMS3003Serial.available() && idx <= MAX_INDEX)
  {
    PMS3003_buf[idx++] = PMS3003Serial.read();
  }
  while (PMS3003Serial.available()) PMS3003Serial.read(); // Clear all the buffer

  if (PMS3003_buf[0] == 0x42 && PMS3003_buf[1] == 0x4d)
  {

    int checksum =   (PMS3003_buf[22] << 8) + PMS3003_buf[MAX_INDEX];
    int checkvalue = (PMS3003_buf[0] + PMS3003_buf[1] + PMS3003_buf[2] + PMS3003_buf[3] + PMS3003_buf[4] + PMS3003_buf[5] + PMS3003_buf[6] + PMS3003_buf[7] + PMS3003_buf[8] + PMS3003_buf[9] + PMS3003_buf[10] +
                      PMS3003_buf[11] + PMS3003_buf[12] + PMS3003_buf[13] + PMS3003_buf[14] + PMS3003_buf[15] + PMS3003_buf[16] + PMS3003_buf[17] + PMS3003_buf[18] + PMS3003_buf[19] + PMS3003_buf[20] + PMS3003_buf[21]);

    if ( checksum == checkvalue ) {
      pmat25  = ( PMS3003_buf[12] << 8 ) | PMS3003_buf[13];
      pmat10  = ( PMS3003_buf[10] << 8 ) | PMS3003_buf[11];
      pmat100 = ( PMS3003_buf[14] << 8 ) | PMS3003_buf[15];

      pmcf25  = ( PMS3003_buf[6] << 8 ) | PMS3003_buf[7];
      pmcf10  = ( PMS3003_buf[4] << 8 ) | PMS3003_buf[5];
      pmcf100 = ( PMS3003_buf[8] << 8 ) | PMS3003_buf[9];
    }
  }
}
////////////////////Read value from PMS3003////////////////////Read value from PMS3003

///////////////////////led////////////////////////////////////////////////////////
void LED(String ledname) 
{
      if (ledname == "Red")
    {digitalWrite(Red_LED, HIGH);
    digitalWrite(Blue_LED, LOW);
    digitalWrite(Green_LED, LOW);
    }
        
        if (ledname == "Green")
    {
    digitalWrite(Red_LED, LOW);
    digitalWrite(Blue_LED, LOW);
    digitalWrite(Green_LED, HIGH);
    }
    
      if (ledname == "Blue")
    {
    digitalWrite(Red_LED, LOW);
    digitalWrite(Blue_LED, HIGH);
    digitalWrite(Green_LED, LOW);
    }
}
///////////////////////led////////////////////////////////////////////////////////
