int incomingByte = 0;

void setup() {
  // put your setup code here, to run once:
  Seral.begin(38400);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0){
    incomingByte = Serial.read();

    Serial.print("I received: ");
    Serial.println(incomingByte, DEC)
  }

}
