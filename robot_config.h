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
float rev = 400;              // steps for 1 revolution of wheels
int turn180 = 326/0.9;      // steps for turning robot 180 deg

//Configs
void ad_config (void);      //LED sensors
void OC_config(void);       //Configure PWM for driving motors
void T1_config (void);      //Competition round
void T2_config (void);      //Counting 6 balls
void CN_config (void);
void pins_config (void);

//Interrupt Actions
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt (void);       //Timer1 interrupt
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void);        //Timer2 interrupt
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt (void);       //Change notification for LEDs, buttons
void __attribute__((interrupt, no_auto_psv)) _OC1Interrupt(void);       //Interrupt for stepper period counter
void __attribute__((interrupt, no_auto_psv)) _OC3Interrupt(void);       //Interrupt for ball collector servo






/********************************************** CONFIGURATIONS ************************************************/


void ad_config () {
    _ADON = 0;    // AD1CON1<15> -- Turn off A/D during config

    // AD1CON1 register
    _ADSIDL = 0;  // AD1CON1<13> -- A/D continues in idle mode
    _MODE12 = 1;  // AD1CON1<10> -- 12-bit A/D operation
    _FORM = 0;    // AD1CON1<9:8> -- Unsigned integer output
    _SSRC = 7;    // AD1CON1<7:4> -- Auto conversion (internal counter)
    _ASAM = 1;    // AD1CON1<2> -- Auto sampling

    // AD1CON2 register
    _PVCFG = 0;   // AD1CON2<15:14> -- Use VDD as positive ref voltage
    _NVCFG = 0;   // AD1CON2<13> -- Use VSS as negative ref voltage
    _BUFREGEN = 1;// AD1CON2<11> -- Result appears in buffer location corresponding to channel
    _CSCNA = 1;   // AD1CON2<10> -- Does not scan inputs(0) Scans inputs (1)
                  // specified in AD1CSSx registers (instead uses channels specified by CH0SA bits in AD1CHS register)
                  //-- Selecting '0' here probably makes writing to the AD1CSSL register unnecessary.
    _SMPI = 2;    // AD1CON2<6:2> -- Each conversion sent to buffer (# of scanning pins - 1)
    _ALTS = 0;    // AD1CON2<0> -- Sample MUXA only

    // AD1CON3 register
    _ADRC = 0;    // AD1CON3<15> -- Use system clock
    _SAMC = 1;    // AD1CON3<12:8> -- Auto sample every A/D period TAD
    _ADCS = 0x3F; // AD1CON3<7:0> -- A/D period TAD = 64*TCY

    //AD1CSS registers
    _CSS4 = 1;      //Pin 6 (Photodiode)
    _CSS13 = 1;     //Pin 7 (Photodiode)
    _CSS15 = 1;     //Pin 9 (Photodiode)

    _ADON = 1;    // AD1CON1<15> -- Turn on A/D
}

void OC_config(void) {
    //Stepper PWM configuration (Pin 14)
    _OC1IP = 4; //OC1 Int Pri = 4
    _OC1IE = 1; //OC1 Int Pri enabled
    _OC1IF = 0; //OC1 clear Int Flag
    OC1CON1bits.OCTSEL = 0b111;     //Compare to system clock
    OC1CON2bits.SYNCSEL = 0x1F;     //Compares to output compare module
    OC1CON2bits.OCTRIG = 0;         //Synchronizes to specified SYNCSEL value
    OC1CON1bits.OCM = 0b110;        //Edge aligned

    OC1RS = 90;                     //Period
    OC1R = 0.5*OC1RS;               //Duty Cycle

    //Turret PWM configuration (Pin 4)
    OC2CON1bits.OCTSEL = 0b111;     //Compare to system clock
    OC2CON2bits.SYNCSEL = 0x1F;     //Compares to output compare module
    OC2CON2bits.OCTRIG = 0;         //Synchronizes to specified SYNCSEL value
    OC2CON1bits.OCM = 0b110;        //Edge aligned

    OC2RS = 309;
    OC2R = OC2RS * 0.078;
    // 0.035 = 0 deg
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

    OC3RS = 300;
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

    PR2 = 5000;                  //Set period for shooting 6 balls
    TMR2 = T2CNT;             //Start at t=0
    _T2IP = 1;                //Highest Priority?
    _T2IE = 1;                //Enable the timer
    _T2IF = 0;                //Clear the flag
}

void CN_config (void) {
    //CNEN1 register
    _CN6IE = 1;  // Enable CN on pin 6 (LLED)
    _CN30IE = 1;  // Enable CN on pin 7 (FLED)
    _CN1IE = 1;  // Enable CN on pin 9 (RLED)


    _CNIP = 6;   // Set CN interrupt priority (IPC4 register)
    _CNIF = 0;   // Clear interrupt flag (IFS1 register)
    _CNIE = 1;   // Enable CN interrupts (IEC1 register)
}

void pins_config (void) {
    //outputs
    _TRISA0 = 0;
    _TRISA1 = 0;
    _TRISB7 = 0;
    _TRISB12 = 0;
    _TRISB13 = 0;
    _TRISB14 = 0;

    //inputs
    // _TRISA1 = 1;
    _TRISB2 = 1;
    _TRISA2 = 1;
    _TRISB4 = 1;

    //analog
    _ANSB2 = 1;
    _ANSA2 = 1;
    _ANSB4 = 1;

    // stepper _SLEEP
    _LATB14 = 1; // this consumes power
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
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt (void) {
    _CNIF = 0;

    //RLED
    if ((ADC1BUF15/4095.0) >= 0.75 && (state == AIM || state == SHOOT)) {
        // 0 deg
        OC2R = 0.035 * OC2RS;
        _LATA0 = 1;
    }
    //FLED
    else if ((ADC1BUF13/4095.0) >= 0.75 && (state == AIM || state == SHOOT || state == ROTATE)) {
        // 90 deg
        OC2R = 0.078 * OC2RS;
        _LATA0 = 1;
    }
    //LLED
    else if ((ADC1BUF4/4095.0) >= 0.75 && (state == AIM || state == SHOOT)) {
        // 180 deg
        OC2R = 0.13 * OC2RS;
        _LATA0 = 1;
    }
    // buttons pressed
    else if (_RA1 == 1) {
        state = COLLECT;
    }
}
void __attribute__((interrupt, no_auto_psv)) _OC1Interrupt(void) {
    _OC1IF = 0;       //Clear interrupt flag

    steps += 1;

    switch (state) {
        case START:
            //Keep rotating until finds beam
            break;
        case ROTATE:
            //rotate 180 deg
            break;
        case REVERSE:
            //Drive until Button Interrupt
            break;
        case FORWARD:
        // 1.872*rev
            if (steps < 815) {
                _LATB12 = 1;
                _LATB13 = 1;
            }
            else {
                // If LED is on when arriving, turn to that LED
                if ((ADC1BUF15/4095.0) >= 0.75) { // R
                    // 0 deg
                    _LATA0 = 1;
                    OC2R = 0.035 * OC2RS;
                }
                else if ((ADC1BUF13/4095.0) >= 0.75) { // F
                    // 90 deg
                    OC2R = 0.078 * OC2RS;
                    _LATA0 = 1;
                }
                else if ((ADC1BUF4/4095.0) >= 0.75) { // L
                    // 180 deg
                    OC2R = 0.13 * OC2RS;
                    _LATA0 = 1;
                }
                state = AIM;
            }
            break;
       default:
           steps = 0;
           break;
   }
}
void __attribute__((interrupt, no_auto_psv)) _OC3Interrupt(void) {
    _OC3IF = 0;       //Clear interrupt flag

}

#endif
