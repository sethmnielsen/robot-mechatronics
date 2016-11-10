/*
 * File:   robot_main.c
 * Author: abame
 *
 * Created on November 4, 2016, 3:10 PM
 */

#include "robot_config.h"

int main(void) {
    
    ad_config();
    OC_config();
    
    //INCLUDE CONFIGURATION FUNCTIONS HERE!!!!!!!!!!
    state=FORWARD;
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
                break;
            case COLLECT:
                break;
            case FORWARD:
                OC1R = 0.5*OC1RS;
                break;
            case AIM:
                OC1R = 0;
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