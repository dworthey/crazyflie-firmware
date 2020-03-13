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
	volatile int flaggy = 0;
	vTaskDelay(M2T(3000));
	ledInit();
	vTaskDelay(M2T(3000));
	motorsInit(&motorMapDefaultBrushed[NBR_OF_MOTORS]);
	vTaskDelay(M2T(MOTORS_TEST_ON_TIME_MS));
	while(1){
		ledTest();
		vTaskDelay(M2T(3000));	
		flaggy = (flaggy^7)<<2;
	}


	while(1);
}



