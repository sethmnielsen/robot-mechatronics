/*
 * File:   Robot Registry Configuration
 * Authors: Aaron Bame, Carson Zaugg, Seth Nielsen, Derek Sanchez
 * Comments:
 * Revision history: 10/22-Started
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
int time3 = 0;

//Configs
void OC_config(void);       //Configure PWM for driving motors
void T1_config (void);      //Competition round
void T2_config (void);      //Counting 6 balls
void T3_config (void);      //Paddle servo back and forth
void T4_config (void);      //Turret servo moving to shoting position
void T5_config (void);      //Check for has aimed
void CN_config (void);      //Change Notification Interrupt
void comp_config (void);    //Comparator Interrupt - Infra sensors
void pins_config (void);

//Interrupt Actions
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt (void);       //Timer1 interrupt
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void);        //Timer2 interrupt
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt (void);       //Change notification for LEDs, buttons
void __attribute__((interrupt, no_auto_psv)) _OC1Interrupt(void);       //Interrupt for stepper period counter






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
    // OC1R = 0;

    //Turret PWM configuration (Pin 4)
    OC2CON1bits.OCTSEL = 0b100;     //Set OC to Timer 1 (100)
    OC2CON2bits.SYNCSEL = 0x1F;     //Source Selection: This output compare module
    OC2CON2bits.OCTRIG = 0;         //Synchronizes OCx with source designated by the SYNCSELx bits
    OC2CON1bits.OCM = 0b110;        //Edge aligned PWM mode

    OC2RS = 313;
    OC2R = OC2RS * 0.075;
    angle_tur = 90;
    // 0.03 = 0 deg
    // 0.075 = 90 deg
    // 0.13  = 180 deg

    //Paddle servo PWM configuration (Pin 5)
    OC3CON1bits.OCTSEL = 0b100;     //Set OC to Timer 1 (100)
    OC3CON2bits.SYNCSEL = 0x1F;     //Source Selection: This output compare module
    OC3CON2bits.OCTRIG = 0;         //Synchronizes OCx with source designated by the SYNCSELx bits
    OC3CON1bits.OCM = 0b110;        //Edge aligned PWM mode

    OC3RS = 313;
    OC3R = 0.06*OC3RS;
    angle_pad = 90;
    // 0.06 = 90 deg
    // 0.11 = 180 deg
}

void T1_config (void) {
    T1CONbits.TON = 1;
    T1CONbits.TCS = 0;
    T1CONbits.TCKPS = 0b11;   // prescale 1:256

    PR1 = 15625;              //Set Period for 1 s
    TMR1 = 0;                 //Start at t=0
    _T1IP = 2;                //Int pri (7 is highest)
    _T1IE = 1;                //Enable the timer
    _T1IF = 0;                //Clear the flag
}

void T2_config (void) {
    T2CONbits.TON = 1;
    T2CONbits.TCS = 0;
    T2CONbits.T32 = 0;
    T2CONbits.TCKPS = 0b11;   // prescale 1:256

    PR2 = 14999;    //Set period for shoting 6 balls
    // TMR2 = T2CNT;   //Start at t=0
    TMR2 = 0;
    _T2IP = 5;      //Int Pri
    _T2IE = 1;      //Enable the timer
    _T2IF = 0;      //Clear the flag
}

void T3_config (void) {
    T3CONbits.TON = 1;
    T3CONbits.TCS = 0;
    T3CONbits.TCKPS = 0b11;

    //Configure Timer1 interrupt
    TMR3 = 0;
    _T3IP = 7; //Int Pri
    _T3IE = 1; //Int Pri enabled
    _T3IF = 0; //clear Flag

    PR3 = 10000;
}

void T4_config (void) {
    T4CONbits.TON = 1;
    T4CONbits.TCS = 0;
    T4CONbits.T32 = 0;
    T4CONbits.TCKPS = 0b11;

    //Configure Timer1 interrupt
    TMR4 = 0;
    _T4IP = 3; //int pri
    _T4IE = 1; //int pri enabled
    _T4IF = 0; //clear Flag

    PR4 = 7812;
}

void T5_config (void) {
    T5CONbits.TON = 1;
    T5CONbits.TCS = 0;
    T5CONbits.TCKPS = 0b11;


    //Configure Timer1 interrupt
    TMR5 = 0;
    _T5IP = 4; //Int Pri
    _T5IE = 1; //Int Pri enabled
    _T5IF = 0; //clear Flag

    PR5 = 1001;
}

void CN_config (void) {
    //CNEN1 register
    _CN0IE = 1;  // Enable CN on pin 10 (LLED)

    _CNIP = 1;   // Set CN interrupt priority (IPC4 register)
    _CNIF = 0;   // Clear interrupt flag (IFS1 register)
    _CNIE = 1;   // Enable CN interrupts (IEC1 register)
}

void comp_config (void) {
    //configure voltage reference
    _CVROE = 0;     // Voltage reference output is internal only
    _CVRSS = 0;     // Vdd and Vss as reference voltages
    _CVR = 0x14;    // set Vref at 20/32*(Vdd-Vss) = 2.06 V (0x14 == 20)
    _CVREN = 1;     // enable the module

    //configure comparator
    //FLED
    CM1CONbits.CON = 1;         //enable module for configuration
    CM1CONbits.COE = 0;         //comparator output is internal only
    CM1CONbits.CPOL = 1;        //comparator output is inverted (output high when Vin+ < Vin-)
    CM1CONbits.EVPOL = 0b10;    //interrupt on low-high transition
    CM1CONbits.CREF = 1;        //Vin+ connected to internal Vref
    CM1CONbits.CCH = 0b00;      //Vin- connected to C1INB (pin 7)

    //RLED
    CM2CONbits.CON = 1;         //enable module for configuration
    CM2CONbits.COE = 0;         //comparator output is internal only
    CM2CONbits.CPOL = 1;        //comparator output is inverted (output high when Vin+ < Vin-)
    CM2CONbits.EVPOL = 0b10;    //interrupt on low-high transition
    CM2CONbits.CREF = 1;        //Vin+ connected to internal Vref
    CM2CONbits.CCH = 0b01;      //Vin- connected to C2INC (pin 8)

    //LLED
    CM3CONbits.CON = 1;         //enable module for configuration
    CM3CONbits.COE = 0;         //comparator output is internal only
    CM3CONbits.CPOL = 1;        //comparator output is inverted (output high when Vin+ < Vin-)
    CM3CONbits.EVPOL = 0b10;    //interrupt on low-high transition
    CM3CONbits.CREF = 1;        //Vin+ connected to internal Vref
    CM3CONbits.CCH = 0b01;      //Vin- connected to C3INC (pin 7)

    //configure interrupt
    CM1CONbits.CEVT = 0;
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

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt (void) {
    _T1IF = 0;        //Reset timer

    if (T1CNT >= 120) {
        state = OFF;
    }
    T1CNT += 1;
}

void __attribute__((interrupt, no_auto_psv)) _T2Interrupt (void) {
    _T2IF = 0;            // Reset timer
    if (state == SHOOT) {
        T2CNT = 0;
        // turn DC motors off
        steps = 0;
        state = REVERSE;
    }
}

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt (void) {
    _T3IF = 0;        //Reset timer
    _LATB7 = 1;
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

void __attribute__((interrupt, no_auto_psv)) _T4Interrupt (void) {
    _T4IF = 0;        //Reset timer

    if (state == AIM) {
        has_aimed = 1;
    }
    PR4 = 7812;
}

void __attribute__((interrupt, no_auto_psv)) _T5Interrupt (void) {
    _T5IF = 0;        //Reset timer

    if (state == AIM && has_aimed == 1) {
        has_aimed = 0;
        TMR2 = 0;
        state = SHOOT;
    }
}

void __attribute__((interrupt, no_auto_psv)) _CNInterrupt (void) {
    _CNIF = 0;

    // buttons pressed
    // if (_RA1 == 1) {
    //     state = COLLECT;
    // }
}

void __attribute__((interrupt, no_auto_psv)) _CompInterrupt(void) {
    _CMIF = 0; // clear interrupt flag

    TMR4 = 0;

    //if LED changes while shoting
    if (state == SHOOT) {
        // T2CNT = TMR2;
        state = AIM;
    }
    else if (state == FORWARD) {
        state = AIM;
    }

    //FLED
    if (CM1CONbits.CEVT == 1 && (state == FORWARD || state == AIM || state == SHOOT || state == START)) {
        OC2R = 0.075 * OC2RS;
        angle_tur = 90;
        if (state == START) {
            steps = 0;
            state = ROTATE;
            return;
        }
    }
    //RLED
    else if (CM2CONbits.CEVT == 1 && (state == FORWARD || state == AIM || state == SHOOT)) {
        OC2R = 0.12 * OC2RS;
        angle_tur = 180;

        // for long turns
        if (angle_tur == 0) {
            PR4 = 14000;
        }
    }
    //LLED
    else if (CM3CONbits.CEVT == 1 && (state == FORWARD || state == AIM || state == SHOOT)) {
        OC2R = 0.03 * OC2RS;
        angle_tur = 0;

        // for long turns
        if (angle_tur == 180 ) {
            PR4 = 14000;
        }
    }

    CM1CONbits.CEVT = 0;
    CM2CONbits.CEVT = 0;
    CM3CONbits.CEVT = 0;
}

void __attribute__((interrupt, no_auto_psv)) _OC1Interrupt(void) {
    _OC1IF = 0;       //Clear interrupt flag

    steps += 1;
}

#endif
