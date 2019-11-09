const int baud_rate = 9600;

const int button_pin = 2;
const int pot_pin = 0;
const int pot_led_pin = 7;
const int slave_led_pin = 6;

int button_state = 0;
int slave_state = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(button_pin, INPUT);
  pinMode(pot_pin, INPUT);

  pinMode(pot_led_pin, OUTPUT);
  pinMode(slave_led_pin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Serial.write(i);
  // i++;
  // delay(500);
  // if (i == 255){i = 0;}
  button_state = digitalRead(button_pin);

  if(Serial.available() > 0){slave_state = Serial.read();}

  if (analogRead(pot_pin) >= 1000) {digitalWrite(pot_led_pin, HIGH);}
  else {digitalWrite(pot_led_pin, LOW);}

  if(button_state == HIGH){
    digitalWrite(pot_led_pin, LOW);
    digitalWrite(slave_led_pin, LOW);
    delay(4000);
  }
}
