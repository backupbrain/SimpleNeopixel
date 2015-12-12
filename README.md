# SimpleNeopixel Library 

Arduino library for controlling single-wire-based LED pixels and strip such as the [Adafruit 60 LED/meter Digital LED strip][strip], the [Adafruit FLORA RGB Smart Pixel][flora], the [Adafruit Breadboard-friendly RGB Smart Pixel][pixel], the [Adafruit NeoPixel Stick][stick], and the [Adafruit NeoPixel Shield][shield].

This library is based on both the [Adafruit NeoPixel Library][adafruit_neopixel] and the [josh.com neopixel bitbang][josh_neopixel] library.

Adafruit's library is robust and has no timing limitations but stores each pixel value in memory, in preparation for outputting to the NeoPixel Strip.  This consumes 4 bytes per pixel, meaning that a maxumum of 500 pixels can be addressed on an Arduino Uno, and only 128 pixels can be addressed on a Trinket

Josh.com's method doesn't store data about the pixels.  It simply streams the pixels out from a loop.  This enables low-memory devices to calculate and output pixel values to thousands of pixels without having to use precious memory on storing pixel values.

Josh.com's method unfortunately has relatively tight timing requirements, so it requires that color calculatins are done within the pixel timing window.  This is a tight requirement on the Adafruit Trinket, which can do basic arithmatic within this window.  On faster processors such as the Uno or on the Mega, more complex calculations can be made within this time window.

By combining both methods, it acts as a drop-in replacement for most Adafruit_Neopixel users while enabling them to power thousands of WS2812 Neopixels on any Arduino-compatible device, even ones with extremely low memory such as the [Adafruit Trinket][trinket]

After downloading, rename folder to 'SimpleNeopixel' and install in Arduino Libraries folder. Restart Arduino IDE, then open File->Sketchbook->Library->Adafruit_NeoPixel->strandtest sketch.


[adafruit_neopixel]: https://github.com/adafruit/Adafruit_NeoPixel
[josh_neopixel]: http://wp.josh.com/2014/05/13/ws2812-neopixels-are-not-so-finicky-once-you-get-to-know-them/
[trinket]: http://adafruit.com/product/1501
[flora]:  http://adafruit.com/products/1060
[strip]:  http://adafruit.com/products/1138
[pixel]:  http://adafruit.com/products/1312
[stick]:  http://adafruit.com/products/1426
[shield]: http://adafruit.com/products/1430
