/**
 *******************************************************************************
 * @file      : chassis.cpp
 * @brief     :
 * @history   :
 *  Version     Date            Author          Note
 *  V0.9.0      2024-03-07      <author>        1. <note>
 *******************************************************************************
 * @attention :
 *******************************************************************************
 *  Copyright (c) 2024 Reborn Team, USTB.
 *  All Rights Reserved.
 *******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "chassis.h"

#include "board_comm.h"
#include "gimbal.h"
#include "remote.h"
#include "user_lib.h"
/* Private macro -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
Chassis chassis;
/* Private function prototypes -----------------------------------------------*/

void Chassis::Init()
{
    follow_.Init(1.0f, 0.0f, 0.0f, 2000.0f, 0.0f);
    follow_.Inprovement(PID_INTEGRAL_LIMIT | PID_DERIVATIVE_ON_MEASUREMENT | PID_DERIVATIVE_FILTER, 200.0f, 0.0f, 0.0f, 0.0f, 0.05f);
}

void Chassis::Control()
{
    x_spd_ = -arm_sin_f32(gimbal.motor_[1].GetAngle() * DEGREE_2_RAD) * y_target_ + arm_cos_f32(gimbal.motor_[1].GetAngle() * DEGREE_2_RAD) * x_target_;
    y_spd_ = +arm_cos_f32(gimbal.motor_[1].GetAngle() * DEGREE_2_RAD) * y_target_ + arm_sin_f32(gimbal.motor_[1].GetAngle() * DEGREE_2_RAD) * x_target_;

    ang_yaw_ = gimbal.motor_[1].GetEncode();
    if (ang_yaw_ - k_yaw_offset > 4096) {
        ang_yaw_ -= 8192;
    } else if (ang_yaw_ - k_yaw_offset < -4096) {
        ang_yaw_ += 8192;
    }

    if (board_comm.GetRotateFlag() == 1) {
        w_spd_ = w_target_;
    } else {
        follow_.SetRef(k_yaw_offset);
        follow_.SetMeasure(ang_yaw_);
        w_spd_ = follow_.Calculate();
    }
    board_comm.SetXSpeed(x_spd_);
    board_comm.SetYSpeed(y_spd_);
    board_comm.SetWSpeed(w_spd_);
}