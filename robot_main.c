// File: robot_main.c
// Authors: Seth Nielsen, Aaron Bame, Derek Sanchez, Carson Zaugg
// Created on November 4, 2016, 3:10 PM

#include "robot_config.h"
#include "xc.h"
#include <stdio.h>

// Speeds (slowest to fastest):
int v1 = 5000;
int v2 = 5500;
int v3 = 6000;
int v4 = 7000;
int v5 = 8000;
int vrotate = 9000;
int vrotate2 = 8000;

int dist = 3220;
int turn = 1280;

void speedup(void) {
    int a = 200;
    int b = 400;
    int c = 550;
    int d = 700;
    int e = 2860;

    if (steps < a) {
        _CMIE = 0;
        OC1RS = v5;
    }
    else if (steps >= a && steps < b) {
        _CMIE = 0;
        OC1RS = v4;
    }
    else if (steps >= b && steps < c) {
        _CMIE = 0;
        OC1RS = v3;
    }
    else if (steps >= c && steps < d) {
        _CMIE = 0;
        OC1RS = v2;
    }
    else if (steps >= d && steps < e) {
        _CMIE = 1;
        OC1RS = v1;
    }
}

void slowdown(void) {
    int a = 2800;
    int b = 2900;
    int c = 3100;

    if (steps >= a && steps < b) {
        OC1RS = v2;
    }
    else if (steps >= b && steps < c) {
        OC1RS = v3;
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
                OC1RS = vrotate;
                // speedup();
                break;
            case ROTATE:
                // Rotate opposite direction to face corner
                //180 deg = 1400
                if (steps < turn) {
                    _LATB12 = 0;
                    _LATB13 = 1;
                    OC1RS = vrotate2;
                }
                else if (steps >= turn) {
                    steps = 10000; // for initially finding corner
                    state = REVERSE;
                }
                break;
            case REVERSE:
                // Drive towards corner until buttons pressed
                _CMIE = 0;
                _LATB12 = 0; // steppers
                _LATB13 = 0;
                stopped = 0;
                OC3R = 0.025*OC3RS; // close release
                _LATB7 = 0;  // shooting motors off
                _LATB4 = 1;  // buttons out
                if (steps < (dist+300)) {
                    speedup();
                }
                else if (steps >= (dist+300) && steps < 10000) {
                    _LATB4 = 0;
                    TMR3 = 0;
                    angle_pad = 0;
                    pad_count = 0;
                    state = COLLECT;
                }
                else if (steps >= 10000 && steps < 13500) {
                    OC1RS = v1;
                }
                else if (steps >= 13500) {
                    _LATB4 = 0;
                    TMR3 = 0;
                    angle_pad = 0;
                    pad_count = 0;
                    state = COLLECT;
                }
                OC1R = 0.9*OC1RS;
                break;
            case COLLECT:
                // Swipe paddle to collect 6 balls
                _LATB4 = 0; // buttons_out
                _LATB8 = 0; // release disconnected
                OC1R = 0;   // steppers
                break;
            case FORWARD:
                // Drive to center, aim for active goal
                _LATB8 = 1; // connect release
                _LATB7 = 1; // shooting motors
                stopped = 0;
                if (steps < dist) {
                    speedup();
                    slowdown();
                    _LATB12 = 1;
                    _LATB13 = 1;
                    OC1R = 0.9*OC1RS;
                }
                else if (steps >= dist) {
                    state = AIM;
                    _CMIE = 1;
                    TMR4 = 0; // time needed to aim
                    OC1R = 0;
                    stopped = 1;
                    steps = 0;
                }
                break;
            case AIM:
                // Rotate turret to face active goal (either stopped or driving forward)
                OC3R = 0.025*OC3RS; // close release
                if (steps < dist) {
                    // keep driving if not yet at center
                    speedup();
                    slowdown();
                }
                else if (steps >= dist) {
                    _CMIE = 1;
                    OC1R = 0;
                    stopped = 1;
                    steps = 0;
                }
                break;
            case SHOOT:
                _CMIE = 1;
                OC3R = 0.06*OC3RS;  // open release
                break;
        }
    }
    return 0;
}
