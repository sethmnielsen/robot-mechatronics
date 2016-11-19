/*
 * File:   robot_main.c
 * Authors: Aaron Bame, Carson Zaugg, Seth Nielsen, Derek Sanchez
 *
 * Created on November 4, 2016, 3:10 PM
 */

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
                //Keep rotating until finds beam
                _LATB12 = 1;
                _LATB13 = 0;
                break;
            case ROTATE:
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
                // T2CNT = 0;
                //Drive until Button Interrupt
                // _LATB12 = 0;
                // _LATB13 = 0;

                // _LATB4 = 1; // buttons_out is high

                // CHANGE IFs TO BE IF _RA4 IS LOW OR HIGH
                if (steps < 900) {
                    _LATB12 = 0;
                    _LATB13 = 0;
                    OC1R = 0.5*OC1RS;
                }
                else if (steps >= 900) {
                    OC1R = 0;
                    _LATB4 = 0;
                    TMR3 = 0;           // left-right paddling
                    angle_pad = 90;
                    pad_count = 0;
                    state = COLLECT;
                }
                break;
            case COLLECT:
                break;
            case FORWARD:
            // 805
                if (steps < 805) {
                    _LATB12 = 1;
                    _LATB13 = 1;
                    OC1R = 0.5*OC1RS;
                }
                else if (steps >= 805) {
                    state = AIM;
                    TMR4 = 0;
                    OC1R = 0;
                    steps = 0;
                }
                break;
            case AIM:
                if (steps < 805) {
                    _LATB12 = 1;
                    _LATB13 = 1;
                }
                else if (steps >= 805) {
                    OC1R = 0;
                    steps = 0;
                }
                break;
            case SHOOT:
                // OC1R = 0;
                break;
        }
    }



    return 0;
}
