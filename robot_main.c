// File: robot_main.c
// Authors: Seth Nielsen, Aaron Bame, Derek Sanchez, Carson Zaugg
// Created on November 4, 2016, 3:10 PM

#include "robot_config.h"
#include "xc.h"
#include <stdio.h>

// Speeds (slowest to fastest):
// const int v1 = 2900;
// const int v2 = 3300;
// const int v3 = 4200;
// const int v4 = 5000;
// const int v5 = 6000;

const int v1 = 4000;
const int v2 = 4500;
const int v3 = 5000;
const int v4 = 6000;
const int v5 = 7000;

const int vturn = 6000; // rotation during START/ROTATE

// Distances
const int dist = 5300; // Dispenser to X center
const int turn = 1900; // ~ 180 deg
const int to_corner = 10000; // Finding dispenser in the beginning
const int slow_dist = 5100;

void speedup(void) {
    int a = 200;
    int b = 400;
    int c = 550;
    int d = 700;
    int e = slow_dist;

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
    int a = slow_dist;
    int b = dist - 100;
    int c = dist;

    if (steps >= a && steps < b) {
        OC1RS = v2;
    }
    else if (steps >= b && steps < c) {
        OC1RS = v3;
    }
}



int main(void) {

    pins_config();
    OC_config();
    T1_config();
    T2_config();
    T3_config();
    T4_config();
    T5_config();
    CN_config();
    ad_config();
    comp_config();

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
                OC1RS = vturn;
                // speedup();
                break;
            case ROTATE:
                // Rotate opposite direction to face corner
                if (steps < turn) {
                    _LATB12 = 0;
                    _LATB13 = 1;
                    OC1RS = vturn;
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
                OC3R = closed*OC3RS; // close release
                _LATB7 = 0;  // shooting motors off
                _LATB4 = 1;  // buttons out
                if (steps < (dist+2000)) {
                    speedup();
                }
                else if (steps >= (dist+2000) && steps < 10000) {
                    _LATB4 = 0;
                    TMR3 = 0;
                    angle_pad = 0;
                    pad_count = 0;
                    state = COLLECT;
                }
                else if (steps >= 10000 && steps < (25000)) {
                    OC1RS = v3;
                }
                else if (steps >= (25000)) {
                    _LATB4 = 0;
                    TMR3 = 0;
                    angle_pad = 0;
                    pad_count = 0;
                    state = COLLECT;
                }
                OC1R = 0.5*OC1RS;
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
                    OC1R = 0.5*OC1RS;
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
                OC3R = closed*OC3RS; // close release
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
                OC3R = open*OC3RS;  // open release
                break;
        }
    }
    return 0;
}
