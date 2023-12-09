/*
 * scan.c
 *
 *  Created on: Apr 20, 2021
 *      Author: jgortiz
 */

#include "scan.h"
#include "servo.h"
#include <math.h>
#include "ping_template.h"
#include "uart.h"
#include "adc.h"


//enum status {IDLE , OBJECT, SCANNING};
//enum status STATUS = IDLE;
#define sinf(x) (float)sin((double)(x)); //Makes sure sinf works

//TODO make it send info to putty
void scan(){
    enum status {IDLE , OBJECT, SCANNING};
    enum status STATUS = IDLE;
    float ping_distance = 0;//s
    int angle_init = 0;//s
    int angle_final = 0;//s
    int objAngle = 0;//s
    float objWidth = 0.0;//s
    float objDistance = 0.0; //s
    int objCount = 0;//s
    float dist = 0;//s
    int count = 0;//num of cycles the object is detected
    int irD = 0;
    int i = 0;
    for(i = 0; i <= 180; i += 2){
        servo_move(i);//i or 2?
        //maybe add a function to stop
        ping_distance = ping_getDistance();//dont forget the include statements
        irD = irDis();
        char b[60];
        //sprintf(b, "IR: %d\n\r", irD);
        //uart_sendStr_A("\r");
        //uart_sendStr_A(b);
        if (STATUS == IDLE && (irD < 100) && (ping_distance < 100)){//object found for the first time
            STATUS = OBJECT;
            angle_init = i;
            objCount++;
            count++;
            dist += ping_distance;
        }
        else if(STATUS == OBJECT && (irD < 100) && (ping_distance < 100)){// same object still detected
            STATUS = SCANNING;
            dist += ping_distance;
            count++;
        }
        else if (STATUS == SCANNING && ((irD > 100) || (ping_distance > 100))){//object no longer detected
            STATUS = IDLE;
            angle_final = i;
            objAngle = angle_final - angle_init - 1;
            int degreeOfObject = ((angle_final + angle_init)/2) - 2;
            objDistance = dist / count;
            objDistance += 10;
            objWidth = widthCalculator(objAngle, objDistance);
            sendToPutty(objCount, degreeOfObject, objDistance, objWidth);
            count = 0;
            dist = 0;
        }
    }
}

float widthCalculator(int angle, float distance){
    float angleInRads = ((float)angle * (float)M_PI)/180.0;

     float width = distance * cosf(angleInRads);

     if (width < 0)
         width *= -1;

     return width;
}

void sendToPutty(int obj, int angle, float distance, float width) {

    uart_sendStr_A("\r");

    float dis = distance - 10;

    if (angle < 1)
        angle = 0;
    if (width <= 1)
        width = 1.0;

    char string[60];
    sprintf(string, "%i\t%i\t%0.0f\t\t%0.2f", obj, angle, dis, width);
    uart_sendStr_A(string);
    uart_sendChar_A('\n');
    uart_sendChar_A('\r');
}

int irDis(void)
{
    double sum = 0;
    int i = 0;
    int dist = 0;
    for(i = 0; i < 16; i++)
    {
        sum += ADC0_InSeq3();//sampling
    }
    sum = sum / 16;
    return (int)(dist = (int)(107689 * pow(sum,-1.156)));
}
