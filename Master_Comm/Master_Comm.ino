const int baud_rate = 9600;

// pin for button reading
const int button_pin = 2;
// pin for the potentiometer reading
const int pot_pin = 0;
// output pin for led indicating if pot is turned to right position
const int pot_led_pin = 7;
// output pin for led indicating if slave has right pressure
const int slave_led_pin = 6;

// high if the button is pressed
int button_state = 0;
// high if slave has right internal pressure
char slave_state = '0';
// high if pot is turned to right position
int pot_state = 0;

void setup() {
  
  Serial.begin(baud_rate);
  // pin set up
  pinMode(button_pin, INPUT);
  pinMode(pot_pin, INPUT);

  pinMode(pot_led_pin, OUTPUT);
  pinMode(slave_led_pin, OUTPUT);
}

void loop() {
  
  // reads if the button has been pushed
  button_state = digitalRead(button_pin);
  // read from slave
  if(Serial.available() > 0){slave_state = Serial.read();}

  if(analogRead(pot_pin) >= 1000) {pot_state = HIGH;
  } else {pot_state = LOW;}

  // if pot is turned right position, turn on LED
  if (pot_state == HIGH) {digitalWrite(pot_led_pin, HIGH);
  } else {digitalWrite(pot_led_pin, LOW);}

  // if slave pressure reading is corret, turn on LED
  if(slave_state == '1'){digitalWrite(slave_led_pin, HIGH); 
  } else {digitalWrite(slave_led_pin, LOW);}

  // if button is pressed, slave at right pressure, and pot at right position launch rocket
  if(button_state == HIGH && slave_state == '1' && pot_state == HIGH){
    // send signal to slave to launch the thing
    Serial.write('2');

    // turn off the LEDs and set the states to low
    digitalWrite(pot_led_pin, LOW);
    digitalWrite(slave_led_pin, LOW);
    slave_state = '0';
    // 5 sec delay
    delay(5000);

    Serial.write('0');
  }
  delay(10);
}
