/*
 * MIT License
 *
 * Copyright (c) 2018 c-s-1
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialise an LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Version
const String version = "v0.1";
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
String mintime_s = "";
String maxtime_s = "";
// Store if LCD light should be on or off
boolean light = false;

void setup() {
  // Initialise pin for IR sensor input and attach an interrupt to it
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), exp, CHANGE);
  /*
   * Initialise pins for buttons.
   * WARNING: don't forget to define it as an input w/ pullup. If you
   * don't you'll very likely fry your Arduino!
   */
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(lightButtonPin, INPUT_PULLUP);
  // Initialise LCD and print intro
  lcd.init();
  lcd.backlight();
  light = true;
  lcd.setCursor(0,0);
  lcd.print("=== Shutter Test ===");
  lcd.setCursor(0,1);
  lcd.print("Version " + version);
  lcd.setCursor(0,2);
  lcd.print("Press reset to");
  lcd.setCursor(0,3);
  lcd.print("start test.");
  // Catch reset button press
  while (digitalRead(buttonPin) == HIGH)
  {
    if (digitalRead(lightButtonPin) == LOW)
    {
      toggle_light();
      delay(500);
    }
  }
  setup_lcd();
}

void setup_lcd() {
  /*
   * Method for setting up the LCD for the next round
   * of shutter tests.
   */
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Exp. #" + String(exposure));
  lcd.setCursor(0,1);
  lcd.print("Speed:");
  lcd.setCursor(0,2);
  lcd.print("Min:");
  lcd.setCursor(0,3);
  lcd.print("Max:");
}

void loop() {
  // If the reset button is pressed, reset shutter test.
  if (digitalRead(buttonPin) == LOW)
  {
    reset();
  }
  // If light button is pressed, toggle LCD background light.
  if (digitalRead(lightButtonPin) == LOW)
  {
    toggle_light();
    // Delay after toggling to avoid "flapping".
    delay(500);
  }
  // If a complete shutter release and reset was detected,
  // print the measured result.
  if (exposed)
  {
    String s = "";
    exposure++;
    if (start_time > 0 && start_time < end_time)
    {
      // Calculate time difference between start and end of exposure in microseconds.
      unsigned long timed = end_time - start_time;
      // To get readable output, distinguish between
      if (timed >= 1000000)
      {
        // Format shutter times >1s.
        s = String((float)timed / 1000000.0);
      }
      else
      {
        // Format shutter times <1s.
        s = String("1/" + String(int(1.0 / (timed / 1000000.0))));
      }
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
    print_result(s);
    exposed = false;
  }
}

void print_result(String s) {
  /*
   * Method for LCD output of results.
   */
  lcd.setCursor(6,0);
  lcd.print(String(exposure));
  lcd.setCursor(7,1);
  lcd.print("            ");
  lcd.setCursor(7,1);
  lcd.print(s + "s");
  lcd.setCursor(5,2);
  lcd.print("               ");
  lcd.setCursor(5,2);
  lcd.print(mintime_s + "s");
  lcd.setCursor(5,3);
  lcd.print("               ");
  lcd.setCursor(5,3);
  lcd.print(maxtime_s + "s");
}

void exp() {
  /*
   * Method called by interrupt attached to IR sensor output.
   */
  unsigned long temp_time = micros();
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
   * Method that toggles the LCD's backlight.
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
   * Method for resetting the LCD's content and some variables.
   */
  lcd.setCursor(6,0);
  lcd.print("0");
  lcd.setCursor(7,1);
  lcd.print("            ");
  lcd.setCursor(5,2);
  lcd.print("               ");
  lcd.setCursor(5,3);
  lcd.print("               ");
  exposed = false;
  exposure = 0;
  mintime = 0;
  maxtime = 0;
  mintime_s = "";
  maxtime_s = "";
}
