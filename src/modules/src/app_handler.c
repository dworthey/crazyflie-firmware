/*
 *    ||          ____  _ __
 * +------+      / __ )(_) /_______________ _____  ___
 * | 0xBC |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * +------+    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *  ||  ||    /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 * LPS node firmware.
 *
 * Copyright 2019, Bitcraze AB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */
/* app_handler.c: App layer handling function implementation */

#include <stdbool.h>
#include <stdint.h>

#include "commander.h"
#include "motors.h"
#include "controller_mellinger.h"
#include "zranger2.h"
#include "pid.h"
#include "stabilizer.h"
#include "sensors.h"
#include "stabilizer_types.h"
#include "imu_types.h"
#include "deck.h"
#include "deck_core.h"
#include "led.h"
#include "log.h"
#include "FreeRTOS.h"
#include "task.h"

#include "system.h"
#include "static_mem.h"

#include "app.h"

#ifndef APP_STACKSIZE
#define APP_STACKSIZE 400
#endif

#ifndef APP_PRIORITY
#define APP_PRIORITY 0
#endif

static bool isInit = false;

STATIC_MEM_TASK_ALLOC(appTask, APP_STACKSIZE);

static void appTask(void *param);

const uint16_t maxRange = 300;
const float maxSpeed = 0.3f;
const uint16_t maxRange2 = 600;
const float height = 0.3f;

typedef struct logger{
	uint16_t logUp;
	uint16_t logDown;
	uint16_t logRight;
	uint16_t logLeft;
	uint16_t logFront;
	uint16_t logBack;
	uint16_t logYaw;
}logger;

bool TOOCLOSE(logger* x){

	static bool rg;
	rg=false;

	if(logGetUint(x->logUp) < maxRange || logGetUint(x->logBack) < maxRange || logGetUint(x->logFront) < maxRange || logGetUint(x->logRight) < maxRange || logGetUint(x->logLeft) < maxRange){
		rg = true;
	}

	return rg;
}

static void setpoint_Init(setpoint_t *setp, float vx, float vy,float z, float yaw ){

	setp->mode.z = modeAbs;
	setp->position.z = z;
	setp->mode.yaw = modeVelocity;
	setp->attitudeRate.yaw = yaw;
	setp->mode.x = modeVelocity;
	setp->mode.y = modeVelocity;
	setp->velocity.x = vx;
	setp->velocity.y = vy;
}
void moveAway(setpoint_t *set, logger* x){

	static float vx = 0;
	static float vy = 0;
	static uint8_t down= height;


	if(logGetUint(x->logUp) < 1.1*maxRange){
		setpoint_Init(set, 0, 0, 0, 0);
		commanderSetSetpoint(set, 3);
	}
	else if(logGetUint(x->logRight) < 1.1*maxRange){
		setpoint_Init(set, maxSpeed, 0, height, 0);
		commanderSetSetpoint(set,3);
	}
	else if(logGetUint(x->logLeft) < 1.1*maxRange){
		setpoint_Init(set, -1*maxSpeed, 0, height, 0);
		commanderSetSetpoint(set,3);
	}
	else if(logGetUint(x->logBack) < 1.1*maxRange){
		setpoint_Init(set, 0, maxSpeed, height, 0);
		commanderSetSetpoint(set,3);
	}
	else if(logGetUint(x->logFront) < maxRange){
		setpoint_Init(set, 0, -1*maxSpeed, height, 0);
		commanderSetSetpoint(set,3);
	}
}
void Spin(setpoint_t *set, logger* x){


	while(logGetUint(x->logYaw) <= 179.9 && logGetUint(x->logUp) > maxRange){
		setpoint_Init(set, 0, 0, height, 22.5);
		commanderSetSetpoint(set, 3);
	}
	while(logGetUint(x->logYaw) != 0 && logGetUint(x->logUp) > maxRange){
		setpoint_Init(set, 0, 0, height, -22.5);
		commanderSetSetpoint(set, 3);
	}
}
static void moveI(setpoint_t *set, logger* x){

	Spin(set, x);
	while(logGetUint(x->logFront) > maxRange2){
		if(TOOCLOSE(x)){
			moveAway(set, x);
		}
		else{
			setpoint_Init(set, maxSpeed, 0, height, 0);
			commanderSetSetpoint(set, 3);
		}
	}
	while(logGetUint(x->logLeft) > maxRange2){
		if(TOOCLOSE(x)){
			moveAway(set, x);
		}
		else{
			setpoint_Init(set, 0, maxSpeed,height, 0);
			commanderSetSetpoint(set, 3);
		}
	}
	Spin(set, x);
	while(logGetUint(x->logBack) > maxRange2){
		if(TOOCLOSE(x)){
			moveAway(set, x);
		}
		else{
			setpoint_Init(set, -1*maxSpeed, 0, 0.3, 0);
			commanderSetSetpoint(set, 3);
		}
	}
	Spin(set, x);
	while(logGetUint(x->logRight) > maxRange2){
		if(TOOCLOSE(x)){
			moveAway(set, x);
		}
		else{
			setpoint_Init(set, 0, -1*maxSpeed, 0.3, 0);
			commanderSetSetpoint(set, 3);
		}
	}
	Spin(set, x);
	setpoint_Init(set, 0, 0, 0, 0);
	commanderSetSetpoint(set, 3);
}

void appMain(){

	static logger logs;

	logs.logUp = logGetVarId("range", "up");
	logs.logBack = logGetVarId("range", "back");
	logs.logFront = logGetVarId("range", "front");
	logs.logRight = logGetVarId("range", "right");
	logs.logLeft = logGetVarId("range", "left");
	logs.logYaw = logGetVarId("stabilizer", "yaw");


	static setpoint_t setp;
	vTaskDelay(M2T(3000));
	for(int i = 0; i < 100; i++){
		setpoint_Init(&setp, 0, 0, 0.3, 0);
		commanderSetSetpoint(&setp, 3);
	}

	vTaskDelay(M2T(5));
	//while(1) continue;
	moveI(&setp, &logs);


	/*
	setpoint_Init(&setp, 0, 0, 0.3, 0);
	commanderSetSetpoint(&setp, 3);
	vTaskDelay(M2T(2000));
	moveI(&setp);

	setpoint_Init(&setp, 0, 0, 0, 0);
	commanderSetSetpoint(&setp, 3);
	*/

	while(1) continue;

	
}


void __attribute__((weak)) appInit()
{
  if (isInit) {
    return;
  }

  STATIC_MEM_TASK_CREATE(appTask, appTask, "app", NULL, APP_PRIORITY);
  isInit = true;
}

static void appTask(void *param)
{
  systemWaitStart();

  appMain();

  while(1) {
    vTaskDelay(portMAX_DELAY);
  }
}
