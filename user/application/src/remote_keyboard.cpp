/**
 *******************************************************************************
 * @file      : remote_keyboard.cpp
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
#include "remote_keyboard.h"

#include "board_comm.h"
#include "chassis.h"
#include "gimbal.h"
#include "referee.h"
#include "remote.h"
#include "shoot.h"
#include "vision.h"
/* Private macro -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t flag = 0;
uint8_t reserve_flag = 0;
uint16_t time_out = 0;
uint8_t shoot_allow_flag = 0;
uint8_t count_flag = 1;
uint8_t change_flag = 1;
/* External variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

static void RemoteChassisCtrl();
static void RemoteGimbalCtrl();
static void RemoteShootCtrl();
static void KeyboardChassisCtrl();
static void KeyboardGimbalCtrl();
static void KeyboardShootCtrl();

void ModeTask()
{
    if (remote.GetS2() == 2) {
        if (remote.GetS1() == 1 || remote.GetS1() == 3) {
            RemoteGimbalCtrl();
            board_comm.SetGimbalFlag(0);
        } else {
            board_comm.SetGimbalFlag(1);
        }
        board_comm.SetShootStop(1);
        board_comm.SetStopFlag(1);
    } else if (remote.GetS2() == 3) {
        RemoteGimbalCtrl();
        RemoteShootCtrl();
        board_comm.SetShootStop(0);
        board_comm.SetGimbalFlag(0);
        board_comm.SetStopFlag(1);
    } else if (remote.GetS2() == 1) {
        if (remote.GetS1() == 1 || remote.GetS1() == 3) {
            RemoteShootCtrl();
            RemoteGimbalCtrl();
            RemoteChassisCtrl();
        } else {
            KeyboardChassisCtrl();
            KeyboardGimbalCtrl();
            KeyboardShootCtrl();
        }
        board_comm.SetShootStop(0);
        board_comm.SetGimbalFlag(0);
        board_comm.SetStopFlag(0);
    } else {
        board_comm.SetShootStop(1);
        board_comm.SetGimbalFlag(1);
        board_comm.SetStopFlag(1);
    }
}

static void RemoteChassisCtrl()
{
    chassis.SetXSpeed(remote.GetCh2() * 2000.0f);
    chassis.SetYSpeed(remote.GetCh3() * 2000.0f);
}

static void RemoteGimbalCtrl()
{
    if (remote.GetS1() == 1 && vision.GetTrack()) {
        gimbal.SetVisionPitch(vision.target_pitch_);
        gimbal.SetVisionYaw(vision.target_yaw_);

    } else {
        gimbal.SetPitchPosition(remote.GetCh1() * 0.5f);
        gimbal.SetYawPosition(remote.GetCh0() * 0.5f);
    }

    if (referee.GetKeyPress(KEY_G)) {
        vision.SetReSetTracker(true);
    } else {
        vision.SetReSetTracker(false);
    }
}

static void RemoteShootCtrl()
{
    static uint8_t flag = 0;
    shoot.SetFricSpeed(6800.0f);

    if (remote.GetS1() == 1) {
        flag = 1;
        shoot.SetFlag(ANGLE_FLAG);
        shoot.SetTriggerPos(0.0f);
    }

    if (remote.GetS1() == 3 && flag == 1) {
        flag = 1;
        shoot.SetFlag(ANGLE_FLAG);
        shoot.SetTriggerPos(-60.0f * 36.0f);
        flag = 0;
    }

    if (remote.GetS1() == 2) {
        shoot.SetFlag(SPEED_FLAG);
        shoot.SetTriggerSpeed(-3.0f * 60 * 36.0f);
    }
}

static void KeyboardChassisCtrl()
{
    if (referee.GetKeyPress(KEY_W)) {
        chassis.SetYSpeed(2000.0f);
    } else if (referee.GetKeyPress(KEY_S)) {
        chassis.SetYSpeed(-2000.0f);
    } else {
        chassis.SetYSpeed(0.0f);
    }

    if (referee.GetKeyPress(KEY_A)) {
        chassis.SetXSpeed(-2000.0f);
    } else if (referee.GetKeyPress(KEY_D)) {
        chassis.SetXSpeed(2000.0f);
    } else {
        chassis.SetXSpeed(0.0f);
    }

    if (referee.GetKeyTick(KEY_C)) {
        board_comm.SetCapFlag(1);
    } else {
        board_comm.SetCapFlag(0);
    }

    if (referee.GetKeyTick(KEY_SHIFT)) {
        chassis.SetWSpeed(2000.0f);
        board_comm.SetRotateFlag(1);
    } else {
        chassis.SetWSpeed(0.0f);
        board_comm.SetRotateFlag(0);
    }
}

static void KeyboardGimbalCtrl()
{
    if (referee.GetKeyPress(MOUSE_R) && vision.GetTrack()) {
        gimbal.SetVisionPitch(vision.GetTargetPitch());
        gimbal.SetVisionYaw(vision.GetTargetYaw());
    } else {
        gimbal.SetPitchPosition(-referee.comma_data.mouse_y * 0.005f);
        gimbal.SetYawPosition(referee.comma_data.mouse_x * 0.005f);
    }
}

static void KeyboardShootCtrl()
{
    if (board_comm.GetShooterOutput() == 1) {
        if (referee.GetKeyTick(KEY_F)) {
            board_comm.SetFricFlag(1);
            shoot.SetFricSpeed(6900.0f);
            if (referee.GetKeyTick(KEY_R)) {
                board_comm.SetShootFlag(1);
                if (change_flag) {
                    shoot.SetTriggerPos(0.0f);
                    shoot.SetFlag(ANGLE_FLAG);
                    change_flag = 0;
                }
                if (count_flag) {
                    time_out++;
                    if (time_out >= 150) {
                        time_out = 0;
                        count_flag = 0;
                        shoot_allow_flag = 1;
                    }
                }
                if (referee.GetKeyTick(KEY_V) || reserve_flag) {
                    shoot.SetTriggerPos(40 * 3 * 36.0f);
                    referee.SetKeyTick(KEY_V, 0);
                    reserve_flag = 0;
                }
                if (!referee.GetKeyPress(MOUSE_L)) {
                    flag = 1;
                }
                if (referee.GetKeyPress(MOUSE_L) && flag && shoot_allow_flag) {
                    shoot.SetTriggerPos(-40.0f * 3 * 36.0f);
                    flag = 0;
                    shoot_allow_flag = 0;
                    count_flag = 1;
                }
            } else {
                board_comm.SetShootFlag(0);
                if (referee.GetKeyPress(MOUSE_L) == 1) {
                    shoot.SetFlag(SPEED_FLAG);
                    if (board_comm.GetCoolingLimit() - board_comm.GetCoolingHeat() > 40) {
                        shoot.SetTriggerSpeed(-2.0f * 60 * 3 * 36.0f);
                    } else {
                        shoot.SetTriggerSpeed(0.0f);
                    }
                } else {
                    shoot.SetTriggerSpeed(0.0f);
                }
                count_flag = 1;
                reserve_flag = 1;
                change_flag = 1;
            }
        } else {
            board_comm.SetFricFlag(0);
            shoot.SetFricSpeed(0.0f);
            shoot.SetFlag(SPEED_FLAG);
            shoot.SetTriggerSpeed(0.0f);
        }
    } else {
        board_comm.SetFricFlag(0);
        shoot.SetFricSpeed(0.0f);
        shoot.SetFlag(SPEED_FLAG);
        shoot.SetTriggerSpeed(0.0f);
    }
}