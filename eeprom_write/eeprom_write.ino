/*
 * EEPROM Write
 *
 * Stores values and string to EEPROM.
 * These values and string will stay in the EEPROM when the board is
 * turned off and may be retrieved later by another sketch.
 */
 
#include <EEPROM.h>
 
// the current address in the EEPROM (i.e. which byte
// we're going to write to next)
int addr = 0;
 
void setup()
{
  EEPROM.begin(512);  //Initialize EEPROM
 
  // write appropriate byte of the EEPROM.
  // these values will remain there when the board is
  // turned off.
  
  EEPROM.write(addr, 'A');    //Write character A
  addr++;                      //Increment address
  EEPROM.write(addr, 'B');    //Write character A
  addr++;                      //Increment address
  EEPROM.write(addr, 'C');    //Write character A
 
  //Write string to eeprom
  String www = "www.circuits4you.com";
  for(int i=0;i<www.length();i++) //loop upto string lenght www.length() returns length of string
  {
    EEPROM.write(0x0F+i,www[i]); //Write one by one with starting address of 0x0F
  }
  EEPROM.commit();    //Store data to EEPROM
}
 
void loop()
{
  //We dont have anything in loop as EEPROM writing is done only once
  delay(10);   
}
