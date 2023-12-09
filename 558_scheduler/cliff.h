#ifndef CLIFF_H_
#define CLIFF_H_

// Initialize cyBot Scan sensors and servo
int checkCliff(oi_t *sensor_data);

// Rotate sensors to specified angle, and get a scan value 
//(Note: It is OK that this is not calabrated to 0 - 180 degrees)
// (You will calibrate your version in Lab 8)
int checkBoundary(oi_t *sensor_data);


#endif /* CYBOT_SCAN_H_ */
