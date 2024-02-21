
Please visit the article on Hackster.io accompanying this example script:
https://www.hackster.io/sensirion-software/easily-read-out-sensirion-sensor-measurements-on-arduino-9c1862

For more information about this library, refer to the README two levels up:

```
Sensirion UPT I2C Auto Detection
├── examples
│   ├── advancedUsage
│   │   └── ...
│   ├── basicUsage
│   │   └── ...
│   └── hacksterExample
│       ├── hacksterExample.ino
│       ├── platformio.ini
│       └── README.md               <=== YOU ARE HERE
├── ...
├── README.md                       <=== THIS ONE
└── src
    └── ...
```
(Also available here: https://github.com/Sensirion/arduino-upt-i2c-auto-detection)

## How to use
Follow the wiring instructions on the Hackster article, and execute the code with the steps outlined below:
### Arduino IDE
Install the software from the [official website](https://www.arduino.cc/en/software) and read [this short tutorial](https://docs.arduino.cc/software/ide-v2/tutorials/getting-started-ide-v2/) to get an introduction to the IDE.
Next, select your board and port in the Board Manager by following [these instructions](https://support.arduino.cc/hc/en-us/articles/4406856349970-Select-board-and-port-in-Arduino-IDE).
Then, we'll need to [install all the libraries](https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries/) required to run Autodetection. In the library manager, search for the _Sensirion UPT I2C Auto Detection_ library. Be sure to select to install all dependencies, lest you'll have to add the following manually:
* [Sensirion Arduino Core](https://www.arduino.cc/reference/en/libraries/sensirion-core/)
* [Sensirion UPT Core](https://www.arduino.cc/reference/en/libraries/sensirion-upt-core/)
* [Sensirion I2C SCD4x](https://www.arduino.cc/reference/en/libraries/sensirion-i2c-scd4x/)
* [Sensirion I2C SFA3x](https://www.arduino.cc/reference/en/libraries/sensirion-i2c-sfa3x/)
* [Sensirion I2C SVM4x](https://www.arduino.cc/reference/en/libraries/sensirion-i2c-svm4x/)
* [Sensirion I2C SHT4x](https://www.arduino.cc/reference/en/libraries/sensirion-i2c-sht4x/)
* [Sensirion I2C SEN5x](https://www.arduino.cc/reference/en/libraries/sensirion-i2c-sen5x/)
* [Sensirion I2C SCD30](https://www.arduino.cc/reference/en/libraries/sensirion-i2c-scd30/)
* [Sensirion I2C SGP41](https://www.arduino.cc/reference/en/libraries/sensirion-i2c-sgp41/)
* [Sensirion I2C STC3x](https://www.arduino.cc/reference/en/libraries/sensirion-i2c-stc3x/)

Then, open the example either by opening `hacksterExample.ino` in Arduino IDE or by navigating to 

    File => Examples => Sensirion UPT I2C Auto Detection => hacksterExample

Finally, click the Upload button (top-left of the window, labelled with an → arrow) and then open the serial monitor (Q-looking button at the top-right of the window). Be sure to select `115200 baud` in the drop-down to the right of the serial monitor.

### PlatformIO
The most straight-forward way to use [PlatformIO](https://platformio.org/platformio-ide) is as an extension to Microsoft's [Visual Studio Code](https://code.visualstudio.com/), you'll find it easily among the extensions available for it. I'll refer to the official installation instructions [here](https://platformio.org/install/ide?install=vscode).

Open this folder (`hacksterExample.ino`) in the IDE and type
```bash
$ pio run -t upload && pio device monitor
```
in the command line (which you can open with `ctrl+J`). The measurements start showing in the console.
