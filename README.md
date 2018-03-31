# Shutter Tester

## Disclaimer

This is my first Arduino project and also my first project on github so please be patient with me. ;) The shutter tester seems to work resonably well for long exposures (1/250s or longer).
I'm not entirely sure about shorter exposure times so take the results with a grain of salt. It works for me, your mileage may vary.
The assembly requires soldering skills if you want to detach the IR emitting diode. Don't try this if you
don't know how to solder.<sup>1</sup> You assemble and use the Shutter Tester at your own risk.

<sup>1</sup>If you don't know how to solder, you can use a light detector module with a digital output pin and an adjustable
threshold instead of an IR module.

## Introduction

Camera enthusiasts know the problem that the shutters of old cameras often lose their calibration over time. This project is
an attempt to help with measuring and calibrating a shutter and to provide a tool for assessing shutter speeds of an old camera.

## Concept

The Arduino platform is a cheap and convenient way to build a shutter tester. To achieve this, the tester consists of the following components:

* an Arduino Uno R3,
* a 20 character, 4 lines LCD to display results (LCD 2004 with a I<sup>2</sup>C module),
* a reset button to reset the tests,
* a light toggle button to toggle the LCD’s backlight,
* an IR emitter and receiver module<sup>2</sup> that acts as the "shutter open / closed" sensor.

<sup>2</sup>Using an IR module has the advantage of working in all sorts of lighting conditions. Alternatively you can use any
light sensor module with a digital output pin and an adjustable threshold. If you use a bright light source you can adjust the
threshold in a way that it'll work even in daylight conditions.

## Assembly

The setup of all components looks like this:

![Breadboard view of Shutter Tester](https://github.com/c-s-1/shutter-tester/raw/master/img/Shutter-Tester.png )

**Warning: The inputs of the buttons use the internal pullup resistors of the Arduino Uno R3. Make sure to always initialise
the input pins as** `INPUT_PULLUP` **because otherwise you might damage your Arduino.**

The reset button is attached to D3, the light toggle button is attached to D4.

*Note: It's a good idea to unsolder the IR emitting diode and to re-attach it using a sufficiently long pair of wires so that
it can be place on the opposite side of the shutter. It may be possible to leave the IR emitter and receiver on the same side
and to use a mirror, however, it's hard to get satisfying results.*

To upload the code to your Arduino, open [src/shutter-tester/shutter-tester.ino](https://raw.githubusercontent.com/c-s-1/shutter-tester/master/src/shutter-tester/shutter-tester.ino) in the Arduino IDE and upload it to your Arduino Uno R3.
You only need to do this once or whenever you want to update the code.

## Troubleshooting / FAQ

* Q: My LCD doesn't show anything, the LED backlight only lights up.
* A: Some I<sup>2</sup>C modules use different addresses than others. In the LCD initialisation line `LiquidCrystal_I2C lcd();` try addresses `0x27` or `0x3F`. If both don't work, try using a I<sup>2</sup>C scanner sketch (there're many available) and use the address that it detects.

## User Manual

### Setting up the Shutter Tester

After assembling the Shutter Tester you basically have two units:

* the IR receiver end with the Arduino and the LCD as the main unit,
* the IR emitter that's attached to the main unit with a resonably long pair of wires.

Place the components on opposite sides of the shutter:

```
         Camera with open back
              +-------+
              |       |
                     ||------
IR emitter           |          +----------------------------+
+----------=>        |         <=   Main unit w/ IR receiver |
|                    |Shutter   +---------------+------------+
|                    ||------                   |
|             |       |                         |
|             +-------+                         |
+-----------------------------------------------+
```

*Note: If the lens of the camera is detachable, it's helpful to remove it. It should also work with the lens
attached but your results may vary.*

### Using the Shutter Tester

After powering up the Arduino, you'll be greeted with a message:

```
+--------------------+
|=== Shutter Test ===|
|                    |
|Press reset to      |
|start test.         |
+--------------------+
```

Set up the Shutter Testers IR emitter and receiver and press the reset button to start the measurements.
After firing the shutter, the measured speed is displayed on the screen:

```
+--------------------+
|Exp #1  Avg: 1/31   |
|Speed: 1/31         |
|Min: 1/31           |
|Max: 1/31           |
+--------------------+
```

The Shutter Tester also records the minimum and maximum speeds of a test run as well as the average speed. The exposure counter counts the number of times
that the shutter was fired:

```
+--------------------+
|Exp #16 Avg: 1/30   |
|Speed: 1/29         |
|Min: 1/32           |
|Max: 1/28           |
+--------------------+
```

Due to the space constraints of the LCD, the test will automatically reset after 99 exposures.

The Shutter Tester prints a CSVs of each test to the serial line at 9600 baud. Each line contains the following values:

```
<exposure#>,<speed>,<minimum speed>,<maximum speed>,<average speed>
```

All speed values are printed as microsecond values. The CSV lines can be pasted to a spreadsheet or some other program for further evaluation. You need to set up the serial monitor of your Arduino IDE to see the serial output.

Pressing the reset button will reset the exposure counter, the speed, the minimum and maximum speeds. This allows for a new
round of tests e. g. on a different shutter speed setting.

<a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-sa/4.0/88x31.png" /></a><br /><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">Shutter Tester README and breadboard view</span> by <span xmlns:cc="http://creativecommons.org/ns#" property="cc:attributionName">c-s-1</span> are licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/">Creative Commons Attribution-ShareAlike 4.0 International License</a>.

The source code is licensed under the MIT license:

```
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
```
