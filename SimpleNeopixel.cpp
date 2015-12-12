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

#include "SimpleNeoPixel.h"

SimpleNeopixel::SimpleNeopixel(uint16_t numPixels, uint8_t pin, uint8_t mode)
: _numPixels(numPixels), _pin(pin), _mode(mode), _brightness(0)
#ifdef __AVR__
  ,_port(portOutputRegister(digitalPinToPort(pin))),
   _pinMask(digitalPinToBitMask(pin))
#endif
{
}

SimpleNeopixel::~SimpleNeopixel() {}

uint16_t SimpleNeopixel::numPixels() const {
  return this->_numPixels;
}

// Actually send a bit to the string. We must to drop to asm to enusre that the complier does
// not reorder things and make it so the delay happens in the wrong place.

void SimpleNeopixel::_sendBit( bool bitVal ) {
  
    if (  bitVal ) {        // 0 bit
      
    asm volatile (
      "sbi %[port], %[bit] \n\t"        // Set the output bit
      ".rept %[onCycles] \n\t"                                // Execute NOPs to delay exactly the specified number of cycles
      "nop \n\t"
      ".endr \n\t"
      "cbi %[port], %[bit] \n\t"                              // Clear the output bit
      ".rept %[offCycles] \n\t"                               // Execute NOPs to delay exactly the specified number of cycles
      "nop \n\t"
      ".endr \n\t"
      ::
      [port]    "I" (_SFR_IO_ADDR(PIXEL_PORT)),
      [bit]   "I" (PIXEL_BIT),
      [onCycles]  "I" (NS_TO_CYCLES(T1H) - 2),    // 1-bit width less overhead  for the actual bit setting, note that this delay could be longer and everything would still work
      [offCycles]   "I" (NS_TO_CYCLES(T1L) - 2)     // Minimum interbit delay. Note that we probably don't need this at all since the loop overhead will be enough, but here for correctness

    );
                                  
    } else {          // 1 bit

    // **************************************************************************
    // This line is really the only tight goldilocks timing in the whole program!
    // **************************************************************************


    asm volatile (
      "sbi %[port], %[bit] \n\t"        // Set the output bit
      ".rept %[onCycles] \n\t"        // Now timing actually matters. The 0-bit must be long enough to be detected but not too long or it will be a 1-bit
      "nop \n\t"                                              // Execute NOPs to delay exactly the specified number of cycles
      ".endr \n\t"
      "cbi %[port], %[bit] \n\t"                              // Clear the output bit
      ".rept %[offCycles] \n\t"                               // Execute NOPs to delay exactly the specified number of cycles
      "nop \n\t"
      ".endr \n\t"
      ::
      [port]    "I" (_SFR_IO_ADDR(PIXEL_PORT)),
      [bit]   "I" (PIXEL_BIT),
      [onCycles]  "I" (NS_TO_CYCLES(T0H) - 2),
      [offCycles] "I" (NS_TO_CYCLES(T0L) - 2)

    );
      
    }
    
    // Note that the inter-bit gap can be as long as you want as long as it doesn't exceed the 5us reset timeout (which is A long time)
    // Here I have been generous and not tried to squeeze the gap tight but instead erred on the side of lots of extra time.
    // This has thenice side effect of avoid glitches on very long strings becuase 

    
}  

  
void SimpleNeopixel::_sendByte( unsigned char byte ) {
    
    for( unsigned char bit = 0 ; bit < 8 ; bit++ ) {
      
      this->_sendBit( bitRead( byte , 7 ) );                // Neopixel wants bit in highest-to-lowest order
                                                     // so send highest bit (bit #7 in an 8-bit byte since they start at 0)
      byte <<= 1;                                    // and then shift left so bit 6 moves into 7, 5 moves into 6, etc
      
    }           
} 

/*

  The following three functions are the public API:
  
  ledSetup() - set up the pin that is connected to the string. Call once at the begining of the program.  
  sendPixel( r g , b ) - send a single pixel to the string. Call this once for each pixel in a frame.
  show() - show the recently sent pixel on the LEDs . Call once per frame. 
  
*/


// Set the specified pin up as digital out

void SimpleNeopixel::begin() {
  
  bitSet( PIXEL_DDR , PIXEL_BIT );
  
}

void SimpleNeopixel::sendPixel( unsigned char r, unsigned char g , unsigned char b )  {    
   if (this->_brightness) { // See notes in setBrightness()
      r = (r * this->_brightness) >> 8;
      g = (g * this->_brightness) >> 8;
      b = (b * this->_brightness) >> 8;
    }


  if(this->_mode & NEO_GRB) { // GRB vs RGB; might add others if needed
    this->_sendByte(g);          // Neopixel wants colors in green then red then blue order
    this->_sendByte(r);
    this->_sendByte(b);
  } else if (this->_mode & NEO_BRG) {
    this->_sendByte(b);          
    this->_sendByte(r);
    this->_sendByte(g);
  } else {
    this->_sendByte(r);          
    this->_sendByte(g);
    this->_sendByte(b);
  }
  
}

// Set the output pin number
void SimpleNeopixel::setPin(uint8_t pin) {
  pinMode(this->_pin, INPUT);
  this->_pin = pin;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
#ifdef __AVR__
  this->_port    = portOutputRegister(digitalPinToPort(pin));
  this->_pinMask = digitalPinToBitMask(pin);
#endif
}


void SimpleNeopixel::setPixelColor(unsigned int pixel, unsigned char r, unsigned char g , unsigned char b )  {  
  this->sendPixel(r, g, b);  
}


// Set pixel color from 'packed' 32-bit RGB color:

void SimpleNeopixel::setPixelColor(uint16_t pixel, uint32_t color) {
    uint8_t
      r = (uint8_t)(color >> 16),
      g = (uint8_t)(color >>  8),
      b = (uint8_t)color;
    this->sendPixel(r, g, b); 
}

// Convert separate R,G,B into packed 32-bit RGB color.
// Packed format is always RGB, regardless of LED strand color order.
uint32_t SimpleNeopixel::Color(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}



// Adjust output brightness; 0=darkest (off), 255=brightest.  This does
// NOT immediately affect what's currently displayed on the LEDs.  The
// next call to show() will refresh the LEDs at this level.  However,
// this process is potentially "lossy," especially when increasing
// brightness.  The tight timing in the WS2811/WS2812 code means there
// aren't enough free cycles to perform this scaling on the fly as data
// is issued.  So we make a pass through the existing color data in RAM
// and scale it (subsequent graphics commands also work at this
// brightness level).  If there's a significant step up in brightness,
// the limited number of steps (quantization) in the old data will be
// quite visible in the re-scaled version.  For a non-destructive
// change, you'll need to re-render the full strip data.  C'est la vie.
void SimpleNeopixel::setBrightness(uint8_t b) {
  // Stored brightness value is different than what's passed.
  // This simplifies the actual scaling math later, allowing a fast
  // 8x8-bit multiply and taking the MSB.  'brightness' is a uint8_t,
  // adding 1 here may (intentionally) roll over...so 0 = max brightness
  // (color values are interpreted literally; no scaling), 1 = min
  // brightness (off), 255 = just below max brightness.
  uint8_t newBrightness = b + 1;
  if(newBrightness != this->_brightness) { // Compare against prior value
    // Brightness has changed -- re-scale existing data in RAM
    uint8_t  oldBrightness = this->_brightness - 1; // De-wrap old brightness value
    uint16_t scale;
    if(oldBrightness == 0) scale = 0; // Avoid /0
    else if(b == 255) scale = 65535 / oldBrightness;
    else scale = (((uint16_t)newBrightness << 8) - 1) / oldBrightness;

    this->_brightness = newBrightness;
  }
}

//Return the brightness value
uint8_t SimpleNeopixel::getBrightness(void) const {
  return this->_brightness - 1;
}

void SimpleNeopixel::clear() {
  // do nothing?
}


// Just wait long enough without sending any bots to cause the pixels to latch and display the last sent frame

void SimpleNeopixel::show() {
  _delay_us( (RES / 1000UL) + 1);       // Round up since the delay must be _at_least_ this long (too short might not work, too long not a problem)
}


