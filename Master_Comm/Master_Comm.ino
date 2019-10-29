int i = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(38400);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.write(i);
  delay(500);
  if (i == 255){i = 0;}
}
