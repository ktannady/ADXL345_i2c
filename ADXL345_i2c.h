#ifndef ADXL345_I2C
#define ADXL345_I2C

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Wire.h>

class ADXL345{
    public:

        float x, y, z;
        
        void rawValue(bool raw);
        void setupADXL345(byte sensor_address);
        void getAxis(byte sensor_address, bool average, int total, bool debug);        
        void calibrate(byte sensor_address, int x, int y, int z);
        void setRangeSetting(byte sensor_address, int val, bool full_res);
        void writeToI2C(byte sensor_address, byte _address, byte _val);
        void readFromI2C(byte sensor_address, byte address, int num, byte _buff[]);

    private:
        int divider = 256;
};


#endif