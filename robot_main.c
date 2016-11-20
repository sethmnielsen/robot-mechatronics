// File: robot_main.c
// Authors: Seth Nielsen, Aaron Bame, Derek Sanchez, Carson Zaugg
// Created on November 4, 2016, 3:10 PM

#include "robot_config.h"
#include "xc.h"
#include <stdio.h>

int main(void) {

    OC_config();
    T1_config();
    T2_config();
    T3_config();
    T4_config();
    T5_config();
    CN_config();
    comp_config();
    pins_config();

    state = START;
    while (1)
    {
        switch (state)
        {
            case OFF:
                return 0;
                break;
            case START:
                // Keep rotating until finds beam
                _LATB12 = 1;
                _LATB13 = 0;
                break;
            case ROTATE:
                // Rotate opposite direction to face corner
                //180 deg = 350
                if (steps < 300) {
                    _LATB12 = 0;
                    _LATB13 = 1;
                }
                else if (steps >= 300) {
                    steps = 0;
                    state = REVERSE;
                }
                break;
            case REVERSE:
                // Drive towards corner until buttons pressed
                stopped = 0;
                _LATB7 = 0;  // shoting motors
                _LATB4 = 1;  // buttons_out
                _LATB12 = 0; // steppers
                _LATB13 = 0;
                OC1R = 0.5*OC1RS;
                break;
            case COLLECT:
                // Swipe paddle to collect 6 balls
                _LATB4 = 0; // buttons_out
                OC1R = 0;   // steppers
                break;
            case FORWARD:
                // Drive to center, aim for active goal
                stopped = 0;
                if (steps < 805) {
                    _LATB12 = 1;
                    _LATB13 = 1;
                    OC1R = 0.5*OC1RS;
                }
                else if (steps >= 805) {
                    state = AIM;
                    TMR4 = 0; // time needed to aim
                    OC1R = 0;
                    stopped = 1;
                    steps = 0;
                }
                break;
            case AIM:
                // Rotate turret to face active goal (either stopped or driving forward)
                _LATB7 = 0; // shoting motors
                if (steps < 805) {
                    // keep driving if not yet at center
                    _LATB12 = 1;
                    _LATB13 = 1;
                }
                else if (steps >= 805) {
                    OC1R = 0;
                    stopped = 1;
                    steps = 0;
                }
                break;
            case SHOOT:
                _LATB7 = 1; // shoting motors
                break;
        }
    }
    return 0;
}
