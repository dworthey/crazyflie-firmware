// My App for Crazyflie 2.1
//
//
#include "app.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "debug.h"
#include "motors.h"

volatile int flag = 1;

void appMain()
{
	vTaskDelay(M2T(3000)); //delay 3000ms
	ledInit();
	vTaskDelay(M2T(3000)); //delay 3000ms
	motorsInit(&motorMapDefaultBrushed[NBR_OF_MOTORS]);
	vTaskDelay(M2T(MOTORS_TEST_ON_TIME_MS));


	blinkLeds(); //Infinite Loop


	while(1) continue; //never return from appMain
}
void blinkLeds()
{
	while(flag){
		DEBUG_PRINT("Blink LEDs\n");
		ledTest();
		vTaskDelay(M2T(1000)); //delay 1000ms	
	}
}



