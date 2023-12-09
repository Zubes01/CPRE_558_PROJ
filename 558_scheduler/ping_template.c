/**
 * Driver for ping sensor
 * @file ping.c
 * @author
 */

#include "ping_template.h"
#include "Timer.h"
#include "lcd.h"
#include "driverlib/interrupt.h"

volatile unsigned long START_TIME = 0;
volatile unsigned long END_TIME = 0;
volatile enum{LOW, HIGH, DONE} STATE = LOW; // State of ping echo pulse

void ping_init (void){

  // YOUR CODE HERE RCGC Needs to be called
    SYSCTL_RCGCGPIO_R |= 0x02;
    timer_waitMillis(500);
    GPIO_PORTB_PCTL_R |= 0x7000;
    GPIO_PORTB_AFSEL_R |= 0b1000;
    timer_waitMillis(500);
    GPIO_PORTB_DEN_R |= 0b1000;
    GPIO_PORTB_DIR_R |= 0b1000;

    SYSCTL_RCGCTIMER_R |= 0b1000;
    timer_waitMillis(500);
    TIMER3_CTL_R &= ~0b100000000;
    TIMER3_CTL_R |= 0b110000000000;
//    while((SYSCTL_PRTIMER_R & 0x0003) == 0){};
//    while((SYSCTL_PRGPIO_R & 0x0002) == 0){};
    timer_waitMillis(500);
//    TIMER3_CTL_R &= ~0b100000000;
    TIMER3_CFG_R = 0b100; //0x4 might need bitwise and beneath
    //TIMER3_CFG_R &= ~0b11;
    TIMER3_TBMR_R |= 0b111;
    TIMER3_TBMR_R &= ~0x10;
    TIMER3_TBILR_R |= 0xFFFF;
    TIMER3_TBPR_R |= 0xFF;
    TIMER3_ICR_R |= 0x400;
    TIMER3_IMR_R |= 0x400;
    NVIC_EN1_R |= 0b10000;
    NVIC_PRI9_R |= 0xE0;

    IntRegister(INT_TIMER3B, TIMER3B_Handler);
    TIMER3_CTL_R |= 0x100; //8 0's  Enables Timer3 on both edges
    IntMasterEnable();

    // Configure and enable the timer 14.4.4 Tiva Sheet
}

void ping_trigger (void){
    STATE = LOW;
    // Disable timer and disable timer interrupt
    TIMER3_CTL_R &= ~0x100;
    TIMER3_IMR_R &= ~0x400;
    // Disable alternate function (disconnect timer from port pin)
    GPIO_PORTB_AFSEL_R &= ~0b1000;//SUS

    // YOUR CODE HERE FOR PING TRIGGER/START PULSE (problem???)
    GPIO_PORTB_DATA_R &= ~0b1000;//puts pb3 to 0
    timer_waitMicros(2);
    GPIO_PORTB_DATA_R |= 0b1000;
    timer_waitMicros(5);
    GPIO_PORTB_DATA_R &= ~0b1000;//SHOULD IT BE DIR OR DATA

    // Clear an interrupt that may have been erroneously triggered
    TIMER3_ICR_R |= 0x400; //which interrupt
    // Re-enable alternate function, timer interrupt, and timer
    GPIO_PORTB_AFSEL_R |= 0b1000;
    TIMER3_IMR_R |= 0x400;
    TIMER3_CTL_R |= 0x100;
}

void TIMER3B_Handler(void){

  // YOUR CODE HERE

        if(STATE == LOW){
            START_TIME = TIMER3_TBV_R;//catch RIsing edge(store TIMER3_TBR_R in a var) HOW TO STORE
//            GPIO_PORTB_DATA_R &= ~0b1000;//Change PB3 to input (should be called when output first goes low) DIR OR DATA?
//            TIMER3_TBMR_R |= 0x3;//Change to input capture mode
            STATE = HIGH;
        }
        else if(STATE == HIGH){
            END_TIME = TIMER3_TBV_R;//CATCH FALLING EDGE
                STATE = DONE;
    }
        TIMER3_ICR_R |= 0x400;//clear timer interrupt(TIMER3_ICR_R)
    }


float ping_getDistance (void){
    ping_trigger();
    while(STATE!= DONE){//idk what this does

    }
    unsigned int TOTAL_TIME = START_TIME - END_TIME;
    return ((float)((TOTAL_TIME/2.0 * (34000.0 / 16000000.0))));;
//    if(TOTAL_TIME < 0){
//        //overflow has occured (no need to fix)
//        lcd_printf("Overflow Detected");
//    }
//    else{
//        //Im going to assume the value of TIMER3_TBR IS IN CLOCK CYCLES
//        lcd_printf("%d clock cycles", TOTAL_TIME);
//    }
    // YOUR CODE HERE
    //Call ping trigger and have while loop
    //Powerpoint in canvas

}
