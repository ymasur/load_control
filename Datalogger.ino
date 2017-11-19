#include <Arduino.h>

/*
  SD card datalogger
 
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 10
 
 original, created  by Tom Igoe
 28.10.12 Yves Masur
 
 */

#include <SD.h>  //Pilote pour carte SD
const char * FNAME = "ENERGY.TXT" // Nom du fichier

void init_file()
{
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(SS_PIN, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(SS_PIN)) 
  {
    Serial.println("SD card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("SD card initialized.");
}

void log2File(String dataString)
{
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open(FNAME, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) 
  {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }  
  // if the file isn't open, pop up an error:
  else 
  {
    Serial.println("error opening " FNAME);
  } 
}









