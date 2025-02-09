<p align="center">
  <img src="https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/malhar-c/46292c13ab8c70e68aa9619cad1d9560/raw/esp32-homekit-SmartRoom-build.json" alt="Build Status">
  <img src="https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/malhar-c/46292c13ab8c70e68aa9619cad1d9560/raw/esp32-homekit-ota.json" alt="OTA Uploads">
</p>

# Homespan-ESP32-SmartRoom

## Overview

Homespan-ESP32-SmartRoom is an ESP32-based smart home project powered by the HomeSpan framework. This firmware brings Apple's HomeKit to your room by integrating multiple smart accessories such as lights, fans, AC units, and motion sensors. Developed with PlatformIO and supporting Over-The-Air (OTA) uploads, this project is designed for modularity and ease of future expansion — eliminating the need to physically connect the switch board to a PC for code changes and updates.

## Features

- **HomeKit Integration**: Seamlessly control and monitor your smart room via Apple HomeKit.
- **Multiple Accessories**: Manage lights, fans, AC, motion sensors, and more.
- **Motion Sensing**: Utilizes a PIR sensor for reliable motion detection.
- **OLED Display**: Provides real-time status information including time, temperature, and accessory states.
- **AC Control**: Converts a conventional (dumb) AC unit into a smart device using IR commands (see AC Control section for details).
- **OTA Uploads**: Supports firmware uploads over the air with binary size checks to ensure compatibility without the need for direct USB connection.
- **IR Remote Control**: Incorporates IR communication for remote control functionality.
- **Extensible Codebase**: Easily add or modify features with a modular project structure.

## Setup & Installation

### Hardware Requirements

- ESP32 development board (recommended: esp32doit-devkit-v1)
- PIR motion sensor (connected to GPIO 13)
- DHT sensor (connected to GPIO 23)
- OLED Display (SSD1306 – I2C based)
- IR LED and receiver for remote control (IR LED connected to GPIO 4)
- Manual switch connected to GPIO 18
- Required wiring and power supplies

### Pin Diagram

#### ESP32 Connections:

- **PIR Sensor**: GPIO 13 (as defined in `pins_config.h`).
- **DHT Sensor**: GPIO 23.
- **IR LED**: GPIO 4.
- **OLED Display**: Uses I2C; typically connect SDA to GPIO 21 and SCL to GPIO 22.
- **Manual Switch**: GPIO 18 (labeled as `man_sw_1`).

#### PCF8574 I/O Expanders:

- **PCF8574 (Address 0x38)**: Controls output relays:
  - P0: Light Pair 1
  - P1: Light Pair 2
  - P2: Tube Light
  - P3: Night Light
  - P4–P6: Fan control outputs.
- **PCF8574 (Address 0x3A)**: Reads inputs from:
  - sw2_input, sw3_input, sw4_input, sw5_input (switches)
  - rot_1_input, rot_2_input, rot_3_input, rot_4_input (rotary encoder positions)

### AC Control

This project includes an AC control module that transforms a conventional (dumb) AC unit into a smart device. Using an IR LED (connected to GPIO 4) along with the `IRremoteESP8266` library, the system sends predefined infrared signals to control the AC unit. Key aspects include:

- **IR Transmission:** The firmware sends IR commands via the IR LED to toggle power, adjust temperature, select operation modes, and control fan speed.
- **Command Mapping:** Specific IR codes for AC functions (such as power, temperature, mode, and fan settings) are programmed into the firmware.
- **HomeKit Integration:** The AC control service is integrated with HomeSpan, enabling HomeKit commands to automatically trigger the corresponding IR transmissions.
- **Dual Control:** The AC can be controlled both manually (via an IR remote) and automatically through HomeKit, providing flexibility and redundancy.

#### Detailed AC Control Configurations

The IR AC control module is configured in the `DEV_AC.h` file with the following settings:

- **IR Transmission:**

  - The IR LED is connected to GPIO 4 and used with the `IRremoteESP8266` library to send signals.
  - The `IRHitachiAc1` class is used to issue commands following Hitachi AC protocols.

- **Initialization (in `setup_init()`):**

  - The DHT sensor (DHT11) connected to GPIO 23 is initialized using `dht.begin()`.
  - The IR transmitter is started with `ac.begin()`.
  - The AC is initially turned off with `ac.off()`.
  - Default settings are applied:
    - Fan mode is set to auto using `ac.setFan(kHitachiAc1FanAuto)`.
    - Operation mode is set to auto using `ac.setMode(kHitachiAc1Auto)`.
    - Temperature is configured to 25°C with `ac.setTemp(25)`.
    - Vertical swing is enabled via `ac.setSwingV(true)`.

- **HomeKit Integration:**

  - The AC control feature is integrated into HomeSpan as a `HeaterCooler` service.
  - This mapping allows HomeKit commands to control the AC (power, temperature, and fan speed) by triggering the corresponding IR transmissions.

- **State Management:**
  - Various global variables (e.g., `AC_Fan_speed`, `AC_Fan_state`, `AC_Swing_mode`, etc.) maintain the current status of the AC unit and fan configuration.
  - The system supports dual control, accommodating both automatic adjustments based on sensor data and manual control via IR remote commands.

### Software Requirements

- [PlatformIO](https://platformio.org/) with the PlatformIO IDE (recommended)
- Git for version control
- A GitHub account to manage repository and CI/CD workflows

### Dependencies

All required libraries are listed in the `platformio.ini` file:

- **HomeSpan** – HomeKit accessory framework
- **DHT sensor library** – For temperature and humidity measurements
- **IRremoteESP8266** – Infrared communication library
- **Adafruit SSD1306** and **Adafruit GFX Library** – For OLED display control
- **PCF8574 Library** – For I2C port expander support

### Project Configuration

- The `platformio.ini` file is preconfigured for the ESP32 board, including OTA partition settings and upload configurations.
- OTA uploads use the `espota` protocol, with partition and size checks integrated into the CI workflow.
- GitHub Actions are set up to build your project and update status badges automatically.

### Building & Uploading Firmware

1. **Clone the Repository:**

   ```bash
   git clone https://github.com/your-username/Homespan-ESP32-SmartRoom.git
   cd Homespan-ESP32-SmartRoom
   ```

2. **Build the Project:**

   ```bash
   pio run
   ```

3. **Upload the Firmware:**
   - **Via USB:**
     ```bash
     pio run --target upload
     ```
   - **OTA Upload:**
     Ensure your device is connected to the network and its IP is correctly set in `platformio.ini` before running:
     ```bash
     pio run --target upload
     ```

## Continuous Integration (CI)

The project uses GitHub Actions with the following features:

- **Build Status Badge:** Indicates whether the current build is successful.
- **OTA Uploads Badge:** Displays OTA upload compatibility based on the firmware binary size.
- The CI workflow is defined in `.github/workflows/platformio.yml`, which also checks OTA partition sizes and updates badges using a GitHub Gist.

## Acknowledgments

- [HomeSpan](https://github.com/HomeSpan/HomeSpan) – Enabling seamless HomeKit integration.
- Adafruit – For providing robust libraries and support for OLED displays and sensors.
- The open source community – For continuous contributions and support.
