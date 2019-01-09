# NeoPixelClockSoundDemo
NeoPixel Ring - Clock - Sound system - Demo lights
Using a DIP-switch to change Led ring funcionality, between:
  - Clock
  - Sound reactive led display
  - DemoMode (from Adafruit)

This project mix ideas from:

- http://andydoro.com/ringclock/ by Andy Doro (mail@andydoro.com)
- Adafruit NeoPixel Library - https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-installation
- Musical Neopixels - https://www.instructables.com/id/Musical-Neopixels-WS2812B/
- Luis Llamas blog - https://www.luisllamas.es/medir-sonido-con-arduino-y-microfono-amplificado-max9812/

## Libraries used
- <Wire.h> - Used for i2c comunication
- <RTClib.h> - Control RTC clock
- <Adafruit_NeoPixel.h> - Control WS2813 RGB LED ring 

## Components

- DS3231 RTC Clock Module - https://amzn.to/2H35AAw
- WS2813 60 pixel ring - https://amzn.to/2SRpm3a
  - can be used 4 of this - https://amzn.to/2H4FAVn
- Microphone module (analog/digital) - https://amzn.to/2C8tepz
- Arduino nano - https://amzn.to/2AAz2IA
- Dip switch (3 positions) - https://amzn.to/2RHm5pO

## Schematic

![Schema](https://github.com/Parufito/NeoPixelClockSoundDemo/raw/master/NeopixelClockAudioDemo_bb.png)

- Using a single WS2813 RGB LED to simulate 60 led RING
- Using 8 positions dipswitch 

## Videos

### Clock 
![Clock](https://pbs.twimg.com/media/Dr1A8KqWsAAFgU3.jpg)

### Sound Demo 
https://twitter.com/parufito/status/1043909507997478912

### Rainbow
https://www.instagram.com/p/BoFdLVcH2R3/
