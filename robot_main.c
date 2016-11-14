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

    ad_config();
    OC_config();
    T1_config();
    CN_config();
    pins_config();

    state = FORWARD;

    while (1)
    {
        switch (state)
        {
            case OFF:
                return 0;
                break;
            case START:
                break;
            case ROTATE:
                break;
            case REVERSE:
                OC1R = 0.5*OC1RS;
                break;
            case COLLECT:
                OC1R = 0;
                break;
            case FORWARD:
                OC1R = 0.5*OC1RS;
                break;
            case AIM:
                OC1R = 0; // steppers off
                break;
            case SHOOT:
                _LATB7 = 1;
                T2_config();
                break;
        }
    }



    return 0;
}
