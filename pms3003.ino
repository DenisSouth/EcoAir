//Code for Arduino Uno and Air Quality Sensor Plantower PMS3003 
//  PMS3003     Arduino(UNO)
//  PIN1        5V
//  PIN2        GND
//  PIN4        TX Pin 5
//  PIN5        RX Pin 4

#define SERIAL_BAUD_RATE 9600
#define READING_SENSOR_INTERVAL 5000  // Interval to read  sensors. 
#define PMS3003_RX_PIN 4
#define PMS3003_TX_PIN 5

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

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  initPMS3003(); // initialize PMS3003
  firstLoop = true;
}

void loop() {
  readPMS3003();
  /*  Serial.println("Atm PM1   " + String(pmcf10) +  " mkg/m3");
    Serial.println("Atm PM2.5 " + String(pmcf25) +  " mkg/m3" );
    Serial.println("Atm PM10  " + String(pmcf100)+  " mkg/m3" );
    Serial.println("Part PM1  " + String(pmat10)+  " mkg/m3" );
    Serial.println("Part PM2  " + String(pmat25) +  " mkg/m3");
    Serial.println("Part PM10 " + String(pmat100) +  " mkg/m3");
     Serial.println(""  );*/

  Serial.println("PM1     " + String(pmcf10) +  "  " + String(pmat10));
  Serial.println("PM2.5   " + String(pmcf25) +  "  " + String(pmat25));
  Serial.println("PM10    " + String(pmat100) +  "  " + String(pmat100));


  Serial.print("2.5 "  );

  if ( pmcf25 >= 0 && pmcf25 < 50) {
    Serial.println("Good");
  }
  if ( pmcf25 >= 51 && pmcf25 < 100) {
    Serial.println("Moderate");
  }
  if ( pmcf25 >= 101 && pmcf25 < 150) {
    Serial.println("Unhealthy for Sensitive Groups");
  }
  if ( pmcf25 >= 151 && pmcf25 < 200) {
    Serial.println("Unhealthy");
  }
  if ( pmcf25 >= 201 && pmcf25 < 300) {
    Serial.println("Very Unhealthy");
  }
  if ( pmcf25 >= 301) {
    Serial.println("Hazardous");
  }

  //////////////////////////////////////////
  Serial.print("10 "  );
  if ( pmat100 >= 0 && pmat100 < 54) {
    Serial.println("Good");
  }
  if ( pmat100 >= 55 && pmat100 < 154) {
    Serial.println("Moderate");
  }
  if ( pmat100 >= 155 && pmat100 < 254) {
    Serial.println("Unhealthy for Sensitive Groups");
  }
  if ( pmat100 >= 255 && pmat100 < 354) {
    Serial.println("Unhealthy");
  }
  if ( pmat100 >= 355 && pmat100 < 424) {
    Serial.println("Very Unhealthy");
  }
  if ( pmat100 >= 425) {
    Serial.println("Hazardous");
  }

  Serial.println(""  );


  delay(READING_SENSOR_INTERVAL);
  firstLoop = false;
}

// Initialize the PMS3003 sensor
void initPMS3003() {
  PMS3003Serial.begin(SERIAL_BAUD_RATE);
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

