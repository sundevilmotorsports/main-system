#define ACCELX A0
#define ACCELY A2
#define ACCELZ A4


const float BIAS_X_1 = -0.09;
const float BIAS_Y_1 = 0.06;
const float BIAS_Z_1 = -0.10;

// increase this to decrease scale of values
const float VOLTS_PER_G_X_1 = 0.329;
const float VOLTS_PER_G_Y_1 = 0.331;
const float VOLTS_PER_G_Z_1 = 0.326;

const float BIAS_X_2 = 0.00;
const float BIAS_Y_2 = 0.03;
const float BIAS_Z_2 = -0.10;

// increase this to decrease scale of values
const float VOLTS_PER_G_X_2 = 0.329;
const float VOLTS_PER_G_Y_2 = 0.336;
const float VOLTS_PER_G_Z_2 = 0.326;

const float RESOLUTION = 1023.0;
const float RESOLUTION_3_3V = RESOLUTION * 3.3/5.0;

bool accelOne = false;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int rawX = analogRead(ACCELX);
  int rawY = analogRead(ACCELY);
  int rawZ = analogRead(ACCELZ);
  float calibratedZ;
  float calibratedY;
  float calibratedX;
  if(accelOne){
    
  }
  else{
    calibratedZ = BIAS_Z_2 + ((rawZ/RESOLUTION_3_3V * 3.3 - 3.3/2)/VOLTS_PER_G_Z_2);
    calibratedY = BIAS_Y_2 + ((rawY/RESOLUTION_3_3V * 3.3 - 3.3/2)/VOLTS_PER_G_Y_2);
    calibratedX = BIAS_X_2 + ((rawX/RESOLUTION_3_3V * 3.3 - 3.3/2)/VOLTS_PER_G_X_2);
  }

  
  Serial.print("x: ");
  Serial.print(calibratedX);
  Serial.print("\ty: ");
  Serial.print(calibratedY);
  Serial.print("\tz: ");
  Serial.println(calibratedZ);
}
