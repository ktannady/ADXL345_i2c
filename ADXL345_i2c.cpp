#include "ADXL345_i2c.h"

void ADXL345::rawValue(bool raw){
    if(raw)
        divider = 1;
}

void ADXL345::setupADXL345(byte sensor_address){
    Wire.begin(); // Initiate the Wire library 
    // Set ADXL345 in measuring mode
    Wire.beginTransmission(sensor_address); // Start communicating with the device 
    Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
    // Enable measurement
    Wire.write(8); // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable 
    Wire.endTransmission();
    delay(10);
}

void ADXL345::getAxis(byte sensor_address, bool average, int total, bool debug){
    // === Read acceleromter data === // 
    if(average){
        for(int i = 0; i<total;i++){
            Wire.beginTransmission(sensor_address);
            Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
            Wire.endTransmission(false);
            Wire.requestFrom(sensor_address, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
            x += float(( Wire.read()| Wire.read() << 8)); // X-axis value
            
            y += float(( Wire.read()| Wire.read() << 8)); // Y-axis value
            
            z += float(( Wire.read()| Wire.read() << 8)); // Z-axis value                                    
        }
        if(debug){
            Serial.print("Xa= ");
            Serial.print((x/(divider*total))+0.01);
            Serial.print("   Ya= ");
            Serial.print(y/(divider*total));
            Serial.print("   Za= ");
            Serial.println(z/(divider*total));
        }
        x = (x/divider*total);
        y = (y/divider*total);
        y = (z/divider*total);
        
    }else{
        Wire.beginTransmission(sensor_address);
        Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
        Wire.endTransmission(false);
        Wire.requestFrom(sensor_address, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
        x = float(( Wire.read()| Wire.read() << 8)) / divider; // X-axis value
        
        y = float(( Wire.read()| Wire.read() << 8)) / divider; // Y-axis value
        
        z = float(( Wire.read()| Wire.read() << 8)) / divider; // Z-axis value
        if(debug){
            Serial.print("Xa= ");
            Serial.print(x/divider);
            Serial.print("   Ya= ");
            Serial.print(y/divider);
            Serial.print("   Za= ");
            Serial.println(z/divider);
        }        
    }    
}

void ADXL345::calibrate(byte sensor_address, int x, int y, int z){
    Wire.beginTransmission(sensor_address);
    Wire.write(0x1E);  // X-axis offset register
    Wire.write(x);
    Wire.endTransmission();
    delay(10);

    Wire.beginTransmission(sensor_address);
    Wire.write(0x1F); // Y-axis offset register
    Wire.write(y);
    Wire.endTransmission();
    delay(10);

    Wire.beginTransmission(sensor_address);
    Wire.write(0x20); // Z-axis offset register
    Wire.write(z);    
    Wire.endTransmission();
    delay(10);
}

void ADXL345::setRangeSetting(byte sensor_address, int val, bool full_res) {
    byte _s;
    byte _b;
    if(full_res){
        Serial.println("Full Resolution Mode");
        switch (val) {
            case 2:
            _s = B00001000;     
            break;
            case 4:
            _s = B00001001;
            break;
            case 8:
            _s = B00001010;
            break;
            case 16:
            _s = B00001011;
            break;
            default:
            _s = B00001000;
        } 
    }else{
        Serial.println("10bit Resolution Mode");
        switch (val) {
            case 2:
            _s = B00000000;     
            break;
            case 4:
            _s = B00000001;
            break;
            case 8:
            _s = B00000010;
            break;
            case 16:
            _s = B00000011;
            break;
            default:
            _s = B00000000;
        }
    }
    
    readFromI2C(sensor_address, 0x31, 1, &_b);
    _s |= (_b & B11101100);
    writeToI2C(sensor_address, 0x31, _s);
}

void ADXL345::writeToI2C(byte sensor_address, byte _address, byte _val) {
    Wire.beginTransmission(sensor_address);
    Wire.write(_address);
    Wire.write(_val);
    Wire.endTransmission();
}

void ADXL345::readFromI2C(byte sensor_address, byte address, uint8_t num, byte _buff[]) {
    Wire.beginTransmission(sensor_address);
    Wire.write(address);
    Wire.endTransmission();

    //  Wire.beginTransmission(ADXL345_DEVICE);
    // Wire.reqeustFrom contains the beginTransmission and endTransmission in it. 
    Wire.requestFrom(sensor_address, num);  // Request 6 Bytes

    int i = 0;
    while(Wire.available())
    {
        _buff[i] = Wire.read();       // Receive Byte
        i++;
    }
    if(i != num){
        Serial.println("Error");
    }
    //  Wire.endTransmission();
}