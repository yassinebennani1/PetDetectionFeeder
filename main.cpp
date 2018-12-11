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
RawSerial  pi(USBTX, USBRX);
AnalogIn water(p20);

// dog sonar initializations
DigitalOut dogTrigger(p10);
DigitalIn  dogEcho(p12);
int dogSonarDistance = 0;
int dogSonarCorrection = 0;
Timer dogSonarTimer;

// cat sonar initializations
DigitalOut catTrigger(p6);
DigitalIn  catEcho(p8);
int catSonarDistance = 0;
int catSonarCorrection = 0;
Timer catSonarTimer;

int dist1=0;
int dist2=0;
int dist3=0;

int dist4=0;
int dist5=0;
int dist6=0;
float wat;
bool cond;
bool dogSensed = false;
bool catSensed = false;
bool waterSensed = false;

void fillDogFood() {
    printf("dooooooooooog!!!!!!");
    // dogTrigger dogSonarTimer to send a ping
    dogTrigger = 1;
    myled = 1;
    myled2 = 0;
    dogSonarTimer.reset();
    wait_us(10.0);
    dogTrigger = 0;
    myled = 0;
    //wait for dogEcho high
    while (dogEcho==0) {};
    myled2=dogEcho;
    //dogEcho high, so start timer
    dogSonarTimer.start();
    //wait for dogEcho low
    while (dogEcho==1) {};
    //stop timer and read value
    dogSonarTimer.stop();
    //subtract software overhead timer delay and scale to cm
    dogSonarDistance = (dogSonarTimer.read_us()-dogSonarCorrection)/58.0;
    myled2 = 0;
    dist1=dist2;
    dist2=dist3;
    dist3 = dogSonarDistance;
    printf(" %d dog cm \n\r",dogSonarDistance);
    if(dist1 >= 50 && dist2>=50 && dist3 >=50){
        dogSensed = false;
        printf("-------------------------------------------------");
        break;
    }
    if(dogSonarDistance>=0 && dogSonarDistance <=40){
        if(dogSonarDistance <= 40 && dogSonarDistance >= 23){
            dogServo = 0.3;
            wait(0.3);
            dogServo = 1.0;
            wait(0.7);
        }
    }
    //wait so that any dogEcho(s) return before sending another ping
    wait(0.2);
}

void fillCatFood() {
    printf("caaaaaaaaaaaaat!!!!!!");
    // trigger sonar to send a ping
    catTrigger = 1;
    catSonarTimer.reset();
    wait_us(10.0);
    catTrigger = 0;
    //wait for echo high
    while (catEcho==0) {};
    //echo high, so start timer
    catSonarTimer.start();
    //wait for echo low
    while (catEcho==1) {};
    //stop timer and read value
    catSonarTimer.stop();
    //subtract software overhead timer delay and scale to cm
    catSonarDistance = (catSonarTimer.read_us()-correction)/58.0;
    dist4=dist5;
    dist5=dist6;
    dist6 = catSonarDistance;
    printf(" %d cat cm \n\r",catSonarDistance);
    if((dist4 >= 41 && dist5>=41 && dist6 >=41) || (dist4 <= 22 && dist5 <=22 && dist6 <=22)){
        printf("-------------------------------------------------");
        catSensed = false;
        break;
    }
    if(catSonarDistance>=0 && catSonarDistance <=40){
        if(catSonarDistance <= 35 && catSonarDistance >= 23){
            catServo = 0.5;
            wait(0.3);
            catServo = 0.0;
            wait(0.7);
        }
    }
    //wait so that any echo(s) return before sending another ping
    wait(0.2);
}

void fillWater() {
    wat = (float) water;
    if(wat >= 0.45){
        Ctrl = 0;
        waterSensed = false;
    } else if(wat<0.20){
        Ctrl = 7;
    } else{
        Ctrl = 0;
        waterSensed = false;
    }
    wait(0.3);
}

void dev_recv()
{
   char temp = 0;
   myled = !myled;
   while(pi.readable()) {
       temp = pi.getc();
       if (temp=='2') {
           myled2 = 1;
           catSensed = true;
       }
       if (temp=='1') {
           myled2 = 1;
           dogSensed = true;
       }
   }
}

int main(){
    catServo = 0.0;
    dogServo = 1.0;
    pi.baud(9600);
    pi.attach(&dev_recv, Serial::RxIrq);
    wait(2);

    while(1){
        dogSensed = false;
        catSensed = false;
        // check for signals from Pi
        while(!dogSensed && !catSensed) {
            sleep();
        }

        // dog
        if (dogSensed) {
            waterSensed = true;
            dogSonarTimer.reset();
            dogSonarTimer.start();
            while (dogEcho==2) {};
            dogSonarTimer.stop();
            dogSonarCorrection = dogSonarTimer.read_us();
            while (dogSensed || waterSensed) {
                if (dogSensed) {
                    fillDogFood();
                }

                if (waterSensed) {
                    fillWater();
                }
            }
        }

        // cat food
        if (catSensed) {
            waterSensed = true;
            catSonarTimer.reset();
            catSonarTimer.start();
            while (catEcho==2) {};
            catSonarTimer.stop();
            catSonarCorrection = catSonarTimer.read_us();
            while (catSensed || waterSensed) {
                if (catSensed) {
                    fillCatFood();
                }

                if (waterSensed) {
                    fillWater();
                }
            }
        }

        wait(10);
        myled2 = 0;
    }
}
