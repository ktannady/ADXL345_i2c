#include "src/ADXL345_i2c.h"
#include <SD.h>
#include <SPI.h>
File myFile;
ADXL345 ADXL345;

//SD Variable
int pinCS = 10;

//Button Variable
const byte ledPin = 3;
const byte buttonPin = 2;
volatile byte state = LOW;
bool buttonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 300;
int button_clicked = 0;

//ADXL345 Variable
#define g_RANGE 2
bool RAW_VALUE = false, average = false, startRecord = false;
byte sensor1 = 0x53;
unsigned long lastRecordTime = 0;
unsigned long recordInterval = 200;
unsigned long recordMillis = 0;
float x[50], y[50], z[50];
int index = 0;

void setupADXL345(){
    ADXL345.rawValue(RAW_VALUE);
    ADXL345.setupADXL345(sensor1);
    ADXL345.setRangeSetting(sensor1, g_RANGE, false);
    //ADXL345.calibrate(sensor1, 33, -28, -405);
}

void setupSD(){
    pinMode(pinCS, OUTPUT);
    if (SD.begin())
    {
        Serial.println("SD card is ready to use.");
    }else
    {
        Serial.println("SD card initialization failed");
        return;
    }
}

void setupButton(){
    pinMode(ledPin, OUTPUT);
    pinMode(buttonPin, INPUT_PULLUP);
}

void writeToSD(float x, float y, float z, unsigned long time){
    myFile = SD.open("Data.txt", FILE_WRITE);
    if (myFile) {    
        myFile.print(x);myFile.print(",");
        myFile.print(y);myFile.print(",");
        myFile.print(z);myFile.print(",");
        myFile.println((float)time/1000);
        myFile.close(); // close the file
    }    
    else {
        Serial.println("Error"); // if the file didn't open, print an error
    }        
}

void writeNewline(){
    myFile = SD.open("Data.txt", FILE_WRITE);
    if (myFile) {    
        myFile.println();        
        myFile.close(); // close the file
    }    
    else {
        Serial.println("Error"); // if the file didn't open, print an error
    } 
}

void writeTableName(){
    myFile = SD.open("Data.txt", FILE_WRITE);
    if (myFile) {    
        myFile.println("X,Y,Z,Time");        
        myFile.close(); // close the file
    }    
    else {
        Serial.println("Error"); // if the file didn't open, print an error
    }    
}

void setup(){
    Serial.begin(9600);
    setupSD();
    setupButton();
    setupADXL345();
    Serial.println("Press button to Start Recording..");
}

void loop(){
    buttonState = digitalRead(buttonPin);
    if ((millis() - lastDebounceTime) > debounceDelay) {    
        if (buttonState == LOW) {
            state = !state;     
            digitalWrite(ledPin, state);
            if(button_clicked % 2 == 0){
                writeTableName();                
            }else{
                Serial.println("Stop Recording..");
                writeNewline();
            }            
            button_clicked++;                    
            startRecord = !startRecord;
            recordMillis = 0;
            lastRecordTime = millis();
            lastDebounceTime = millis();                        
        }
    }
    if(startRecord == true){
        if((millis() - lastRecordTime) > recordInterval){
            ADXL345.getAxis(sensor1, average, 20, false);
            recordMillis = millis() - lastRecordTime + recordMillis;
            writeToSD(ADXL345.x, ADXL345.y, ADXL345.z, recordMillis);
            Serial.println(ADXL345.x);
            Serial.println(ADXL345.y);
            Serial.println(ADXL345.z);                                              
            Serial.print("Millis : ");
            Serial.println(recordMillis);            
            lastRecordTime = millis();
        }
    }                
}