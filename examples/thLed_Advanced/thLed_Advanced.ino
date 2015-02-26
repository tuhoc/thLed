#include <thLed.h>

int SDI_pin = 4;
int SCK_pin = 5;
int RCK_pin = 6;
int numModules = 2;

void setup()
{
  thLed.begin(SDI_pin, SCK_pin, RCK_pin, numModules, MANUAL_UPDATE);
}

void loop()
{
  byte shiftRight = _BV(7) | _BV(6);
  byte shiftLeft  = _BV(0) | _BV(1);

  for (int i = 0; i < 8; i++)
  {
    thLed.setBitmap(0, shiftRight);
    thLed.setLed(i, HIGH);
    
    thLed.setBitmap(1, shiftLeft);
    thLed.setLed(15 - i, HIGH);
    
    thLed.update();
    delay(250);
    
    shiftRight = shiftRight >> 1;
    shiftLeft  = shiftLeft  << 1;
  }
  
  thLed.clearAll();
  thLed.update();
  delay(500);
}
