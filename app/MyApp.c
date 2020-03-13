// My App for Crazyflie 2.1
//
//
#include "app.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "debug.h"
#include "motors.h"

void appMain()
{
	volatile int flaggy = 1;
	vTaskDelay(M2T(3000));
	ledInit();
	vTaskDelay(M2T(3000));
	motorsInit(&motorMapDefaultBrushed[NBR_OF_MOTORS]);
	vTaskDelay(M2T(MOTORS_TEST_ON_TIME_MS));
	while(1){
		ledTest();
		vTaskDelay(M2T(3000));	
		//while(flaggy != 0) flaggy *= 2;
	}


	while(1);
}



