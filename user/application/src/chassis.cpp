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

void Chassis::Init() {
  follow_.Init(1.0f, 0.0f, 0.0f, 2000.0f, 0.0f);
  follow_.Inprovement(PID_INTEGRAL_LIMIT | PID_DERIVATIVE_ON_MEASUREMENT |
                          PID_DERIVATIVE_FILTER,
                      200.0f, 0.0f, 0.0f, 0.0f, 0.05f);
}

void Chassis::Control() {
  board_comm.SetXSpeed(x_target_);
  board_comm.SetYSpeed(y_target_);
}