# Dual Access Mobile Charging System

## Overview

An Arduino-based mobile charging system designed with two access methods: coin-based and Bluetooth-based control.

## Features

- Coin-based access
- Bluetooth-based access
- Automatic charging time control
- Relay-based switching
- 16x2 I2C LCD display
- Coin pulse detection
- Timer-based operation
- System reset through Bluetooth

## Technologies Used

- Arduino UNO
- Embedded C / Arduino C++
- I2C LCD
- Bluetooth Module
- Coin Acceptor
- Relay Module
- LED
- SoftwareSerial

## Working

The system provides two ways to activate mobile charging. Users can insert a supported coin or send a Bluetooth command from a mobile device.

The coin pulses are detected using an interrupt. Bluetooth commands are used to add different credit values. Based on the credited amount, the relay remains active for a predefined charging duration.

The LCD displays the credit and remaining charging time.

## Control Commands

| Command | Credit |
|---|---:|
| A | Rs 1 |
| B | Rs 2 |
| C | Rs 5 |
| D | Rs 10 |
| E | Reset |

## Project Structure

src/
└── dual_access_mobile_charging.ino

results/
└── Simulation / Output Images

##Results

The system was tested for coin detection, Bluetooth control, relay operation, LCD display, and automatic time-based switching.
