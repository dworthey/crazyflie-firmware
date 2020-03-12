

// My App for Crazyflie 2.1
//
//
#include "app.h"
#include "led.h"
#include "motors.h"
#include "FreeRTOS.h"
#include "task.h"
#include "debug.h"

void appMain()
{
	vTaskDelay(M2T(3000));
	ledInit();
	vTaskDelay(M2T(3000));
	motorsInit(&motorMapDefaultBrushed[NBR_OF_MOTORS]);
	vTaskDelay(M2T(MOTORS_TEST_ON_TIME_MS));
	motorsTest();
	motorsTest();
	while(1){
	ledTest();
	motorsTest();
	for(int i=0;i<100000;i++);
	}


	while(1);
}



