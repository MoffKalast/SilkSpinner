# SilkSpinner

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

An ESP32 powered lab machine for automatic silk cocoons winding. This repository contains the electronics schematic, firmware and 3D printed parts required for full assembly.

The system was designed for the purpose of winding single silkworm cocoons onto sample bobbins for fiber analisys. The current iteration can monitor, drive, and log data for two separete instances at the same time.

The project was developed in cooperation with the Faculty of Natural Sciences and Engineering, University of Ljubljana.

| ![App](docs/motor.jpg) | ![App](docs/stand.jpg)  | 
| ----------- | ----------- |
| ![App](docs/app.jpg) | ![App](docs/estop.jpg) |

## Functonality

The system implements the following functionality:

- guide wheels on bearings to line up and stretch the string

- servo actuators that guide the string onto specific positions of the bobbin, with constant sinusoidal movement that prevents the string from sticking to itself

- automatic sample taking by winding a set distance to a set section of the bobbin (e.g. five samples of 200m, each wound 1 cm apart)

- manual servo control

- smooth start and stop movements for servos and main motors to emulate gentle hand-winding

- adjustable throttle and direction for main motors, speed and angle for actuators

- saving measurement distances and metadata to a `.csv` file and downloading them to the client machine from the web app

- an emergency stop push button, which quickly stops all motors



https://github.com/MoffKalast/SilkSpinner/assets/9977799/55012754-9cc3-48a4-b408-5423bcaef4a0



## Circuit Diagram

The system runs in an ESP32-WROOM, specifically a devkit V1 wired to the following components as shown:

![App](docs/diagram.png)

```
#define PIN_SERVO_RIGHT 13
#define PIN_SERVO_LEFT 27

#define PIN_MOTOR1_RIGHT 25
#define PIN_MOTOR2_RIGHT 26

#define PIN_MOTOR1_LEFT 32
#define PIN_MOTOR2_LEFT 33

#define PIN_HALL_RIGHT 34
#define PIN_HALL_LEFT 35

#define PIN_ESTOP 18
```

## Firmware

The ESP firmware is split into two parts: the Arduino control backend and the browser web app that's served from SPIFFS via a WiFi hotspot, both linked together by WebSockets for telemetry and control. 

#### 1. Install Arduino IDE and the following plugins/libraries
- [ESP32](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/)
- [SPIFFS](https://github.com/me-no-dev/arduino-esp32fs-plugin)
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
- [AsyncTCP](https://github.com/me-no-dev/AsyncTCP)
- [Arduino_JSON](https://github.com/arduino-libraries/Arduino_JSON)
- [ESP32Servo](https://github.com/madhephaestus/ESP32Servo)

#### 2. Write the web app to flash

Running the [ESP32 Sketch Data Upload](https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/) should upload everything in the /data directory that's not part of the main sketch.

#### 3. Build and upload the sketch

As normal, upload the sketch and reset the ESP32. The Wifi hotspot should show up automatically with the SSID `SilkSpinner` and password `sviloprejka`. The web app can be accessed in a browser at `http://192.168.4.1`

## Mechanical Assembly

The current design uses the following BoM of aluminium extrusions and mounting pieces:

- 2x 30x30 U8 900 mm (cross bars)
- 2x 30x30 U8 1000 mm (vertical bars)
- 2x 30x30 U8 600 mm (stand)
- 8x 30x30 right angle bracket
- 12x U8 M4 mounting stone
- 12x M4 L40 cylinderical bolt 
- 16x M6 M12 conical head bolt
- 16x U8 M6 hammerhead bolt

![App](docs/aluminiumschematic.jpg)

The mounting heights for the guide wheel crossbar and servo crossbar are adjustable per-experiment.

## Cite

If you're using the Silk Spinner for your research, please cite:

```bibtex
@article{TBD}
``` 

-------------
<img src="docs/ntf.png" width="400" />
