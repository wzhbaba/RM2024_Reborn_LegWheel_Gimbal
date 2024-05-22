/**
 *******************************************************************************
 * @file      : gimbal.cpp
 * @brief     :
 * @history   :
 *  Version     Date            Author          Note
 *  V0.9.0      yyyy-mm-dd      <author>        1. <note>
 *******************************************************************************
 * @attention :
 *******************************************************************************
 *  Copyright (c) 2024 Reborn Team, USTB.
 *  All Rights Reserved.
 *******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "gimbal.h"

#include "ins.h"
#include "user_lib.h"
/* Private macro -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
Gimbal gimbal;
static float calc_tor;
/* Private function prototypes -----------------------------------------------*/

static void PitchMotorCallback();
static void YawMotorCallback();

/**
 * @brief Initializes the Gimbal.
 */
void Gimbal::PidInit() {
  angle_[0].Init(50.0f, 0.0f, 2.0f, 2000.0f, 0.0f);
  speed_[0].Init(100.0f, 1000.0f, 0.0f, 16384.0f, 0.0f);
  angle_[1].Init(55.0f, 0.0f, 1.0f, 2000.0f, 0.0f);
  speed_[1].Init(110.0f, 0.0f, 0.0f, 16384.0f, 0.0f);
  angle_[0].Inprovement(PID_DERIVATIVE_FILTER | PID_DERIVATIVE_ON_MEASUREMENT,
                        0, 0, 0, 0, 0.05f);
  speed_[0].Inprovement(PID_INTEGRAL_LIMIT | PID_DERIVATIVE_FILTER |
                            PID_DERIVATIVE_ON_MEASUREMENT,
                        3000.0f, 0, 0, 0, 0.05f);
  angle_[1].Inprovement(PID_DERIVATIVE_FILTER | PID_DERIVATIVE_ON_MEASUREMENT,
                        0, 0, 0, 0, 0.05f);
  speed_[1].Inprovement(PID_INTEGRAL_LIMIT | PID_DERIVATIVE_FILTER |
                            PID_DERIVATIVE_ON_MEASUREMENT,
                        3000.0f, 0, 0, 0, 0.05f);
}

/**
 * @brief Initializes the motor for the gimbal.
 */
void Gimbal::MotorInit() {
  motor_[0].Init(0x205, &hcan2, ABSOLUTE_FLAG);
  motor_[1].Init(0x206, &hcan1, ABSOLUTE_FLAG);
  motor_[1].SetOffest(k_yaw_offset);
  motor_[0].pdji_motor_instance->pCanCallBack = PitchMotorCallback;
  motor_[1].pdji_motor_instance->pCanCallBack = YawMotorCallback;
}

/**
 * @brief Controls the gimbal.
 */
void Gimbal::Control() {
  angle_[0].SetMeasure(INS.Pitch);
  angle_[1].SetMeasure(INS.YawTotalAngle);

  speed_[0].SetRef(angle_[0].Calculate());
  speed_[1].SetRef(angle_[1].Calculate());

  speed_[0].SetMeasure(INS.Gyro[X] * RAD_2_DEGREE);
  speed_[1].SetMeasure(INS.Gyro[Z] * RAD_2_DEGREE);

  output_speed_[0] = (int16_t)speed_[0].Calculate();
  output_speed_[1] = (int16_t)speed_[1].Calculate();

  calc_tor = -output_speed_[0];
}

/**
 * @brief Set the pitch position of the gimbal.
 *
 * This function sets the desired pitch position for the gimbal.
 *
 * @param position The desired pitch position in degrees.
 */
void Gimbal::SetPitchPosition(float _ang) {
  set_pitch_ += _ang;
  set_pitch_ = math::AbsLimit(set_pitch_, 25.0f);
  angle_[0].SetRef(set_pitch_);
}

/**
 * @brief Sets the yaw position of the gimbal.
 *
 * @param _ang The desired yaw angle in degrees.
 */
void Gimbal::SetYawPosition(float _ang) {
  set_yaw_ -= _ang;
  angle_[1].SetRef(set_yaw_);
}

void Gimbal::SetVisionPitch(float _ang) {
  set_pitch_ = _ang;
  angle_[0].SetRef(set_pitch_);
}

void Gimbal::SetVisionYaw(float _ang) {
  float temp = _ang - INS.Yaw;
  if (temp > 180.0f) {
    temp -= 360.0f;
  } else if (temp < -180.0f) {
    temp += 360.0f;
  }
  set_yaw_ = temp + INS.YawTotalAngle;
  angle_[1].SetRef(set_yaw_);
}

/**
 * @brief Callback function for the pitch motor.
 */
static void PitchMotorCallback() {
  gimbal.motor_[0].Update();
}

/**
 * @brief Callback function for the yaw motor.
 */
static void YawMotorCallback() {
  gimbal.motor_[1].Update();
}