#define ACCELX_1 A0
#define ACCELY_1 A2
#define ACCELZ_1 A4



const float BIAS_X_1 = -0.09;
const float BIAS_Y_1 = 0.06;
const float BIAS_Z_1 = -0.10;

// increase this to decrease scale of values
const float VOLTS_PER_G_X_1 = 0.329;
const float VOLTS_PER_G_Y_1 = 0.331;
const float VOLTS_PER_G_Z_1 = 0.326;

const float BIAS_X_2 = -0.09;
const float BIAS_Y_2 = 0.06;
const float BIAS_Z_2 = -0.10;

// increase this to decrease scale of values
const float VOLTS_PER_G_X_2 = 0.329;
const float VOLTS_PER_G_Y_2 = 0.331;
const float VOLTS_PER_G_Z_2 = 0.326;

const float RESOLUTION = 1023.0;
const float RESOLUTION_3_3V = RESOLUTION * 3.3/5.0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int rawX = analogRead(ACCELX_1);
  int rawY = analogRead(ACCELY_1);
  int rawZ = analogRead(ACCELZ_1);

  float calibratedZ = BIAS_Z_1 + ((rawZ/RESOLUTION_3_3V * 3.3 - 3.3/2)/VOLTS_PER_G_Z_1);
  float calibratedY = BIAS_Y_1 + ((rawY/RESOLUTION_3_3V * 3.3 - 3.3/2)/VOLTS_PER_G_Y_1);
  float calibratedX = BIAS_X_1 + ((rawX/RESOLUTION_3_3V * 3.3 - 3.3/2)/VOLTS_PER_G_X_1);
  
  Serial.print("x: ");
  Serial.print(calibratedX);
  Serial.print("\ty: ");
  Serial.print(calibratedY);
  Serial.print("\tz: ");
  Serial.println(calibratedZ);
}
