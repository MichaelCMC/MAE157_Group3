#include <SPI.h>
#include <SD.h>

double bits_to_psi(double bits);
double tim(double offset = 0);
void record(double data);
void dump();
void Display();
float truncate(float val, byte dec);

const int baud_rate = 115200;
// SD Card
const int chipSelect = 4;
const String filename = "Pressure.csv";
String sdata = "";

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
const int final(10000); //number of total samples to record (altered from 1000000 to 10000)
const double interval(1.0/sam); //time interval between samples in [s]
double t(0); // sampling time tracking variable
double tlaunch(0);
//signal rate
const double sig(2); //signal frequency in hz
const double sInt(1.0/sig); //signal time period in seconds
double sT(0); //signal time tracking variable
// Communication Variables
bool state(false);
char master_state = '0';
char current('0');

int i = 0;
double i_interval = 0;

//////////////////////////End Header

void setup() {
  Serial.begin(baud_rate);
  // see if the card is present and can be initialized, keep waiting if not
  while (!SD.begin(chipSelect)) {
    delay(1000);
  }
  // set up pressure reading
  pinMode(press_int_pin, INPUT);
  pinMode(press_ext_pin, INPUT);
  pinMode(relay_pin, OUTPUT);
}

void loop() {
  ///////////////////Signal Processing
  while(state == false){
    press_int_val = bits_to_psi(analogRead(press_int_pin));
    press_ext_val = bits_to_psi(analogRead(press_ext_pin));

    //TODO: figure out what signals we will recieve from master and from there interperet signals
    if(Serial.available() > 0)
    {
      master_state = Serial.read();
    }
    // send signal to master that the right pressure has been reached
    if(press_int_val >= desired_press_int){current = '1';}

    if(tim() - sT >= sInt){
      Display();
      sT = tim();
    }

    if(master_state = 'y')
      state = true
  }

  ///////////////////Launch and Collect Data loop
  while(state == true){
    //LAUNCH CONTROL
    //Begins recording data for 2 seconds and then opens the relay while still collecting data
    //then closes the relay after 1.5 seconds and will continue collecting data for up to 100 seconds
    //data collection cap is defined by [final] which control max # of samples to be taken and [sam] which controls interval between samples

    //Relay control setup
    if(i*interval >= 2.0)
      digitalWrite(relay_pin, HIGH);
      tlaunch = tim();
    if(i*interval >= 3.5 && i_interval <= 4.0)
      digitalWrite(relay_pin, LOW);

    //DATA COLLECTION
    //first if statement is for sampling frequency and will control the rate at which data is collected on the very long sdata string
    //second if statement controls the rate at which sdata is dumped onto the SD card and rate at which a data sample is sent to the master
    //best to send data fairly slowly, maybe 1.5hz or so since updating the lcd too fast makes it look ugly. Also to prevent filling up the buffer
    press_int_val = bits_to_psi(analogRead(press_int_pin));
    press_ext_val = bits_to_psi(analogRead(press_ext_pin));
    if(i < final && tim() - t >= interval){
      record(tim(tlaunch));
      record(press_int_val);
      record(press_ext_val);
      sdata += '\n'

      t = tim();
      i++;
    }
    if(tim() - sT >= sInt){
      dump(); //dump function for writing to SD card and clearing out sdata
      Display();
      sT = tim();
    }
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
double tim(double offset = 0){
  return((micros()/1000000.0) - offset);
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
    dataFile.print(sdata);
    dataFile.close();
    sdata = "";
  }
  // if the file isn't open, pop up an error:
  else {
    // Put some kind of error message signal
  }
}
//Takes pressure readings, typecasts them and appends respective variables beforehand
void Display(){
  String out = "";
  out += 'a';
  out += current;
  out += 'b';
  out += String(truncate(press_int_val,2)); // internal pressure
  out += 'c';
  out += String(truncate(press_ext_val,2)); // external pressure
  out += 'd';
  out += String(truncate(time(), 2));
  out += 'e';
  //Serial.println(out);
  //c_array = out.c_str();
  char c_array[30] = "";
  for (i = 0; i < out.length(); i++)
      c_array[i] = out[i];// comment back in
  //char c_array[22] = "a0b0.123c0.234d0.345e"; // comment this out
  Serial.write(c_array);
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
