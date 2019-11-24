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

// Serial parsing
double curr_time = 0;
double curr_p_int = 0;
double curr_p_ext = 0;
char curr_sign ='0';

// high if the button is pressed
int button_state = 0;
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
  // read from slave
  slave_reading();

  // reads if the button has been pushed
  button_state = digitalRead(button_pin);

  // turn on RGB LED
  RGB(analogRead(pot_pin));

  if(analogRead(pot_pin) >= 681) {pot_state = HIGH;
  } else {pot_state = LOW;}

  // if slave pressure reading is corret, turn on LED
  if(curr_sign == '1'){digitalWrite(slave_led_pin, HIGH); 
  } else {digitalWrite(slave_led_pin, LOW);}
  

  // if button is pressed, slave at right pressure, and pot at right position launch rocket
  if(button_state == HIGH && curr_sign == '1' && pot_state == HIGH){
    // send signal to slave to launch the thing
    Serial.write('2');

    // turn off the LEDs and set the states to low
    RGB(0);
    digitalWrite(slave_led_pin, LOW);
    curr_sign = '0';
  }
  lcd.clear();
  // lcd.print(Serial.read());
  delay(10);
}

// RBG LED tuning
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

// begin reading slave data
void slave_reading(){
  // currently read char
  char curr_read = 'z';
  // data indicator
  char indicator = 'z';
  // actual data read
  String data_string = "";
  // signaling chars
  String sig_chars = "abcd";

  if (Serial.available() > 0){
    // inf while loop
    while (true){
      // read first char
      curr_read = Serial.read();

      // if read char is an indicator
      if (sig_chars.indexOf(curr_read) != -1){
        switch(indicator){
          // skip if first instance
          case 'z':
            break;
          // if a, return signal as char
          case 'a':
            curr_sign = data_string.charAt(0);
            data_string = "";
            break;
          // if b, return as internal pressure as double
          case 'b':
            curr_p_int = data_string.toDouble();
            data_string = "";
            break;
          // if c, return as external pressure as double
          case 'c':
            curr_p_ext = data_string.toDouble();
            data_string = "";
            break;
          // if d, return as time read as double
          case 'd':
            curr_time = data_string.toDouble();
            data_string = "";
            break;
        }
        // set indicator to currently read value
        indicator = curr_read;
      // if currently read is e, end function
      } else if (curr_read == 'e'){ return;
      } else{
          // add to the data string
          data_string += curr_read;
      }
    }
  }
}