const int baud_rate = 9600;

// set up pins
const int relay_pin = 0;
const int press_int_pin = A0;
const int press_ext_pin = A1;

// initialize pressure readings
double press_int_val = 0;
double press_ext_val = 0;
double desired_press_int = 0;

char master_state = '0';

void setup() {
  Serial.begin(96000);

  // set up pressure reading
  pinMode(press_int_pin, INPUT);
  pinMode(press_ext_pin, INPUT);
  // set up relay output pin
  pinMode(relay_pin, OUTPUT);
}

void loop() {
  press_int_val = bits_to_psi(analogRead(press_int_pin));
  press_ext_val = bits_to_psi(analogRead(press_ext_pin));
  
  // read from master
  if(Serial.available() > 0){master_state = Serial.read();}

  // send signal to master that the right pressure has been reached
  if(press_int_val >= desired_press_int){Serial.write('1');}

  // if master is ready to launch rocket, open the solenoid valve
  if(master_state = '1'){digitalWrite(relay_pin, HIGH);
  } else {digitalWrite(relay_pin, LOW);}

  delay(10);
}

// function that takes pressure reading in bits and outputs psi
double bits_to_psi(double bits){
  double voltage = bits*5/1023.0;
  double psi = voltage *50 - 25;
  return psi;
}
