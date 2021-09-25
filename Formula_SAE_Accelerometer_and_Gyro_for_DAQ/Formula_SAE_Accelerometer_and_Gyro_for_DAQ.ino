#include<Wire.h>

float alpha = 0.03;
float raw_acceleration[3];
float filtered_acceleration[3];
float previous_filtered_acceleration[3];
float calibration_value[3];
unsigned int i=1;
const int R_10k = 10000;

void setup() {
  
Serial.begin(9600);
Wire.begin();

chipSetup();
accelSetup();
accel_calibration();
accel_read();
previous_filtered_acceleration[0] = raw_acceleration[0] - calibration_value[0];
previous_filtered_acceleration[1] = raw_acceleration[1] - calibration_value[1];
previous_filtered_acceleration[2] = raw_acceleration[2] - calibration_value[2];
}

void loop() {
accel_read();

filtered_acceleration[0] = (1-alpha)*previous_filtered_acceleration[0] + alpha*(raw_acceleration[0] - calibration_value[0]);
filtered_acceleration[1] = (1-alpha)*previous_filtered_acceleration[1] + alpha*(raw_acceleration[1] - calibration_value[1]);
filtered_acceleration[2] = (1-alpha)*previous_filtered_acceleration[2] + alpha*(raw_acceleration[2] - calibration_value[2]);

previous_filtered_acceleration[0] = filtered_acceleration[0];
previous_filtered_acceleration[1] = filtered_acceleration[1];
previous_filtered_acceleration[2] = filtered_acceleration[2];

Serial.print("!");
Serial.print(filtered_acceleration[0]);
Serial.print("?");
Serial.print(filtered_acceleration[1]);
Serial.print("?");
Serial.println(filtered_acceleration[2]);
//Serial.print("   ");
//Serial.print(filtered_acceleration[1]);
//Serial.print("   ");
//Serial.print(filtered_acceleration[2]);
//Serial.print("   ");
//Serial.print(millis());
//Serial.print("   ");
//Serial.println(i);
//i++;
}

void chipSetup(){
  Wire.beginTransmission(0b1101001);
  Wire.write(0x6B);
  Wire.write(0b00000001);
  Wire.endTransmission(); 
}

void accelSetup(){
  Wire.beginTransmission(0x1C);
  Wire.write(0b00000000); // +/- 2g
  Wire.endTransmission();
}

  void accel_read(){
  Wire.beginTransmission(0b1101001);
  Wire.write(0x3B);
  Wire.endTransmission();
  Wire.requestFrom(0b1101001,6);
  while(Wire.available()<6);
  raw_acceleration[0] = (Wire.read()<<8|Wire.read())/16384.0;;
  raw_acceleration[1] = (Wire.read()<<8|Wire.read())/16384.0;
  raw_acceleration[2] = (Wire.read()<<8|Wire.read())/16384.0;
  }

void accel_calibration(){
  float sum[] = {0,0,0};
  for(byte i=0;i<200;i++){
    accel_read();
    sum[0] += raw_acceleration[0];
    sum[1] += raw_acceleration[1];
    sum[2] += raw_acceleration[2];}  
  
  calibration_value[0] = sum[0]/200.0;
  calibration_value[1] = sum[1]/200.0;
  calibration_value[2] = sum[2]/200.0;
}

void calc_resistance() {
  float V4 = analogRead(0xA4);
  float V5 = analogRead(0xA5);
  float V6 = analogRead(0xA6);
  float V7 = analogRead(0xA7);

  float Resistance_V4 = (R_10k*V4)/(5-V4);
  float Resistance_V5 = (R_10k*V5)/(5-V5);
  float Resistance_V6 = (R_10k*V6)/(5-V6);
  float Resistance_V7 = (R_10k*V7)/(5-V7);
}
