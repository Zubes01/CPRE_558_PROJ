/**
 * cliff.c
 *
 * Template file for CprE 288 Lab 6
 *
 * @author Diane Rover, 2/15/2020
 *
 */

#include <open_interface.h>
#include <stdint.h>
#include "cliff.h"

int checkCliff(oi_t *sensor_data)
{

    if (sensor_data->cliffLeft)
        return 1;
    else if (sensor_data->cliffRight)
        return 2;
    else if (sensor_data->cliffFrontLeft)
        return 3;
    else if (sensor_data->cliffFrontRight)
        return 4;
    else
        return 0;

}

int checkBoundary(oi_t *sensor_data)
{

    uint16_t data[4];

    data[0] = sensor_data->cliffFrontRightSignal;
    data[1] = sensor_data->cliffFrontLeftSignal;
    data[2] = sensor_data->cliffRightSignal;
    data[3] = sensor_data->cliffLeftSignal;

    int i;
    for (i = 0; i < 4; i++)
    {
        if (data[i] > 2600)
        {
            //white line detected

            return i;
        }
    }
    return 0;
}
