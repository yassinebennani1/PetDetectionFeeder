#include "mbed.h"
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

const int DOG_SIGNAL = 1;
const int CAT_SIGNAL = 2;
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
        signal = 0;
        // read in signal from Pi 3
        if (pc.readable()) {
            signal = pc.getc();
        }

        // keep looping if no signal
        if (signal != 0) {
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
