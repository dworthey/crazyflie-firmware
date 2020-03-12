

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
<<<<<<< HEAD
	vTaskDelay(M2T(3000));
	ledInit();
	vTaskDelay(M2T(3000));
	motorsInit(&motorMapDefaultBrushed[NBR_OF_MOTORS]);
	vTaskDelay(M2T(MOTORS_TEST_ON_TIME_MS));//FUNCTION OF FREERTOS
	for(int i=0; i<1000000; i++);
=======
	motorsInit(&motorMapDefaultBrushed[NBR_OF_MOTORS]);
	vTaskDelay(M2T(MOTORS_TEST_ON_TIME_MS));

>>>>>>> 1d93c510163fa76f79ee25bf77a80c8e755af8e9
	motorsTest();
	motorsTest();
	while(1){
	ledTest();
	motorsTest();
	for(int i=0;i<100000;i++);
	}


	while(1);
}



