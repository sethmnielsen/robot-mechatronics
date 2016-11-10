/*
 * File:   Robot Registry Configuration
 * Authors: Aaron Bame, Carson Zaugg, Seth Nielsen, Derek Sanchez
 * Comments:
 * Revision history: 10/22-Started
 * 11/4-state variables created, A/D configuration, OC config, FORWARD state - AB/SN
 *
 * To-do list:
 * AIM state
 * Servo config
 */


#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H
#include <xc.h>
#pragma config FNOSC=LPRC //31kHz Oscillator

// Global Variables
enum state {OFF, START, ROTATE, REVERSE, COLLECT, FORWARD, AIM, SHOOT};
int T2CNT = 0;              // save count of TMR2 for returning to SHOOT
int steps = 0;
int rev = 400;              // steps for 1 revolution of wheels
int turn180 = 326/0.9;      // steps for turning robot 180 deg

void ad_config (void);      //LED sensors
void OC_config(void);       //Configure PWM for driving motors

//Configure Change Notifications
void T1_config (void);              //Competition round
void T2_config (void);              //Counting 6 balls
void buttons_config (void);         //Counting 6 balls

//Interrupt Actions
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt (void);        //Timer1 interrupt
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void);         //Timer2 interrupt
void __attribute__((interrupt, no_auto_psv)) switch_change (void);       //Sensor Change Notification
void __attribute__((interrupt, no_auto_psv)) buttons (void);             //R Button Change Notification
void __attribute__((interrupt, no_auto_psv)) RLED (void);                //R LED Change Notification
void __attribute__((interrupt, no_auto_psv)) FLED (void);                //Forward LED Change Notification
void __attribute__((interrupt, no_auto_psv)) LLED (void);                //L LED Change Notification
void __attribute__((interrupt, no_auto_psv)) _OC1Interrupt(void);        //Interrupt for stepper period counter



/********************************************** FUNCTION DECLARATIONS ************************************************/
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
    _CSS14 = 1;     //Pin 8 (Photodiode)

    _ADON = 1;    // AD1CON1<15> -- Turn on A/D
}

void OC_cofig(void) {
    //Stepper PWM configuration(Pin 14)
    OC1CON1bits.OCTSEL = 0b111;     //Compare to system clock
    OC1CON2bits.SYNCSEL = 0x1F;     //Compares to output compare module
    OC1CON2bits.OCTRIG = 0;         //Synchronizes to specified SYNCSEL value
    OC1CON1bits.OCM = 0b110;        //Edge aligned

    OC1RS = 300;                    //Period
    OC1R = 0;                       //Duty Cycle (0 = hold stationary)

    //Ball Collection servo PWM configuration (Pin 5)
    OC3CON1bits.OCTSEL = 0b111;     //Compare to system clock
    OC3CON2bits.SYNCSEL = 0x1F;     //Compares to output compare module
    OC3CON2bits.OCTRIG = 0;         //Synchronizes to specified SYNCSEL value
    OC3CON1bits.OCM = 0b110;        //Edge aligned

    OC3RS = 300;                    //Period
    OC3R = 0.5*OC2RS;               //Duty Cycle
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

    PR2 = 5;                  //Set period for shooting 6 balls
    TMR2 = T2CNT;             //Start at t=0
    _T2IP = 1;                //Highest Priority?
    _T2IE = 1;                //Enable the timer
    _T2IF = 0;                //Clear the flag
}

void switch_config (void) {
    _CN5IE = 1;   //Does this enable change notification on pin 5 (CN5)???????????
    _CN5PUE = 0;  //?????
    _CNIP = 6;    //Priority
    _CNIF = 0;    //Clear Notification Flag
    _CNIE = 1;    //Enable interrupt
}

void buttons_config (void) {
    // asdf
}


void __attribute__((interrupt, no_auto_psv)) _T1Interrupt (void) {
    _T1IF = 0;        //Reset timer
    _TRISA = 0;
    _TRISB = 0;
}
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt (void) {
    _T2IF = 0;        // Reset timer
    T2CNT = TMR2;     // Save value of timer2
    _LATB15 = 0;      // Pin 18 low, turn shooter motors off
}
void __attribute__((interrupt, no_auto_psv)) switch_change (void) {
    // asdf
}
void __attribute__((interrupt, no_auto_psv)) buttons (void) {
void __attribute__((interrupt, no_auto_psv)) RLED (void) {
    // set duty for 180 deg position
}
void __attribute__((interrupt, no_auto_psv)) FLED (void) {
    // set duty for 90 deg position
}
void __attribute__((interrupt, no_auto_psv)) LLED (void) {
    // set duty for 0 deg position
}
void __attribute__((interrupt, no_auto_psv)) _OC1Interrupt(void) {
    _OC1IF = 0;       //Clear interrupt flag

    steps += 1;

    switch (state) {
        case START:
            //Keep rotating until finds beam
            break;
        case ROTATE:
            //Drive until Button Interrupt
            break;
        case REVERSE:
            break;
        case FORWARD:
            if (steps < 1.872*rev)
            {
                _LATB13 = 1;
                _LATB12 = 1;
            }
             else
            {
                state = AIM;
                steps = 0;
            }

            break;
        default:
            steps = 0;
            break;
    }
}
