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
            // get sonar correction
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
            // get sonar correction
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
