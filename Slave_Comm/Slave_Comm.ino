#include <SPI.h>
#include <SD.h>

const int chipSelect = 4;

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

/////////////////////////////////////////////////////////////////
//////////////////////////End Header/////////////////////////////
/////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(96000);
  
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.write("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  
  // set up pressure reading
  pinMode(press_int_pin, INPUT);
  pinMode(press_ext_pin, INPUT);
  // set up relay output pin
  pinMode(relay_pin, OUTPUT);
}

void loop() {
  press_int_val = bits_to_psi(analogRead(press_int_pin));
  press_ext_val = bits_to_psi(analogRead(press_ext_pin));
  
  ////////////////////////////////////////////////////////
  /////////////////SD CARD DATA Recording/////////////////
  ////////////////////////////////////////////////////////
  String dataString = "";

  // read and record pressure data to a single line
  //time,internal P, external P
  //typecasting the data
  dataString += String(millis()/1000000.0);
  dataString += ",";
  dataString += String(press_int_val);
  dataString += ",";
  dataString += String(press_ext_val);
  
  File dataFile = SD.open("Pressure.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.write(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
  
  /////////////////////////////////////////////////////////
  ///////////////////Signal Processing/////////////////////
  /////////////////////////////////////////////////////////
  // read from master
  if(Serial.available() > 0){master_state = Serial.read();}

  // send signal to master that the right pressure has been reached
  if(press_int_val >= desired_press_int){Serial.write('1');}

  // if master is ready to launch rocket, open the solenoid valve
  if(master_state = '1'){digitalWrite(relay_pin, HIGH);
  } else {digitalWrite(relay_pin, LOW);}
  
  delay(10); //TODO: Replace delay with proper sampling control method
}


// function that takes pressure reading in bits and outputs psi
double bits_to_psi(double bits){
  double voltage = bits*5/1023.0;
  double psi = voltage *50 - 25;
  return psi;
}
