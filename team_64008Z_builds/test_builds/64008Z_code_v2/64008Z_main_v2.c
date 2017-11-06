///Test code
///

/*Sensors*/
#pragma config(Sensor, in1,    gyro,           	sensorGyro)
#pragma config(Sensor, in2,    accelX,         	sensorAccelerometer)
#pragma config(Sensor, in3,    accelY,         	sensorAccelerometer)
#pragma config(Sensor, dgtl1,  enc1,           	sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  enc2,           	sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  handsUp,        	sensorTouch)
#pragma config(Sensor, dgtl6,  handsDown,      	sensorTouch)
#pragma config(Motor,  port1,  clawMotor,      	tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port2,  frontRightMotor,	tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,  backRightMotor, 	tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port4,  frontLeftMotor, 	tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,  backLeftMotor, 	tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port6,  coneArmTopR, 	  tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port6,  coneArmBottomR, 	tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port7,  coneArmTopL,  	  tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,  coneArmBottomL, 	tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port10,  handMotors,     tmotorVex393_HBridge, openLoop)

/*Compo Init*/
#pragma platform(VEX);

#pragma competitionControl(Competition);
#pragma autonomousDuration(9000);
#pragma userControlDuration(9000);

#include "Vex_Competition_Includes.c" //backcode no modify pls.

/*Other Scripts*/
#include "64008Z_auto_v2.c" //autonomous code.

///initialize the gyroscope
void gyroInit () {
	SensorType[gyro] = sensorNone;
	Wait1Msec(1000);
	SensorType[gyro] = sensorGyro;
	Wait1Msec(2000);
	//SensorScale[Gyro] = 260;
	SensorValue[gyro] = 0;
	SensorFullCount[gyro] = 3600;
}

///sets the speed of the left and right sides of the drive.  Empty is not moving.
void setLeftRightMoveSpeed(int leftSpeed=0, int rightSpeed=0) {
	//left side
	motor[frontLeftMotor] = leftSpeed;
	motor[backLeftMotor] = leftSpeed;
	//right side
	motor[frontRightMotor] = rightSpeed;
	motor[backRightMotor] = rightSpeed;
}

///sets cone pickup speed.  Empty is not moving.
void setConePickUpSpeed (int val=0) {
	//left side
	motor[coneArmTopL] = val;
	motor[coneArmBottomL] = val;
	//right side
	motor[coneArmTopR] = val;
	motor[coneArmBottomR] = val;
}

///holds autonomous commands and main function.
///

///moves goal arms up or down based on value of direction. (positive or negitave)
void autoMoveGoalArms(int direction) {
	direction > 0 ? motor[handMotors] = 127 : motor[handMotors] = -127;
	wait1Msec(1250);
	motor[handMotors] = 0;
}

///time is in ms, speeds fromn -127 to 127.
void moveLeftRightFor(int time, int leftSpeed, int rightSpeed) {
	//left side
	motor[frontLeftMotor] = leftSpeed;
	motor[backLeftMotor] = leftSpeed;
	//right side
	motor[frontRightMotor] = rightSpeed;
	motor[backRightMotor] = rightSpeed;

  wait1Msec(time);

  //left side
	motor[frontLeftMotor] = 0;
	motor[backLeftMotor] = 0;
	//right side
	motor[frontRightMotor] = 0;
	motor[backRightMotor] = 0;
}

//rotates until over a certain gyro value.  //TODO: remove mod?
void rotateUntilDegrees(float degrees, int speed, float mod=1) {
  setLeftRightMoveSpeed(speed, -speed);

  if (degrees > 0) {
    WaitUntil(abs(SensorValue[gyro]) >= degrees - mod);
  }
  else {
    WaitUntil(abs(SensorValue[gyro]) <= degrees + mod);
  }

  setLeftRightMoveSpeed();
}

///runs autonomous
void runAuto() {
  /*Drop Goal*/
	autoMoveGoalArms(-127);

	/*Move Forwards*/
	moveLeftRightFor(2200, 70, 70);

	/*Pick Up Goal*/
	autoMoveGoalArms(127);

	/*Pull Out*/
	moveLeftRightFor(2200, -70, -70);

	/*Rotate ~ >180 Degrees*/
  rotateUntilDegrees(18 * 7.5, 110);

	/*Move to Place Goal*/
	moveLeftRightFor(1500, 100, 100);

	/*Drop Goal*/
	autoMoveGoalArms(-127);

	/*Move Away From Goal.*/
  pullBack();
}

void pullBack () {
  //small jolt
	moveLeftRightFor(150, -127, -127);
	Wait1Msec(150);

  //small jolt
	moveLeftRightFor(150, -127, -127);
	Wait1Msec(150);

  //large pull
	moveLeftRightFor(1500, -127, -127);
	Wait1Msec(150);

  //large pull
	moveLeftRightFor(1500, -127, -127);
  Wait1Msec(150);

  //large pull
	moveLeftRightFor(1500, -127, -127);
  Wait1Msec(150);

  //large pull
	moveLeftRightFor(3000, -127, -127);
}

void pre_auton() {
	gyroInit();
}

task autonomous	{
	runAuto();  //this calls the autonomous script.
}

bool isHoldingClaw = false;
task usercontrol {
	while(true)
	{
		/*Goal Arm*/
		if(vexRT[Btn8R] == 1 && SensorValue[handsUp] == 0)	{
			motor[handMotors] = 127;
		}
		else if (vexRT[Btn8D] == 1 && SensorValue[handsDown] == 0) {
			motor[handMotors] = -127;
		}
		else {
			motor[handMotors] = 0;
		}

		/*Cone Arm*/
		if(vexRT[Btn5U] == 1)	{
			setConePickUpSpeed(127);
		}
		else if (vexRT[Btn6U] == 1) {
			setConePickUpSpeed(-127);
		}
		else {
			setConePickUpSpeed();
		}

		/*Claws*/
		if(vexRT[Btn6D] == 1)	{
			motor[clawMotor] = 127;
			isHoldingClaw = true;
		}
		else if (vexRT[Btn5D] == 1) {
			motor[clawMotor] = -127;
			isHoldingClaw = false;
		}
		else {
			//keeps pressure on the cone when picked up.
			if(isHoldingClaw == true) {
				motor[clawMotor] = 40;  //TODO: change value down if pressure is too much.
			}
			else {
				motor[clawMotor] = 0;
			}
		}

		/*Tank Drive*/
		setLeftRightMoveSpeed(vexRT[Ch3], vexRT[Ch2]);
	}
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
