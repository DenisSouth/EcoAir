#define Red_LED 12
#define Blue_LED 13
#define Green_LED 14

void setup() {
pinMode(Red_LED, OUTPUT);
pinMode(Blue_LED, OUTPUT);
pinMode(Green_LED, OUTPUT);

digitalWrite(Red_LED, HIGH); //led off
digitalWrite(Blue_LED, HIGH);  //led off
digitalWrite(Green_LED, HIGH);  //led off
}


void loop() {

LED ("Red");
delay(1000);

LED ("Blue");
delay(1000);  

LED ("Green");
delay(1000);  
}

void LED(String ledname) 
{
      if (ledname == "Red")
    {digitalWrite(Red_LED, LOW); //led off
    digitalWrite(Blue_LED, HIGH);  //led off
    digitalWrite(Green_LED, HIGH);  //led off
    }
        
        if (ledname == "Green")
    {
    digitalWrite(Red_LED, HIGH); //led off
    digitalWrite(Blue_LED, HIGH);  //led off
    digitalWrite(Green_LED, LOW);  //led off
    }
    
      if (ledname == "Blue")
    {
    digitalWrite(Red_LED, HIGH); //led off
    digitalWrite(Blue_LED, LOW);  //led off
    digitalWrite(Green_LED, HIGH);  //led off
    }
    
}
