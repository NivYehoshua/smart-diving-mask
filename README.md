# Smart Diving Mask Embedded System

## About The Project
An embedded system developed for a smart diving mask prototype to monitor, real time dive metrics and ensure diver safety. The system continuously calculates depth, vertical speed and dive time, providing instant visual alerts to prevent Decompression Sickness and Nitrogen Narcosis.

## Key Features
* **Real Time Depth Monitoring:** Converts mechanical pressure into depth data using exponential analog-to-digital calibration (calculated by experiments on FSR).
* **Safety Alert System:** Triggers visual warnings on an LCD for high-risk situations (e.g., crossing the 30m narcosis threshold or exceeding safe ascend rates).
* **Maximum Bottom Time Calculation:** Dynamically calculates allowed dive time based on standard decompression tables.

## Hardware & Technologies
* **Microcontroller:** Arduino Uno (ATmega328P)
* **Language:** C++ (Arduino Framework)
* **Sensors:** FSR (Force Sensing Resistor) for pressure simulation, Potentiometer for initualizing depth.
* **Display:** 16x2 LCD Module.

## System Logic & Architecture
The software architecture is built around a continuous polling loop that gathers sensor data, processes it through physics based algorithms, and feeds it into a state machine safety protocol.

## 📂 Project Structure
* 'smart-mask-code.cpp' - Main application code containing initialization, sensor reading, and the safety state machine.
* 'smart-diving-mask-Project book' - Contains the full technical project book detailing algorithmic logic, hardware integration, and calibration processes.
