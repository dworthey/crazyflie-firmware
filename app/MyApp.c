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
	volatile int WATCHPOINTFLAG = 0;
	vTaskDelay(M2T(3000));
	ledInit();
	vTaskDelay(M2T(3000));
	motorsInit(&motorMapDefaultBrushed[NBR_OF_MOTORS]);
	vTaskDelay(M2T(MOTORS_TEST_ON_TIME_MS));
	while(1){
		ledSet(LED_GREEN_L, 1);
		ledSet(LED_GREEN_R, 1);
		ledSet(LED_RED_L, 0);
		ledSet(LED_RED_R, 0);
		vTaskDelay(M2T(250));
		ledSet(LED_GREEN_L, 0);
		ledSet(LED_GREEN_R, 0);
		ledSet(LED_RED_L, 1);
		ledSet(LED_RED_R, 1);
		vTaskDelay(M2T(250));
		
		ledClearAll();
		vTaskDelay(M2T(3000));	
		WATCHPOINTFLAG += 1;	
	}


	while(1);
}



