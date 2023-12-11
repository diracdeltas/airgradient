# AirGradient Arduino Library for ESP8266 (Wemos D1 MINI)

This is my fork of https://github.com/airgradienthq/arduino which I used to get a homemade co2 + wifi signal sensor running on my local network (no data sent to airgradient servers). Demo here: https://twitter.com/bcrypt/status/1619041607403323393

To build this, you can mostly follow the instructions from https://www.airgradient.com/open-airgradient/instructions/diy/ (manual flashing) with a few modifications.

1. The only components you need are the Wemos D1 Mini, Wemos OLED display, and Senseair S8 CO2 Sensor.
2. **UPDATE: As of https://github.com/airgradienthq/arduino/commit/9722eda5fa9ae873e4c21733a644718a706e4420, this step is no longer necessary.** After you install the AirGradient libraries in the Arduino IDE, replace `AirGradient.h` and `AirGradient.cpp` in your local library folder (ex: `~/Documents/Arduino/libraries/AirGradient_Air_Quality_Sensor/`) with the versions from this repo.
3. Instead of flashing the AirGradient builtin example, flash `examples/DIY_basic_co2/DIY_basic_co2.ino` from this repo after editing that file's config section to contain your wifi SSID and password.
4. For soldering the components, all you need to do is solder the OLED display to the D1 mini as shown in the AirGradient instructions, solder the pins onto the co2 sensor, and then connect the following pins from the co2 sensor to the d1 mini: `TX -> D4, RX -> D3, G- -> Gnd, G+ -> 5V`. I simply soldered wires between these pins but you can also use a breadboard or whatever you want to connect them.

Once this is done:

1. Plug in the D1 mini
2. It will show you the board ID and then the board's IP address once it connects to wifi. Visit that IP address in your browser to see the metrics if you are on the same wifi network as the board.
3. Or just look at the LED screen and enjoy!
