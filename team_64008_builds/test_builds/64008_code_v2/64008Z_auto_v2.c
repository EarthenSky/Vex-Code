///holds autonomous commands and main function.
///

///moves goal arms up or down based on value of direction. (positive or negitave)
void autoMoveGoalArms(int diriction) {
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
