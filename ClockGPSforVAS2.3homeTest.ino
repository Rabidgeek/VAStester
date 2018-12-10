// Visual Alert System - using GPS for Clock
//
// Must have the Adafruit GPS library installed too!  See:
//   https://github.com/adafruit/Adafruit-GPS-Library
//
// Designed specifically to work with the Adafruit ultimate GPS returnout/shield
//
// Written by Jesse Ragsdale, cited some lines from Tony DiCola and Adafruit
//
// Version 2.3 updates:
// - Change "It is now / XXXX hours" on LCD to "It is XXXX Hours" 
// - Add date to second line - print("Today is " + month3Ltr + " " + dayNbr);
// - changed from if loop to switch statement. 
//
// Version 2.2 updates:
// - Useing GPS to get time
// - Change LCD output to LCD shield


#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GPS.h>
#include <LiquidCrystal.h>
#include <Adafruit_NeoPixel.h>

// Set to false to display time in 12 hour format, or true to use 24 hour:
#define TIME_24_HOUR      true

// Offset the hours from UTC (universal time) to your local time by changing
// this value.  
#define HOUR_OFFSET       -5

SoftwareSerial gpsSerial(3, 2);  // TX = pin 3 and RX = pin 2.

Adafruit_GPS gps(&gpsSerial);

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

#define PIN 11 // define which pin has the led strip

Adafruit_NeoPixel strip = Adafruit_NeoPixel(29, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  //while (!Serial);
  
  // Setup led strip
  strip.begin();
  strip.show();
  strip.setBrightness(10);

  // Setup Serial port to print debug output.
  Serial.begin(115200);
  Serial.println("Clock starting!");

  // Setup LCD's # of column/rows
  lcd.begin(16, 2);

  // Setup the GPS using a 9600 baud connection 
  gps.begin(9600);

  // Configure GPS to onlu output minimum data (location, time, fix).
  gps.sendCommand(PMTK_SET_NMEA_OUTPUT_ALLDATA);

  // Use a 1 hz, once a second, update rate.
  gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  gps.sendCommand(PMTK_API_SET_FIX_CTL_1HZ);
  
  // Enable the interrupt to parse GPS data.
  enableGPSInterrupt();
}

void loop() {
  // set the LCD cursor to column 0
  lcd.setCursor(0, 0);

  // Check if GPS has new data and parse it.
  if (gps.newNMEAreceived()) {
    gps.parse(gps.lastNMEA());
  }
  
  // Grab the current hours, minutes, seconds from the GPS.
  int hours = gps.hour + HOUR_OFFSET;  
  
  // Handle when UTC + offset wraps around to a negative or > 23 value.
  if (hours < 0) {
    hours = 24+hours;
  }
  if (hours > 23) {
    hours = 24-hours;
  }
  int minutes = gps.minute;
  int seconds = gps.seconds;
  
  // Show the time on the display by turning it into a numeric value
  int displayValue = hours*100 + minutes;

  // Do 24 hour to 12 hour format conversion when required.
  if (!TIME_24_HOUR) {
    // Handle when hours are past 12 by subtracting 12 hours (1200 value).
    if (hours > 12) {
      displayValue -= 1200;
    }
    // Handle hour 0 (midnight) being shown as 12.
    else if (hours == 0) {
      displayValue += 1200;
    }
  }

  // Now print the time value to the display.
  lcd.print("It is ");
  lcd.print(displayValue);
  lcd.print(" hours");
  lcd.setCursor(0, 1);
  lcd.print("on ");
  lcd.print(gps.month);
  lcd.print("/");
  lcd.print(gps.day);
  lcd.print("/20");
  lcd.print(gps.year);

  Serial.println(displayValue + "hours now.");

  // this is home test - customize time for testing needs
  switch (displayValue) {
    case 2208:
    rainbowCycleslow(1);
    colorWipe(strip.Color(0,0,0), 50);
    Serial.println(displayValue);
    break;
    case 2210:
    rainbowCycle(1);
    colorWipe(strip.Color(0,0,0), 50);
    Serial.println(displayValue + " 2210");
    break;
    case 2212:
    rainbowCycleslow(1);
    colorWipe(strip.Color(0,0,0), 50);
    Serial.println(displayValue + " 2212");
    break;
    case 2215:
    rainbowCycle(1);
    colorWipe(strip.Color(0,0,0), 50);
    Serial.println(displayValue + " 2215");
    break;
    default:
    colorWipe(strip.Color(0,0,0), 50);
    Serial.println(displayValue + "hours");
  }
}

SIGNAL(TIMER0_COMPA_vect) {
  // Use a timer interrupt once a millisecond to check for new GPS data.
  gps.read();
}

void enableGPSInterrupt() {
  // Function to enable the timer interrupt that will parse GPS data.
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

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

void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

void rainbowCycleslow(uint8_t wait) {
  uint16_t r, j;
 
  for(j=0; j<256*3; j++) { // 3 cycles of all colors on wheel
    for(r=0; r< strip.numPixels(); r++) {
      strip.setPixelColor(r, Wheel(((r * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

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
