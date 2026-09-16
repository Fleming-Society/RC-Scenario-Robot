# Omni-Directional Robot with ESP32 and Arduino Motor Shields

## Overview
This project implements a versatile omni-directional robot controlled via an ESP32 microcontroller, featuring four omni-directional wheels each powered by individual DC geared motors. Two Arduino boards, each equipped with motor shields, manage precise control over the wheels, enabling smooth, multi-directional movement such as forward, backward, turning left/right, and diagonal movements.

The ESP32 acts as the central communication hub, hosting a web server for remote control via a user-friendly web interface and managing communications through the I²C protocol to coordinate the Arduino slaves.

## Hardware Components
- **ESP32 Microcontroller**: Manages Wi-Fi connectivity, hosts the web server, and communicates via I²C.
- **Arduino Uno/Nano Boards (x2)**: Function as I²C slave devices controlling the motor shields.
- **Arduino Motor Shields (x2)**: Allow individual control over each DC geared motor, enabling precise directional movement.
- **Omni-Directional Wheels (x4)**: Enable smooth and versatile directional mobility without mechanical steering systems.
- **DC Geared Motors (x4)**: Provide strong and reliable torque to power the robot wheels.

## Communication Protocol
The ESP32 communicates with the Arduino boards via I²C, sending integer command values to instruct wheel movements:
- `1`: Forward
- `2`: Backward
- `3`: Turn Left
- `4`: Turn Right
- `5`: Diagonal Forward Left
- `6`: Diagonal Forward Right
- `7`: Diagonal Backward Left
- `8`: Diagonal Backward Right
- `0`: Stop

## Software Components
- **ESP32 Web Server**: Hosts an intuitive web interface for remote control using WebSockets for real-time responsiveness.
- **Arduino Firmware**: Listens for I²C commands from the ESP32 and controls motor directions and speeds accordingly.

## Web Interface
The robot is controlled via a responsive web interface designed with mobile compatibility. Users interact using directional buttons to command robot movement.

## Setup and Usage
### 1. Hardware Assembly:
- Connect ESP32 to two Arduino controllers via I²C lines (`SDA`, `SCL`).
- Attach motor shields to Arduino boards and connect motors appropriately.

### 2. Firmware Installation:
- Flash the ESP32 code provided in `esp32_code.ino`.
- Flash Arduino boards with respective `arduino1.ino` and `arduino2.ino` firmware.

### 3. Wi-Fi Configuration:
- Update Wi-Fi credentials in ESP32 firmware.
- Connect to the ESP32-hosted Wi-Fi network to access the web interface.

### 4. Running the Robot:
- Navigate to the ESP32's local IP address in your web browser.
- Use directional buttons for real-time control.

## Applications
- Educational robotics
- Remote surveillance platforms
- Automated logistics
- Interactive demonstration robots

## Contributions
Feel free to fork, improve, and contribute to this project by opening pull requests or raising issues for discussion.

## Licensing
Except where a file or notice says otherwise, the original teaching material, firmware and hardware designs are available under the [MIT License](LICENSE).

