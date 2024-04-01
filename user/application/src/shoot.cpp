/**
 *******************************************************************************
 * @file      : shoot.cpp
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
#include "shoot.h"
/* Private macro -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
Shoot shoot;
/* Private function prototypes -----------------------------------------------*/

/**
 * @brief This function is a callback for the left friction motor.
 *
 * This function is called when an event related to the left friction motor occurs.
 * It is responsible for handling the event and performing the necessary actions.
 *
 * @return void
 */
static void LeftFricMotorCallback();

/**
 * @brief This function is a callback for the right friction motor.
 *
 * This function is called when a specific event occurs related to the right friction motor.
 * It is responsible for handling the event and performing the necessary actions.
 *
 * @return void
 */
static void RightFricMotorCallback();

/**
 * @brief This function is a callback for triggering the motor.
 */
static void TriggerMotorCallback();

void Shoot::PidInit()
{
    fric_speed_[0].Init(10.0f, 0.2f, 0.0f, 15000.0f, 0.0f);
    fric_speed_[1].Init(10.0f, 0.2f, 0.0f, 15000.0f, 0.0f);
    fric_tor_[0].Init(1.0f, 0.0f, 0.0f, 15000.0f, 0.0f);
    fric_tor_[1].Init(1.0f, 0.0f, 0.0f, 15000.0f, 0.0f);
    trigger_pos_.Init(10.0f, 0.0f, 0.5f, 5000.0f, 2.0f);
    trigger_speed_.Init(20.0f, 1.0f, 0.0f, 10000.0f, 0.0f);
    trigger_tor_.Init(0.7f, 0.0f, 0.0f, 10000.0f, 0.0f);
    fric_speed_[0].Inprovement(PID_INTEGRAL_LIMIT, 2000.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    fric_speed_[1].Inprovement(PID_INTEGRAL_LIMIT, 2000.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    fric_tor_[0].Inprovement(PID_INTEGRAL_LIMIT, 10000.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    fric_tor_[1].Inprovement(PID_INTEGRAL_LIMIT, 10000.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    trigger_pos_.Inprovement(PID_DERIVATIVE_ON_MEASUREMENT | PID_DERIVATIVE_FILTER, 0.0f, 0.0f, 0.0f, 0.0f, 0.05f);
    trigger_speed_.Inprovement(PID_INTEGRAL_LIMIT, 2000.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    trigger_tor_.Inprovement(PID_INTEGRAL_LIMIT, 5000.0f, 0.0f, 0.0f, 0.0f, 0.0f);
}

void Shoot::MotorInit()
{
    fric_motor_[0].Init(0x201, &hcan2, ABSOLUTE_FLAG);
    fric_motor_[1].Init(0x202, &hcan2, ABSOLUTE_FLAG);
    trigger_motor_.Init(0x203, &hcan2, RELATIVE_FLAG);
    fric_motor_[0].pdji_motor_instance->pCanCallBack = LeftFricMotorCallback;
    fric_motor_[1].pdji_motor_instance->pCanCallBack = RightFricMotorCallback;
    trigger_motor_.pdji_motor_instance->pCanCallBack = TriggerMotorCallback;
}

void Shoot::Control()
{
    FricCalc();
    TrigCalc();
}

void Shoot::FricCalc()
{
    for (int i = 0; i < 2; i++) {
        fric_speed_[i].SetMeasure(fric_motor_[i].GetSpd());
        fric_tor_[i].SetRef(fric_speed_[i].Calculate());
        fric_tor_[i].SetMeasure(fric_motor_[i].GetTor());
        fric_output_[i] = fric_tor_[i].Calculate();
    }
}

void Shoot::TrigCalc()
{
    if (shoot_flag_ == ANGLE_FLAG) {
        trigger_pos_.SetMeasure(trigger_motor_.GetAngle());
        trigger_speed_.SetRef(trigger_pos_.Calculate());
    }

    trigger_speed_.SetMeasure(trigger_motor_.GetSpd());
    trigger_tor_.SetRef(trigger_speed_.Calculate());
    trigger_tor_.SetMeasure(trigger_motor_.GetTor());
    trig_output_ = trigger_tor_.Calculate();
}

void Shoot::SetFricSpeed(float _speed)
{
    fric_speed_[0].SetRef(-_speed);
    fric_speed_[1].SetRef(_speed);
}

void Shoot::SetTriggerPos(float _pos)
{
    trigger_pos_.SetRef(trigger_motor_.GetAngle() + _pos);
}

void Shoot::SetTriggerSpeed(float _speed)
{
    trigger_speed_.SetRef(_speed);
}

void Shoot::SetFlag(uint16_t _flag)
{
    shoot_flag_ = _flag;
}

static void LeftFricMotorCallback()
{
    shoot.fric_motor_[0].Update();
}

static void RightFricMotorCallback()
{
    shoot.fric_motor_[1].Update();
}

static void TriggerMotorCallback()
{
    shoot.trigger_motor_.Update();
}