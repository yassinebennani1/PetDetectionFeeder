/*#include "mbed.h"
#include "ultrasonic.h"
#include "Servo.h"

Serial pc(USBTX, USBRX);
AnalogIn water_sensor(p20);
// Set the trigger pin to p6 and the echo pin to p7 have updates every .1 seconds and a timeout after 1
// second, and call dispenseDogFood when the distance changes
ultrasonic dog_food_sensor(p6, p7, 0.1, 1, &dispenseDogFood);
// Set the trigger pin to p12 and the echo pin to p13 have updates every .1 seconds and a timeout after 1
// second, and call dispenseCatFood when the distance changes
ultrasonic cat_food_sensor(p12, p13, 0.1, 1, &dispenseCatFood);
PwmOut water_pump(p25);
Servo dog_food_servo(p21);
Servo cat_food_servo(p22);

bool food_is_filling;
bool water_is_filling;

const char DOG_SIGNAL = "1";
const char CAT_SIGNAL = "2";
const float WATER_HIGH_THRESHOLD = 0.40;
const float WATER_LOW_THRESHOLD = 0.33;
const float WATER_PUMP_ON = 7.0;
const float WATER_PUMP_OFF = 0.0;
const int CAT_FOOD_THRESHOLD = 200.0;
const int DOG_FOOD_THRESHOLD = 200.0;
const float SERVO_CLOSE = 0.0;
const float SERVO_OPEN = 0.5;

void dispenseCatFood(int distance) {
    // return true if still filling up food, false otherwise
    if (distance < CAT_FOOD_THRESHOLD) {
        cat_food_servo = SERVO_OPEN;
        food_is_filling = true;
    } else {
        cat_food_servo = SERVO_CLOSE;
        food_is_filling = false;
    }
}

void dispenseDogFood(int distance) {
    // return true if still filling up food, false otherwise
    if (distance < DOG_FOOD_THRESHOLD) {
        dog_food_servo = SERVO_OPEN;
        food_is_filling = true;
    } else {
        dog_food_servo = SERVO_CLOSE;
        food_is_filling = false;
    }
}

bool dispenseWater() {
    // return true if still filling up water, false otherwise
    float water = (float) water_sensor;
    // for debugging:
    // pc.printf("water level:  %9f\n\r", water);
    if (water > WATER_HIGH_THRESHOLD) {
        // if water level is too high, turn off pump
        water_pump = WATER_PUMP_OFF;
        water_is_filling = false;
    } else if (water < WATER_LOW_THRESHOLD) {
        // if water level is too low, turn on pump
        water_pump = WATER_PUMP_ON;
        water_is_filling = true;
    } else {
        // if water level is between high and low, turn off pump
        water_pump = WATER_PUMP_OFF;
        water_is_filling = false;
    }
}

int main() {
    char signal;
    dog_food_sensor.startUpdates(); //start measuring the distance
    cat_food_sensor.startUpdates(); //start measuring the distance
    while (1) {
        signal = "0";
        // read in signal from Pi 3
        if (pc.readable()) {
            signal = pc.getc();
        }

        // keep looping if no signal
        if (signal != "0") {
            water_is_filling = true;
            food_is_filling = true;
            while (water_is_filling && food_is_filling) {
                // call function to dispense water
                dispenseWater();

                // dispense type of food depending on signal from Pi 3
                if (signal == DOG_SIGNAL) {
                    // dog
                    dog_food_sensor.checkDistance();
                } else if (signal == CAT_SIGNAL) {
                    // cat
                    cat_food_sensor.checkDistance();
                }
            }
            wait(10);
        }
    }
}
*/



#include "mbed.h"
#include "Servo.h"
Servo catServo(p21);
Servo dogServo(p22);
DigitalOut myled(LED1);
DigitalOut myled2(LED2);
PwmOut Ctrl(p25);
Serial pc(USBTX, USBRX);
AnalogIn water(p20);

DigitalOut trigger(p10);
DigitalIn  echo(p12);
int distance = 0;
int correction = 0;
Timer sonar;

DigitalOut trigger1(p6);
DigitalIn  echo1(p8);
int distance1 = 0;
int correction1 = 0;
Timer sonar1;

int dist1=0;
int dist2=0;
int dist3=0;

int dist4=0;
int dist5=0;
int dist6=0;
float wat;
bool cond;
bool dogSensed = true;
bool catSensed = true;
bool waterSensed = dogSensed || catSensed;
    
int main(){
    catServo = 0.0;
    dogServo = 1.0;
    wait(2);
    
    while(1){
        //dog food
        if(dogSensed){
            printf("dooooooooooog!!!!!!");
            sonar.reset();
            // measure actual software polling timer delays
            // delay used later in time correction
            // start timer
            sonar.start();
            // min software polling delay to read echo pin
            while (echo==2) {};
            myled2 = 0;
            // stop timer
            sonar.stop();
            // read timer
            correction = sonar.read_us();
            printf("Approximate software overhead timer delay is %d uS\n\r",correction);
            //Loop to read Sonar distance values, scale, and print
            while(dogSensed) {
                // trigger sonar to send a ping
                trigger = 1;
                myled = 1;
                myled2 = 0;
                sonar.reset();
                wait_us(10.0);
                trigger = 0;
                myled = 0;
                //wait for echo high
                while (echo==0) {};
                myled2=echo;
                //echo high, so start timer
                sonar.start();
                //wait for echo low
                while (echo==1) {};
                //stop timer and read value
                sonar.stop();
                //subtract software overhead timer delay and scale to cm
                distance = (sonar.read_us()-correction)/58.0;
                myled2 = 0;
                dist1=dist2;
                dist2=dist3;
                dist3 = distance;
                printf(" %d dog cm \n\r",distance);
                if(dist1 >= 50 && dist2>=50 && dist3 >=50){
                    dogSensed = false;
                    printf("-------------------------------------------------");
                    break;
                }
                if(distance>=0 && distance <=40){
                    if(distance <= 40 && distance >= 23){
                        dogServo = 0.3;
                        wait(0.3);
                        dogServo = 1.0;
                        wait(0.7); 
                    }   
                }
                //wait so that any echo(s) return before sending another ping
                wait(0.2);
            }
        }


        // cat food
        if (catSensed){
            printf("caaaaaaaaaaaaat!!!!!!");
            sonar1.reset();
            // measure actual software polling timer delays
            // delay used later in time correction
            // start timer
            sonar1.start();
            // min software polling delay to read echo pin
            while (echo1==2) {};
            myled2 = 0;
            // stop timer
            sonar1.stop();
            // read timer
            correction1 = sonar1.read_us();
            printf("Approximate software overhead timer delay is %d uS\n\r",correction1);
            //Loop to read Sonar distance values, scale, and print
            while(catSensed) {
                // trigger sonar to send a ping
                trigger1 = 1;
                myled = 1;
                myled2 = 0;
                sonar1.reset();
                wait_us(10.0);
                trigger1 = 0;
                myled = 0;
                //wait for echo high
                while (echo1==0) {};
                myled2=echo1;
                //echo high, so start timer
                sonar1.start();
                //wait for echo low
                while (echo1==1) {};
                //stop timer and read value
                sonar1.stop();
                //subtract software overhead timer delay and scale to cm
                distance1 = (sonar1.read_us()-correction)/58.0;
                myled2 = 0;
                dist4=dist5;
                dist5=dist6;
                dist6 = distance1;
                printf(" %d cat cm \n\r",distance1);
                if((dist4 >= 41 && dist5>=41 && dist6 >=41) || (dist4 <= 22 && dist5 <=22 && dist6 <=22)){
                    printf("-------------------------------------------------");
                    catSensed = false;
                    break;
                }
                if(distance1>=0 && distance1 <=40){
                    if(distance1 <= 35 && distance1 >= 23){
                        catServo = 0.5; 
                        wait(0.3);
                        catServo = 0.0; 
                        wait(0.7);
                    }
                }
                //wait so that any echo(s) return before sending another ping
                wait(0.2);
            }
        }
        //water filling
        if(waterSensed){
            cond = true;
            while(cond) {
                wat = (float) water;
                pc.printf("water level:  %9f\n\r",wat);
                if(wat>=0.45){
                    myled= 1;
                    Ctrl =0;
                    break;
                }
                else if(wat<0.20){
                    myled=0;
                    Ctrl = 7;
                }
                else{
                    Ctrl = 0;
                }
                wait(0.3);
            }
        }
    }
}


    

    
