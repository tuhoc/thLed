/*

TUHOC - thLed library

See thLed.h

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/


#include "thLed.h"

#define  MAX_MODULES    8
#define  MAX_POSISION   (MAX_MODULES * 8)
#define  ALL_LEDS_OFF   0x00

thLedClass thLed;

byte _SDI, _SCLK, _RCLK, _numModules;
byte _thLed_bitmaps[MAX_MODULES];
boolean _autoUpdate = false;

// Shift out nonInvert
void _ShiftOut(byte bb)
{
  uint8_t SDI_bit = digitalPinToBitMask(_SDI);
  uint8_t CLK_bit = digitalPinToBitMask(_SCLK);

  volatile uint8_t *SDI_port = portOutputRegister(digitalPinToPort(_SDI));
#if defined(__AVR_ATmega8P__) || defined(__AVR_ATmega8__)
  volatile uint8_t *CLK_port = portOutputRegister(digitalPinToPort(_SCLK));
#else
  volatile uint8_t *CLK_pin  = portInputRegister(digitalPinToPort(_SCLK));
#endif

  uint8_t oldSREG = SREG;     // Save Interrupt_Enable status
  cli();                      // Disable Interrupt - prevent changes to SDI_port

  uint8_t SDI1 = *SDI_port | SDI_bit;
  uint8_t SDI0 =  SDI1     ^ SDI_bit;     //  & ~SDI_bit

  for (uint8_t i = 8; i > 0; i--)
  {
    if (bb & 0x80)   *SDI_port = SDI1;    // shift out bb
    else             *SDI_port = SDI0;    // 

    bb = bb << 1;

#if defined(__AVR_ATmega8P__) || defined(__AVR_ATmega8__)
    *CLK_port |= CLK_bit;     // clk edge here       invert SCLK pin
    *CLK_port ^= CLK_bit;     // or here             invert SCLK pin
#else
    *CLK_pin = CLK_bit;       // clk edge here       invert SCLK pin
    *CLK_pin = CLK_bit;       // or here             invert SCLK pin
#endif
  }                                                
  SREG = oldSREG;             // Restore Interrupt_Enable status
}

void _update()
{
  for (byte i = _numModules; i > 0 ; i--)
  {
    _ShiftOut(_thLed_bitmaps[i-1]);
  }
  
  digitalWrite(_RCLK, LOW);     // Latch pin RCLK
  digitalWrite(_RCLK, HIGH);    // positive edge on RCLK
  digitalWrite(_RCLK, LOW);     // RCLK = LOW: output enable
}

void checkUpdate()
{
  if (_autoUpdate)
  {
    _update();
  }
}

void thLedClass::begin(byte SDI, byte SCK, byte RCK)
{
  begin(SDI, SCK, RCK, 1, true);
}

void thLedClass::begin(byte SDI, byte SCK, byte RCK, byte numModules)
{
  begin(SDI, SCK, RCK, numModules, true);
}

void thLedClass::begin(byte SDI, byte SCK, byte RCK, byte numModules, boolean autoUpdate)
{
  _SDI  = SDI;
  _SCLK = SCK;
  _RCLK = RCK;
  _numModules = min(numModules, MAX_MODULES);
  _autoUpdate = autoUpdate;

  digitalWrite(_RCLK, HIGH);
  pinMode(_SDI,  OUTPUT);
  pinMode(_SCLK, OUTPUT);
  pinMode(_RCLK, OUTPUT);
  
  clearAll();
  update();
}

void thLedClass::setBitmap(byte module, byte bitmap)
{
  if (module < MAX_MODULES)
  {
    _thLed_bitmaps[module] = bitmap;
  }

  checkUpdate();
}

void thLedClass::setLed(byte pos, byte state)
{
  if (pos < MAX_POSISION)
  {
    uint8_t module  = pos / 8;
    uint8_t led_pos = pos % 8;
    _thLed_bitmaps[module]  &=  ~( 1 << (led_pos));    // Clear bit
    _thLed_bitmaps[module]  |=  state << (led_pos);
  }  
  
  checkUpdate();
}

void thLedClass::clearAll()
{
  for (byte i = _numModules; i > 0 ; i--)
  {
    _thLed_bitmaps[i-1] =  ALL_LEDS_OFF;
  }

  checkUpdate();
}

void thLedClass::update()
{
  _update();
}
