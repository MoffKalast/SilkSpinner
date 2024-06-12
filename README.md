# SilkSpinner


[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)


An ESP32 powered lab machine for winding silk cocoons. This repository contains the electronics schematic, firmware and 3D printed parts required for full assembly.

| ![App](docs/motor.jpg) | ![App](docs/stand.jpg)  | 
| ----------- | ----------- |
| ![App](docs/app.jpg) | ![App](docs/estop.jpg) |


## Circuit Diagram

The system runs in an ESP32-WROOM, specifically a devkit V1 wired to the following components as shown:

## Firmware

The ESP firmware is split into two parts: the Arduino control backend and the browser web app that's served from SPIFFS, both linked together by WebSockets for telemetry and control. 

## Mechanical Assembly

The current design uses 30mm aluminium extrusions of the following lengths:

- 2x 900mm (cross bars)
- 2x 1000mm (vertical bars)
- 2x 600mm (stand)
