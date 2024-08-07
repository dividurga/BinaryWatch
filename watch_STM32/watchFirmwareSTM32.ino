/*
This code uses the in-built RTC of the STM32G070KBT6, and then converts this data into a "binary but base 10 format". 
This means that each digit of the time in base 10 format is displayed in binary. 

For example, 17:45 is expressed on the LED matrix as: 
  0   0 
  1 1 1
0 1 0 0
1 1 0 1 

where 1 = LED ON and 0 = LED OFF

This code makes optimizations, like using the lowpower library to conserve battery life, and using the EEPROM
to store if the time has been reset already.

Created by Divija Durga, 31 July, 2024
*/


#include <STM32RTC.h>
#include "STM32LowPower.h"
#include <EEPROM.h>  // Include EEPROM library

bool awake = false;  // Flag to indicate if the device is awake
uint8_t hour;
uint8_t minutes;
int onTime = 3000;  // Time to keep the LED matrix on
unsigned long sleepTimer;  // Timer to track sleep time

// Get the RTC object
STM32RTC& rtc = STM32RTC::getInstance();

// Define pin constants for waking up and LED matrix columns/rows
const int pin = PA6;                             // Pin for waking up
const uint8_t col[4] = { PB2, PB1, PB0, PA7 };   // Columns of the LED matrix
const uint8_t row[4] = { PA8, PA9, PC6, PA10 };  // Rows of the LED matrix

const int eepromAddress = 0;               // EEPROM address to store the reset flag
void setup() {
  Serial.begin(9600);  // Start serial communication at 9600 baud rate
  
  // Initialize the column and row pins
  for (int i = 0; i < 4; i++) {
    pinMode(col[i], OUTPUT);
    pinMode(row[i], OUTPUT);
  }
  
  // Set pin as INPUT_PULLUP to avoid spurious wakeup
  pinMode(pin, INPUT_PULLUP);
  
  // Clear the LED matrix
  clearMatrix();
  
  // Configure low power mode
  LowPower.begin();

  // Select RTC clock source: LSI_CLOCK, LSE_CLOCK or HSE_CLOCK.
  // By default the LSI is selected as source.
  rtc.setClockSource(STM32RTC::LSE_CLOCK);
  rtc.begin();  // Initialize RTC in 24H format

  // Check if the RTC time has been set before
  if (EEPROM.read(eepromAddress) != 1) {
    // RTC time has not been set, set it to compile time
    const char compileTime[] = __TIME__;
    int hour, minute, second;
    sscanf(compileTime, "%d:%d:%d", &hour, &minute, &second);

    rtc.setHours(hour);
    rtc.setMinutes(minute);
    rtc.setSeconds(second);
    EEPROM.write(eepromAddress, 1);  // Write flag to EEPROM indicating that the time has been set

  } else {
    // RTC time has been set before, set it to 00:00
    rtc.setHours(0);
    rtc.setMinutes(0);
    rtc.setSeconds(0);
  }
  
  // Clear the LED matrix again
  clearMatrix();

  // Attach an interrupt to wake up the device and print time
  LowPower.attachInterruptWakeup(pin, printTime, RISING, SLEEP_MODE);
}

void loop() {
  // Check if the device is awake
  if (awake) {
    // Check if the LED display time is still valid
    if (millis() - sleepTimer < onTime) {
      // Set LED for hour tens position
      binaryColumn(0, hour / 10);
      // Set LED for hour ones position
      binaryColumn(1, hour % 10);
      // Set LED for minutes tens position
      binaryColumn(2, minutes / 10);
      // Set LED for minutes ones position
      binaryColumn(3, minutes % 10);
    } else {
      // Turn off the LED display
      awake = false;
    }
  }

  // Put the device to sleep if it is not awake
  if (!awake) {
    LowPower.sleep();
  }
}

// Print date and time
void printTime() {
  hour = rtc.getHours();
  minutes = rtc.getMinutes();
  sleepTimer = millis();  // Start the sleep countdown
  awake = true;  // Flag to start LED matrix
}

// Set a specific LED in the matrix
void setLED(uint8_t rowNum, uint8_t colNum) {
  digitalWrite(col[colNum], HIGH);  // Set column HIGH
  digitalWrite(row[rowNum], LOW);   // Set row LOW
}

// Clear the LED matrix
void clearMatrix() {
  // Set all columns to LOW and all rows to HIGH to clear the matrix
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      digitalWrite(col[i], LOW);
      digitalWrite(row[j], HIGH);
    }
  }
}

// Display a binary number on a specific column of the LED matrix
void binaryColumn(uint8_t colPin, uint8_t num) {
  int numLEDs;
  // Determine number of LEDs in the column based on the column position
  if (colPin == 0) {
    numLEDs = 2;
  } else if (colPin == 1 || colPin == 3) {
    numLEDs = 4;
  } else if (colPin == 2) {
    numLEDs = 3;
  }

  // Set LEDs based on the binary representation of the number
  for (uint8_t i = 0; i < numLEDs; i++) {
    uint8_t bit = (num >> i) & 1;  // Get the bit at position i
    if (bit == 1) {
      setLED(i, colPin);  // Turn on the LED if the bit is 1
      delayMicroseconds(300); // Persistence of vision
      clearMatrix();  // Clear the matrix after each bit
    }
  }
}
