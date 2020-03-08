// My App for Crazyflie 2.1
//
//
#include "app.h"
#include "led.h"
#include "motors.h"
#include "FreeRTOS.h"
#include "task.h"
#include "debug.h"

void appMain(){
	
	while(1){
	ledTest();
	motorsTest();	
	for(int i=0;i<100000;i++);
	}


	while(1);
}
