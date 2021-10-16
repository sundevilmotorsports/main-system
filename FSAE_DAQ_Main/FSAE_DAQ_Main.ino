#include <SD.h>

File dataSD;

// change this to match your SD shield or module;
const int chipSelect = 4;
int runNumber;
String strSDName;

void setup() {
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
  } else {
    Serial.println("SD card successfully initialized.");
  }

  strSDName = "mainrun1.txt";
  runNumber = 1;

  while (SD.exists(strSDName)) { // Find a file name that doesn't exist
    runNumber += 1;
    strSDName = "mainrun" + String(runNumber) + ".txt"; // Create the next file name to check.
    // If it doesn't exist, we will simply leave this as the SD card file name that we use.
  }
  Serial.println(strSDName);
  dataSD = SD.open(strSDName, FILE_WRITE); //, O_CREAT | O_WRITE);
  dataSD.println("Time (s), Accel1X (g), Accel1Y, Accel1Z, Accel2X, Accel2Y, Accel2Z, Wheelspeed1 (mph), Wheelspeed2 (mph)");
  dataSD.close();
  
}

float resistance_1;
float v1;
int time_last1= 0; // Last time peak was detected for HE sensor 1
int time_last2 = 0; //       "           "        for HE sensor 2
int time_curr1;
int time_curr2;
bool justPassed1 = false;
bool justPassed2 = false;
int he1_value;
int he2_value;
float frequency1; // hall effect sensor 1 magnet pass frequency, in Hz
float frequency2;
const float RESISTOR_1_10K_NOM = 10000.0;
const float VOLTAGE_NOM = 5.0;
const int R1_PIN = A4; // Slidepot 1 analog pin
const int HE1_PIN = A8;
const int HE2_PIN = A9; // Hall effect sensor #2 analog pin

const int ACCEL1_X = A10;
const int ACCEL1_Y = A14;
const int ACCEL1_Z = A12;
const int ACCEL2_X = A11;
const int ACCEL2_Y = A15;
const int ACCEL2_Z = A13;

const float BIAS_X_1 = -.09;
const float BIAS_Y_1 = 0.06;
const float BIAS_Z_1 = 0.05;
const float BIAS_X_2 = -0.05;
const float BIAS_Y_2 = 0.08;
const float BIAS_Z_2 = 0.07;

float accel1X;
float accel1Y;
float accel1Z;
float accel2X;
float accel2Y;
float accel2Z;

const float VOLTS_PER_G_X_1 = 0.329;
const float VOLTS_PER_G_Y_1 = 0.331;
const float VOLTS_PER_G_Z_1 = 0.326;
const float VOLTS_PER_G_X_2 = 0.325;
const float VOLTS_PER_G_Y_2 = 0.332;
const float VOLTS_PER_G_Z_2 = 0.325;

const float RESOLUTION = 1023.0;
const float RESOLUTION_3_3V = RESOLUTION * 3.3/5.0;
const float WHEEL_CIRCUMFERENCE = 1.27674325; // in m

void loop() {
  dataSD = SD.open(strSDName, FILE_WRITE);
  // Calculate resistance across slidepot:
  //v1 = analogRead(R1_PIN)/RESOLUTION * VOLTAGE_NOM;
  //resistance_1 = RESISTOR_1_10K_NOM * v1 / (VOLTAGE_NOM - v1);
  //Serial.println(resistance_1);
  dataSD.print(millis()/1000.0); // Time in seconds
  dataSD.print(", ");
  // Calculate acceleration
  accel1X = BIAS_X_1 + ((analogRead(ACCEL1_X)/RESOLUTION_3_3V * 3.3 - 3.3/2)/VOLTS_PER_G_X_1);
  //Serial.print("1X: ");
  //Serial.print(accel1X);
  
  dataSD.print(accel1X);
  dataSD.print(", ");
  
  accel1Y = BIAS_Y_1 + ((analogRead(ACCEL1_Y)/RESOLUTION_3_3V * 3.3 - 3.3/2)/VOLTS_PER_G_Y_1);
  //Serial.print(" Y: ");
  //Serial.print(accel1Y);
  
  dataSD.print(accel1Y);
  dataSD.print(", ");
  
  accel1Z = BIAS_Z_1 + ((analogRead(ACCEL1_Z)/RESOLUTION_3_3V * 3.3 - 3.3/2)/VOLTS_PER_G_Z_1);
  //Serial.print(" Z: ");
  //Serial.println(accel1Z);
  
  dataSD.print(accel1Z);
  dataSD.print(", ");

  
  accel2X = BIAS_X_2 + ((analogRead(ACCEL2_X)/RESOLUTION_3_3V * 3.3 - 3.3/2)/VOLTS_PER_G_X_2);
  //Serial.print("2X: ");
  //Serial.print(accel2X);
  
  dataSD.print(accel2X);
  dataSD.print(", ");
  
  accel2Y = BIAS_Y_2 + ((analogRead(ACCEL2_Y)/RESOLUTION_3_3V * 3.3 - 3.3/2)/VOLTS_PER_G_Y_2);
  //Serial.print(" Y: ");
  //Serial.print(accel2Y);
  
  dataSD.print(accel2Y);
  dataSD.print(", ");
  
  accel2Z = BIAS_Z_2 + ((analogRead(ACCEL2_Z)/RESOLUTION_3_3V * 3.3 - 3.3/2)/VOLTS_PER_G_Z_2);
  //Serial.print(" Z: ");
  //Serial.println(accel2Z);
  
  dataSD.print(accel2Z);
  dataSD.print(", ");

  // Calculate frequency of hall effect sensor
  he1_value = analogRead(HE1_PIN);
  if ((he1_value < 10) && !justPassed1) { // Usually 1 or 0 when magnet near, >1000 if not
    time_last1 = time_curr1;
    time_curr1 = millis();
    justPassed1 = true;
  } else if (he1_value > 1000) {
    justPassed1 = false; // Reset
  }
  frequency1 = 1000.0/(time_curr1-time_last1); // Frequency of magnet passing, Hz
  
  he2_value = analogRead(HE2_PIN);
  if ((he2_value < 10) && !justPassed2) { // Usually 1 or 0 when magnet near, >1000 if not
    time_last2 = time_curr2;
    time_curr2 = millis();
    justPassed2 = true;
  } else if (he2_value > 1000) {
    justPassed2 = false; // Reset
  }
  frequency2 = 1000.0/(time_curr2-time_last2); // Frequency of magnet passing, Hz
  Serial.print(he1_value);
  Serial.print(" // ");
  Serial.println(2.23694 * WHEEL_CIRCUMFERENCE * frequency1); // Speed of car in mph

  dataSD.print(2.23694 * WHEEL_CIRCUMFERENCE * frequency1);
  dataSD.print(", ");
  dataSD.println(2.23694 * WHEEL_CIRCUMFERENCE * frequency2);

  dataSD.close();
}
