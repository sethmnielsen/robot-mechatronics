// File: robot_config.h
// Authors:  Seth Nielsen, Aaron Bame, Derek Sanchez, Carson Zaugg
// Created on November 4, 2016, 3:10 PM

#ifndef ROBOT_CONFIG_H
#define	ROBOT_CONFIG_H
#include "xc.h"
#include <stdio.h>

// Pins 8,9 and 10 are available
#pragma config OSCIOFNC = OFF
#pragma config SOSCSRC = DIG
#pragma config ICS = PGx2
#pragma config FWDTEN = OFF


#pragma config FNOSC = FRC // 8 MHz Oscillator

// Global Variables
enum action {OFF, START, ROTATE, REVERSE, COLLECT, FORWARD, AIM, SHOOT} state;

int steps = 0;
int angle_pad = 0;   // angle of 'paddle' servo (ball collection)
int pad_count = 0;   // swipes of paddle servo
int has_aimed = 0;   // turret in aiming position

    // Turret
    // DUTY btw 0.112 and 0.015
    // LOWER OR HIGHER WILL SCRATE EFF UP
    const float left = 0.1097;
    const float front = 0.0637;
    const float right = 0.0192;
    // Averaging signal
    float leftsum = 0;
    float leftavg = 0;
    float frontsum = 0;
    float frontavg = 0;
    float rightsum = 0;
    float rightavg = 0;

    // Paddle/Release
    const float closed = 0.0255;
    const float open = 0.06;


//Configs
void pins_config (void);    // Inputs/Outputs
void T1_config (void);      // Time for shooting 6 balls
void T2_config (void);      // Paddle servo back and forth
void CN_config (void);      // Change Notification Interrupt - buttons
void ad_config (void);
void OC_config(void);       // Configure PWM for steppers and servos

//Interrupt Actions
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void);
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt (void);
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt (void);
void __attribute__((interrupt, no_auto_psv)) _OC1Interrupt(void);       // stepper counter





/********************************************** CONFIGURATIONS ************************************************/

void pins_config (void) {
    //outputs
    _TRISB4 = 0;    // buttons out
    _TRISB7 = 0;    // DC motors switch
    _TRISB8 = 0;    // Relsease servo switch
    _TRISB12 = 0;   // Steppers D2
    _TRISB13 = 0;   // Steppers D1
    _TRISB14 = 0;   // Steppers _SLEEP

    //inputs
    _TRISA4 = 1;    // buttons in
    _TRISA3 = 1;
    _TRISB2 = 1;
    _TRISB15 = 1;
    _TRISA2 = 1;

    // AD input
    _ANSB2 = 1;     // FLED2
    _ANSA2 = 1;     // FLED
    _ANSB15 = 1;    // LLED
    _ANSA3 = 1;     // RLED

    _LATB14 = 1;    // _SLEEP
    _LATB8 = 1;     // connect release
}

void T1_config (void) {
    T1CONbits.TON = 1;
    T1CONbits.TCS = 0;
    T1CONbits.TCKPS = 0b11;
    TMR1 = 0;
    _T1IP = 7;
    _T1IE = 1;
    _T1IF = 0;
    PR1 = 29999; // 1.912 sec
}

void T2_config (void) {
    T2CONbits.TON = 1;
    T2CONbits.TCS = 0;
    T2CONbits.T32 = 0;
    T2CONbits.TCKPS = 0b11;
    TMR2 = 0;
    _T2IP = 4;
    _T2IE = 1;
    _T2IF = 0;
    PR2 = 8000;
}

void CN_config (void) {
    _CN0IE = 1;  // Enable CN on pin 10 (LLED)

    _CN5PUE = 0;
    _CNIP = 5;
    _CNIF = 0;
    _CNIE = 1;
}

void ad_config (void) {
    _ADON = 0;

    // AD1CON1 register
    _ADSIDL = 0;
    _MODE12 = 1;
    _FORM = 0;
    _SSRC = 7;
    _ASAM = 1;

    // AD1CON2 register
    _PVCFG = 0;
    _NVCFG = 0;
    _BUFREGEN = 1;
    _CSCNA = 1;
    _SMPI = 3;
    _ALTS = 0;

    // AD1CON3 register
    _ADRC = 0;
    _SAMC = 1;
    _ADCS = 0x3F;

    //AD1CSS registers
    _CSS4 = 1;    //Pin 6 (Photodiode)
    _CSS9 = 1;
    _CSS13 = 1;
    _CSS14 = 1;

    _ADON = 1;    //Turn on A/D
}

void OC_config(void) {
    //Stepper PWM configuration (Pin 14)
    _OC1IP = 6; //OC1 Int Priority
    _OC1IE = 1; //OC1 Int Pri enabled
    _OC1IF = 0; //OC1 clear Int Flag
    OC1CON1bits.OCTSEL = 0b111;     //Set OC (output compare) to system clock (111)
    OC1CON2bits.SYNCSEL = 0x1F;     //Source Selection: This output compare module
    OC1CON2bits.OCTRIG = 0;         //Synchronizes OCx with source designated by the SYNCSELx bits
    OC1CON1bits.OCM = 0b110;        //Edge aligned PWM mode

    OC1RS = 9000;                  //Period
    OC1R = 0.5*OC1RS;              //Duty Cycle

    //Turret (Pin 4)
    OC2CON1bits.OCTSEL = 0b100;     //Set OC to Timer 1 (100)
    OC2CON2bits.SYNCSEL = 0x1F;
    OC2CON2bits.OCTRIG = 0;
    OC2CON1bits.OCM = 0b110;

    OC2RS = 313;
    // angle_tur = 90;
    OC2R = front*OC2RS;

    //Paddle + Release servos PWM (Pin 5)
    OC3CON1bits.OCTSEL = 0b100;     //Set OC to Timer 1 (100)
    OC3CON2bits.SYNCSEL = 0x1F;
    OC3CON2bits.OCTRIG = 0;
    OC3CON1bits.OCM = 0b110;

    OC3RS = 313;
    OC3R = closed*OC3RS;
    angle_pad = 0;
}








/********************************************** INTERRUPTS ************************************************/

//Time for shooting 6 balls
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt (void) {
    _T1IF = 0;
    if (state == SHOOT) {
        steps = 0;
        has_aimed = 0;
        TMR1 = 0;
        state = REVERSE;
    }
}

//Paddle servo back and forth
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt (void) {
    _T2IF = 0;
    if (state == COLLECT) {
        if (pad_count >= 6) {
            OC3R = closed*OC3RS;
            angle_pad = 0;
            steps = 0;
            pad_count = 0;
            state = FORWARD;
        }
        else if (angle_pad == 45) {
            OC3R = closed*OC3RS;
            angle_pad = 0;
            pad_count += 1;
        }
        else if (angle_pad == 0) {
            OC3R = open*OC3RS;
            angle_pad = 45;
            pad_count += 1;
        }
    }
}

// buttons pressed
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt (void) {
    _CNIF = 0;
    if (_RA4 == 1 && state == REVERSE) {
        _LATB4 = 0;
        TMR2 = 0;
        angle_pad = 0;
        pad_count = 0;
        state = COLLECT;
    }
}

// steppers counter
void __attribute__((interrupt, no_auto_psv)) _OC1Interrupt(void) {
    _OC1IF = 0;
    steps += 1;
}


#endif
