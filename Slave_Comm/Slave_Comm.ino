#include <SPI.h>
#include <SD.h>


const float baud_rate = 115200;
//
String sdata = "";
String in = "";
String out = "";

// set up pins
const int relay_pin = 2;
const int press_int_pin = A0;
const int press_ext_pin = A1;

// initialize pressure readings
double press_int_val = 0;
double press_ext_val = 0;
const double desired_press_int = 0;

//Rate Control
//sample rate
const double sam(1000); // sampling frequency in hz
const long final(100000); //number of total samples to record
const double interval(1.0/sam); //time interval between samples in [s]
double t(0); // sampling time tracking variable
double tlaunch(0);
//signal rate
const double sig(800); //signal frequency in hz
const double sInt(1.0/sig); //signal time period in seconds
double sT(0); //signal time tracking variable
double dT(0); //Display time tracker
// Communication Variables
bool state(false);
char master_state = '0';
char current('0');

long i = 0;
double i_interval = 0;

//////////////////////////End Header

void setup() {
  Serial.begin(baud_rate);
  pinMode(press_int_pin, INPUT);
  pinMode(press_ext_pin, INPUT);
  pinMode(relay_pin, OUTPUT);
  delay(10);
  Serial.read();
  delay(10);
}

void loop() {
  ///////////////////Signal Processing
  while(state == false){
    current ='1';
    press_int_val = bits_to_psi(analogRead(press_int_pin));
    press_ext_val = bits_to_psi(analogRead(press_ext_pin));

    //TODO: figure out what signals we will recieve from master and from there interperet signals
    if(Serial.available() > 0)
    {
      master_state = Serial.read();
    }
    // send signal to master that the right pressure has been reached
    if(press_int_val >= desired_press_int){current = '1';}
    if(time(0) - sT >= sInt){
      Display();
      sT = time(0);
    }

    if(master_state == 'y'){
      state = true;
      }
  }

  ///////////////////Launch and Collect Data loop
  while(state == true){
    //LAUNCH CONTROL
    //Begins recording data for 2 seconds and then opens the relay while still collecting data
    //then closes the relay after 1.5 seconds and will continue collecting data for up to 100 seconds
    //data collection cap is defined by [final] which control max # of samples to be taken and [sam] which controls interval between samples
    current = '0';
    //Relay control setup
    if(i*interval >= 1 && tlaunch == 0){
      digitalWrite(relay_pin, HIGH);
      tlaunch = time(0);
    }
    if(i*interval >= 2.5 && i_interval <= 3.0)
      digitalWrite(relay_pin, LOW);

    //DATA COLLECTION
    //first if statement is for sampling frequency and will control the rate at which data is collected on the very long sdata string
    //second if statement controls the rate at which sdata is dumped onto the SD card and rate at which a data sample is sent to the master
    //best to send data fairly slowly, maybe 1.5hz or so since updating the lcd too fast makes it look ugly. Also to prevent filling up the buffer
    press_int_val = bits_to_psi(analogRead(press_int_pin));
    press_ext_val = bits_to_psi(analogRead(press_ext_pin));
    in = String(press_int_val,3);
    out = String(press_ext_val,3);
    if(i < final && time(0) - t >= interval){
      record();
      t = time(0);
      i++;
    }
    if(time(0) - sT >= sInt || sizeof(sdata) >= 20){
      dump(); //dump function for writing to SD card and clearing out sdata
      sT = time(0);
    }
    if(time(0) - dT >= sInt*10){
      Display();
      dT = time(0);
    }
  }
}

/////////////////////////Functions
// function that takes pressure reading in bits and outputs psi
double bits_to_psi(double bits){
  double voltage = bits*5/1023.0;
  //double psi = voltage *37.5 - 18.75;
  return voltage;
}
//Function that grabs current time, based on our standardized format
double time(double offset){
  return((micros()/1000000.0) - offset);
}
//Modifies sdata by concatenating the passed data with a conditional comma preceeding it
void record(){
  //Serial.print("record");
  sdata += String(time(tlaunch), 3);
  sdata += ',';
  sdata += in;
  sdata += ',';
  sdata += out;
  sdata += '\n';
}
//dumps recorded data to SD card
void dump(){
  Serial.print(sdata);
  sdata = "";
}
//Takes pressure readings, typecasts them and appends respective variables beforehand
void Display(){
  String out = "";
  out += 'a';
  out += current;
  out += 'b';
  //out += String(truncate(press_int_val,2)); // internal pressure
  out += String(press_int_val *37.5 - 18.75,4);
  out += 'c';
  //out += String(truncate(press_ext_val,2)); // external pressure
  out += String(press_ext_val *37.5 - 18.75,4);
  out += 'd';
  out += String(time(0),4);
  out += 'e';
  //Serial.println(out);
  char c_array[30] = "";
  for (size_t i = 0; i < out.length(); i++)
  {
      c_array[i] = out[i];// comment back in
  }
  Serial.write(c_array);
  Serial.println("");
}

float truncate(float val, byte dec)
  {
    float x = val * pow(10, dec);
    float y = round(x);
    float z = x - y;
    if ((int)z == 5)
    {
        y++;
    } else {}
    x = y / pow(10, dec);
    return x;
}
