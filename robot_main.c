// File: robot_main.c
// Authors: Seth Nielsen, Aaron Bame, Derek Sanchez, Carson Zaugg
// Created on November 4, 2016, 3:10 PM

#include "robot_config.h"
#include "xc.h"
#include <stdio.h>

void speedup(void) {
    if (steps < 30) {
        OC1RS = 40000;
    }
    else if (steps >= 30 && steps < 60) {
        OC1RS = 30000;
    }
    else if (steps >= 60 && steps < 90) {
        OC1RS = 20000;
    }
    else if (steps >= 90 && steps < 715) {
        _CMIE = 1;
        OC1RS = 20000;
    }
}

void slowdown(void) {
    if (steps >= 715 && steps < 745) {
        OC1RS = 20000;
    }
    else if (steps >= 745 && steps < 775) {
        OC1RS = 30000;
    }
    else if (steps >= 775 && steps < 805)  {
        OC1RS = 40000;
    }
}

int main(void) {
    
    OC_config();
    T1_config();
    T2_config();
    T3_config();
    T4_config();
    T5_config();
    CN_config();
    ad_config();
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
                if (steps < 30) {
                    OC1RS = 40000;
                }
                else if (steps >= 30 && steps < 60) {
                    OC1RS = 30000;
                }
                else if (steps >= 60) {
                    OC1RS = 30000;
                }
                break;
            case ROTATE:
                // Rotate opposite direction to face corner
                //180 deg = 350
                if (steps < 320) {
                    _LATB12 = 0;
                    _LATB13 = 1;
                }
                else if (steps >= 320) {
                    steps = 10000; // for initially finding corner
                    state = REVERSE;
                }
                break;
            case REVERSE:
                _LATA1 = 0;
                // Drive towards corner until buttons pressed
                _CMIE = 0;
                _LATB12 = 0; // steppers
                _LATB13 = 0;
                stopped = 0;
                OC3R = 0.025*OC3RS; // close release
                _LATB7 = 0;  // shooting motors off
                _LATB4 = 1;  // buttons out
                if (steps < 805) {
                    speedup();
                    slowdown();
                }
                else if (steps >= 10000) {
                    OC1RS = 30000;
                }
                // else if (steps >= 805) {
                //     _LATB4 = 0;
                //     TMR3 = 0;
                //     angle_pad = 0;
                //     pad_count = 0;
                //     state = COLLECT;
                // }
                OC1R = 0.5*OC1RS;
                break;
            case COLLECT:
            _LATA1 = 0;
                // Swipe paddle to collect 6 balls
                _LATB4 = 0; // buttons_out
                _LATB8 = 0; // release disconnected
                OC1R = 0;   // steppers
                break;
            case FORWARD:
            _LATA1 = 0;
                // Drive to center, aim for active goal
                _LATB8 = 1; // connect release
                _LATB7 = 1; // shooting motors
                stopped = 0;
                if (steps < 805) {
                    speedup();
                    slowdown();
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
                _LATA1 = 1;
                // Rotate turret to face active goal (either stopped or driving forward)
                OC3R = 0.025*OC3RS; // close release
                if (steps < 805) {
                    // keep driving if not yet at center
                    speedup();
                    slowdown();
                }
                else if (steps >= 805) {
                    OC1R = 0;
                    stopped = 1;
                    steps = 0;
                }
                break;
            case SHOOT:
                _LATA1 = 0;
                OC3R = 0.06*OC3RS;  // open release
                break;
        }
    }
    return 0;
}
