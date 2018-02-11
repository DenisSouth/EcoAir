#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

#define READING_SENSOR_INTERVAL 5000  // Interval to read  sensors. 
#define PMS3003_RX_PIN 4
#define PMS3003_TX_PIN 5

SoftwareSerial PMS3003Serial(PMS3003_RX_PIN, PMS3003_TX_PIN);

int pmcf10 = 0; // PM1.0  In the Atmosphere
int pmcf25 = 0; // PM2.5  In the Atmosphere
int pmcf100 = 0; // PM10.0 In the Atmosphere
int pmat10 = 0; // PM1.0  Standard Particles
int pmat25 = 0; // PM2.5  Standard Particles
int pmat100 = 0; // PM10.0 Standard Particles
String out ;

int alarmold = 0;
int alarmnew = 0;
#define PMS3003_DATALEN 32
uint8_t PMS3003_buf[PMS3003_DATALEN];

#define FIRST_LOOP_DELAY 2000  // delay seconds to wait for PMS3003 sensor on first-loop.
boolean firstLoop = true;

#include "MQ135.h"
MQ135 gasSensor = MQ135(A0);
#define RZERO 76.63
//https://hackaday.io/project/3475-sniffing-trinket/log/12363-mq135-arduino-library

//bot
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// Initialize Wifi connection to the router
char ssid[] = "AndroidAP";     // your network SSID (name)
char password[] = "12345777"; // your network key

// Initialize Telegram BOT
#define BOTtoken "469253688:AAFg4EKAG0QXD5TL574_UqMZRQWa7wjL43c"  // your Bot Token (Get from Botfather)

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done
bool Start = false;

void alarmMSG(int i) {

  Serial.println("alarmMSGstart");
  
    String chat_id = String(bot.messages[0].chat_id);
    String event = "";

    if (i==1){event = "ALARM";}
    if (i==0) {event = "Back to normal";}
 Serial.println(event);
    bot.sendMessage(chat_id, event + "\n\n" + out);
     Serial.println("chat_id " + chat_id + "sent change");
}





////////////////////////////////////////////////////
void handleNewMessages(int numNewMessages) {
 // Serial.println("handleNewMessages");
 // Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    if (text == "/send_test_action") {
      bot.sendChatAction(chat_id, "typing");
      delay(4000);
      bot.sendMessage(chat_id, "Did you see the action message? " + out  );

      // You can't use own message, just choose from one of bellow

      //typing for text messages
      //upload_photo for photos
      //record_video or upload_video for videos
      //record_audio or upload_audio for audio files
      //upload_document for general files
      //find_location for location data

      //more info here - https://core.telegram.org/bots/api#sendchataction
    }

    if (text == "/start") {
      String welcome = "Welcome to Universal Arduino Telegram Bot library, " + from_name + ".\n";
      welcome += "This is Chat Action Bot example.\n\n";
      welcome += "/send_test_action : to send test chat action message\n";
      bot.sendMessage(chat_id, welcome);
    }

    if (text == "/sensor") {
      bot.sendMessage(chat_id, out);
    }

    if (text == "/1") {
      bot.sendMessage(chat_id, out);
    }


  }
}

//botend
void setup() {
  Serial.begin(115200);
  initPMS3003(); // initialize PMS3003
  firstLoop = true;
  delay(10);


  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // attempt to connect to Wifi network:
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




 delay(1000);
 alarmMSG(0);
}

void loop() {
  
  readPMS3003();
  int gas =  gasSensor.getPPM();
  out = "Dust sensor\n" "PM1 - " + String(pmcf10)   +  " mkg/m3 \n" + "PM2.5 - " + String(pmcf25) +  " mkg/m3 \n" + "PM10 - " + String(pmcf100);

  out = out + " mkg/m3 \n PM2.5 ";
  if ( pmcf25 >= 0 && pmcf25 < 50) {    out = out + "Good";  }
  if ( pmcf25 >= 51 && pmcf25 < 100) {    out = out + "Moderate";  }
  if ( pmcf25 >= 101 && pmcf25 < 150) {    out = out + "Unhealthy for Sensitive Groups";  }
  if ( pmcf25 >= 151 && pmcf25 < 200) {    out = out + "Unhealthy";  }
  if ( pmcf25 >= 201 && pmcf25 < 300) {    out = out + "Very Unhealthy";  }
  if ( pmcf25 >= 301) {    out = out + "Hazardous";  }

  out = out + "\n PM10 ";
  if ( pmcf100 >= 0 && pmcf100 < 54) {    out = out + "Good";  }
  if ( pmcf100 >= 55 && pmcf100 < 154) {    out = out + "Moderate";  }
  if ( pmcf100 >= 155 && pmcf100 < 254) {    out = out + "Unhealthy for Sensitive Groups";  }
  if ( pmcf100 >= 255 && pmcf100 < 354) {    out = out + "Unhealthy";  }
  if ( pmcf100 >= 355 && pmcf100 < 424) {    out = out + "Very Unhealthy";  }
  if ( pmcf100 >= 425) {    out = out + "Hazardous";  }

  out = out + "\n CO2 in air " +  gas;
{
  
  delay(READING_SENSOR_INTERVAL);}
  




  if (pmcf100 >= 50  || pmcf25 >= 50 || gas  >= 0.1 ){alarmnew = 1;}
  else {alarmnew = 0;}
Serial.println("check change");
Serial.println(alarmnew);
Serial.println(alarmold);

if ( alarmold != alarmnew)
  { 
    if (alarmnew == 1){alarmMSG(1);
    Serial.println("change send 1 ");}
    else {alarmMSG(0);
    Serial.println("change send 0");}
  }
  else{Serial.println("change no");}


alarmold = alarmnew;
  
  firstLoop = false;

  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got request");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    Bot_lasttime = millis();
  }
}

// Initialize the PMS3003 sensor
void initPMS3003() {
  PMS3003Serial.begin(9600);
}

// Read value from PMS3003
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
