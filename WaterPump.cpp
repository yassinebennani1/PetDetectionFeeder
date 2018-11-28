#include "mbed.h"


DigitalOut myled(LED1);
PwmOut Ctrl(p25);
Serial pc(USBTX, USBRX);
AnalogIn water(p20);
int main() {
    float wat;
    while(1) {
        wat = (float) water;
        pc.printf("water level:  %9f\n\r",wat);
        if(wat>0.40){
            myled= 1;
            Ctrl =0;
            }
        else if(wat<0.33){
            myled=0;
            Ctrl = 7;
            }
        else {
            Ctrl = 0;
        }
        wait(0.3);
       
    }
}
