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
#include "log.h"
#include "FreeRTOS.h"
#include "task.h"

#include "system.h"
#include "static_mem.h"

#include "app.h"

#ifndef APP_STACKSIZE
#define APP_STACKSIZE 300
#endif

#ifndef APP_PRIORITY
#define APP_PRIORITY 0
#endif

static bool isInit = false;

STATIC_MEM_TASK_ALLOC(appTask, APP_STACKSIZE);

static void appTask(void *param);

//void control_Init(control_t* con){
//
//	int roll0 = logGetVarId("stabilizer", "roll");
//	int pitch0 = logGetVarId("stabilizer", "pitch");
//	int yaw0 = logGetVarId("stabilizer", "yaw");
//
//	con->roll = logGetFloat(roll0);
//	con->pitch = logGetFloat(pitch0);
//	con->yaw = logGetFloat(yaw0);
//}
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

//attitude_t* attitude_init(void){
//
//	attitude_t att;
//
//	int roll0 = logGetVarId("stabilizer", "roll");
//	int pitch0 = logGetVarId("stabilizer", "pitch");
//	int yaw0 = logGetVarId("stabilizer", "yaw");
//
//	att.roll = logGetFloat(roll0);
//	att.pitch = logGetFloat(pitch0);
//	att.yaw = logGetFloat(yaw0);
//
//	return &att;
//
//}
//vec3_t* vec_Init(void){
//	 vec3_t* vec;
//
//	 vec->x = 0;
//	 vec->y = 0;
//	 vec->z = 0;
//
//	 return vec;
//}
//
//quaternion_t* quat_Init(void){
//
//	quaternion_t *quat;
//
//	quat->x = 0;
//	quat->y = 0;
//	quat->z = 0;
//	quat->w = 0;
//
//	return quat;
//}

void appMain(){

	
	static setpoint_t setp;
	vTaskDelay(M2T(3000));


	uint16_t logUp = logGetVarId("range", "up");
//	uint16_t logDown = logGetVarId("range", "down");
//	uint16_t logBack = logGetVarId("range", "back");
	uint16_t logFront = logGetVarId("range", "front");
	uint16_t logRight = logGetVarId("range", "right");
//	uint16_t logLeft = logGetVarId("range", "left");


	setpoint_Init(&setp, 0, 0, 0.3, 0);
	commanderSetSetpoint(&setp, 3);
	vTaskDelay(M2T(2000));
	while (1) {
		vTaskDelay(M2T(10));
		setpoint_Init(&setp, 0.3, 0, 0.3, 0);
			commanderSetSetpoint(&setp, 3);

		if (logGetUint(logFront) < 600){
			while(logGetUint(logRight) >= 600){
				setpoint_Init(&setp, 0, 0, .3, 22.5);
				commanderSetSetpoint(&setp, 3);
			}
			vTaskDelay(M2T(20));
		}
		if (logGetUint(logUp) < 300){
			setpoint_Init(&setp, 0, 0, 0, 0);
			commanderSetSetpoint(&setp, 3);
			break;
		}


	}
	while(1) continue;

	
}

double smoothThrust(double deg){
	double thrust = 65536;//maximum thrust capable of the motors
	if(deg < 0){
		deg = -1*deg;
	}
	return (double).3*thrust*deg/180;
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
