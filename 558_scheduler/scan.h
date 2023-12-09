/*
 * scan.h
 *
 *  Created on: Apr 20, 2021
 *      Author: jgortiz
 */

#ifndef SCAN_H_
#define SCAN_H_

#include "scan.h"
#include <stdio.h>
#include "servo.h"
#include <math.h>
#include "ping_template.h"
#include <string.h>
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"
#define M_PI 3.14159265358979323846

void scan();

float widthCalculator(int angle, float distance);

void sendToPutty(int obj, int angle, float distance, float width);

int irDis(void);

#endif /* SCAN_H_ */
