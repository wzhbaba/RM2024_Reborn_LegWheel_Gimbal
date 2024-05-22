/**
 *******************************************************************************
 * @file      : vision.cpp
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
#include "vision.h"

#include "board_comm.h"
#include "bsp_dwt.h"
#include "crc.h"
#include "ins.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include "user_lib.h"
/* Private macro -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
Vision vision;
/* Private function prototypes -----------------------------------------------*/

void Vision::Init()
{
    send_.bullet_speed = 27.0f;
}

void Vision::Receive()
{
    if (UserRxBufferFS[0] == 0xA5) {
        if (Verify_CRC16_Check_Sum(UserRxBufferFS, sizeof(rece_packet))) {
            memcpy(&rece_, &UserRxBufferFS, sizeof(rece_packet));
        }
    }
}

void Vision::Encode()
{
    send_.header = 0x5A;
    if (board_comm.GetRobotID() > 100) {
        send_.detect_color = 0;
    } else {
        send_.detect_color = 1;
    }
    send_.reserved = 0;

    send_.pitch = -INS.Pitch * DEGREE_2_RAD;
    send_.yaw = INS.Yaw * DEGREE_2_RAD;

    send_.checksum = Get_CRC16_Check_Sum((uint8_t *)&send_, sizeof(send_packet) - 2, 0xffff);
}

void Vision::Send()
{
    Encode();
    CDC_Transmit_FS((uint8_t *)&send_, sizeof(send_packet));
}

void Vision::Ctrl()
{
    Receive();
    if (rece_.tracking == 1) {
        board_comm.SetAimFlag(1);
    } else {
        board_comm.SetAimFlag(0);
    }
}