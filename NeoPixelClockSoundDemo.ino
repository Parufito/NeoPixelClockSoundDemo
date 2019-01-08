/**************************************************************************
 *                                                                       *
     NeoPixel Ring - Clock - Sound system - Demo lights
     by Parufito
     http://www.parufito.info
     Twitter: @parufito

    This project mix ideas from:
     - http://andydoro.com/ringclock/ by Andy Doro (mail@andydoro.com)
     - Adafruit NeoPixel Library - https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-installation
     - Musical Neopixels - https://www.instructables.com/id/Musical-Neopixels-WS2812B/
     - Luis Llamas blog - https://www.luisllamas.es/medir-sonido-con-arduino-y-microfono-amplificado-max9812/

    Pins
    Neopixel 5
    Microphone A0
    Clock
      D (sda?) A4 
      C (scl?) A5
     
 *                                                                      *
 **************************************************************************
   Revision History

   Date        By    What
   20180923    Parufito   Debug and improove
   20180917    Parufito   First attempt
*/

//Libraries used
#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_NeoPixel.h>

// DEFINE NeoPixel Ring
#define NEOPIN 5
#define BRIGHTNESS 64 // set max brightness
#define STARTPIXEL 0 // where do we start on the loop? use this to shift the arcs if the wiring does not start at the "12" point
#define PIXEL_COUNT 60

// DEFINE RTC
// Do you live in a country or territory that observes Daylight Saving Time?
// https://en.wikipedia.org/wiki/Daylight_saving_time_by_country
// Use 1 if you observe DST, 0 if you don't. This is programmed for DST in the US / Canada. If your territory's DST operates differently,
// you'll need to modify the code in the calcTheTime() function to make this work properly.
#define OBSERVE_DST 0

// DEFINE Microphone
#define MIC_PIN A0 // Microphone
#define N 20 // Number of samples to take each time readSamples is called
#define fadeDelay 20 // delay time for each fade amount
#define noiseLevel 20 // slope level of average mic noise without sound

//Define DIP-Switch
#define DIPPIN1 6
#define DIPPIN2 7
#define DIPPIN3 8


RTC_DS1307 RTC; // Establish clock object
DateTime theTime; // Holds current clock time
byte hourval, minuteval, secondval; // holds the time

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, NEOPIN, NEO_GRB + NEO_KHZ800); // strip object
byte pixelColorRed, pixelColorGreen, pixelColorBlue; // holds color values

int samples[N]; // storage for a sample collection set
int periodFactor = 0; // keep track of number of ms for period calculation
int t1 = -1; // times of slope > 100 detected.
int T; // period between times scaled to milliseconds
int slope; // the slope of two collected data sample points
byte periodChanged = 0;

void setup () {

  //Prepare RTC
  Wire.begin();  // Begin I2C
  RTC.begin();   // begin clock
  Serial.begin(9600);

  //Define DIp Switch Pins
  pinMode(DIPPIN1, INPUT_PULLUP);
  pinMode(DIPPIN2, INPUT_PULLUP);
  pinMode(DIPPIN3, INPUT_PULLUP);


  if (! RTC.isrunning()) {
    //Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
    theTime = RTC.now();
    // DST? If we're in it, let's subtract an hour from the RTC time to keep our DST calculation correct. This gives us
    // Standard Time which our DST check will add an hour back to if we're in DST.
    if (OBSERVE_DST == 1) {
      if (checkDst() == true) { // check whether we're in DST right now. If we are, subtract an hour.
        theTime = theTime.unixtime() - 3600;
      }
    }
    RTC.adjust(theTime);
  }
  
  //Prepare Neopixel
    // set pinmodes
  pinMode(NEOPIN, OUTPUT);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(BRIGHTNESS); // set brightness
 
    // startup sequence
  delay(500);
  Serial.println("Red");
  colorWipe(strip.Color(255, 0, 0), 5); // Red
  Serial.println("Green");
  colorWipe(strip.Color(0, 255, 0), 5); // Green
  Serial.println("Blue");
  colorWipe(strip.Color(0, 0, 255), 5); // Blue
  delay(500);

}

// Arduino loop Method
void loop() {
  //Initial check
  Serial.print("Pin Status: DIP1: ");
  Serial.println(digitalRead(DIPPIN1));
  Serial.print("Pin Status: DIP2: ");
  Serial.println(digitalRead(DIPPIN2));
  Serial.print("Pin Status: DIP3: ");
  Serial.println(digitalRead(DIPPIN3));
  
  if(digitalRead(DIPPIN1) == HIGH){
    Serial.println("Loop DIP1 - Clock");
    clock_loop();
  } else if (digitalRead(DIPPIN2) == HIGH){
    Serial.println("Loop DIP2 - Audio");
    readSamples();
  } else if (digitalRead(DIPPIN3) == HIGH){
    Serial.println("Loop DIP3 - Demo");
    startShow();
  }
}
  


void clock_loop() {

  char* colon = ":"; // static characters save a bit
  char* slash = "/"; // of memory

  // get time
  //Clock = RTC.now(); // get the RTC time
  theTime = calculateTime(); // takes into account DST

  secondval = theTime.second();  // get seconds
  minuteval = theTime.minute();  // get minutes
  hourval = theTime.hour();   // get hours

    // for serial debugging
    
    Serial.print("PRE:");
    Serial.print(hourval, DEC);
    Serial.print(":");
    Serial.print(minuteval, DEC);
    Serial.print(":");
    Serial.println(secondval, DEC);
    

  //if (hourval > 11) hourval -= 12;
  hourval = hourval % 12; // This clock is 12 hour, if 13-23, convert to 0-11`

  hourval = (hourval * 60 + minuteval) / 12; //each red dot represent 24 minutes.

  // arc mode
  for (uint8_t i = 0; i < strip.numPixels(); i++) {
    /*Eliminar segons*/
    /*
    if (i <= secondval) {
      // calculates a faded arc from low to maximum brightness
      pixelColorBlue = (i + 1) * (255 / (secondval + 1));
      //pixelColorBlue = 255;
    }
    else {
      pixelColorBlue = 0;
    }*/

    if (i <= minuteval) {
      pixelColorGreen = (i + 1) * (255 / (minuteval + 1));
      //pixelColorGreen = 255;
    }
    else {
      pixelColorGreen = 0;
    }

    if (i <= hourval) {
      pixelColorRed = (i + 1) * (255 / (hourval + 1));
      //pixelColorRed = 255;
    }
    else {
      pixelColorRed = 0;
    }

    strip.setPixelColor((60 - i + STARTPIXEL) % 60, strip.Color(pixelColorRed, pixelColorGreen, pixelColorBlue));
  }
  
    // for serial debugging
    
    Serial.print("POST:");
    Serial.print(hourval, DEC);
    Serial.print(":");
    Serial.print(minuteval, DEC);
    Serial.print(":");
    Serial.println(secondval, DEC);
    
  
  //display
  strip.show();

  // wait
  delay(1000);

}


// Microphone Functions

// Read and Process Sample Data from Mic
void readSamples() {
  for(int i=0; i<N; i++) {
    samples[i] = analogRead(0);
    if(i>0) {
      slope = samples[i] - samples[i-1];
    }
    else {
      slope = samples[i] - samples[N-1];
    }
    // Check if Slope greater than noiseLevel - sound that is not at noise level detected
    if(abs(slope) > noiseLevel) {
      if(slope < 0) {
        calculatePeriod(i);
        if(periodChanged == 1) {
          displayColor(getColor(T));
        }
      }
    }
    else {
      ledsOff();
    }
    periodFactor += 1;
    delay(1);
  }
}

void calculatePeriod(int i) {
  if(t1 == -1) {
    // t1 has not been set
    t1 = i;
  }
  else {
    // t1 was set so calc period
    int period = periodFactor*(i - t1);
    periodChanged = T==period ? 0 : 1;
    T = period;
    Serial.println(T);
    // reset t1 to new i value
    t1 = i;
    periodFactor = 0;
  }
}

uint32_t getColor(int period) {
  if(period == -1)
    return Wheel(0);
  else if(period > 400)
    return Wheel(5);
  else
    return Wheel(map(-1*period, -400, -1, 50, 255));
}

void fadeOut()
{
  for(int i=0; i<5; i++) {
    strip.setBrightness(110 - i*20);
    strip.show(); // Update strip
    delay(fadeDelay);
    periodFactor +=fadeDelay;
  }
}

void fadeIn() {
  strip.setBrightness(100);
  strip.show(); // Update strip
  // fade color in
  for(int i=0; i<5; i++) {
    //strip.setBrightness(20*i + 30);
    //strip.show(); // Update strip
    delay(fadeDelay);
    periodFactor+=fadeDelay;
  }
}

void ledsOff() {
  fadeOut();
  for(int i=0; i<PIXEL_COUNT; i++) {
    strip.setPixelColor(i, 0, 0, 0);
  }
}

void displayColor(uint32_t color) {
  for(int i=0; i<PIXEL_COUNT; i++) {
    strip.setPixelColor(i, color);
  }
  fadeIn();
}

// Demo Adafruit NeoPixel

void startShow() {
  Serial.println("DemoStart");
  Serial.println("Black OFF");
  colorWipeDemo(strip.Color(0, 0, 0), 50);    // Black/off
  Serial.println("Red");
  colorWipeDemo(strip.Color(255, 0, 0), 50);  // Red
  Serial.println("Green");
  colorWipeDemo(strip.Color(0, 255, 0), 50);  // Green
  Serial.println("Blue");
  colorWipeDemo(strip.Color(0, 0, 255), 50);  // Blue
  Serial.println("White theater");
  theaterChase(strip.Color(127, 127, 127), 50); // White
  Serial.println("Red theater");
  theaterChase(strip.Color(127,   0,   0), 50); // Red
  Serial.println("Blue theater");
  theaterChase(strip.Color(  0,   0, 127), 50); // Blue
  Serial.println("Rainbow");
  rainbow(20);
  Serial.println("RainbowCicle");
  rainbowCycle(20);
  Serial.println("Rainbow theater");
  theaterChaseRainbow(50);
  }


void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}


//COMMON FUNCTIONS

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    //strip.setPixelColor(i, c);
    strip.setPixelColor((i + STARTPIXEL) % 60, c);
    strip.show();
    delay(wait);
  }
}

// Fill the dots one after the other with a color
void colorWipeDemo(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
