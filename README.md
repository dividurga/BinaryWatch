# Binary Watch Project

Welcome to the Binary Watch project! This repository contains all the files related to the design and development of binary watches using STM32 and ATMega328p microcontrollers.

## Project Overview

The Binary Watch is designed to display the time in a "binary but base 10 format". This means each digit of the time in base 10 format is displayed in binary on an LED matrix.

For example, 17:15 is expressed on the LED matrix as:

<pre>
  0   0
  1 0 1
0 1 0 0
1 1 1 1
</pre>

where 1 = LED ON and 0 = LED OFF

## Repository Structure

### watch_STM32
This directory contains the design and firmware for the STM32-based binary watch.

### watch_ATMega328p
This directory contains the design for the ATMega328p-based binary watch.

The firmware from the STM32 version can be modified to work for this hardware. Use the arduino versions of all the libraries and an RTC library for the DS3231 RTC chip

### Setting the Time

- When the code is uploaded for the first time, the RTC time is set to the compile time.
- The current time is displayed on the LED matrix in binary format.
- Everytime you reset or change the battery, the time is set to midnight. This choice is purely aesthetic; I would prefer that my watch always resets to a defined time instead of a random compile time everytime setup() runs. This is achieved by storing a flag in the EEPROM.

## Contributing

Contributions are welcome! I'm still struggling with getting the external oscillator to work with my setup. It works perfectly fine on an external Nucleo board. If you have any suggestions or improvements, please open an issue or submit a pull request.
