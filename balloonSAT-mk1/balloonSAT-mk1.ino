#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include "RTClib.h"

const int chipSelect = 4;

Adafruit_BNO055 bno = Adafruit_BNO055();

Adafruit_GPS GPS(&Serial1);
HardwareSerial mySerial = Serial1;

RTC_DS3231 rtc;

void setup() {
while(!Serial)  //needed for 32u4

  Serial.begin(115200);

pinMode(6, OUTPUT);     //pin #6 connected to Enable pin on GPS
digitalWrite(6, HIGH);  //set enable pin on GPS to high (enabled)


  //Serial.println("Hello");
  if(!bno.begin()){
    Serial.print("BNO055 not detected");
    while(1);
  }
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while(1);
  }
   if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  GPS.begin(9600);

    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
   // GPS.sendCommand(PGCMD_ANTENNA);
    mySerial.println(PMTK_Q_RELEASE);

//Serial.println("Initialized");

Serial.print("\nSTARTING LOGGING....");
  if (GPS.LOCUS_StartLogger())
    Serial.println(" STARTED!");
  else
    Serial.println(" no response :(");
  delay(500);

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

delay(500);

bno.setExtCrystalUse(true);

}

 uint32_t timer = 0;

void loop() {
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER); 

  DateTime now = rtc.now();


 
  
//  // if a sentence is received, we can check the checksum, parse it...
//  if (GPS.newNMEAreceived()) {
//    // a tricky thing here is if we print the NMEA sentence, or data
//    // we end up not listening and catching other sentences! 
//    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
//    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
//  
//    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
//      return;  // we can fail to parse a sentence in which case we should just wait for another
//  }



  // approximately every 2 seconds or so, print out the current stats
  if (now.unixtime() - timer > 5) { 
    timer = now.unixtime(); // reset the timer

    String dataString = "";

dataString += String(now.hour(), DEC);
dataString += ":";
dataString += String(now.minute(), DEC);
dataString += ":";
dataString += String(now.second(), DEC);
dataString += ":";
dataString += String(euler.x());
dataString += ",";
dataString += String(euler.y());
dataString += ",";
dataString += String(euler.z());
dataString += ",";

//add calibration status?


// open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

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

  }



