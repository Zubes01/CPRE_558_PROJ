/**
 * lab6_template.c
 *
 * Template file for CprE 288 Lab 6
 *
 * @author Diane Rover, 2/15/2020
 *
 */

#include "Timer.h"
#include "lcd.h"
#include "uart.h"

// Uncomment or add any include directives that are needed
#include "open_interface.h"
#include "movement.h"
//#include "button.h"
#include "resetSimulation.h"
#include "scan.h"
#include "cliff.h"
#include "adc.h"

//int main(void) {
//    resetSimulationBoard();
//	timer_init(); // Must be called before lcd_init(), which uses timer functions
//	lcd_init();
//	uart_init();
//	ping_init();
//	servo_init();
//	ADC0_InitSWTriggerSeq3_Ch10();
//
//	oi_t *sensor_data = oi_alloc();
//	oi_init(sensor_data);
//	// YOUR CODE HERE
//
//	uart_sendStr_A("W = Move forward 30cm\n\rA = Turn left 30 degrees\n\rS = Move backwards 15cm\n\rD = Turn right 30 degrees\n\rE = Initiate scan\n\r");
//
//	while(1)
//	{
//
//      // YOUR CODE HERE
//	  char data;
//	  data = uart_receive();
//	  int Distance;
//	  char s[15];
//	  uart_sendStr_A("\r");
//	  sprintf(s, "Input: %c\n\r", data);
//	  uart_sendStr_A(s);
//	  switch (data)
//	  {
//	  case 'w':
//	      //move
//	      Distance = moveForward(sensor_data, (double)300);
//	      if (sensor_data->bumpLeft || sensor_data->bumpRight)
//	      {
//	          if (sensor_data->bumpLeft)
//	          {
//	              //hit left
//	              uart_sendStr_A("Hit something on the left\n\r");//, moving to the right\n\r");
//	              backup(sensor_data, (double) 150);
//	              Distance-=150;
//	              /*
//	              turnRight(sensor_data, (double) 90);
//	              moveForward(sensor_data, (double) 150);
//	              turnLeft(sensor_data, (double) 90);
//	              moveForward(sensor_data, (double) 150);
//	              */
//	          }
//	          else
//	          {
//	              //hit right
//	              uart_sendStr_A("Hit something on the right\n\r");//, moving to the left\n\r");
//	              backup(sensor_data, (double) 150);
//	              Distance -= 150;
//	              /*
//	              turnLeft(sensor_data, (double) 90);
//	              moveForward(sensor_data, (double) 150);
//	              turnRight(sensor_data, (double) 90);
//	              moveForward(sensor_data, (double) 150);
//	              */
//	          }
//	      }
//	      if (checkCliff(sensor_data))
//	      {
//	          char str[80];
//	          sprintf(str, "Cliff detected %d cm into movement, moving back 15cm!\n\r", (Distance/10));
//	          Distance -= 150;
//	          uart_sendStr_A(str);
//	          backup(sensor_data, (double)150);
//	      }
//	      int bCheck = checkBoundary(sensor_data);
//	      if (bCheck)
//	      {
//	          if ((bCheck == 0 || bCheck == 1) || ((sensor_data->cliffRightSignal > 2600) && (sensor_data->cliffLeftSignal > 2600)))
//	          {
//	              uart_sendStr_A("Boundary detected to the front!\n\r");
//	          }
//	          else
//	          {
//	              if (bCheck==2)
//	              {
//	                  uart_sendStr_A("Boundary detected to the right!\n\r");
//	              }
//	              else
//	              {
//	                  uart_sendStr_A("Boundary detected to the left!\n\r");
//	              }
//	          }
//	          char str[80];
//	          sprintf(str, "Boundary detected %d cm into movement, moving back 10cm!\n\r", (Distance/10));
//	          Distance -= 100;
//	          uart_sendStr_A(str);
//	          backup(sensor_data, (double)100);
//	      }
//	      char s[30];
//	      sprintf(s, "Moved forward %icm\n\r", Distance/10);
//	      uart_sendStr_A(s);
//	      break;
//	  case 's':
//	      //backwards
//	      backup(sensor_data, (double)150);
//	      uart_sendStr_A("Backed up 15cm!\n\r");
//	      break;
//	  case 'a':
//	      //turn left 30 degrees
//	      turnLeft(sensor_data, (double)30);
//	      uart_sendStr_A("Turned left 30 degrees!\n\r");
//	      break;
//	  case 'd':
//	      //turn right 30 degrees
//	      turnRight(sensor_data, (double)30);
//	      uart_sendStr_A("Turned right 30 degrees!\n\r");
//	      break;
//	  case 'e':
//	      //scan for objects and output to putty
//	      uart_sendStr_A("Object\tAngle\tDistance\tWidth\n\r\0");
//
//	      scan();
//	      break;
//	  }
///*
//	  int bCheck = checkBoundary(sensor_data);
//	  if (bCheck)
//	  {
//	      if (bCheck%2==0)
//	      {
//	          //boundary in front of you
//	          uart_sendStr_A("front\n\r");
//	      }
//	      else
//	      {
//	          if (bCheck==1)
//	          {
//	              //right
//	              uart_sendStr_A("right\n\r");
//	          }
//	          else
//	          {
//	              //left
//	              uart_sendStr_A("left\n\r");
//	          }
//	      }
//	  }
//
//    */
//
//	  timer_waitMillis(100);
//
//	}
//
//}
//
