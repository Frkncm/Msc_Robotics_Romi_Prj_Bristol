/* ROMI PROJECT
   University Of Bristol
   Robotics System
   Furkan Cam
*/

#include "inc/bsp.h"
#include "inc/Romi.h"
#include "inc/lineSensor.hpp"
#include "inc/pidLib.h"

/* we have created with default pins */
lineSensor<uint8_t> lineSensorIns;
sensorMotorPowers sMPower;
PID pidForLineFollowing(0, 0, 0);
PID pidForDriving(0, 0, 0);
uint8_t currentState;

/* Implemented Tasks */

/* Define all used instances for
   non-blocking millis functions */
void lineSensingTask(void);
taskInsert lineSensingTaskIns(lineSensingTask, 20);

void offLineStateTask(void);
taskInsert offLineStateTaskIns(offLineStateTask, 20);

void lineSensingTask(void) {
  /* if the obtained values are higher than
     the determined threshold, we are on line
     and we should implement our motor speed
     according to this situation.
  */
  if (lineSensorIns.isOnLine()) {
    if (lineSensorIns.isLeftOnline() || lineSensorIns.isRightOnline()) {
      //if our robot on the right or left of the line, set it on the midst
      lineSensorIns.calculateMotorSpeed(sMPower);
      smartMotorControl((int)sMPower.left_motor_power, (int)sMPower.right_motor_power);
    } else {
      //Follow the line with smooth motor speed
      float motor_speed = pidForLineFollowing.updateValue(50, readMotorSpeedTask());
      smartMotorControl(motor_speed, motor_speed);
    }
    GO_HANDLE(ON_LINE_STATE);
  } else {
    GO_HANDLE(OFF_LINE_STATE);
  }
}

void offLineStateTask() {
    float motor_speed = pidForDriving.updateValue(50, readMotorSpeedTask());
    smartMotorControl(motor_speed, motor_speed);
}

void setup() {
  bsp_ctor();
  aLastState = digitalRead(outputA);
  lineSensorIns.setTreshold(LINE_TRESHOLD);
  pidForLineFollowing.reset();
  pidForDriving.reset();
  GO_HANDLE(IDLE_STATE); // start with handling IDLE state
}

void loop() {
  taskInsert::executeTasks();
  switch (currentState) {

    case IDLE_STATE: {

        GO_HANDLE(ON_LINE_STATE);
        break;
      }

    case READ_MOTOR_SPEED: {


        break;
      }

    case ON_LINE_STATE: {

        lineSensingTaskIns.callMyTask();

        GO_HANDLE(IDLE_STATE);
        break;
      }

    case OFF_LINE_STATE: {
        Serial.println(readMotorSpeedTask());
        break;
      }

    case ADJUST_MOTOR_SPEED: {

        GO_HANDLE(IDLE_STATE);
        break;
      }

    default : {
        GO_HANDLE(IDLE_STATE);
        break;
      }
  }
}


