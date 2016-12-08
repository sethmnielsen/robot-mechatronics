// File: robot_main.c
// Authors: Seth Nielsen, Aaron Bame, Derek Sanchez, Carson Zaugg
// Created on November 4, 2016, 3:10 PM

#include "robot_config.h"
#include "xc.h"
#include <stdio.h>

// Speeds (slowest to fastest):
const int v1 = 2900;
const int v2 = 3300;
const int v3 = 4200;
const int v4 = 5000;
const int v5 = 6000;

// const int v1 = 4000;
// const int v2 = 4500;
// const int v3 = 5000;
// const int v4 = 6000;
// const int v5 = 7000;

const int vturn = 6000; // rotation during START/ROTATE

// Distances
const int dist = 5300; // Dispenser to X center
const int turn = 1900; // ~ 180 deg
const int to_corner = 10000; // Finding dispenser in the beginning
const int slow_dist = 4700;

const float r_mult = 1.0;
const int avgtime = 100;

void speedup(void) {
    int a = 200;
    int b = 400;
    int c = 550;
    int d = 700;
    int e = slow_dist;

    if (steps < a) {
        OC1RS = v5;
    }
    else if (steps >= a && steps < b) {
        OC1RS = v4;
    }
    else if (steps >= b && steps < c) {
        OC1RS = v3;
    }
    else if (steps >= c && steps < d) {
        OC1RS = v2;
    }
    else if (steps >= d && steps < e) {
        OC1RS = v1;
    }
}

void slowdown(void) {
    int a = slow_dist;
    int b = slow_dist + (dist - slow_dist) / 2;
    int c = dist;

    if (steps >= a && steps < b) {
        OC1RS = v2;
    }
    else if (steps >= b && steps < c) {
        OC1RS = v4;
    }
}



int main(void) {

    pins_config();
    OC_config();
    T1_config();
    T2_config();
    CN_config();
    ad_config();

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
                if (ADC1BUF4/4095.0 > 0.7 && ADC1BUF13/4095.0 > 0.7) {
                    if (steps < 75) {
                        steps = -30;
                    }
                    else {
                        steps = 0;
                    }
                    state = ROTATE;
                }
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
                _LATB12 = 0; // steppers
                _LATB13 = 0;
                OC2R = front*OC2RS; // point shooter forwards
                OC3R = closed*OC3RS; // close release
                _LATB7 = 0;  // shooting motors off
                _LATB4 = 1;  // buttons out
                if (steps < (dist+2000)) {
                    speedup();
                }
                else if (steps >= (dist+2000) && steps < 10000) {
                    _LATB4 = 0;
                    TMR2 = 0;
                    angle_pad = 0;
                    pad_count = 0;
                    state = COLLECT;
                }
                else if (steps >= 10000 && steps < (25000)) {
                    OC1RS = v3;
                }
                else if (steps >= (25000)) {
                    _LATB4 = 0;
                    TMR2 = 0;
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
                if (steps < slow_dist) {
                    speedup();
                    _LATB12 = 1;
                    _LATB13 = 1;
                    OC1R = 0.5*OC1RS;
                }
                else if (steps >= slow_dist) {
                    leftsum = 0;
                    frontsum = 0;
                    rightsum = 0;
                    state = AIM;
                }
                break;
            case AIM:
                // Rotate turret to face active goal
                slowdown();
                if (steps > slow_dist && steps < dist && has_aimed == 0) {
                    // AD: LLED == 9, FLED//FLED2 == 13/4, RLED == 14;
                    if (steps < slow_dist + avgtime) {
                        leftsum += ADC1BUF9/4095.0;
                        frontsum += ADC1BUF13/4095.0;
                        rightsum += ADC1BUF14/4095.0;
                    }
                    else if (steps >= slow_dist + avgtime) {
                        leftavg = leftsum / avgtime;
                        frontavg = frontsum / avgtime;
                        rightavg = (rightsum / avgtime) * r_mult;
                        if (leftavg > frontavg && leftavg > rightavg && leftavg > 0.5) {
                            OC2R = left*OC2RS;
                        }
                        else if (frontavg > rightavg && frontavg > leftavg && frontavg > 0.5) {
                            OC2R = front*OC2RS;
                        }
                        else if (rightavg > frontavg && rightavg > leftavg && rightavg > 0.5) {
                            OC2R = right*OC2RS;
                        }
                        else {
                            OC2R = front*OC2RS;
                        }
                        has_aimed = 1;
                    }
                }
                else if (steps >= dist) {
                    OC1R = 0;
                    TMR1 = 0;
                    steps = 0;
                    state = SHOOT;
                }
                break;
            case SHOOT:
                steps = 0;
                OC3R = open*OC3RS;  // open release
                break;
        }
    }
    return 0;
}
