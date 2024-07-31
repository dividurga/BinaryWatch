## Directory Structure

- **EEPROMReset**: The firmware uses a flag stored in the EEPROM to see if the RTC needs to be reset. This file resets the flag. Flash this code before using flashing watchFirmwareSTM32.ino .
- **watchFirmwareSTM32.ino**: The main firmware file for the binary watch.
- **watch_STM32 v26.f3z**: Fusion 360 archive file containing the design of the watch, including PCB and 3D models.
- **ReadMe.md**: The current readme file.

### Prerequisites

- [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) or [Arduino IDE](https://www.arduino.cc/en/software) (with STM32 support)
- [Fusion 360](https://www.autodesk.com/products/fusion-360/overview) for 3D modeling and PCB design
- [Arduino IDE](https://www.arduino.cc/en/software) for flashing the firmware
