#include<LiquidCrystal.h>
const int baud_rate = 9600;

// pin for button reading
const int button_pin = 2;
// pin for the potentiometer reading
const int pot_pin = 8;
// output pin for led indicating if pot is turned to right position
const int pot_led_pin = 7;
// output pin for led indicating if slave has right pressure
const int slave_led_pin = 6;

// rgb LED pin set up
const int RGB_red_pin = 11;
const int RGB_green_pin = 10;
const int RGB_blue_pin = 9;

// high if the button is pressed
int button_state = 0;
// high if slave has right internal pressure
char slave_state = '0';
// high if pot is turned to right position
int pot_state = 0;
// RGB value from pot reading
int RGB_pot;
//////////////////////////////////////////////////////////////////
/////////////////////////LCD Variables////////////////////////////
//////////////////////////////////////////////////////////////////
/*
by omitting pins d0 - d3 we use the 4 bit lcd mode which takes double processing time to use the lcd but since the controller only uses

vss - gnd
vdd - 5v
v0(constrast pin) - R to ground or pot to ground
rs - r1 = [digital pin]
rw - gnd
e - r2 = [digital pin]
d0 - none
d1 - none
d2 - none
d3 - none
d4 - r3
d5 - r4
d6 - r5
d7 - r6
A - resistor to 5v
k - gnd
*/
const int r1 = 0; //pin rs
const int r2 = 1; //pin e
const int r3 = 2; //pin d4
const int r4 = 3; //pin d5
const int r5 = 4; //pin d6
const int r6 = 5; //pin d7

LiquidCrystal lcd(r1, r2, r3, r4, r5, r6);

int t =0;
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

  // turn on RGB LED
  RGB(analogRead(pot_pin));

  // read from slave
  if(Serial.available() > 0){slave_state = Serial.read();}

  if(analogRead(pot_pin) >= 681) {pot_state = HIGH;
  } else {pot_state = LOW;}

  // if slave pressure reading is corret, turn on LED
  if(slave_state == '1'){digitalWrite(slave_led_pin, HIGH); 
  } else {digitalWrite(slave_led_pin, LOW);}
  

  // if button is pressed, slave at right pressure, and pot at right position launch rocket
  if(button_state == HIGH && slave_state == '1' && pot_state == HIGH){
    // send signal to slave to launch the thing

    // turn off the LEDs and set the states to low
    digitalWrite(pot_led_pin, LOW);
    digitalWrite(slave_led_pin, LOW);
    slave_state = '0';
    // 5 sec delay
    delay(5000);

    Serial.write('0');
  }
  lcd.clear();
  lcd.print(Serial.read());
  delay(10);
}

void RGB(int pot){
  RGB_pot = map(pot, 0, 1023, 0, 255);
  if (pot<341 && pot > 5){
      analogWrite(RGB_red_pin, RGB_pot);
      analogWrite(RGB_green_pin, 0);
      analogWrite(RGB_blue_pin, 0);
  } else if(pot >= 341 && pot< 682){
      analogWrite(RGB_red_pin, 255);
      analogWrite(RGB_green_pin, RGB_pot);
      analogWrite(RGB_blue_pin, 0);
  } else if (pot >= 682){
      analogWrite(RGB_red_pin, 255);
      analogWrite(RGB_green_pin, 255);
      analogWrite(RGB_blue_pin, RGB_pot);
  }
}
