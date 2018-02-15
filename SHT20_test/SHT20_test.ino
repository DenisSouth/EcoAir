//Temp sensor
//default i2c
// -SDA = D2 
// -SCL = D1

#include <Wire.h>
#include "SHT20.h"
SHT20    sht20;
//Temp sensor
void setup()
{
    Serial.begin(115200);
    Serial.println("SHT20 Example!");
    sht20.initSHT20();                                  // Init SHT20 Sensor
    delay(100);
    sht20.checkSHT20();                                 // Check SHT20 Sensor
}

void loop()
{
    float humd = sht20.readHumidity();                  // Read Humidity
    float temp = sht20.readTemperature();               // Read Temperature
    Serial.print(" Temperature:");
    Serial.print(temp, 1);
    Serial.print("C");
    Serial.print(" Humidity:");
    Serial.print(humd, 1);
    Serial.print("%");
    Serial.println();
    delay(1000);
}
