#include "mbed.h"
#include "FSR.h"
#include "Servo.h"

Serial pc(USBTX, USBRX);
AnalogIn water_sensor(p20);
FSR dog_food_sensor(p18, 10); // Pin 18 is used as the AnalogIn pin and a 10k resistor is used as a voltage divider
FSR cat_food_sensor(p19, 10); // Pin 19 is used as the AnalogIn pin and a 10k resistor is used as a voltage divider
PwmOut water_pump(p25);
Servo dog_food_servo(p21);
Servo cat_food_servo(p22);

const int DOG_SIGNAL = 1;
const int CAT_SIGNAL = 2;
const float WATER_HIGH_THRESHOLD = 0.40;
const float WATER_LOW_THRESHOLD = 0.33;
const float WATER_PUMP_ON = 7.0;
const float WATER_PUMP_OFF = 0.0;
const float CAT_FOOD_THRESHOLD = 200.0;
const float DOG_FOOD_THRESHOLD = 200.0;
const float SERVO_CLOSE = 0.0;
const float SERVO_OPEN = 0.5;

bool dispenseCatFood() {
    // return true if still filling up food, false otherwise
    if (cat_food_sensor.readWeight() < CAT_FOOD_THRESHOLD) {
        cat_food_servo = SERVO_OPEN;
        return true;
    } else {
        cat_food_servo = SERVO_CLOSE;
        return false;
    }
}

bool dispenseDogFood() {
    // return true if still filling up food, false otherwise
    if (dog_food_sensor.readWeight() < DOG_FOOD_THRESHOLD) {
        dog_food_servo = SERVO_OPEN;
        return true;
    } else {
        dog_food_servo = SERVO_CLOSE;
        return false;
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
        return false;
    } else if (water < WATER_LOW_THRESHOLD) {
        // if water level is too low, turn on pump
        water_pump = WATER_PUMP_ON;
        return true;
    } else {
        // if water level is between high and low, turn off pump
        water_pump = WATER_PUMP_OFF;
        return false;
    }
}

int main() {
    char signal;
    while (1) {
        signal = 0;
        // read in signal from Pi 3
        if (pc.readable()) {
            signal = pc.getc();
        }

        // keep looping if no signal
        if (signal != 0) {
            bool water_is_filling = true;
            bool food_is_filling = true;
            while (water_is_filling && food_is_filling) {
                // call function to dispense water
                water_is_filling = dispenseWater();

                // dispense type of food depending on signal from Pi 3
                if (signal == DOG_SIGNAL) {
                    // dog
                    food_is_filling = dispenseDogFood();
                } else if (signal == CAT_SIGNAL) {
                    // cat
                    food_is_filling = dispenseCatFood();
                }
            }
            wait(10);
        }
    }
}
