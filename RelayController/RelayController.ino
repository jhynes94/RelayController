/*
 * Derived from:
 * SD card Standard ReadWrite code
 * RTC code from RTClib - PCF8523
 */


#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>

File myFile;

String buffer;

RTC_PCF8523 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

uint32_t relayArray[8];

void setup () {
  
  Serial.begin(9600);
  
  Serial.print("Initializing SD card...");
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  
  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.initialized()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    //rtc.adjust(DateTime(2019, 6, 3, 12, 30, 0));
  }  

  
  for(int i=0; i < sizeof(relayArray); i++){
//    pinMode(i, OUTPUT);
//    digitalWrite(i, 0);
  }
//  for(int i=0; i < sizeof(relayArray); i++){
//    digitalWrite(i, 1);
//    delay(300);
//    digitalWrite(i, 0);
//    
//  }
  
}

void loop () {
  
  Serial.println("Main Loop!");

  //Set all relays to zero
  //memset(relayArray, 0, sizeof relayArray);
  for(int i=0; i < sizeof(relayArray); i++){
    relayArray[i] = 0;
  }

  DateTime now = rtc.now();
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);
  
  
  Serial.println("opened 0");
  myFile = SD.open("schedule.txt");
  
  Serial.println("opened 1");

        
  if (myFile) {
    
  Serial.println("attempting to read");
    
    // read from the file until there's nothing else in it:
    while (myFile.available()) {

      //Read one line
      buffer = myFile.readStringUntil('\n');  

      //For line
      char charBuf[buffer.length()+1];
      buffer.toCharArray(charBuf, buffer.length());
      Serial.print("Command: ");
      Serial.println(charBuf);

      //For Relay
      Serial.print("Relay #");
      uint32_t relayNum = *(charBuf+1)- '0';
      Serial.println(relayNum);
      
      char * pch;
      pch = strtok (charBuf,"_");
     
      while (pch != NULL)
      {

        //See if it is today
        if(*(pch)- '0' == now.dayOfTheWeek()){

          //Parse Time data
          uint32_t startHour = (*(pch+2)-'0')*10 + (*(pch+3)-'0');
          uint32_t endHour = (*(pch+11)-'0')*10 + (*(pch+12)-'0');
          uint32_t startMinute = (*(pch+5)-'0')*10 + (*(pch+6)-'0');
          uint32_t endMinute = (*(pch+14)-'0')*10 + (*(pch+15)-'0');
          uint32_t startSecond = (*(pch+8)-'0')*10 + (*(pch+9)-'0');
          uint32_t endSecond = (*(pch+17)-'0')*10 + (*(pch+18)-'0');


          Serial.print(startHour);
          Serial.print(':');
          Serial.print(startMinute);
          Serial.print(':');
          Serial.print(startSecond);
          Serial.print(' ');
          Serial.print(endHour);
          Serial.print(':');
          Serial.print(endMinute);
          Serial.print(':');
          Serial.println(endSecond);
          
          DateTime startTime(now.year(), now.month(), now.day(), startHour, startMinute, startSecond);
          DateTime endTime(now.year(), now.month(), now.day(), endHour, endMinute, endSecond);

          //See if the Hours Match
          if(startTime.unixtime() <= now.unixtime() && now.unixtime() < endTime.unixtime()){
            Serial.println("Status: ON");
            relayArray[relayNum] = 1; //Turn on Relay
          }
          
          
        }
        
        pch = strtok (NULL, "_");
      }
      //Serial.println (pch);
        
      
    }
    // close the file:
    myFile.close();
    Serial.println();
  } else {
    Serial.println("error opening test.txt");
  }

  for(int i=0; i < sizeof(relayArray); i++){
    digitalWrite(i, relayArray[i]);
  }
  
  delay(10000);
}
