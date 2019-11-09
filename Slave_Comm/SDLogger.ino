/*
Modified code from arduino examples

 * analog sensors on analog ins 0, 1, and 2
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)
 */

#include <SPI.h>
#include <SD.h>

const int chipSelect = 4;
const int p1(A0);
const int p2(A1);

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(250000);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
}

void loop() {
  // make a string for assembling the data to log:
  String dataString = "";

  // read and record pressure data to a single line
  for (int rec = 0; rec < 3; rec++) {
    int sensor = analogRead(p1);
    int sensor2 = analogRead(p2);
    
    dataString += String(sensor);
    dataString += ",";
    dataString += String(sensor2);
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("Pressure.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
}
