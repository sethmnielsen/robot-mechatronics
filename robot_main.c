/*
 * File:   robot_main.c
 * Authors: Aaron Bame, Carson Zaugg, Seth Nielsen, Derek Sanchez
 *
 * Created on November 4, 2016, 3:10 PM
 */

#include "robot_config.h"

int main(void) {

    ad_config();
    OC_config();

    //INCLUDE CONFIGURATION FUNCTIONS HERE!!!!!!!!!!
    //configure all pins as inputs/outputs, digital
    state = FORWARD;
    while (1)
    {
        switch (state)
        {
            case OFF:
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
                OC1R = 0;
                OC2R = 0;

                break;
            case SHOOT:
                _LATB15 = 1;
                config_timer2();
                break;
            default:
                //????????????????
                break;
        }
    }


    return 0;
}
