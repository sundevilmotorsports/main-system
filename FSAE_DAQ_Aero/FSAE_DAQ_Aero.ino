#include <BitBang_I2C.h>
#include <SPI.h>
#include <SD.h>
#include "Adafruit_MPRLS.h"
#include <Wire.h>

File dataSD;

// change this to match your SD shield or module;
const int chipSelect = 53;
String strSDName;
int intRunNumber;
long time; // Time in milliseconds
int intI2CLoop;

// Pitot tube I2C info
#define PITOT_ADDRESS 0x28 // 7-bit address
#define VALUE_COUNT 4 // Look for this many values

// Use ports 20 (SDA) and 21 (SCL) for I2C on Mega 2560.
// https://github.com/PX4/PX4-Autopilot/blob/master/src/drivers/differential_pressure/ms4525/ms4525_airspeed.cpp

// Pressure transducer bit bang info
#define PRESSURE_ADDRESS 0x18

#define SDA_PIN 31
#define SCL_PIN 30
int pressure_hPA;
uint8_t* pu8Data;

BBI2C bbi2c;



// Constants and variables for Pitot tube
const float P_min = -1.0f;
const float P_max = 1.0f;
const float PSI_to_Pa = 6894.757f;
const int ARRAY_LENGTH = 7;
const float conversionFactor = 2.23694;
const float offset = 28.4;

float press_values[ARRAY_LENGTH];
//float diff_press_filtered;
int loopCount = 0;
float filteredPressure;
float airspeed;
float airDensity = 1.225;

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  Wire.begin();

  bbi2c.bWire = 0; // use bit bang, not wire library
  bbi2c.iSDA = SDA_PIN;
  bbi2c.iSCL = SCL_PIN;
  I2CInit(&bbi2c, 100000L);

  
  if (!SD.begin()) {
    Serial.println("SD card initialization failed!");
    return;
  }

  strSDName = "run1.txt";
  intRunNumber = 1;

  while (SD.exists(strSDName)) { // Find a file name that doesn't exist
    intRunNumber += 1;
    strSDName = "run" + String(intRunNumber) + ".txt"; // Create the next file name to check.
    // If it doesn't exist, we will simply leave this as the SD card file name that we use.
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  dataSD = SD.open(strSDName, O_CREAT | O_WRITE);
  dataSD.println("Time (ms), Pressure Diff (Pa), Temperature (C), Airspeed (mph);");
  dataSD.close();

}

float value1;
float value2;

int maxIndex(float input_array[]) { // Find the index of the maximum value of an array
  int max_value = input_array[0];
  int max_value_index = 0;
  for (int i = 1; i < ARRAY_LENGTH; i++) {
    value1 = input_array[i];
    if (value1 > max_value) {
      max_value = value1;
      max_value_index = i;
    }
  }
  return max_value_index;
}

int minIndex(float input_array[]) { // Find the index of minimum value of an array
  int min_value = input_array[0];
  int min_value_index = 0;
  for (int i = 1; i < ARRAY_LENGTH; i++) {
    value2 = input_array[i];
    if (value2 < min_value) {
      min_value = value2;
      min_value_index = i;
    }
  }
  return min_value_index;
}

int filtered_index1;
int filtered_index2;

float filter1(float input_array[]) { // Filter by removing high and low from last seven then averaging
  filtered_index1 = maxIndex(input_array);
  filtered_index2 = minIndex(input_array);

  float sum = 0;

  for (int i = 0; i < ARRAY_LENGTH; i++) {
    if (i != filtered_index1 && i != filtered_index2) {
      sum += input_array[i];
    }
  }
  return (sum / (ARRAY_LENGTH - 2));
}

void loop()
{
  uint8_t val[4] = {0, 0, 0, 0};
  loopCount += 1;
  intI2CLoop = 0;
  time = millis(); // Time in milliseconds
  Wire.requestFrom(PITOT_ADDRESS, VALUE_COUNT);

  while (Wire.available()) { // slave may send less than requested
    //int c = Wire.read(); // receive a byte as character
    //Serial.println(c);         // print the character
    val[intI2CLoop] = Wire.read();
    intI2CLoop += 1;
    // 1st value relates to status
  }

  // BEGIN PASTE

  uint8_t status = (val[0] & 0xC0) >> 6;

  int16_t dp_raw = 0, dT_raw = 0;
  dp_raw = (val[0] << 8) + val[1];
  /* mask the used bits */
  dp_raw = 0x3FFF & dp_raw;
  dT_raw = (val[2] << 8) + val[3];
  dT_raw = (0xFFE0 & dT_raw) >> 5;

  // dT max is almost certainly an invalid reading
  if (dT_raw == 2047) {
    Serial.println("oops");
  }

  float temperature = ((200.0f * dT_raw) / 2047) - 50;

  // Calculate differential pressure. As its centered around 8000
  // and can go positive or negative
  /*
    this equation is an inversion of the equation in the
    pressure transfer function figure on page 4 of the datasheet
    We negate the result so that positive differential pressures
    are generated when the bottom port is used as the static
    port on the pitot and top port is used as the dynamic port

    diff pressure = dynamic pressure
    known conditions at track = static pressure
    total pressure = dyanmic + static
  */
  float diff_press_PSI = -((dp_raw - 0.1f * 16383) * (P_max - P_min) / (0.8f * 16383) + P_min); // diff_press_PSI is applied pressure in data sheet
  float diff_press_pa_raw = diff_press_PSI * PSI_to_Pa;
  Serial.print("Raw Pressure: ");
  Serial.print(diff_press_pa_raw);
  Serial.print("\t");

  press_values[loopCount % ARRAY_LENGTH] = diff_press_pa_raw - offset; // Prepare to filter pressure values
  
  dataSD = SD.open(strSDName, FILE_WRITE);
  dataSD.print(time);
  dataSD.print(", ");
  dataSD.print(filteredPressure);  
  dataSD.print(", ");
  //Serial.print("Pressure diff: ");
  //Serial.println(filteredPressure);
  filteredPressure = filter1(press_values);
  if (filteredPressure < 0) {
    filteredPressure = 0;
  }
  airspeed = sqrt(2*filteredPressure/airDensity) * conversionFactor;
  dataSD.print(temperature);
  dataSD.print(", ");
  //Serial.print("Temperature: ");
  //Serial.println(temperature);
  Serial.print("Time:");
  Serial.print(time);
  Serial.print("; Pressure diff: ");
  Serial.print(filteredPressure);
  Serial.print("\tAir speed: ");
  Serial.println(airspeed);
  dataSD.print(airspeed);
  dataSD.println(";");
  dataSD.close();
  
  // ============================== PRESSURE TRANSDUCER STUFF ===============================================

  //I2CRead(&bbi2c, PITOT_ADDRESS, pu8Data, VALUE_COUNT);
  
  //unsigned long test = (unsigned long)transducerPressure1;
  
  //Serial.print("\tTransducer pressure: ");
  //Serial.println(*pu8Data);

  //delay(50);
}
