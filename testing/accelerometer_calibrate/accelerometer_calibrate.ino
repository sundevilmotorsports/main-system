#define ACCELX A0
#define ACCELY A2
#define ACCELZ A4

const float BIAS_X_1 = -.09;
const float BIAS_Y_1 = 0.06;
const float BIAS_Z_1 = 0.05;

const float VOLTS_PER_G_X_1 = 0.329;
const float VOLTS_PER_G_Y_1 = 0.331;
const float VOLTS_PER_G_Z_1 = 0.326;

const float RESOLUTION = 1023.0;
const float RESOLUTION_3_3V = RESOLUTION * 3.3/5.0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int rawX = analogRead(ACCELX);
  int rawY = analogRead(ACCELY);
  int rawZ = analogRead(ACCELZ);
  Serial.print("raw x: ");
  Serial.print(rawX);
  Serial.print("\traw y: ");
  Serial.print(rawY);
  Serial.print("\traw z: ");
  Serial.println(rawZ);
}
