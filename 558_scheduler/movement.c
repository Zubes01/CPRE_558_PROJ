/*
 * movement.c
 *
 *  Created on: Feb 12, 2021
 *      Author: rowan
 */

#include "open_interface.h"
#include "math.h"

int moveForward(oi_t *sensor_data, double distance_mm);
void turnRight(oi_t *sensor_data, double degrees);

int moveForward(oi_t *sensor_data, double distance_mm)
{
    double sum = 0;
    oi_setWheels(350,350);
    while (sum < distance_mm)
    {
       oi_setWheels(350,350);
       oi_update(sensor_data);
       sum += sensor_data -> distance;
       if (sensor_data->bumpLeft || sensor_data->bumpRight)
       {
           break;
       }

       if (checkCliff(sensor_data))
       {
           break;
       }

       if (checkBoundary(sensor_data))
       {
           break;
       }
    }
    oi_setWheels(0,0);
    if (sum > distance_mm)
        sum = distance_mm;
    return (int) sum;
}

double ABS(double number)
{
    if (number <= 0)
        return -1 * number;
    else
        return number;
}

void turnLeft(oi_t *sensor_data, double degrees)
{
    double sum = 0;
    oi_setWheels(-30,30);
    while (sum < degrees)
    {
       oi_update(sensor_data);
       sum += ABS(sensor_data -> angle);
    }
    oi_setWheels(0,0);

}

void backup(oi_t *sensor_data, double distance_mm)
{
    double sum = 0;
    oi_setWheels(-350,-350);
    while (sum < 150)
    {
       oi_update(sensor_data);
       sum += ABS(sensor_data -> distance);
    }
    oi_setWheels(0,0);
}

void turnRight(oi_t *sensor_data, double degrees)
{
    double sum = 0;
    oi_setWheels(30, -30);
    while (sum < degrees)
    {
       oi_update(sensor_data);
       sum += ABS(sensor_data -> angle);
    }
    oi_setWheels(0,0);

}



