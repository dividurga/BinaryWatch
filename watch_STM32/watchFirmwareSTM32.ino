/*
This code uses the in-built RTC of the STM32G070KBT6, and then converts this data into a "binary but base 10 format". 
This means that each digit of the time in base 10 format is displayed in binary. 

For example, 17:15 is expressed on the LED matrix as: 
  0   0 
  1 0 1
0 1 0 0
1 1 1 1 

where 1 = LED ON and 0 = LED OFF

This code makes optimizations, like using the lowpower library to conserve battery life, and using the EEPROM memory
to store if the time has been reset already.

Created by Divija Durga, 23 July, 2024
*/

#include <STM32RTC.h>
#include "STM32LowPower.h"
#include <EEPROM.h> // Include EEPROM library

// Get the rtc object
STM32RTC& rtc = STM32RTC::getInstance();

// Define pin constants
const int pin = PA0; // Pin for waking up
const uint8_t col[4] = {PB2, PB1, PB0, PA7}; // Columns of the LED matrix
const uint8_t row[4] = {PA8, PA9, PC6, PA10}; // Rows of the LED matrix

const int eepromAddress = 0; // EEPROM address to store the reset flag

void setup() {
  Serial.begin(9600); // Start serial communication at 9600 baud rate
// Initialize the column and row pins
  for (int i = 0; i < 4; i++) {
    pinMode(col[i], OUTPUT);
    pinMode(row[i], OUTPUT);
  }
  // Set pin as INPUT_PULLUP to avoid spurious wakeup
  pinMode(pin, INPUT_PULLUP);

  // Configure low power mode
  LowPower.begin();
  
  // Select RTC clock source: LSI_CLOCK, LSE_CLOCK or HSE_CLOCK.
  // By default the LSI is selected as source.
  //rtc.setClockSource(STM32RTC::LSE_CLOCK);

  rtc.begin(); // Initialize RTC in 24H format
  // Check if the RTC time has been set before
  if (EEPROM.read(eepromAddress) != 1) {
    // RTC time has not been set, set it to compile time
    const char compileTime[] = __TIME__;
    int hour, minute, second;
    sscanf(compileTime, "%d:%d:%d", &hour, &minute, &second);

    rtc.setHours(hour);
    rtc.setMinutes(minute);
    rtc.setSeconds(second);

    // Write flag to EEPROM indicating that the time has been set
    EEPROM.write(eepromAddress, 1);
    //EEPROM.commit();
  } else {
    // RTC time has been set before, set it to 00:00
    rtc.setHours(0);
    rtc.setMinutes(0);
    rtc.setSeconds(0);
  }

  // Clear the LED matrix
  clearMatrix();

  // Attach an interrupt to wake up the device and print time
  LowPower.attachInterruptWakeup(pin, printTime, RISING, SLEEP_MODE);
}

void loop() {
  // Empty loop
}

// Print date and time
void printTime() {
  // Print time
  Serial.printf("%02d:%02d:%02d.%03d\n", rtc.getHours(), rtc.getMinutes(), rtc.getSeconds(), rtc.getSubSeconds());

  // Display hours and minutes in binary on LED matrix
  uint8_t hour = rtc.getHours();
  uint8_t min = rtc.getMinutes();

  // Set LED for hour tens position
  binaryColumn(0, hour / 10);
  // Set LED for hour ones position
  binaryColumn(1, hour % 10);
  // Set LED for minutes tens position
  binaryColumn(2, min / 10);
  // Set LED for minutes ones position
  binaryColumn(3, min % 10);

  // Display for two seconds
  delay(2000);
  clearMatrix();
}

// Set a specific LED in the matrix
void setLED(uint8_t rowNum, uint8_t colNum) {
  digitalWrite(col[colNum], HIGH); // Set column HIGH
  digitalWrite(row[rowNum], LOW);  // Set row LOW
}

// Clear the LED matrix
void clearMatrix() {
  // Set all columns to LOW and all rows to HIGH to clear the matrix
  for (int i = 0; i < 4; i++) {
    digitalWrite(col[i], LOW);
    digitalWrite(row[i], HIGH);
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
    uint8_t bit = (num >> i) & 1; // Get the bit at position i
    if (bit == 1) {
      setLED(i, colPin); // Turn on the LED if the bit is 1
      // For debugging purposes
      Serial.println("(" + String(i) + ", " + String(colPin) + ")");
    }
  }
}
