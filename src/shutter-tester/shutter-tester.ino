/*
   MIT License

   Copyright (c) 2018 c-s-1

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/*
   Initialise an LCD

   Please note that some I2C chips use a different address. If your display
   doesn't display anything, comment the active line and uncomment the other
   line.
*/
LiquidCrystal_I2C lcd(0x27, 20, 4);
//LiquidCrystal_I2C lcd(0x3F, 20, 4);

// Version
const String version = "v0.3";
// Pin for IR sensor
const byte interruptPin = 2;
// Pin for reset button
const byte buttonPin = 3;
// Pin for light toggle button
const byte lightButtonPin = 4;
boolean exposed = false;
// Variables for storing beginning and end time of exposure
unsigned long start_time = 0;
unsigned long end_time = 0;
// Exposure counter
int exposure = 0;
// Store the minimum and maximum exposure of a test run
unsigned long mintime = 0;
unsigned long maxtime = 0;
// Sum of times
unsigned long sum = 0;
String mintime_s = "";
String maxtime_s = "";
// Store if LCD light should be on or off
boolean light = false;

void setup() {
  // Initialise serial line for printing CSVs of results.
  Serial.begin(9600);
  // Initialise pin for IR sensor input and attach an interrupt to it
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), exp, CHANGE);
  /*
     Initialise pins for buttons.
     WARNING: don't forget to define it as an input w/ pullup. If you
     don't you'll very likely fry your Arduino!
  */
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(lightButtonPin, INPUT_PULLUP);
  // Initialise LCD and print intro
  lcd.init();
  lcd.backlight();
  light = true;
  lcd.setCursor(0, 0);
  lcd.print("=== Shutter Test ===");
  lcd.setCursor(0, 1);
  lcd.print("Version " + version);
  lcd.setCursor(0, 2);
  lcd.print("Press reset to");
  lcd.setCursor(0, 3);
  lcd.print("start test.");
  // Catch reset button press
  while (digitalRead(buttonPin) == HIGH)
  {
    if (digitalRead(lightButtonPin) == LOW)
    {
      toggle_light();
      while (!digitalRead(lightButtonPin));
    }
  }
  setup_lcd();
}

void setup_lcd() {
  /*
     Method for setting up the LCD for the next round
     of shutter tests.
  */
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Exp #" + String(exposure));
  lcd.setCursor(8, 0);
  lcd.print("Avg:");
  lcd.setCursor(0, 1);
  lcd.print("Speed:");
  lcd.setCursor(0, 2);
  lcd.print("Min:");
  lcd.setCursor(0, 3);
  lcd.print("Max:");
}

void loop() {
  // If the reset button is pressed, reset shutter test.
  if (digitalRead(buttonPin) == LOW or exposure == 100)
  {
    reset();
  }
  // If light button is pressed, toggle LCD background light.
  if (digitalRead(lightButtonPin) == LOW)
  {
    toggle_light();
    // Endless loop until button is released.
    while (!digitalRead(lightButtonPin));
  }
  // If a complete shutter release and reset was detected,
  // print the measured result.
  if (exposed)
  {
    unsigned long timed;
    String s = "";
    exposure++;
    if (start_time > 0 && start_time < end_time)
    {
      // Calculate time difference between start and end of exposure in milliseconds.
      timed = end_time - start_time;
      s = t_to_string(timed);
      // Check if the last exposure is a new minimum.
      if (timed < mintime || mintime == 0)
      {
        mintime = timed;
        mintime_s = s;
      }
      // Check if the last exposure is a new maximum.
      if (timed > maxtime)
      {
        maxtime = timed;
        maxtime_s = s;
      }
    }
    sum += timed;
    unsigned long avg = sum / exposure;
    print_result(s, t_to_string(avg));
    Serial.println(String(exposure) + "," + String(timed) + "," + String(mintime) + "," + String(maxtime) + "," + String(avg));
    exposed = false;
  }
}

void print_result(String s, String avg) {
  /*
     Method for LCD output of results.
  */
  lcd.setCursor(5, 0);
  lcd.print("   ");
  lcd.setCursor(5, 0);
  lcd.print(String(exposure));
  lcd.setCursor(13, 0);
  lcd.print("       ");
  lcd.setCursor(13, 0);
  lcd.print(avg);
  lcd.setCursor(7, 1);
  lcd.print("            ");
  lcd.setCursor(7, 1);
  lcd.print(s);
  lcd.setCursor(5, 2);
  lcd.print("               ");
  lcd.setCursor(5, 2);
  lcd.print(mintime_s);
  lcd.setCursor(5, 3);
  lcd.print("               ");
  lcd.setCursor(5, 3);
  lcd.print(maxtime_s);
}

String t_to_string(unsigned long timed) {
  /*
     Method for formating unsigned long timed (measured speed in milliseconds)
     to a string "x.xs" for speeds over 1s and "1/xs" for speeds unter a
     second. Returns string of converted time.
  */
  String s = "";
  if (timed >= 1000)
  {
    // Format shutter times >1s.
    s = String((float)timed / 1000.0);
  }
  else
  {
    // Format shutter times <1s.
    s = String("1/" + String(int(1.0 / (float(timed) / 1000.0))));
  }
  return s;
}

void exp() {
  /*
     Method called by interrupt attached to IR sensor output.
  */
  unsigned long temp_time = millis();
  // Pin is LOW if IR sensor detects IR (i. e. shutter is open).
  if (digitalRead(interruptPin) == LOW)
  {
    start_time = temp_time;
  }
  // Pin is HIGH if IR sensor doesn't detect IR (i. e. shutter is closed).
  else
  {
    if (not exposed)
    {
      end_time = temp_time;
      exposed = true;
    }
  }
}

void toggle_light() {
  /*
     Method that toggles the LCD's backlight.
  */
  if (light)
  {
    lcd.noBacklight();
    light = false;
  }
  else
  {
    lcd.backlight();
    light = true;
  }
}

void reset() {
  /*
     Method for resetting the LCD's content and some variables.
  */
  exposed = false;
  exposure = 0;
  sum = 0;
  mintime = 0;
  maxtime = 0;
  mintime_s = "";
  maxtime_s = "";
  setup_lcd();
}
