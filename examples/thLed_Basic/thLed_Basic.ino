#include <thLed.h>

int SDI_pin = 4;  
int SCK_pin = 5; 
int RCK_pin = 6;  

void setup()
{ 
  thLed.begin(SDI_pin, SCK_pin, RCK_pin);
}

void loop()
{
  for (int i = 0; i < 8; i++)
  {
    thLed.setLed(i, HIGH);
    delay(1000);
    thLed.setLed(i, LOW);
  } 
}
