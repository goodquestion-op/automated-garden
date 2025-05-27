#include <DFRobot_DS1307.h>
DFRobot_DS1307 DS1307;

const int DHTPIN = 9;
#include "DHT.h"
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#include <SD.h>
File myFile;
const int CSPin = 10;

const int p1 = 5;
const int p2 = 6;

const int photo = A2;

const int d = 250;
const int pumping = 1000;

const int moist =  A0;
float value;

const int timing = 1;


int old = 0;
int notold = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();

  pinMode(p1, OUTPUT);
  pinMode(p2, OUTPUT);

  

  pinMode(photo, INPUT);

  pinMode(moist, INPUT);

  RTC_setup();

  SD_set();
  //Serial.println("Test");

  old = DS1307.getTypeTime(DS1307.eHR);
  notold = old+timing;
}

void loop() {
  
  float vals[5];
  // put your main code here, to run repeatedly:
  if (notold == old){
    DHTISFUN(vals[2],vals[3]);
    moist_sen(vals[1]);
    light(vals[0]);
    SDwrite(vals[0],vals[1],vals[2],vals[3],vals[4]);

    old = DS1307.getTypeTime(DS1307.eHR);
    notold = (old +1);
 //stuff
  if (value <= 400 ){
    moist_sen(vals[1]);
    pump();
    delay(pumping);
    notpump();
   }
  }else{
    

  
  
  DHTISFUN(vals[2],vals[3]);
  delay(d);
  RTC_go(vals[4]);
  Serial.println(" ");
 
  //pump();
  delay(d);
 
 light(vals[0]);
 Serial.println(" ");
 delay(d);

  
  Serial.println(" ");
  delay(d);
  
 moist_sen(vals[1]);


  Serial.println(" ");
  old = DS1307.getTypeTime(DS1307.eHR);
 // Serial.println(old);
 // Serial.println(notold);


  }
}
void notpump(){
digitalWrite(p1, LOW);
digitalWrite(p2, LOW);

}

void DHTISFUN(float &val3,float &val4){
 // Read Values

  float humid = dht.readHumidity();
  float temp = dht.readTemperature();
  Serial.print("Humidity = ");
  Serial.print((float)humid,1); // The 1 denotes how many decimal places we want
  Serial.println("%");         // This library can only return 1 decimal place
  Serial.print("Temperature = ");
  Serial.print((float)temp,1);
  Serial.println("C");
  val3 = temp;
  val4 = humid;
}

void coversion(float sensorValue){
  float volatage = (float)sensorValue * 5 / 1023;
  Serial.print(volatage*1000);
  Serial.println("mv");
}

void pump(){

digitalWrite(p1, LOW);
digitalWrite(p2, HIGH);

}



void moist_sen(float &val2){
  value = analogRead(moist);
  Serial.println("moist");
  Serial.print(value);
  Serial.println(" ");
  val2 = value;

}

void light(float &val1){
  float photoData = analogRead(photo);
  Serial.print("photoresistor ");
  coversion(photoData);
  photoData = (photoData*5 / 1023)*1000;
  val1 = photoData;
}

void RTC_setup(){

  while( !(DS1307.begin()) ){

    Serial.println("Communication with device failed, please check connection");

    delay(3000);

  }

  Serial.println("Begin ok!");

  DS1307.start();



  DS1307.setSqwPinMode(DS1307.eSquareWave_1Hz);
  old = DS1307.getTypeTime(DS1307.eMIN);
}

void RTC_go(float &val5){
   uint16_t getTimeBuff[7] = {0};

  DS1307.getTime(getTimeBuff);

  char outputarr[128];

  sprintf(outputarr, "time: %d/%d/%d-%d %d:%d:%d\r\n",

            getTimeBuff[6],

            getTimeBuff[5],

            getTimeBuff[4],

            getTimeBuff[3],

            getTimeBuff[2],

            getTimeBuff[1],

            getTimeBuff[0]

            );

  Serial.print(outputarr);
 // val5 = outputarr;
   
}

void SD_set(){

  Serial.print("Initializing SD card ;)");
  pinMode(CSPin, OUTPUT);

   if (!SD.begin(CSPin)) {

    Serial.println("initialization failed!");

    return;

  }

  Serial.println("initialization done!");

  myFile = SD.open("data3.txt", FILE_WRITE);

   if (myFile) {
    Serial.print("Writing to data.txt...");
    myFile.println("Initializing file. 1, 2, 3.");

    myFile.close();
    Serial.println("done.");

  } else {
  Serial.println("error opening test.txt :(");


  }
}

void SDwrite(float &val1,float &val2,float &val3,float &val4,float &val5){
   myFile = SD.open("data3.txt", FILE_WRITE);
      if (myFile) {
      uint16_t getTimeBuff[7] = {0};

      DS1307.getTime(getTimeBuff);

      char outputarr[128];

    
    sprintf(outputarr, "TIMESTAMP: %d %d %d %d:%d:%d",
                      getTimeBuff[6],
                      getTimeBuff[5],
                      getTimeBuff[4],
                      getTimeBuff[2],
                      getTimeBuff[1],
                      getTimeBuff[0]
    );
    myFile.print(outputarr);
    //time?
    myFile.print(", Humidity  ");
    myFile.print(val4);
    myFile.print("%");
    //humid
    myFile.print(", Temperature  ");
    myFile.print(val3);
    myFile.print("C");
     //temp
    myFile.print(", moisture value ");
    myFile.print(val2);
    
    //moist
    myFile.print(", ");
    myFile.print(val1);
    myFile.print("mv");
    myFile.print (" of sun");
    //sun
    myFile.println(" ");

    myFile.close();
    Serial.println("done.");

  } else {
  Serial.println("error opening test.txt :(");

  }
}
