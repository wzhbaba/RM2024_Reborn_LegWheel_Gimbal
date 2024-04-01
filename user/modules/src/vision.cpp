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
    st_.k = 0.092f;
    st_.bullet_type = BULLET_17;
    st_.current_v = 28.0f;

    st_.bias_time = 150.0f;
    st_.s_bias = 0.134f;
    st_.z_bias = 0.0f;
}

float Vision::MonoDirectionalAirResistanceModel(float _s, float _v, float _ang)
{
    float z;

    t_ = (expf(st_.k * _s) - 1) / (st_.k * _v * arm_cos_f32(_ang));

    if (t_ < 0) {
        return 0;
    }

    z = (_v * arm_sin_f32(_ang) * t_ - k_gravity * t_ * t_ / 2);

    return z;
}

float Vision::PitchTrajectoryCompensation(float _s, float _z, float _v)
{
    float z_temp, z_actual, dz;
    float ang_pitch;

    z_temp = _z;
    // iteration
    for (int i = 0; i < 20; i++) {
        ang_pitch = atan2f(z_temp, _s);
        z_actual = MonoDirectionalAirResistanceModel(_s, _v, ang_pitch);
        dz = 0.3f * (_z - z_actual);
        z_temp = z_temp + dz;
        if (fabsf(dz) < 0.0001f) {
            break;
        }
    }
    t_ = 0.0f;
    return ang_pitch;
}

void Vision::AutoSolveTrajectory()
{
    float time_delay = st_.bias_time / 1000.0f + t_;
    st_.tar_yaw += st_.v_yaw * time_delay;

    // 计算四块装甲板的位置
    // 装甲板id顺序，以四块装甲板为例，逆时针编号
    //       2
    //    3     1
    //       0
    int use_1 = 1;
    int i = 0;
    int idx = 0;  // 选择的装甲板
    // armor_num = ARMOR_NUM_BALANCE 为平衡步兵
    if (st_.armor_num == ARMOR_NUM_BALANCE) {
        for (i = 0; i < 2; i++) {
            float tmp_yaw = st_.tar_yaw + i * PI;
            float r = st_.r1;
            tar_pos_[i].x = st_.xw - r * arm_cos_f32(tmp_yaw);
            tar_pos_[i].y = st_.yw - r * arm_sin_f32(tmp_yaw);
            tar_pos_[i].z = st_.zw;
            tar_pos_[i].yaw = tmp_yaw;
        }

        float yaw_diff_min = fabsf(aim_yaw_ - tar_pos_[0].yaw);

        // 因为是平衡步兵 只需判断两块装甲板即可
        float temp_yaw_diff = fabsf(aim_yaw_ - tar_pos_[1].yaw);
        if (temp_yaw_diff < yaw_diff_min) {
            yaw_diff_min = temp_yaw_diff;
            idx = 1;
        }

    } else if (st_.armor_num == ARMOR_NUM_OUTPOST) {  // 前哨站
        for (i = 0; i < 3; i++) {
            float tmp_yaw = st_.tar_yaw + i * 2.0f * PI / 3.0f;  // 2/3PI
            float r = (st_.r1 + st_.r2) / 2;                     // 理论上r1=r2 这里取个平均值
            tar_pos_[i].x = st_.xw - r * arm_cos_f32(tmp_yaw);
            tar_pos_[i].y = st_.yw - r * arm_sin_f32(tmp_yaw);
            tar_pos_[i].z = st_.zw;
            tar_pos_[i].yaw = tmp_yaw;
        }

        // TODO 选择最优装甲板 选板逻辑你们自己写，这个一般给英雄用

    } else {
        for (i = 0; i < 4; i++) {
            float tmp_yaw = st_.tar_yaw + i * PI / 2.0;
            float r = use_1 ? st_.r1 : st_.r2;
            tar_pos_[i].x = st_.xw - r * arm_cos_f32(tmp_yaw);
            tar_pos_[i].y = st_.yw - r * arm_sin_f32(tmp_yaw);
            tar_pos_[i].z = use_1 ? st_.zw : st_.zw + st_.dz;
            tar_pos_[i].yaw = tmp_yaw;
            use_1 = !use_1;
        }

        // 2种常见决策方案：
        // 1.计算枪管到目标装甲板yaw最小的那个装甲板
        // 2.计算距离最近的装甲板

        // 计算距离最近的装甲板
        float dis_diff_min = sqrt(tar_pos_[0].x * tar_pos_[0].x + tar_pos_[0].y * tar_pos_[0].y);
        int idx = 0;
        for (i = 1; i < 4; i++) {
            float temp_dis_diff = sqrt(tar_pos_[i].x * tar_pos_[0].x + tar_pos_[i].y * tar_pos_[0].y);
            if (temp_dis_diff < dis_diff_min) {
                dis_diff_min = temp_dis_diff;
                idx = i;
            }
        }

        // 计算枪管到目标装甲板yaw最小的那个装甲板
        // float yaw_diff_min = fabsf(aim_yaw_ - tar_pos_[0].yaw);
        // for (i = 1; i < 4; i++) {
        //     float temp_yaw_diff = fabsf(aim_yaw_ - tar_pos_[i].yaw);
        //     if (temp_yaw_diff < yaw_diff_min) {
        //         yaw_diff_min = temp_yaw_diff;
        //         idx = i;
        //     }
        // }
    }
    aim_z_ = tar_pos_[idx].z + st_.vzw * time_delay;
    aim_x_ = tar_pos_[idx].x + st_.vxw * time_delay;
    aim_y_ = tar_pos_[idx].y + st_.vyw * time_delay;
    // 这里符号给错了
    float temp_pitch = PitchTrajectoryCompensation(sqrtf((aim_x_) * (aim_x_) + (aim_y_) * (aim_y_)) - st_.s_bias,
                                                   aim_z_ + st_.z_bias, st_.current_v);
    if (temp_pitch) {
        aim_pitch_ = temp_pitch;
    }

    if (aim_x_ || aim_y_) {
        aim_yaw_ = atan2f(aim_y_, aim_x_);
    }
}

void Vision::AngTrans()
{
    target_pitch_ = (float)aim_pitch_ * RAD_2_DEGREE;
    target_yaw_ = (float)INS.YawTotalAngle - INS.Yaw + aim_yaw_ * RAD_2_DEGREE;
}

void Vision::Receive()
{
    if (UserRxBufferFS[0] == 0xA5) {
        if (Verify_CRC16_Check_Sum(UserRxBufferFS, sizeof(rece_packet))) {
            memcpy(&rece_, &UserRxBufferFS, sizeof(rece_packet));
            st_.armor_id = rece_.id;
            st_.armor_num = rece_.armors_num;
            memcpy(&st_.xw, &rece_.x, sizeof(float) * 11);
        }
    }
}

void Vision::SetAngle()
{
    st_.current_pitch = -INS.Pitch * DEGREE_2_RAD;
    st_.current_yaw = INS.Yaw * DEGREE_2_RAD;
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

    send_.pitch = st_.current_pitch;
    send_.yaw = st_.current_yaw;

    send_.aim_x = aim_x_;
    send_.aim_y = aim_y_;
    send_.aim_z = aim_z_;

    send_.checksum = Get_CRC16_Check_Sum((uint8_t *)&send_, sizeof(send_packet) - 2, 0xffff);
}

void Vision::Send()
{
    SetAngle();
    Encode();
    CDC_Transmit_FS((uint8_t *)&send_, sizeof(send_packet));
}

void Vision::Ctrl()
{
    Receive();
    if (rece_.tracking == 1) {
        AutoSolveTrajectory();
        AngTrans();
    }
}