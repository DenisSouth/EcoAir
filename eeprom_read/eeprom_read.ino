/*
 * EEPROM Read 
 * Circuits4you.com
 */
 
#include <EEPROM.h>
 
// the current address in the EEPROM (i.e. which byte
// we're going to read from)
int addr = 0;
 
void setup()
{
  EEPROM.begin(512);  //Initialize EEPROM
  Serial.begin(9600); //Serial communication to display data
  // read appropriate byte of the EEPROM.  
  Serial.println(""); //Goto next line, as ESP sends some garbage when you reset it  
  Serial.print(char(EEPROM.read(addr)));    //Read from address 0x00
  addr++;                      //Increment address
  Serial.print(char(EEPROM.read(addr)));    //Read from address 0x01
  addr++;                      //Increment address
  Serial.println(char(EEPROM.read(addr)));    //Read from address 0x02
 
  //Read string from eeprom
  String www;   
  //Here we dont know how many bytes to read it is better practice to use some terminating character
  //Lets do it manually www.circuits4you.com  total length is 20 characters
  for(int i=0;i<20;i++) 
  {
    www = www + char(EEPROM.read(0x0F+i)); //Read one by one with starting address of 0x0F    
  }  
 
  Serial.print(www);  //Print the text on serial monitor
}
 
void loop()
{
  //We dont have anything in loop as EEPROM reading is done only once
  delay(10);   
}
