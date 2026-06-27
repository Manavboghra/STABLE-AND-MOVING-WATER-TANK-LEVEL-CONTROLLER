# Smart Water Tank Monitoring System

This project is an Arduino-based Smart Water Tank Monitoring and Automatic Pump Control System designed to improve water management and safety. It continuously monitors the water level using an ultrasonic sensor, measures water temperature with a TMP36 sensor, and detects tank tilt using a tilt sensor.

The system automatically turns the water pump ON when the water level falls below the minimum threshold and OFF when the desired level is reached. It also provides visual and audible alerts for low water level, overflow, high temperature, and tank tilt conditions through an I2C LCD display and buzzer.

## Features
- Automatic water pump control
- Real-time water level monitoring
- Temperature monitoring (TMP36)
- Tank tilt detection
- Overflow and low-water detection
- Audible buzzer alerts
- 16×2 I2C LCD status display
- Energy-efficient automatic operation

## Components Used
- Arduino Uno
- HC-SR04 Ultrasonic Sensor
- TMP36 Temperature Sensor
- Tilt Sensor
- Relay Module
- DC Water Pump
- Passive Buzzer
- 16×2 I2C LCD
- Jumper Wires and Breadboard
