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
                break;
            case ROTATE:
                break;
            case REVERSE:
                // T2CNT = 0;
                // OC1R = OC1RS*0.5;
                // OC2R = OC2RS * 0.078;
                break;
            case COLLECT:
                // OC1R = 0;
                break;
            case FORWARD:
                // OC1R = 0.5*OC1RS;
                break;
            case AIM:
                break;
            case SHOOT:
                // OC1R = 0;
                break;
        }
    }



    return 0;
}
