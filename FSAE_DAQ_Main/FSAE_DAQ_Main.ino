#include <SD.h>

File dataSD;

// change this to match your SD shield or module;
const int chipSelect = 53;
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
  dataSD.println("Time (s), Accel1X (g), Accel1Y, Accel1Z, Accel2X, Accel2Y, Accel2Z, AccelMainX, AccelMainY, AccelMainZ, Wheelspeed1 (mph), Wheelspeed2 (mph), LinPot1 (in), LinPot2 (in)");
  dataSD.close();

}

float resistance_1;
float v1;
int time_last1 = 0; // Last time peak was detected for HE sensor 1
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

/**
   Accelerometer stuff
*/

// ports
const int ACCEL1_X = A2;
const int ACCEL1_Y = A4;
const int ACCEL1_Z = A6;
const int ACCEL2_X = A10;
const int ACCEL2_Y = A12;
const int ACCEL2_Z = A14;
const int ACCEL_MAIN_X = A11;
const int ACCEL_MAIN_Y = A13;
const int ACCEL_MAIN_Z = A15;

// biases/weights
const float BIAS_X_1 = -0.09;
const float BIAS_Y_1 = 0.06;
const float BIAS_Z_1 = -0.10;
const float BIAS_X_2 = 0.00;
const float BIAS_Y_2 = 0.03;
const float BIAS_Z_2 = -0.10;
const float BIAS_X_MAIN = 0.11;
const float BIAS_Y_MAIN = 0.08;
const float BIAS_Z_MAIN = 0.05;

const float VOLTS_PER_G_X_1 = 0.329;
const float VOLTS_PER_G_Y_1 = 0.331;
const float VOLTS_PER_G_Z_1 = 0.326;
const float VOLTS_PER_G_X_2 = 0.329;
const float VOLTS_PER_G_Y_2 = 0.336;
const float VOLTS_PER_G_Z_2 = 0.326;
const float VOLTS_PER_G_X_MAIN = 0.325;
const float VOLTS_PER_G_Y_MAIN = 0.332;
const float VOLTS_PER_G_Z_MAIN = 0.330;

// values to store data
float accel1X;
float accel1Y;
float accel1Z;
float accel2X;
float accel2Y;
float accel2Z;
float accelMainX;
float accelMainY;
float accelMainZ;


/*
   potentiometer stuff
*/
const float POT_IN_CONVERSION_SCALE = .00210396;

// ports
const int linPot1Port = A1;
const int linPot2Port = A3;
const int linPot3Port = A5;
const int linPot4Port = A7;

// gets the raw potentiometer value and converts into inches
// potPort - analog port of the linear potentiometer
float getLPotValue(const int potPort) {
  float position;
  float valOfNegative = (analogRead(potPort) - 1010);
  float positiveValue = abs(valOfNegative);
  position = POT_IN_CONVERSION_SCALE * (positiveValue);
  return position;
}


const float RESOLUTION = 1023.0;
const float RESOLUTION_3_3V = RESOLUTION * 3.3 / 5.0;
const float WHEEL_CIRCUMFERENCE = 1.27674325; // in m



void loop() {
  dataSD = SD.open(strSDName, FILE_WRITE);

  dataSD.print(millis() / 1000.0); // Time in seconds
  dataSD.print(", ");
  // Calculate acceleration
  accel1X = BIAS_X_1 + ((analogRead(ACCEL1_X) / RESOLUTION_3_3V * 3.3 - 3.3 / 2) / VOLTS_PER_G_X_1);

  dataSD.print(accel1X);
  dataSD.print(", ");

  accel1Y = BIAS_Y_1 + ((analogRead(ACCEL1_Y) / RESOLUTION_3_3V * 3.3 - 3.3 / 2) / VOLTS_PER_G_Y_1);

  dataSD.print(accel1Y);
  dataSD.print(", ");

  accel1Z = BIAS_Z_1 + ((analogRead(ACCEL1_Z) / RESOLUTION_3_3V * 3.3 - 3.3 / 2) / VOLTS_PER_G_Z_1);

  dataSD.print(accel1Z);
  dataSD.print(", ");


  accel2X = BIAS_X_2 + ((analogRead(ACCEL2_X) / RESOLUTION_3_3V * 3.3 - 3.3 / 2) / VOLTS_PER_G_X_2);

  dataSD.print(accel2X);
  dataSD.print(", ");

  accel2Y = BIAS_Y_2 + ((analogRead(ACCEL2_Y) / RESOLUTION_3_3V * 3.3 - 3.3 / 2) / VOLTS_PER_G_Y_2);

  dataSD.print(accel2Y);
  dataSD.print(", ");

  accel2Z = BIAS_Z_2 + ((analogRead(ACCEL2_Z) / RESOLUTION_3_3V * 3.3 - 3.3 / 2) / VOLTS_PER_G_Z_2);


  dataSD.print(accel2Z);
  dataSD.print(", ");

  accelMainX = BIAS_X_MAIN + ((analogRead(ACCEL_MAIN_X) / RESOLUTION_3_3V * 3.3 - 3.3 / 2) / VOLTS_PER_G_X_MAIN);


  dataSD.print(accelMainX);
  dataSD.print(", ");

  accelMainY = BIAS_Y_MAIN + ((analogRead(ACCEL_MAIN_Y) / RESOLUTION_3_3V * 3.3 - 3.3 / 2) / VOLTS_PER_G_Y_MAIN);


  dataSD.print(accelMainY);
  dataSD.print(", ");

  accelMainZ = BIAS_Z_MAIN + ((analogRead(ACCEL_MAIN_Z) / RESOLUTION_3_3V * 3.3 - 3.3 / 2) / VOLTS_PER_G_Z_MAIN);

  Serial.print("1X: ");
  Serial.print(accel1X);
  Serial.print(" Y: ");
  Serial.print(accel1Y);
  Serial.print(" Z: ");
  Serial.print(accel1Z);
  
  Serial.print("\t2X: ");
  Serial.print(accel2X);
  Serial.print(" Y: ");
  Serial.print(accel2Y);
  Serial.print(" Z: ");
  Serial.print(accel2Z);
  
  Serial.print("\tMAINX: ");
  Serial.print(accelMainX);
  Serial.print(" Y: ");
  Serial.print(accelMainY);
  Serial.print(" Z: ");
  Serial.println(accelMainZ);

  dataSD.print(accelMainZ);
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
  frequency1 = 1000.0 / (time_curr1 - time_last1); // Frequency of magnet passing, Hz

  he2_value = analogRead(HE2_PIN);
  if ((he2_value < 10) && !justPassed2) { // Usually 1 or 0 when magnet near, >1000 if not
    time_last2 = time_curr2;
    time_curr2 = millis();
    justPassed2 = true;
  } else if (he2_value > 1000) {
    justPassed2 = false; // Reset
  }
  frequency2 = 1000.0 / (time_curr2 - time_last2); // Frequency of magnet passing, Hz

  dataSD.print(2.23694 * WHEEL_CIRCUMFERENCE * frequency1);
  dataSD.print(", ");
  dataSD.print(2.23694 * WHEEL_CIRCUMFERENCE * frequency2);
  dataSD.print(", ");

  /**
   * potentiometer stuff
   */
  // get values
  float pot1Value = getLPotValue(linPot1Port);
  float pot2Value = getLPotValue(linPot2Port);
  //float pot3Value = getLPotValue(linPot3Port);
  //float pot4Value = getLPotValue(linPot4Port);

  // write stuff to sd card
  dataSD.print(pot1Value);
  dataSD.print(", ");
  dataSD.println(pot2Value);
  //dataSD.print(", ");
  //dataSD.print(pot3Value);
  //dataSD.print(", ");
  //dataSD.println(pot4Value);
  
  dataSD.close();
}
