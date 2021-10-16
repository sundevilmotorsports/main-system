float val;
float scale = .00210396;

void setup() {
Serial.begin(9600);
}

void loop() {
  val = convert();
 Serial.println(val);

delay(500);
//
}
float convert(void){
  float position;
  float valOfNegative = (analogRead(A3)- 1010);
  float positiveValue = abs(valOfNegative);
  position = scale * (positiveValue);
  return position;
  
 }
