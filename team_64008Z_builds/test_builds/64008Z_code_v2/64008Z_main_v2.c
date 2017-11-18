#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    gyro,           sensorGyro)
#pragma config(Sensor, in2,    accelX,         sensorAccelerometer)
#pragma config(Sensor, in3,    accelY,         sensorAccelerometer)
#pragma config(Sensor, dgtl1,  encArm,         sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  handsUp,        sensorTouch)
#pragma config(Sensor, dgtl6,  handsDown,      sensorTouch)
#pragma config(Sensor, dgtl8,  urfOut,         sensorSONAR_inch)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           pushGoalHand,  tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port2,           claw,          tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,           coneArmsOuter, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           frontLeftDrive, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           frontRightDrive, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port6,           coneArmsInner, tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port7,           backLeftDrive, tmotorVex393HighSpeed_MC29, openLoop, reversed, encoderPort, I2C_2)
#pragma config(Motor,  port8,           backRightDrive, tmotorVex393HighSpeed_MC29, openLoop, encoderPort, I2C_1)
#pragma config(Motor,  port9,           goalHandLeft,  tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port10,          goalHandRight, tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//warning code is slightly agressive.

/*Compo Init*/
#pragma platform(VEX);

#pragma competitionControl(Competition);
#pragma autonomousDuration(9000);
#pragma userControlDuration(9000);

#include "Vex_Competition_Includes.c" //backcode no modify pls.

/*Other Scripts*/
//#include "64008Z_auto_v2.c" //autonomous code.

int roundToInt(float f) {  //rounds value to int.
  if(f>0)
  	return (int)(f + 0.5);
  else
  	return (int)(f - 0.5);
}

///initialize the gyroscope
void gyroInit() {
	SensorType[gyro] = sensorNone;
	wait1Msec(1000);
	SensorType[gyro] = sensorGyro;
	wait1Msec(2000);
	//SensorScale[Gyro] = 260;
	SensorValue[gyro] = 0;
	SensorFullCount[gyro] = 36000;
}

///sets the speed of the left and right sides of the drive.  Empty is not moving.
void setLeftRightMoveSpeed(int leftSpeed=0, int rightSpeed=0) {
	//left side
	motor[frontLeftDrive] = leftSpeed;
	motor[backLeftDrive] = leftSpeed;
	//right side
	motor[frontRightDrive] = rightSpeed;
	motor[backRightDrive] = rightSpeed;
}

///sets cone pickup speed.  Empty is not moving.
void setConePickUpSpeed (int val=0) {
	motor[coneArmsOuter] = val;
	motor[coneArmsInner] = val;
}

/*
///sets cone pickup speed.  Empty is not moving.
void setConePickUpSpeed (int val=0) {
	//left side
	motor[coneArmTopL] = val;
	motor[coneArmBottomL] = val;
	//right side
	motor[coneArmTopR] = val;
	motor[coneArmBottomR] = val;
}*/

///holds autonomous commands and main function.
///

///is a task
///moves goal arms up or down based on value of direction. (positive or negitave) //make task
void autoMoveGoalArms(int direction) {
	if(direction > 0) {
		motor[goalHandLeft] = 127;
		motor[goalHandRight] = 127;
	}
	else {
		motor[goalHandLeft] = -127;
		motor[goalHandRight] = -127;
	}

	wait1Msec(1250);

	motor[goalHandLeft] = 0;
	motor[goalHandRight] = 0;
}

///time is in ms, speeds fromn -127 to 127.
void moveLeftRightFor(int time, int leftSpeed, int rightSpeed) {
	//left side
	motor[frontLeftDrive] = leftSpeed;
	motor[backLeftDrive] = leftSpeed;
	//right side
	motor[frontRightDrive] = rightSpeed;
	motor[backRightDrive] = rightSpeed;

  wait1Msec(time);

  //left side
	motor[frontLeftDrive] = 0;
	motor[backLeftDrive] = 0;
	//right side
	motor[frontRightDrive] = 0;
	motor[backRightDrive] = 0;
}

float kp = 3; /*TODO: tune this */
float error = 0;
float rMod = 0;

float encLVal;
float encRVal;

float valMod = 0;
bool exitLoop = false;

float currentTick = 0;

//moves straight
void moveRotations(float rotations, float speed=50) {
	bool isForwards = true;

	currentTick = 0;

	//init encoders
	nMotorEncoder[backLeftDrive] = 0;
	nMotorEncoder[backRightDrive] = 0;

	while(exitLoop == false) {
		//DEBUG: set encoder values
		encLVal = nMotorEncoder[backLeftDrive];
		encRVal = nMotorEncoder[backRightDrive];

		error = nMotorEncoder[backLeftDrive] - nMotorEncoder[backRightDrive];  //set offset value, if 0 both are moveing at same speed.
		writeDebugStreamLine("The error of (L - R) is %d degrees -> at tick %d", error, currentTick);  //DEBUG: this

		//if both encoders share same value (about) {abs(x/2)*2}, it is moving straight so don't change mod.  //TODO: remove?
		if (abs(roundToInt(nMotorEncoder[backLeftDrive]/2))*2 != abs(roundToInt(nMotorEncoder[backRightDrive]/2))*2) {
			rMod += error / kp; //update modifier.
		}
		writeDebugStreamLine("The motor modifier of (mod += error[%d] / kp[%d]) is +[%d / 127] motor speed -> at tick %d", error, kp, rMod, currentTick);  //DEBUG: this

		if (abs(nMotorEncoder[backLeftDrive]) > abs(rotations * 360)) {  //moving back
			if(isForwards == false) { valMod += 1.5; } //each change in direction makes speed smaller
			isForwards = true;

			setLeftRightMoveSpeed(-speed / valMod, -(speed * rMod) / valMod);  //applies the modifier.
		}
		else if (abs(nMotorEncoder[backLeftDrive]) < abs(rotations * 360)) {  //moving forwards
			if(isForwards == true) { valMod += 1.5; } //each change in direction makes speed smaller
			isForwards = false;

			setLeftRightMoveSpeed(speed / valMod, (speed * rMod) / valMod);  //applies the modifier.

			currentTick++;  //DEBUG: find ticks
		}

		if (abs(roundToInt(nMotorEncoder[backLeftDrive]/2))*2 == (abs(rotations) * 360) || valMod >= 6) { //case: loop is done motor is at correct position or speed is too slow.
			exitLoop = true; //main loop exit.
		}

		wait1Msec(100);  //100ms polling time.  No float math if too fast.
	}

	//calcSpeed = (nMotorEncoder[backLeftMotor] / calcSpeed) * 10;  //find avg per second  (*10 is converting ticks to seconds)
	setLeftRightMoveSpeed(); //turn off motors.
}

bool hitLine = false;
task lookForLine {
	//set hitLine to not have been hit.
	hitLine = false;
	while (hitLine == false) {
		//look for the line.

		//if found line hitLine = true;
	}
}

float wheelRadius = 4 * 3.1415926535897932; //in inches.  (that's right, I memorized that many characters...)
void moveInches(float value) {
	moveRotations(value / wheelRadius);  //converts inches to rotations.
}

//rotates until over a certain gyro value.  //TODO: remove mod?
void rotateUntilDegrees(float degrees, int speed, float mod=2) {
  setLeftRightMoveSpeed(speed, -speed);

	//check if over degrees.
  if (degrees > 0) {
    waitUntil(abs(SensorValue[gyro]) >= degrees - mod);
  }
  else {
    waitUntil(abs(SensorValue[gyro]) <= degrees + mod);
  }
	setLeftRightMoveSpeed(-speed, speed);
	wait1Msec(40);  //2polls

  setLeftRightMoveSpeed();
}

// **GYRO TURN**
// target (in degrees) is added/subtracted from current gyro reading to get a target gyro reading
// run PD loop to turn to target
// checks if target has been reached AND is at target for over 250ms before moving on
void gyroTurn (bool isDirectValue, int turnDirection, int targetDegrees, int maxPower=87, int minPower=22, int timeOut=3000) {
	// initialize PD loop variables
	float kp = 0.33; // TO BE TUNED
	int error = targetDegrees;
	int drivePower = 0;

	clearTimer(T2);

	// finish check variables
	bool atTarget = false;

	// initialize gyro data variables
	int targetReading = isDirectValue == false ? SensorValue[gyro] : 0;  //test.

	// get gyroscope target reading
	if (turnDirection >= 1)
		targetReading += targetDegrees;
	else if (turnDirection <= 0)
		targetReading -= targetDegrees;

	// change kp if target is under 20 degree threshold
	if (targetDegrees < 200) { kp = 0.4; }  //ok?

	// run motors until target is within 1/10 degree certainty
	while (!atTarget && (time1[T2] < timeOut))
	{
		error = targetReading - SensorValue[gyro]; 	// calculate error
		drivePower = error * kp;	// calculate PD loop output  //speed

		//keep speed between min and max power.
		if(drivePower < minPower && drivePower > 0)  {
			drivePower = minPower;
		}
		else {
			if(drivePower > -minPower && drivePower < 0)  {
				drivePower = -minPower;
			}
		}

		if(drivePower > maxPower)  {
			drivePower = maxPower;
		}
		else {
			if(drivePower < -maxPower)  {
				drivePower = -maxPower;
			}
		}

		//send power to motors.
		setLeftRightMoveSpeed(drivePower, -drivePower);

		// check for finish
		if (abs(error) > 10) 	// if robot is within 1 degree off target and timer flag is off
			clearTimer(T1);			// start a timer

		if (time1(T1) > 200)	// if the timer is over 200ms and timer flag is true
			atTarget = true;	// set boolean to complete while loop
	}

	setLeftRightMoveSpeed();  //reset motors.

	// reset kp
	kp = 0.33;
}

//value is in inches.
//precision is for how accurate the value is going to be.  More accurate -> longer time to get value.
float getRFDistance (int precision=5, int pollingTime=40) {
	float sumDistances = 0;  //holds all of the distances from the rangefinder.

	for (int i = 0; i < precision; i++) {
		sumDistances += SensorValue[urfOut];  //fetch!  Good boy!

		wait1Msec(pollingTime);  //longer pollingTime means longer to get values.  Too quick and cortex cant keep up / gives same number.  Sensor polling is 3ms.
	}

	return sumDistances / precision;
}

void pullBackDeprecated() {
  //small jolt
	moveLeftRightFor(150, -127, -127);
	wait1Msec(150);

  //small jolt
	moveLeftRightFor(150, -127, -127);
	wait1Msec(150);

  //large pull
	moveLeftRightFor(1500, -127, -127);
	wait1Msec(150);

  //large pull
	moveLeftRightFor(1500, -127, -127);
  wait1Msec(150);

  //large pull
	moveLeftRightFor(1500, -127, -127);
  wait1Msec(150);

  //large pull
	moveLeftRightFor(3000, -127, -127);
}

///runs autonomous
void runAutoDeprecated() {
  /*Drop Goal*/
	autoMoveGoalArms(-127);

	/*Move Forwards*/
	moveLeftRightFor(2200, 70, 70);

	/*Pick Up Goal*/
	autoMoveGoalArms(127);

	/*Pull Out*/
	moveLeftRightFor(2200, -70, -70);

	/*Rotate ~ >180 Degrees*/
  rotateUntilDegrees(180 * 7.5, 110);

	/*Move to Place Goal*/
	moveLeftRightFor(1500, 100, 100);

	/*Drop Goal*/
	autoMoveGoalArms(-127);

	/*Move Away From Goal.*/
  pullBackDeprecated();
}

void runAuto() {
	/*Drop Goal Arms*/
	autoMoveGoalArms(-127);

	/*Move Forwards One Rotation*/
	moveRotations(1, 100);

	/**/
}

void pre_auton() {
	gyroInit();
}

task autonomous	{
	//DEBUG: just test code now.
	moveRotations(3);
	moveRotations(-3);

	wait1Msec(1000);

	gyroTurn(false, 1, 180);  //turn 180 degrees.

	wait1Msec(1000);

	gyroTurn(false, 1, 90);  //turn 90 degrees.

	wait1Msec(1000);

	gyroTurn(false, 1, 90);  //turn 90 degrees.

	wait1Msec(1000);

	gyroTurn(false, -1, 90);  //turn -90 degrees.

	wait1Msec(1000);

	gyroTurn(true, 1, 270);  //turn to 270 degrees.

	wait1Msec(1000);

	gyroTurn(true, 1, 15);  //turn to 15 degrees.

	wait1Msec(1000);

	gyroTurn(true, -1, 270);  //turn to 270 degrees.

	//runAuto();  //this calls the autonomous script.
}

bool tempLock = false;

bool isHoldingClaw = false;
bool isGoalArmMovingDown = false;
task usercontrol {
	while(true)
	{
		/*Goal Arm*/
		if(vexRT[Btn8R] == 1)	{
			isGoalArmMovingDown = true;
		}
		else if (vexRT[Btn8D] == 1 ) {
			isGoalArmMovingDown = false;
		}

		if(isGoalArmMovingDown == true && SensorValue[handsDown] == 0) {
			motor[goalHandLeft] = -127;
			motor[goalHandRight] = -127;
		}
		else if (isGoalArmMovingDown == false && SensorValue[handsUp] == 0){
			motor[goalHandLeft] = 127;
			motor[goalHandRight] = 127;
		}
		else {
			motor[goalHandLeft] = 0;
			motor[goalHandRight] = 0;
		}

		/*Cone Arm*/
		if(vexRT[Btn5U] == 1)	{
			setConePickUpSpeed(87);
		}
		else if (vexRT[Btn5D] == 1) {
			setConePickUpSpeed(-87);
		}
		else {
			setConePickUpSpeed();
		}

		/*Claws*/
		if(vexRT[Btn6U] == 1)	{
			motor[claw] = 127;
			isHoldingClaw = false;
		}
		else if (vexRT[Btn6D] == 1) {
			motor[claw] = -127;
			isHoldingClaw = true;
		}
		else {
			//keeps pressure on the cone when picked up.
			if(isHoldingClaw == true) {
				motor[claw] = -20;  //TODO: change value down or up if pressure is wrong.
			}
			else {
				motor[claw] = 0;
			}
		}

		if(vexRT[Btn7D] == 1)	{
			moveRotations(5);  //move 5 rotations fwd.
		}

		/*Tank Drive*/
		setLeftRightMoveSpeed(vexRT[Ch3], vexRT[Ch2]);
	}
}
