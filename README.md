# 🚗 Smart Parking System Using STM32 Nucleo-F446RE

An embedded systems project that monitors parking slot availability using ultrasonic sensors, displays real-time parking information on an OLED display, and indicates slot status using RGB LEDs. The system also monitors temperature and humidity and incorporates a servo motor for automated parking gate control.


### ✨ Features

- **Four-slot parking detection** using HC-SR04 ultrasonic sensors.
- **Real-time parking status** displayed on an SSD1306 OLED.
- **RGB LED indicators** for individual parking slots.
  - 🔴 Red — Slot occupied.
  - 🟢 Green — Slot vacant.
- **Temperature and humidity monitoring** using a DHT sensor.
- **Temperature graph** displayed on the OLED.
- **Automatic gate control** using an SG90 servo motor.
- **Sensor filtering** using consecutive readings and separate occupied/vacant thresholds to reduce false state changes.

## 🛠️ Hardware Requirements

| Component | Quantity | Purpose |
|---|---:|---|
| STM32 Nucleo-F446RE | 1 | Main controller |
| HC-SR04 ultrasonic sensor | 4 | Detect vehicles in parking slots |
| SSD1306 OLED display (128 × 64, I²C) | 1 | Display parking status and sensor data |
| RGB LED module | 4 | Indicate individual slot availability |
| 220 Ω resistors | 8 | Limit LED current |
| DHT11 temperature and humidity sensor | 1 | Monitor environmental conditions |
| SG90 servo motor | 1 | Control the parking gate |
| Breadboard and jumper wires | As required | Circuit connections |
| Suitable power supply | As required | Power the board and peripherals |

> **Note:** The hardware and software must use the same DHT sensor model. If your circuit uses a DHT22 instead of a DHT11, configure the corresponding sensor type in the code.

## 🔌 Pin Configuration

The following table describes the pin assignments for the project.

### 1. Ultrasonic Sensors

| Parking Slot | TRIG Pin | ECHO Pin |
|---|---|---|
| Slot 1 | A0 | A1 |
| Slot 2 | A2 | A3 |
| Slot 3 | A4 | A5 |
| Slot 4 | D10 | D11 |

Each HC-SR04 measures the distance to an object in its corresponding parking slot. The measured distance is used to determine whether the slot is occupied.

### 2. RGB LED Indicators

The LEDs use a common-anode configuration.

| Parking Slot | Red LED | Green LED |
|---|---|---|
| Slot 1 | D2 | D3 |
| Slot 2 | D4 | D5 |
| Slot 3 | D6 | D7 |
| Slot 4 | D8 | D9 |

Each LED channel is connected through a current-limiting resistor.

### 3. OLED Display

| OLED Pin | STM32 Pin |
|---|---|
| VCC | 3.3 V |
| GND | GND |
| SDA | D14 |
| SCL | D15 |

The OLED communicates with the STM32 using the I²C protocol. Its configured I²C address is `0x3C`.

### 4. Temperature and Humidity Sensor

| Sensor Pin | STM32 Pin |
|---|---|
| VCC | 3.3 V, if supported by the sensor |
| GND | GND |
| DATA | D12 |

The sensor provides temperature and humidity readings for display and monitoring.

### 5. Servo Motor

| Servo Pin | STM32 Pin / Connection |
|---|---|
| PWM / Signal | D13 |
| V+ | Suitable servo power supply |
| GND | Common ground |

The servo motor is used to control the parking gate. The gate movement depends on the control logic implemented in the firmware.

## ⚙️ Working Principle

The system operates through the following steps:

1. **Distance measurement:** Each ultrasonic sensor measures the distance between the sensor and a nearby object.
2. **Slot classification:** The controller compares the measured distance with predefined thresholds to determine whether a vehicle is present.
3. **Noise reduction:** Multiple consecutive readings are used to confirm a change in parking status.
4. **LED indication:** The corresponding LED changes to red when a slot is occupied and green when it is vacant.
5. **OLED update:** The display presents parking availability and environmental readings.
6. **Environmental monitoring:** The DHT sensor provides temperature and humidity data.
7. **Gate operation:** The servo motor controls the parking gate according to the implemented control logic.

## 📏 Parking Detection Logic

The parking detection logic uses separate thresholds for occupied and vacant states.

| Parameter | Value | Description |
|---|---:|---|
| Occupied threshold | 15 cm | A vehicle is detected when the measured distance falls below this threshold |
| Vacant threshold | 20 cm | A slot is considered vacant when the measured distance exceeds this threshold |
| Confirmation count | 3 readings | Consecutive readings are used to confirm a state change |

The difference between the occupied and vacant thresholds provides **hysteresis**, helping prevent rapid switching when a measured distance fluctuates around a single threshold.

Distances between the two thresholds retain the previously confirmed state.

## 💻 Software Requirements

- [Visual Studio Code](https://code.visualstudio.com/)
- [PlatformIO IDE](https://platformio.org/)
- STM32 platform support through PlatformIO
- USB connection for programming and debugging

### Libraries

The project may use the following Arduino-compatible libraries, depending on the firmware version:

- `Wire.h` — I²C communication
- `Adafruit_GFX` — Graphics primitives
- `Adafruit_SSD1306` — OLED display control
- DHT sensor library, if used by the firmware

The servo implementation depends on the library or timer-based PWM method used in the source code.

## 🚀 Getting Started

### 1. Clone the Repository

Open a terminal and run:

```bash
git clone https://github.com/noxibwoi/stm32-smart-parking
```


### 2. Open the Project

1. Open Visual Studio Code.
2. Install PlatformIO IDE if it is not already installed.
3. Open the cloned project folder.

### 3. Check the Configuration

Verify the board configuration in `platformio.ini`. It must match the actual development board and framework used by the project.

For example, a PlatformIO project using the Arduino framework may have a configuration similar to:

```ini
[env:nucleo_f446re]
platform = ststm32
board = nucleo_f446re
framework = arduino
monitor_speed = 115200
```

Use this configuration only if it matches your existing project setup.

### 4. Connect the Hardware

Connect the ultrasonic sensors, OLED, LEDs, environmental sensor, and servo motor according to the pinout tables above.

Before powering the circuit, verify the wiring and voltage requirements of every component.

### 5. Build and Upload

In PlatformIO:

1. Build the project.
2. Connect the Nucleo-F446RE through USB.
3. Upload the firmware.
4. Open the serial monitor if serial debugging is enabled.

### 6. Test the System

Place an object in front of each ultrasonic sensor and check that:

- The corresponding parking slot changes status.
- The correct LED illuminates.
- The OLED displays the updated parking information.
- Temperature and humidity readings are reasonable.
- The servo responds according to the programmed gate-control logic.

## ⚠️ Hardware Considerations

- **Ultrasonic sensors:** Standard HC-SR04 modules generally require a 5 V supply. Their ECHO outputs can reach 5 V, so use a suitable voltage divider or level shifter before connecting them to STM32 GPIO pins.
- **Power supply:** Do not assume that the Nucleo board's 3.3 V output can power every peripheral. Check the current requirements of the sensors and servo.
- **Servo motor:** Use an appropriate external supply for the SG90 when required, and connect its ground to the STM32 ground.
- **Common-anode LEDs:** The common anode connects to the positive supply. The GPIO logic for turning the LEDs on and off must account for the common-anode configuration.
- **I²C display:** Confirm the OLED's I²C address and the selected SDA/SCL pins.
- **Sensor model:** Ensure the DHT sensor type selected in the code matches the physical sensor.

## 🔮 Possible Future Improvements

- Add an entrance counter to track the number of available spaces.
- Add Wi-Fi connectivity for remote parking-status monitoring.
- Develop a mobile or web dashboard.
- Add RFID-based vehicle access control.
- Implement fault detection for disconnected or malfunctioning sensors.
- Add data logging for parking utilization analysis.


## 📚 Project Information

**Project:** Smart Parking System  
**Microcontroller:** STM32 Nucleo-F446RE  
**Programming Environment:** Visual Studio Code and PlatformIO  
**Main Technologies:** Embedded C/C++, ultrasonic sensing, I²C communication, OLED graphics, GPIO control, and servo actuation

---

*This project demonstrates the application of embedded systems and sensor-based automation to improve parking-space monitoring and management.*
