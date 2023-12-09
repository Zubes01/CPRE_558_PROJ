/*
 * servo.c
 *
 *  Created on: Apr 7, 2021
 *      Author: jgortiz
 */
#include "servo.h"
#include "Timer.h"
#include "lcd.h"
#include "driverlib/interrupt.h"
//comment out ping_template or whatever
void servo_init(void){

    // TODO configure gpio and alt function
    SYSCTL_RCGCGPIO_R |= 0x02;
    timer_waitMillis(500);
    GPIO_PORTB_PCTL_R |= 0x700000;// PB5
    GPIO_PORTB_AFSEL_R |= 0b100000;
    timer_waitMillis(500);
    GPIO_PORTB_DEN_R |= 0b100000;
    GPIO_PORTB_DIR_R |= 0b100000;

    SYSCTL_RCGCTIMER_R |= 0b10;

    TIMER1_CTL_R &= ~0b100000000;//Ensure the timer is disabled (the TnEN bit is cleared) before making any changes.
    TIMER1_CFG_R |= 0x4;//. Write the GPTM Configuration (GPTMCFG) register with a value of 0x0000.0004.
    TIMER1_TBMR_R |= 0b1010;//In the GPTM Timer Mode (GPTMTnMR) register, set the TnAMS bit to 0x1, the TnCMR bit 0x0, and the TnMR field to 0x2.
    //Configure the output state of the PWM signal (whether or not it is inverted) in the TnPWML field of the GPTM Control (GPTMCTL) register.
    TIMER1_CTL_R &= ~0x400;
    TIMER1_TBILR_R = (0x4E200 & 0xFFFF);
    TIMER1_TBPR_R = (0x4E200 >> 16);//If a prescaler is to be used, write the prescale value to the GPTM Timer n Prescale Register (GPTMTnPR
    //If PWM interrupts are used, configure the interrupt condition in the TnEVENT field in the
    //    GPTMCTL register and enable the interrupts by setting the TnPWMIE bit in the GPTMTnMR
    //    register. Note that edge detect interrupt behavior is reversed when the PWM output is inverted
    //    (see page 737).
    TIMER1_TBMATCHR_R = (0x4BED8 & 0xFFFF);//what is match timer value
    TIMER1_TBPMR_R = (0x4BED8 >> 16);
    TIMER1_CTL_R |= 0x100;

}
void servo_move(float degrees){
    int match = 320000 - 9000 - ((22000 * degrees) / 180);// 22000(inconsistency value) 320,000(clock) 9000(20 miliseconds)
    TIMER1_TBMATCHR_R = (match & 0xFFFF);//what is match timer value
    TIMER1_TBPMR_R = (match >> 16);


}


