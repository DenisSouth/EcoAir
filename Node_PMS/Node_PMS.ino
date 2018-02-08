#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
//const char* ssid = "MYFABLAB";
//const char* password = "fab17chile";

const char* ssid = "AndroidAP";
const char* password = "12345777";
WiFiServer server(80);

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

#define PMS3003_DATALEN 32
uint8_t PMS3003_buf[PMS3003_DATALEN];


#define FIRST_LOOP_DELAY 2000  // delay seconds to wait for PMS3003 sensor on first-loop.
boolean firstLoop = true;



//const int AnalogIn  = A0;
//int readingIn = 0;

#include "MQ135.h"
MQ135 gasSensor = MQ135(A0);
#define RZERO 76.63
//https://hackaday.io/project/3475-sniffing-trinket/log/12363-mq135-arduino-library

void setup() {
  Serial.begin(115200);
  initPMS3003(); // initialize PMS3003
  firstLoop = true;
  delay(10);
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {

  readPMS3003();

  String out = "PM1     " + String(pmcf10)   +  " <br> " + "PM2.5   " + String(pmcf25) +  " <br> " + "PM10    " + String(pmcf100);

  out = out + "  <br> PM2.5 ";
  if ( pmcf25 >= 0 && pmcf25 < 50) {
    out = out + "Good";

  }
  if ( pmcf25 >= 51 && pmcf25 < 100) {

    out = out + "Moderate";
  }
  if ( pmcf25 >= 101 && pmcf25 < 150) {

    out = out + "Unhealthy for Sensitive Groups";
  }
  if ( pmcf25 >= 151 && pmcf25 < 200) {
    out = out + "Unhealthy";

  }
  if ( pmcf25 >= 201 && pmcf25 < 300) {
    out = out + "Very Unhealthy";

  }
  if ( pmcf25 >= 301) {

    out = out + "Hazardous";
  }

  //////////////////////////////////////////


  out = out + " <br> PM10 ";
  if ( pmcf100 >= 0 && pmcf100 < 54) {
    out = out + "Good";

  }
  if ( pmcf100 >= 55 && pmcf100 < 154) {
    out = out + "Moderate";

  }
  if ( pmcf100 >= 155 && pmcf100 < 254) {
    out = out + "Unhealthy for Sensitive Groups";

  }
  if ( pmcf100 >= 255 && pmcf100 < 354) {
    out = out + "Unhealthy";

  }
  if ( pmcf100 >= 355 && pmcf100 < 424) {
    out = out + "Very Unhealthy";

  }
  if ( pmcf100 >= 425) {
    out = out + "Hazardous";

  }

  out = out + " <br>CO2 " +  gasSensor.getPPM();



  delay(READING_SENSOR_INTERVAL);
  firstLoop = false;


  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();





  if (req.indexOf("/pms") != -1) {
    client.flush();

    // Prepare the response
    String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n   Air Quality now is <br>";
    s += out;
    s += "</html>\n";

    // Send the response to the client
    client.print(s);
    delay(1);
    Serial.println("Client disonnected");
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
