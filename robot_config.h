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
#pragma config FWDTEN = OFF


#pragma config FNOSC = FRC // 8 MHz Oscillator

// Global Variables
enum action {OFF, START, ROTATE, REVERSE, COLLECT, FORWARD, AIM, SHOOT} state;

int T1CNT = 0;       // count number of seconds from TMR1
int T2CNT = 0;       // save count of TMR2 for returning to shoot
int steps = 0;
int angle_pad = 0;   // angle of 'paddle' servo (ball collection)
int angle_tur = 0;   // angle of turret servo
int pad_count = 0;   // swipes of paddle servo
int has_aimed = 0;   // if aimed while in forward state, shoot right away
int stopped = 0;     // stopped at center X



//Configs
void pins_config (void);    //Inputs/Outputs
void T1_config (void);      //Competition round
void T2_config (void);      //Time for shooting 6 balls
void T3_config (void);      //Paddle servo back and forth
void T4_config (void);      //Turret servo moving to shooting position
void T5_config (void);      //Check for has aimed, is stopped; start shooting
void CN_config (void);      //Change Notification Interrupt - buttons
void ad_config (void);
void OC_config(void);       //Configure PWM for steppers and servos
void comp_config (void);    //Comparator Interrupt - IR sensors

//Interrupt Actions
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt (void);
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void);
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt (void);
void __attribute__((interrupt, no_auto_psv)) _T4Interrupt (void);
void __attribute__((interrupt, no_auto_psv)) _T5Interrupt (void);
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt (void);
void __attribute__((interrupt, no_auto_psv)) _OC1Interrupt(void);       // stepper counter

void __attribute__((interrupt, no_auto_psv)) _CompInterrupt (void);     // IR sensors






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
    _TRISA0 = 1;    // LLED
    _TRISA2 = 1;    // FLED
    _TRISA3 = 1;    // RLED
    _TRISA4 = 1;    // buttons in

    // AD input
    _ANSB2 = 1;     // FLED2

    _LATB14 = 1;    // _SLEEP
    _LATB8 = 1;     // connect release
}

void T1_config (void) {
    T1CONbits.TON = 1;
    T1CONbits.TCS = 0;
    T1CONbits.TCKPS = 0b11; // prescale 1:256
    TMR1 = 0;
    _T1IP = 7;              // Select interrupt priority (7 is highest)
    _T1IE = 1;              // Enable interrupt
    _T1IF = 0;              // Clear interrupt flag
    PR1 = 15000;            // Set period for interrupt to occur (1.0 sec)
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
    PR3 = 9000; // 0.576 sec
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
    PR5 = 1001; // 0.0254 sec (15.6 Hz)
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
    _SMPI = 2;
    _ALTS = 0;

    // AD1CON3 register
    _ADRC = 0;
    _SAMC = 1;
    _ADCS = 0x3F;

    //AD1CSS registers
    _CSS4 = 1;    //Pin 6 (Photodiode)
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
    OC1R = 0.9*OC1RS;             //Duty Cycle

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
    // 0.12  = 180 deg (right)

    //Paddle + Release servos PWM (Pin 5)
    OC3CON1bits.OCTSEL = 0b100;     //Set OC to Timer 1 (100)
    OC3CON2bits.SYNCSEL = 0x1F;
    OC3CON2bits.OCTRIG = 0;
    OC3CON1bits.OCM = 0b110;

    OC3RS = 313;
    OC3R = 0.025*OC3RS;
    angle_pad = 0;
    // 0.025 = 0 deg (paddle in, release closed)
    // 0.06 = 45 deg (paddle out, release opened)
}

void comp_config (void) {
    //configure voltage reference
    _CVROE = 0;     // Voltage reference output is internal only
    _CVRSS = 0;     // Vdd and Vss as reference voltages
    _CVR = 0x1F;    // set Vref at 32/32*(Vdd-Vss) = 3.3 V (0x2 == 32)
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







/********************************************** INTERRUPTS ************************************************/

//Competition round
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt (void) {
    _T1IF = 0;

    // 120 counts = 120 sec
    if (T1CNT >= 120) {
//        state = OFF;
    }
    T1CNT += 1;
}

//Time for shooting 6 balls
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt (void) {
    _T2IF = 0;
    if (state == SHOOT) {
        steps = 0;
        T2CNT = 0;
        OC2R = 0.075*OC2RS;
        state = REVERSE;
    }
}

//Paddle servo back and forth
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt (void) {
    _T3IF = 0;
    if (state == COLLECT) {
        if (pad_count >= 6) {
            OC3R = 0.025*OC3RS;
            angle_pad = 0;
            steps = 0;
            pad_count = 0;
            state = FORWARD;
        }
        else if (angle_pad == 45) {
            OC3R = 0.025*OC3RS;
            angle_pad = 0;
            pad_count += 1;
        }
        else if (angle_pad == 0) {
            OC3R = 0.06*OC3RS;
            angle_pad = 45;
            pad_count += 1;
        }
    }
}

//Turret servo moving to shooting position
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
        TMR2 = T2CNT;
        state = SHOOT;
    }
}

// buttons pressed
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt (void) {
    _CNIF = 0;
    if (_RA4 == 1 && state == REVERSE) {
        _LATB4 = 0;
        TMR3 = 0;
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

// IR sensors low to high
void __attribute__((interrupt, no_auto_psv)) _CompInterrupt(void) {
    _CMIF = 0;

    TMR4 = 0;

    if (state == START && CM1CONbits.CEVT == 1) {
        if (steps < 75) {
            steps = -30;
        }
        else {
            while ((ADC1BUF4/4095.0) < 0.65) {};
            steps = 0;
        }
        state = ROTATE;
        return;
    }

    //if LED changes while shooting
    if (state == SHOOT) {
        // if ((CM1CONbits.CEVT == 1 && angle_tur == 90)  ||
        //     (CM2CONbits.CEVT == 1 && angle_tur == 180) ||
        //     (CM3CONbits.CEVT == 1 && angle_tur == 0)    )
        //     { return; }
        T2CNT = TMR2;
        state = AIM;
    }
    else if (state == FORWARD) {
        state = AIM;
    }

    if (state == AIM) {
        //FLED
        if (CM1CONbits.CEVT == 1) {
            angle_tur = 90;
            OC2R = 0.075 * OC2RS;
        }
        //RLED
        else if (CM2CONbits.CEVT == 1) {
            if (angle_tur == 0) {
                PR4 = 14000;
            }
            angle_tur = 180;
            OC2R = 0.12 * OC2RS;
        }
        //LLED
        else if (CM3CONbits.CEVT == 1) {
            if (angle_tur == 180) {
                PR4 = 14000;
            }
            angle_tur = 0;
            OC2R = 0.03 * OC2RS;
        }
    }
    CM1CONbits.CEVT = 0; // clear event bits
    CM2CONbits.CEVT = 0;
    CM3CONbits.CEVT = 0;
}

#endif
