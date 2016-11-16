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

#pragma config FNOSC=LPRC //31kHz Oscillator

// Global Variables
enum action {OFF, START, ROTATE, REVERSE, COLLECT, FORWARD, AIM, SHOOT} state;

int T2CNT = 0;              // save count of TMR2 for returning to SHOOT
int steps = 0;
int angle = 0;

//Configs
// void ad_config (void);      //LED sensors
void OC_config(void);       //Configure PWM for driving motors
void T1_config (void);      //Competition round
void T2_config (void);      //Counting 6 balls
void CN_config (void);      //Change Notification Interrupt
void comp_config (void);    //Comparator Interrupt - Infra sensors
void pins_config (void);

//Interrupt Actions
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt (void);       //Timer1 interrupt
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void);        //Timer2 interrupt
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt (void);       //Change notification for LEDs, buttons
void __attribute__((interrupt, no_auto_psv)) _OC1Interrupt(void);       //Interrupt for stepper period counter
void __attribute__((interrupt, no_auto_psv)) _OC3Interrupt(void);       //Interrupt for ball collector servo






/********************************************** CONFIGURATIONS ************************************************/

void OC_config(void) {
    //Stepper PWM configuration (Pin 14)
    _OC1IP = 4; //OC1 Int Pri = 4
    _OC1IE = 1; //OC1 Int Pri enabled
    _OC1IF = 0; //OC1 clear Int Flag
    OC1CON1bits.OCTSEL = 0b111;     //Compare to system clock
    OC1CON2bits.SYNCSEL = 0x1F;     //Compares to output compare module
    OC1CON2bits.OCTRIG = 0;         //Synchronizes to specified SYNCSEL value
    OC1CON1bits.OCM = 0b110;        //Edge aligned

    OC1RS = 70;                     //Period
    OC1R = OC1RS*0.5;               //Duty Cycle

    //Turret PWM configuration (Pin 4)
    OC2CON1bits.OCTSEL = 0b111;     //Compare to system clock
    OC2CON2bits.SYNCSEL = 0x1F;     //Compares to output compare module
    OC2CON2bits.OCTRIG = 0;         //Synchronizes to specified SYNCSEL value
    OC2CON1bits.OCM = 0b110;        //Edge aligned

    OC2RS = 309;
    OC2R = OC2RS * 0.078;
    // 0.03 = 0 deg
    // 0.078 = 90 deg
    // 0.13  = 180 deg

    //Ball Collection servo PWM configuration (Pin 5)
    _OC3IP = 4; //OC1 Int Pri = 4
    _OC3IE = 1; //OC1 Int Pri enabled
    _OC3IF = 0; //OC1 clear Int Flag
    OC3CON1bits.OCTSEL = 0b111;     //Compare to system clock
    OC3CON2bits.SYNCSEL = 0x1F;     //Compares to output compare module
    OC3CON2bits.OCTRIG = 0;         //Synchronizes to specified SYNCSEL value
    OC3CON1bits.OCM = 0b110;        //Edge aligned

    OC3RS = 309;
    OC3R = 0;
}

void T1_config (void) {
    T1CONbits.TON = 0;
    T1CONbits.TCS = 0;
    T1CONbits.TCKPS = 0b11;   // prescale 1:256

    PR1 = 7266;               //Set Period for 120 s
    TMR1 = 0;                 //Start at t=0
    _T1IP = 1;                //Highest Priority?
    _T1IE = 1;                //Enable the timer
    _T1IF = 0;                //Clear the flag
}

void T2_config (void) {
    T2CONbits.TON = 0;
    T2CONbits.TCS = 0;
    T2CONbits.TCKPS = 0b11;   // prescale 1:256

    PR2 = 5000;               //Set period for shooting 6 balls
    TMR2 = T2CNT;             //Start at t=0
    _T2IP = 1;                //Highest Priority?
    _T2IE = 1;                //Enable the timer
    _T2IF = 0;                //Clear the flag
}

void T3_config (void) {
    T3CONbits.TON = 1;
    T3CONbits.TCS = 0;
    T3CONbits.TCKPS = 0b11;

    //Configure Timer1 interrupt
    TMR3 = 0;
    _T3IP = 4; //T1 Int Pri = 4
    _T3IE = 1; //T1 Int Pri enabled
    _T3IF = 0; //T1 clear Int Flag

    PR3 = 30;
}

void T4_config (void) {
    T4CONbits.TON = 1;
    T4CONbits.TCS = 0;
    T4CONbits.TCKPS = 0b11;

    //Configure Timer1 interrupt
    TMR4 = 0;
    _T4IP = 2; //T1 Int Pri = 4
    _T4IE = 1; //T1 Int Pri enabled
    _T4IF = 0; //T1 clear Int Flag

    PR4 = 180;
}

void CN_config (void) {
    //CNEN1 register
    _CN0IE = 1;  // Enable CN on pin 10 (LLED)

    _CNIP = 6;   // Set CN interrupt priority (IPC4 register)
    _CNIF = 0;   // Clear interrupt flag (IFS1 register)
    _CNIE = 1;   // Enable CN interrupts (IEC1 register)
}

void comp_config (void) {
    //configure voltage reference
    _CVROE = 0;     // Voltage reference output is internal only
    _CVRSS = 0;     // Vdd and Vss as reference voltages
    _CVR = 0x10;    // set Vref at 16/32*(Vdd-Vss) = 1.65 V (0x10 == 16)
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
    // state = OFF;
}

void __attribute__((interrupt, no_auto_psv)) _T2Interrupt (void) {
    _T2IF = 0;          // Reset timer
    // T2CNT = TMR2;    // Save value of timer2
    // _LATB7 = 0;      // Pin 18 low, turn shooter motors off
    // state = REVERSE;
}

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt (void) {
    _T3IF = 0;        //Reset timer

    if (state == COLLECT && angle == 45) {
        OC3R = 0.104*OC3RS;
        angle = 135;
    }
    else if (state == COLLECT && angle == 135) {
        OC3R = 0.056*OC3RS;
        angle = 45;
    }
    else if (state != COLLECT) {
        OC3R = 0;
    }
}

void __attribute__((interrupt, no_auto_psv)) _T4Interrupt (void) {
    _T4IF = 0;        //Reset timer

    if (state == COLLECT) {
        steps = 0;
        state = FORWARD;
    }
}

void __attribute__((interrupt, no_auto_psv)) _CNInterrupt (void) {
    _CNIF = 0;

    // buttons pressed
    if (_RA1 == 1) {
        state = COLLECT;
    }
}

void __attribute__((interrupt, no_auto_psv)) _CompInterrupt(void) {
    _CMIF = 0; // clear interrupt flag

    //FLED
    if (CM1CONbits.CEVT == 1 && (state == FORWARD || state == AIM || state == SHOOT || state == START)) {
        // 90 deg
        OC2R = 0.078 * OC2RS;
        if (state == START) {
            steps = 0;
            state = ROTATE;
        }
    }
    //RLED
    else if (CM2CONbits.CEVT == 1 && (state == FORWARD || state == AIM || state == SHOOT)) {
        // 0 deg
        OC2R = 0.03 * OC2RS;
    }
    //LLED
    else if (CM3CONbits.CEVT == 1 && (state == FORWARD || state == AIM || state == SHOOT)) {
        // 180 deg
        OC2R = 0.13 * OC2RS;
    }

    CM1CONbits.CEVT = 0;
    CM2CONbits.CEVT = 0;
    CM3CONbits.CEVT = 0;
}

void __attribute__((interrupt, no_auto_psv)) _OC1Interrupt(void) {
    _OC1IF = 0;       //Clear interrupt flag

    steps += 1;

    switch (state) {
        case START:
            //Keep rotating until finds beam
            _LATB12 = 1;
            _LATB13 = 0;
            break;
        case ROTATE:
            //rotate 180 deg
            if (steps < 300) {
                _LATB12 = 1;
                _LATB13 = 0;
            }
            else {
                steps = 0;
                state = REVERSE;
            }
            break;
        case REVERSE:
            //Drive until Button Interrupt
            _LATB12 = 0;
            _LATB13 = 0;

            _LATB4 = 1; // buttons_out is high

            // CHANGE IFs TO BE IF _RA4 IS LOW OR HIGH
            if (steps < 300) {
                _LATB12 = 1;
                _LATB13 = 0;
            }
            else {
                steps = 0;
                _LATB4 = 0;
                TMR3 = 0;            // left-right paddling
                TMR4 = 0;            // count time to collect 6 balls
                OC3R = 0.056*OC3RS;  // start paddling servo at 45 deg
                angle = 45;
                state = COLLECT;
            }
            break;
        case FORWARD:
            if (steps < 805) {
                _LATB12 = 1;
                _LATB13 = 1;
            }
            else {
                // If LED is on when arriving, turn to that LED
                if (_C2OUT == 1) { // R
                    // 0 deg
                    OC2R = 0.03 * OC2RS;
                }
                else if (_C1OUT == 1) { // F
                    // 90 deg
                    OC2R = 0.078 * OC2RS;
                }
                else if (_C3OUT == 1) { // L
                    // 180 deg
                    OC2R = 0.13 * OC2RS;
                }
                steps = 0;
                state = AIM;
            }
            break;
       default:
           steps = 0;
           break;
   }
}

#endif
