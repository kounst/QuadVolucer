/* =========================================================================== */
/* Project:        quadcontrol */
/* File:           mixer.c */
/* Organization:    */
/* Author:         Konstantin Reichmeyer */
/* Date:           02/09/2009 */
/* =========================================================================== */


/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "mixer.h"
#include "main.h"
#include "GPIO.h"


/* Private variables ---------------------------------------------------------*/  
struct pid level = {6107,6107}; //26413

struct pdyaw yaw = {30400};
union channels set;
	
extern u8 TxBuffer[];
extern struct props gas;
extern union channels neutral;
extern union pulsw pulswidth;

extern int16_t GyroX, GyroY, GyroZ;
volatile uint8_t idle_throttle;
int differential_error =0;
int differential_error1 =0;
int differential_error2 =0;

volatile float K_p, K_i, K_d;
volatile float level_stick_sense, yaw_stick_sense;



int temp;

volatile float K_pY, K_iY, K_dY;



enum mode
{
	off,
	standby,
	flight
}pwrmode = off;

int throttle = 0;
int modeswitch = 0;


/*******************************************************************************
* Function Name  : mixer
* Description    : calculates each props speed
* Input          : pulswidth
* Output         : gas
* Return         : None
*******************************************************************************/
void mixer()
{
	//throttle: 0 ... 4000; roll,pitch,yaw: -2000 ... 2000;
  set.rotate.throttle = (pulswidth.puls.pw4 - neutral.rotate.throttle) * 1.67; 
	set.rotate.roll = (-pulswidth.puls.pw1 + neutral.rotate.roll) * level_stick_sense;
  set.rotate.pitch = (pulswidth.puls.pw2 - neutral.rotate.pitch) * level_stick_sense;
  set.rotate.yaw = (pulswidth.puls.pw3 - neutral.rotate.yaw) * yaw_stick_sense;



	//pid controller: pitch, roll
  levelcontroller(GyroX, 0);
  levelcontroller(GyroY, 1);
  yawcontroller(GyroZ);

  throttle = 0.95 * throttle + (1 - 0.95) * pulswidth.puls.pw4 * 2.5;       //used for mode selection only
  modeswitch = 0.95  * modeswitch + (1 - 0.95) * pulswidth.puls.pw6 * 2.5;  //used for mode selection only



	//FSM Power modes: off, standby (props rotating very slowly), flight (normal,full operation)
	switch(pwrmode)
	{
		case off:
		{
			mode_off();

			//pwrmode change
			if(modeswitch > 3000 && throttle < 3000)
				pwrmode = standby;
			break;
		}
		
		case standby:
		{		
			mode_standby();

			//pwrmode change
			if(modeswitch < 3000)
				pwrmode = off;
			else
			{
				if(modeswitch > 6000 && throttle < 3000)
					pwrmode = flight;
			}
			break;
		}
		
		case flight:
		{
			mode_flight();
			
			//pwrmode change
 			if(modeswitch < 6000 && throttle < 3000)
				pwrmode = standby;
		}		
	}
}

/*******************************************************************************
* Function Name  : levelcontroller
* Description    : pid controller for roll and pitch
* Input          : channels, ADCValue[]
* Output         : level.levelsign
* Return         : None
*******************************************************************************/
void levelcontroller(int16_t gyro, int axis)
{
  level.levelerror[axis] = set.level[axis] - gyro;

	if(pwrmode == flight)
    level.levelerror_int[axis] += level.levelerror[axis];
  else
    level.levelerror_int[axis] = 0; 

	if(level.levelerror_int[axis] > 20000)
	  level.levelerror_int[axis] = 20000;
	if(level.levelerror_int[axis] < -20000)
	  level.levelerror_int[axis] = -20000;

  differential_error = (level.levelerror[axis] - level.levellasterror[axis]) + differential_error1 + differential_error2;
  differential_error2 = differential_error1;
  differential_error1 = (level.levelerror[axis] - level.levellasterror[axis]);
  

  //differential_error = 
  if(differential_error < -20000)   //limit differential_error 
    differential_error = -20000;
  if(differential_error > 20000)
    differential_error = 20000;
	
  level.levelsign[axis] = K_p * level.levelerror[axis] + K_i * level.levelerror_int[axis] + K_d * differential_error;
  if(level.levelsign[axis] > 10000)
    level.levelsign[axis] = 10000;
  if(level.levelsign[axis] < -10000)
    level.levelsign[axis] = -10000;
 
  level.levellasterror[axis] = level.levelerror[axis];
  
}

void yawcontroller(int16_t gyro)
{
  yaw.yawerror = set.rotate.yaw + gyro*2;

  if(pwrmode == flight)
    yaw.yawerror_int += yaw.yawerror;
  else
    yaw.yawerror_int = 0; 

	if(yaw.yawerror_int > 20000)
	  yaw.yawerror_int = 20000;
	if(yaw.yawerror_int < -20000)
	  yaw.yawerror_int = -20000;


  yaw.yawsign = K_pY * yaw.yawerror + K_iY * yaw.yawerror_int - K_dY * (yaw.yawerror - yaw.yawlasterror);

  yaw.yawlasterror = yaw.yawerror;
  
}



//powermode functions

void mode_off()
{
	gas.front = 0;
	gas.rear = 0;
	gas.left = 0;
	gas.right = 0;
  
}

void mode_standby()
{
	gas.front = idle_throttle;
	gas.rear = idle_throttle;
	gas.left = idle_throttle;
	gas.right = idle_throttle;

}

void mode_flight()
{
  //FRONT
	temp = (set.rotate.throttle - level.levelsign[0] + yaw.yawsign)/24;
  
  temp = limitStep(gas.front, temp);

	if(temp < idle_throttle)
		gas.front = idle_throttle;
	else
	{
		if(temp > 255)
			gas.front = 255;
		else
			gas.front = temp;
	}

	//REAR
	temp = (set.rotate.throttle + level.levelsign[0] + yaw.yawsign )/24;

  temp = limitStep(gas.rear, temp);

	if(temp < idle_throttle)
		gas.rear = idle_throttle;
	else
	{
		if(temp > 255)
			gas.rear = 255;
		else
			gas.rear = temp;
	}

	//LEFT
	temp = (set.rotate.throttle - level.levelsign[1] - yaw.yawsign )/24;

  temp = limitStep(gas.left, temp);

	if(temp < idle_throttle)
		gas.left = idle_throttle;
	else
	{
		if(temp > 255)
			gas.left = 255;
		else
			gas.left = temp;
	}

	//RIGHT
	temp = (set.rotate.throttle + level.levelsign[1] - yaw.yawsign )/24;

  temp = limitStep(gas.right, temp);

	if(temp < idle_throttle)
		gas.right = idle_throttle;
	else
	{
		if(temp > 255)
			gas.right = 255;
		else
			gas.right = temp;
	}
}


int limitStep(int gas, int gasneu)
{
  if(gasneu >= gas)
  {
    if(gasneu - gas > GASLIMIT)
      gasneu = gas + GASLIMIT;
  }
  else
  {
    if(gas - gasneu > GASLIMIT)
      gasneu = gas - GASLIMIT;
  }
  return gasneu;
}
