
int press_1 = 0;

double pressure = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  pressure = bits_to_psi(analogRead(press_1));
  Serial.println(pressure);
  delay(10);
}

double bits_to_psi(double bits){
  double voltage = bits*5/1023.0;
  double psi = voltage *50 - 25;
  return psi;
}
