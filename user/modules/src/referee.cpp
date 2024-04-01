/**
 *******************************************************************************
 * @file      : referee.cpp
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
#include "referee.h"

#include "board_comm.h"
#include "crc.h"
#include "string.h"
/* Private macro -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
Referee referee;
/* Private function prototypes -----------------------------------------------*/

void Referee::Update(uint8_t *_p_data)
{
    uint16_t judge_length;  // 统计一帧数据长度
    //	CmdID = 0; //数据命令码解析
    // 空数据包，则不作任何处理
    if (_p_data == NULL)
        return;

    // 写入帧头数据(5-byte),用于判断是否开始存储裁判数据
    memcpy(&FrameHeader, _p_data, LEN_HEADER);

    // 判断帧头数据(0)是否为0xA5
    if (_p_data[SOF] == JUDGE_FRAME_HEADER) {
        // 帧头CRC8校验
        if (Verify_CRC8_Check_Sum(_p_data, LEN_HEADER) == TRUE) {
            // 统计一帧数据长度(byte),用于CR16校验
            judge_length = _p_data[DATA_LENGTH] + LEN_HEADER + LEN_CMDID + LEN_TAIL;
            // 帧尾CRC16校验
            if (Verify_CRC16_Check_Sum(_p_data, judge_length) == TRUE) {
                // 2个8位拼成16位int
                CmdID = (_p_data[6] << 8 | _p_data[5]);
                // 解析数据命令码,将数据拷贝到相应结构体中(注意拷贝数据的长度)
                // 第8个字节开始才是数据 data=7
                switch (CmdID) {
                    case ID_command_data:  // 0x0304
                        memcpy(&comma_data, (_p_data + DATA), LEN_command_data);
                        KeyProcess();
                        break;
                }
            }
        }
        // 首地址加帧长度,指向CRC16下一字节,用来判断是否为0xA5,从而判断一个数据包是否有多帧数据
        if (*(_p_data + sizeof(xFrameHeader) + LEN_CMDID + FrameHeader.DataLength + LEN_TAIL) == 0xA5) {
            // 如果一个数据包出现了多帧数据,则再次调用解析函数,直到所有数据包解析完毕
            Update(_p_data + sizeof(xFrameHeader) + LEN_CMDID + FrameHeader.DataLength + LEN_TAIL);
        }
    }
}

void Referee::KeyProcess()
{
    for (short i = 0; i < 16; i++) {
        if (comma_data.keyboard_value & (0x01 << i)) {
            keyboard_[i].is_pressed = 1;
        } else {
            keyboard_[i].is_pressed = 0;
        }
    }

    if (comma_data.left_button_down == 0x01) {
        keyboard_[MOUSE_L].is_pressed = 1;
    } else {
        keyboard_[MOUSE_L].is_pressed = 0;
    }
    if (comma_data.right_button_down == 0x01) {
        keyboard_[MOUSE_R].is_pressed = 1;
    } else {
        keyboard_[MOUSE_R].is_pressed = 0;
    }

    for (short i = 0; i < 18; i++) {
        if (keyboard_[i].is_pressed == 1 && keyboard_[i].last_state == 0) {
            keyboard_[i].is_ticked++;
        }

        if (keyboard_[i].is_ticked % 2 == 0) {
            keyboard_[i].is_ticked = 0;
        }

        keyboard_[i].last_state = keyboard_[i].is_pressed;
    }
}

static void RefereeCallback()
{
    referee.Update(referee.p_instance_->rx_buffer);
}

void Referee::Init(UART_HandleTypeDef *_phuart)
{
    UartInitConfig conf;
    conf.huart = _phuart;
    conf.rx_buffer_size = 256;
    conf.callback_function = RefereeCallback;
    referee.p_instance_ = pUartRegister(&conf);
    return;
}
