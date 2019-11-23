#include <SPI.h>
#include <SD.h>

double bits_to_psi(double bits);
double time();
void record(double data);
void dump();
void Display();

const int baud_rate = 9600;
// SD Card
const int chipSelect = 4;
const String filename = "Pressure.csv"
String sdata = "";

// set up pins
const int relay_pin = 0;
const int press_int_pin = A0;
const int press_ext_pin = A1;

// initialize pressure readings
double press_int_val = 0;
double press_ext_val = 0;
const double desired_press_int = 0;

//Rate Control
//sample rate
const double sam(1000); // sampling frequency in hz
const int final(1000000); //number of total samples to record
const double interval(1.0/sam); //time interval between samples in [s]
double t(0); // sampling time tracking variable
//signal rate
const double sig(100); //signal frequency in hz
const double sInt(1.0/sig); //signal time period in seconds
double sT(0); //signal time tracking variable
// Communication Variables
bool state(false);
char master_state = '0';

//////////////////////////End Header

void setup() {
  Serial.begin(baud_rate);

  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized, keep waiting if not
  while (!SD.begin(chipSelect)) {
    Serial.write("Card failed, or not present");
    delay(1000);
  }
  Serial.println("card initialized.");
  // set up pressure reading
  pinMode(press_int_pin, INPUT);
  pinMode(press_ext_pin, INPUT);
  pinMode(relay_pin, OUTPUT);
}

void loop() {
  //launch loop
  while(state == true){
    press_int_val = bits_to_psi(analogRead(press_int_pin));
    press_ext_val = bits_to_psi(analogRead(press_ext_pin));
    if(i < final && time() - t >= interval){
      record(time());
      record(press_int_val);
      record(press_ext_val);
      t = time();
      i++;
    }
    if(time() - sT >= sInt){
      dump();
      Serial.write(//NEEDS FORMATTED STRING TO SEND);
      sT = time();
    }
  }
  ///////////////////Signal Processing
  // read from master
  while(state == false){
    press_int_val = bits_to_psi(analogRead(press_int_pin));
    press_ext_val = bits_to_psi(analogRead(press_ext_pin));

    if(Serial.available() > 0){master_state = Serial.read();}

    // send signal to master that the right pressure has been reached
    if(press_int_val >= desired_press_int){Serial.write('1');}

    // if master is ready to launch rocket, open the solenoid valve
    if(master_state = '1'){digitalWrite(relay_pin, HIGH);
    } else {digitalWrite(relay_pin, LOW);}
  }
}

/////////////////////////Functions
// function that takes pressure reading in bits and outputs psi
double bits_to_psi(double bits){
  double voltage = bits*5/1023.0;
  double psi = voltage *50 - 25;
  return psi;
}

//Function that grabs current time, based on our standardized format
double time(){
  return(micros()/1000000.0);
}
//Modifies sdata by concatenating the passed data with a conditional comma preceeding it
void record(double data){
  if(sdata != "")
    sdata += ","; //playing fast and loose
  sdata += String(data);
}
//dumps recorded data to SD card
void dump(){
  File dataFile = SD.open(filename, FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(sdata);
    dataFile.close();
    sdata = "";
  }
  // if the file isn't open, pop up an error:
  else {
    //TODO: Put some kind of error message signal
  }
}
//Takes pressure readings, typecasts them and appends respective variables beforehand
void Display(double p1, double p2, char sig = '0' ){
  String out = "";
  out += 'a';
  out += sig;
  out += 'd';
  out += String(time());
  out += 'b';
  out += String(p1);
  out += 'c';
  out += String(p2);
  Serial.write(out);
}
