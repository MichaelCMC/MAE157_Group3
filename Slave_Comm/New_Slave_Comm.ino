#include <SPI.h>
#include <SD.h>

// Function declarations:
double bits_to_psi(double bits);
double time();
void record(double data);
void dump();
void Display(double press_int_val, double press_ext_val, int sig);
float truncate(float val, byte dec);

// SD Card:
const int chipSelect = 4;
const String filename = "Pressure.csv";
String sdata = "";

// Set up pins:
const int relay_pin = 0;
const int press_int_pin = A0;
const int press_ext_pin = A1;

const int baud_rate = 9600;

// Initialize pressure readings:
double press_int_val = 0;
double press_ext_val = 0;
const double desired_press_int = 0;

// Rate Control, sample rate:
const double sam(1000);         // sampling frequency in Hz
const int final(10000);         // number of total samples to record (altered from 1000000 to 10000)
const double interval(1.0/sam); // time interval between samples in [s]
double t(0);                    // sampling time tracking variable
// Signal rate:
const double sig(1);            // signal frequency in Hz
const double sInt(1.0/sig);     // signal time period in seconds
double sT(0);                   // signal time tracking variable

// Communication Variables:
bool state(false);
char master_state = '0';

int i = 0;
double i_interval = 0;


void setup() 
{
  Serial.begin(baud_rate);
  int sig = 1;                   // not sure if this does anything
}

void loop() 
{
  // Signal Processing:
  press_int_val = bits_to_psi(analogRead(press_int_pin));
  press_ext_val = bits_to_psi(analogRead(press_ext_pin));
  while (1)
  {
    Display(press_int_val, press_ext_val, sig);
    delay(2000);
  }
}

//=================================================================================

// Functions:

// Function that takes pressure reading in bits and outputs psi
double bits_to_psi(double bits){
  double voltage = bits*5/1023.0;
  double psi = voltage *50 - 25;
  return psi;
}

// Function that grabs current time, based on our standardized format
double time(){
  return(micros()/1000000.0);
}

// Modifies sdata by concatenating the passed data with a conditional comma preceeding it
void record(double data){
  if(sdata != "")
    sdata += ","; // playing fast and loose
  sdata += String(data);
}

// Dumps recorded data to SD card
void dump(){
  File dataFile = SD.open(filename, FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(sdata);
    dataFile.close();
    sdata = "";
  }
  // If the file isn't open, pop up an error:
  else {
    // Put some kind of error message signal
    Serial.print("File unable to be opened");
  }
}

// Takes pressure readings, typecasts them and appends respective variables beforehand
void Display(double p1, double p2, int sig)
{
  String out = "";
  out += 'a';
  out += sig;
  out += 'b';
  out += String(p2); // external pressure
  out += 'c';
  out += String(p1); // internal pressure
  out += 'd';
  out += String(truncate(time(), 5));
  out += 'e';
  //Serial.println(out);
  //c_array = out.c_str(); 
  char c_array[30] = "";
  for (i = 0; i < 9; i++) // comment back in
      c_array[i] = out[i];// comment back in
  //char c_array[22] = "a0b0.123c0.234d0.345e"; // comment this out
  Serial.write(c_array);
  delay(1000);
}

// Truncates time to only a few decimal places
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
