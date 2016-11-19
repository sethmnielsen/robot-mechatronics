/*
 * File:   Robot Registry Configuration
 * Authors: Aaron Bame, Carson Zaugg, Seth Nielsen, Derek Sanchez
 */


#ifndef ROBOT_CONFIG_H
#define	ROBOT_CONFIG_H
#include "xc.h"
#include <stdio.h>

// Pins 8,9 and 10 are available
#pragma config OSCIOFNC = OFF
#pragma config SOSCSRC = DIG

#pragma config FNOSC = FRC // 8 MHz Oscillator

// Global Variables
enum action {OFF, START, ROTATE, REVERSE, COLLECT, FORWARD, AIM, SHOOT} state;

int T1CNT = 0;       // count number of seconds from TMR1
int T2CNT = 0;       // save count of TMR2 for returning to shot
int steps = 0;
int angle_pad = 0;   // angle of 'paddle' servo (ball collection)
int angle_tur = 0;   // angle of turret servo
int pad_count = 0;   // swipes of paddle servo
int has_aimed = 0;   // if aimed while in forward state, shot right away
int stopped = 0;     // stopped at center X

//Configs
void OC_config(void);       //Configure PWM for steppers and servos
void T1_config (void);      //Competition round
void T2_config (void);      //Time for shooting 6 balls
void T3_config (void);      //Paddle servo back and forth
void T4_config (void);      //Turret servo moving to shoting position
void T5_config (void);      //Check for has aimed, is stopped; start shooting
void CN_config (void);      //Change Notification Interrupt - buttons
void comp_config (void);    //Comparator Interrupt - Infra sensors
void pins_config (void);    //Inputs/Outputs

//Interrupt Actions
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt (void);
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void);
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt (void);
void __attribute__((interrupt, no_auto_psv)) _T4Interrupt (void);
void __attribute__((interrupt, no_auto_psv)) _T5Interrupt (void);
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt (void);
void __attribute__((interrupt, no_auto_psv)) _OC1Interrupt(void);       //Interrupt for stepper counter






/********************************************** CONFIGURATIONS ************************************************/

void OC_config(void) {
    //Stepper PWM configuration (Pin 14)
    _OC1IP = 6; //OC1 Int Priority
    _OC1IE = 1; //OC1 Int Pri enabled
    _OC1IF = 0; //OC1 clear Int Flag
    OC1CON1bits.OCTSEL = 0b111;     //Set OC (output compare) to system clock (111)
    OC1CON2bits.SYNCSEL = 0x1F;     //Source Selection: This output compare module
    OC1CON2bits.OCTRIG = 0;         //Synchronizes OCx with source designated by the SYNCSELx bits
    OC1CON1bits.OCM = 0b110;        //Edge aligned PWM mode

    OC1RS = 18000;                  //Period
    OC1R = 0.5*OC1RS;               //Duty Cycle

    //Turret (Pin 4)
    OC2CON1bits.OCTSEL = 0b100;     //Set OC to Timer 1 (100)
    OC2CON2bits.SYNCSEL = 0x1F;
    OC2CON2bits.OCTRIG = 0;
    OC2CON1bits.OCM = 0b110;

    OC2RS = 313;
    OC2R = 0.075*OC2RS;
    angle_tur = 90;
    // 0.03 = 0 deg (left)
    // 0.075 = 90 deg (front)
    // 0.13  = 180 deg (right)

    //Paddle servo PWM (Pin 5)
    OC3CON1bits.OCTSEL = 0b100;     //Set OC to Timer 1 (100)
    OC3CON2bits.SYNCSEL = 0x1F;
    OC3CON2bits.OCTRIG = 0;
    OC3CON1bits.OCM = 0b110;

    OC3RS = 313;
    OC3R = 0.06*OC3RS;
    angle_pad = 90;
    // 0.06 = 90 deg
    // 0.11 = 180 deg
}

//
void T1_config (void) {
    T1CONbits.TON = 1;
    T1CONbits.TCS = 0;
    T1CONbits.TCKPS = 0b11; // prescale 1:256
    TMR1 = 0;
    _T1IP = 7;              // Select interrupt priority (7 is highest)
    _T1IE = 1;              // Enable interrupt
    _T1IF = 0;              // Clear interrupt flag
    PR1 = 15625;            // Set period for interrupt to occur (1.0 sec)
}

void T2_config (void) {
    T2CONbits.TON = 1;
    T2CONbits.TCS = 0;
    T2CONbits.T32 = 0;
    T2CONbits.TCKPS = 0b11;
    TMR2 = 0;
    _T2IP = 5;
    _T2IE = 1;
    _T2IF = 0;
    PR2 = 29999; // 1.912 sec
}

void T3_config (void) {
    T3CONbits.TON = 1;
    T3CONbits.TCS = 0;
    T3CONbits.TCKPS = 0b11;
    TMR3 = 0;
    _T3IP = 4;
    _T3IE = 1;
    _T3IF = 0;
    PR3 = 10000; // 0.64 sec
}

void T4_config (void) {
    T4CONbits.TON = 1;
    T4CONbits.TCS = 0;
    T4CONbits.T32 = 0;
    T4CONbits.TCKPS = 0b11;
    TMR4 = 0;
    _T4IP = 3;
    _T4IE = 1;
    _T4IF = 0;
    PR4 = 7812; // 0.5 sec
}

void T5_config (void) {
    T5CONbits.TON = 1;
    T5CONbits.TCS = 0;
    T5CONbits.TCKPS = 0b11;
    TMR5 = 0;
    _T5IP = 4;
    _T5IE = 1;
    _T5IF = 0;
    PR5 = 1001; // 0.064 sec (15.6 Hz)
}

void CN_config (void) {
    _CN0IE = 1;  // Enable CN on pin 10 (LLED)

    _CN5PUE = 0; // Disable pull-up resistor (CNPU1 register)
    _CNIP = 5;   // Set CN interrupt priority (IPC4 register)
    _CNIF = 0;   // Clear interrupt flag (IFS1 register)
    _CNIE = 1;   // Enable CN interrupts (IEC1 register)
}

void comp_config (void) {
    //configure voltage reference
    _CVROE = 0;     // Voltage reference output is internal only
    _CVRSS = 0;     // Vdd and Vss as reference voltages
    _CVR = 0x1B;    // set Vref at 8/32*(Vdd-Vss) = 2.8875 V (0x1B == 28)
    _CVREN = 1;     // enable the module

    //configure comparators
    //FLED
    CM1CONbits.CON = 1;         //enable module for configuration
    CM1CONbits.COE = 0;         //comparator output is internal only
    CM1CONbits.CPOL = 1;        //comparator output is inverted (output high when Vin+ < Vin-)
    CM1CONbits.EVPOL = 0b10;    //interrupt on low-high transition
    CM1CONbits.CREF = 1;        //Vin+ connected to internal Vref
    CM1CONbits.CCH = 0b00;      //Vin- connected to C1INB (pin 7)

    //RLED
    CM2CONbits.CON = 1;
    CM2CONbits.COE = 0;
    CM2CONbits.CPOL = 1;
    CM2CONbits.EVPOL = 0b10;
    CM2CONbits.CREF = 1;
    CM2CONbits.CCH = 0b01;      //Vin- connected to C2INC (pin 8)

    //LLED
    CM3CONbits.CON = 1;
    CM3CONbits.COE = 0;
    CM3CONbits.CPOL = 1;
    CM3CONbits.EVPOL = 0b10;
    CM3CONbits.CREF = 1;
    CM3CONbits.CCH = 0b01;      //Vin- connected to C3INC (pin 2)

    //configure interrupt
    CM1CONbits.CEVT = 0; // event bits
    CM2CONbits.CEVT = 0;
    CM3CONbits.CEVT = 0;
    _CMIF = 0;
    _CMIE = 1;
    _CMIP = 4;
}

void pins_config (void) {
    //outputs
    _TRISA1 = 0;
    _TRISB4 = 0;
    _TRISB7 = 0;
    _TRISB12 = 0;
    _TRISB13 = 0;
    _TRISB14 = 0;

    //inputs
    _TRISA0 = 1;
    _TRISA2 = 1;
    _TRISA3 = 1;
    _TRISA4 = 1;

    // stepper _SLEEP
    _LATB14 = 1;
}







/********************************************** INTERRUPTS ************************************************/

//Competition round
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt (void) {
    _T1IF = 0;

    // 120 counts = 120 sec
    if (T1CNT >= 120) {
        state = OFF;
    }
    T1CNT += 1;
}

//Time for shooting 6 balls
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt (void) {
    _T2IF = 0;
    if (state == SHOOT) {
        steps = 0;
        OC2R = 0.075*OC2RS;
        state = REVERSE;
    }
}

//Paddle servo back and forth
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt (void) {
    _T3IF = 0;
    if (state == COLLECT && pad_count >= 6) {
        OC3R = 0.06*OC3RS;
        angle_pad = 90;
        steps = 0;
        pad_count = 0;
        state = FORWARD;
    }
    else if (state == COLLECT && angle_pad == 180) {
        OC3R = 0.06*OC3RS;
        angle_pad = 90;
        pad_count += 1;
    }
    else if (state == COLLECT && angle_pad == 90) {
        OC3R = 0.11*OC3RS;
        angle_pad = 180;
        pad_count += 1;
    }
}

//Turret servo moving to shoting position
void __attribute__((interrupt, no_auto_psv)) _T4Interrupt (void) {
    _T4IF = 0;

    if (state == AIM) {
        has_aimed = 1;
    }
    PR4 = 7812;
}

//Check for has aimed, is stopped; start shooting
void __attribute__((interrupt, no_auto_psv)) _T5Interrupt (void) {
    _T5IF = 0;

    if (stopped == 1 && has_aimed == 1) {
        has_aimed = 0;
        TMR2 = 0;
        state = SHOOT;
    }
}

// buttons pressed
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt (void) {
    _CNIF = 0;
    if (_RA4 == 1 && state == REVERSE) {
        _LATB4 = 0;
        TMR3 = 0;
        angle_pad = 90;
        pad_count = 0;
        state = COLLECT;
    }
}

// IR sensors low to high
void __attribute__((interrupt, no_auto_psv)) _CompInterrupt(void) {
    _CMIF = 0;

    TMR4 = 0;
    //if LED changes while shoting
    if (state == SHOOT) {
        state = AIM;
    }
    else if (state == FORWARD) {
        state = AIM;
    }

    //FLED
    if (CM1CONbits.CEVT == 1 && (state == FORWARD || state == AIM || state == SHOOT || state == START)) {
        angle_tur = 90;
        OC2R = 0.075 * OC2RS;
        if (state == START) {
            steps = 0;
            state = ROTATE;
            return;
        }
    }
    //RLED
    else if (CM2CONbits.CEVT == 1 && (state == FORWARD || state == AIM || state == SHOOT)) {
        angle_tur = 180;
        OC2R = 0.12 * OC2RS;
        // for long turns
        if (angle_tur == 0) {
            PR4 = 14000;
        }
    }
    //LLED
    else if (CM3CONbits.CEVT == 1 && (state == FORWARD || state == AIM || state == SHOOT)) {
        angle_tur = 0;
        OC2R = 0.03 * OC2RS;
        // for long turns
        if (angle_tur == 180 ) {
            PR4 = 14000;
        }
    }

    CM1CONbits.CEVT = 0; // clear event bits
    CM2CONbits.CEVT = 0;
    CM3CONbits.CEVT = 0;
}

// steppers counter
void __attribute__((interrupt, no_auto_psv)) _OC1Interrupt(void) {
    _OC1IF = 0;

    steps += 1;
}

#endif
