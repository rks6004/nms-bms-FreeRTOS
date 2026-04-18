# HV BMS Software

## Table of Contents

- [Introduction](#introduction)
- [Folder Structure](#folder-structure)
- [Setup and Installation](#setup-and-installation)
- [Details](#details)
- [Contributing](#contributing)
- [License](#license)

## Introduction

This folder contains the source code for the main controller of the High Voltage BMS. The system architecture will involve a main controller to manage the BMS and satellite battery monitors (kindly provided by Tesla and Analog Devices) that will perform the actuall voltage, current, and temperature measurements. For further details on the chips and the system please consult the documentation at `nms-electonics/hv-electronics/hv-bms/bms-docs`


## Folder Structure

- `/Core`: Contains the main code used to initialize the MCU
- `/Drivers`: Internal STM32 drivers
- `/Drivers_Custom`: Custom drivers to allow the MCU to interact with additional external components
- `/Drivers_Custom/ADBMS6830`:  Device driver to interact with the BMS chip ADBMS6830


## Setup and Installation

Please follow the Setup and Installation guide for Software Development in `nms-electronics\lv-electronics\README.md`

## Contributing

Follow the guidelines highlighed in `nms-electronics/README.md`

## License
If we ever establish licensing then list those here.


