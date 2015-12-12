/**
 * SimpleNeoPixel.cpp- Neopixel library inspired by josh.com
 * http://wp.josh.com/2014/05/13/ws2812-neopixels-are-not-so-finicky-once-you-get-to-know-them/
 * 
 * Designed to be an (almost) drop-in replacement to the Adafruit_Neopixel library
 * This library allows you to power thousands of WS2812 NeoPixels with almost no memory,
 * even on small ardiunos such as the ATTiny85!
 * 
 * For this version, you'll need to look up the PORT and DDR of the pin on your chip using Google
 * 
 * @author Tony Gaitatzis backupbrain@gmail.com
 * @date 2015-04-15
 */
/*--------------------------------------------------------------------
  This file is part of the SimpleNeoPixel library.

  NeoPixel is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  NeoPixel is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with SimpleNeoPixel.  If not, see
  <http://www.gnu.org/licenses/>.
  --------------------------------------------------------------------*/

#ifndef SimpleNeopixel_h
#define SimpleNeopixel_h

#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
 #include <pins_arduino.h>
#endif

#include <util/delay.h>

// 'type' flags for LED pixels (third parameter to constructor):
#define NEO_RGB     0x00 // Wired for RGB data order
#define NEO_GRB     0x01 // Wired for GRB data order
#define NEO_BRG     0x04
  
#define NEO_COLMASK 0x01
#define NEO_KHZ800  0x02 // 800 KHz datastream
#define NEO_SPDMASK 0x02
// Trinket flash space is tight, v1 NeoPixels aren't handled by default.
// Remove the ifndef/endif to add support -- but code will be bigger.
// Conversely, can comment out the #defines to save space on other MCUs.
#ifndef __AVR_ATtiny85__
#define NEO_KHZ400  0x00 // 400 KHz datastream
#endif

// These values depend on which pin your string is connected to and what board you are using 
// More info on how to find these at http://www.arduino.cc/en/Reference/PortManipulation

// These values are for digital pin 8 on an Arduino Yun or digital pin 12 on a DueMilinove/UNO
// Note that you could also include the DigitalWriteFast header file to not need to to this lookup.

#define PIXEL_PORT  PORTB  // Port of the pin the pixels are connected to
#define PIXEL_DDR   DDRB   // Port of the pin the pixels are connected to
#define PIXEL_BIT   5      // Bit of the pin the pixels are connected to

// These are the timing constraints taken mostly from the WS2812 datasheets 
// These are chosen to be conservative and avoid problems rather than for maximum throughput 

#define T1H  900    // Width of a 1 bit in ns
#define T1L  600    // Width of a 1 bit in ns

#define T0H  400    // Width of a 0 bit in ns
#define T0L  900    // Width of a 0 bit in ns

#define RES 6000    // Width of the low gap between bits to cause a frame to latch

// Here are some convience defines for using nanoseconds specs to generate actual CPU delays

#define NS_PER_SEC (1000000000L)          // Note that this has to be SIGNED since we want to be able to check for negative values of derivatives
#define CYCLES_PER_SEC (F_CPU)
#define NS_PER_CYCLE ( NS_PER_SEC / CYCLES_PER_SEC )
#define NS_TO_CYCLES(n) ( (n) / NS_PER_CYCLE )


class SimpleNeopixel
{
public:
  SimpleNeopixel(uint16_t numPixels, uint8_t pin, uint8_t mode=NEO_GRB + NEO_KHZ800);
  ~SimpleNeopixel();
  
  void 
    begin(),
    show(),
    sendPixel(unsigned char r, unsigned char g , unsigned char b),
    setPin(uint8_t pin),
    setPixelColor(unsigned int pixel, unsigned char r, unsigned char g , unsigned char b),
    setPixelColor(unsigned int pixel, uint32_t color),
    setBrightness(uint8_t),
    clear();
  uint8_t
    getBrightness(void) const;
  uint16_t
    numPixels(void) const;
  static uint32_t
    Color(uint8_t r, uint8_t g, uint8_t b);
    
private:
  const uint16_t _numPixels;
  uint8_t
    _pin,
    _brightness,
    _mode;          // Pixel flags (400 vs 800 KHz, RGB vs GRB color)

  void _sendBit(bool bitVal);
  void _sendByte(unsigned char byte);
  

  
#ifdef __AVR__
  const volatile uint8_t
    *_port;         // Output PORT register
  uint8_t
    _pinMask;       // Output PORT bitmask
#endif

};

#endif


