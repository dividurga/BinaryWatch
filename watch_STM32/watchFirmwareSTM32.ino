#include <STM32RTC.h>
#include "STM32LowPower.h"

// Get the rtc object
STM32RTC& rtc = STM32RTC::getInstance();

// Define pin constants
const int pin = PA0; // Pin for waking up
const uint8_t col[4] = {PB2, PB1, PB0, PA7}; // Columns of the LED matrix
const uint8_t row[4] = {PA8, PA9, PC6, PA10}; // Rows of the LED matrix

void setup() {
  Serial.begin(9600); // Start serial communication at 9600 baud rate
  pinMode(LED_BUILTIN, OUTPUT); // Set built-in LED pin as output

  // Set pin as INPUT_PULLUP to avoid spurious wakeup
  pinMode(pin, INPUT_PULLUP);

  // Configure low power mode
  LowPower.begin();
  
  // Select RTC clock source: LSI_CLOCK, LSE_CLOCK or HSE_CLOCK.
  // By default the LSI is selected as source.
  //rtc.setClockSource(STM32RTC::LSE_CLOCK);

  rtc.begin(); // Initialize RTC in 24H format

  // Extract compile date and time
  const char compileTime[] = __TIME__;

  int hour, minute, second;
  
  

  // Parse the compile time
  sscanf(compileTime, "%d:%d:%d", &hour, &minute, &second);

  // Set RTC time with compile time
  rtc.setHours(hour);
  rtc.setMinutes(minute);
  rtc.setSeconds(second);

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
