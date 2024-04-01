/**
 *******************************************************************************
 * @file      : vision.h
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __VISION_H_
#define __VISION_H_

#ifdef __cplusplus

/* Includes ------------------------------------------------------------------*/
#include "user_lib.h"
/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
const float k_gravity = 9.78f;

enum ARMOR_ID {
    ARMOR_OUTPOST = 0,
    ARMOR_HERO = 1,
    ARMOR_ENGINEER = 2,
    ARMOR_INFANTRY3 = 3,
    ARMOR_INFANTRY4 = 4,
    ARMOR_INFANTRY5 = 5,
    ARMOR_GUARD = 6,
    ARMOR_BASE = 7
};

enum ARMOR_NUM {
    ARMOR_NUM_BALANCE = 2,
    ARMOR_NUM_OUTPOST = 3,
    ARMOR_NUM_NORMAL = 4
};

enum BULLET_TYPE {
    BULLET_17 = 0,
    BULLET_42 = 1
};

/* Exported types ------------------------------------------------------------*/
// 设置参数
typedef struct {
    float k;  // 弹道系数

    // 自身参数
    enum BULLET_TYPE bullet_type;  // 自身机器人类型 0-步兵 1-英雄
    float current_v;               // 当前弹速
    float current_pitch;           // 当前pitch
    float current_yaw;             // 当前yaw

    // 目标参数
    float xw;           // ROS坐标系下的x
    float yw;           // ROS坐标系下的y
    float zw;           // ROS坐标系下的z
    float vxw;          // ROS坐标系下的vx
    float vyw;          // ROS坐标系下的vy
    float vzw;          // ROS坐标系下的vz
    float tar_yaw;      // 目标yaw
    float v_yaw;        // 目标yaw速度
    float r1;           // 目标中心到前后装甲板的距离
    float r2;           // 目标中心到左右装甲板的距离
    float dz;           // 另一对装甲板的相对于被跟踪装甲板的高度差
    int bias_time;      // 偏置时间
    float s_bias;       // 枪口前推的距离
    float z_bias;       // yaw轴电机到枪口水平面的垂直距离
    uint8_t armor_id;   // 装甲板类型  0-outpost 6-guard 7-base
                        // 1-英雄 2-工程 3-4-5-步兵
    uint8_t armor_num;  // 装甲板数字  2-balance 3-outpost 4-normal
} solve_trajectory_params;

// 用于存储目标装甲板的信息
typedef struct {
    float x;    // 装甲板在世界坐标系下的x
    float y;    // 装甲板在世界坐标系下的y
    float z;    // 装甲板在世界坐标系下的z
    float yaw;  // 装甲板坐标系相对于世界坐标系的yaw角
} tar_pos;

struct rece_packet {
    uint8_t header = 0xA5;
    bool tracking : 1;
    uint8_t id : 3;          // 0-outpost 6-guard 7-base
    uint8_t armors_num : 3;  // 2-balance 3-outpost 4-normal
    uint8_t reserved : 1;
    float x;
    float y;
    float z;
    float yaw;
    float vx;
    float vy;
    float vz;
    float v_yaw;
    float r1;
    float r2;
    float dz;
    uint16_t checksum = 0;
} __attribute__((packed));

struct send_packet {
    uint8_t header = 0x5A;
    uint8_t detect_color : 1;  // 0-red 1-blue
    bool reset_tracker : 1;
    uint8_t reserved : 6;
    float pitch;
    float yaw;
    float aim_x;
    float aim_y;
    float aim_z;
    uint16_t checksum = 0;
} __attribute__((packed));

class Vision
{
   public:
    float target_pitch_, target_yaw_;
    void Init();
    void Ctrl();
    void Send();
    void SetReSetTracker(bool _data)
    {
        send_.reset_tracker = _data;
    };
    bool GetTrack()
    {
        return rece_.tracking;
    };
    float GetTargetPitch()
    {
        return target_pitch_;
    };
    float GetTargetYaw()
    {
        return target_yaw_;
    };

   private:
    void AutoSolveTrajectory();
    void Receive();
    void Encode();
    void SetAngle();
    void AngTrans();
    float MonoDirectionalAirResistanceModel(float _s, float _v, float _ang);
    float PitchTrajectoryCompensation(float _s, float _z, float _v);
    tar_pos tar_pos_[4];
    solve_trajectory_params st_;
    send_packet send_;
    rece_packet rece_;
    float t_;
    float aim_x_, aim_y_, aim_z_, aim_pitch_, aim_yaw_;
    uint32_t dwt_cnt;
    float dt_;
};

/* Exported variables --------------------------------------------------------*/
extern Vision vision;
/* Exported function prototypes ----------------------------------------------*/

#endif

#endif /* __VISION_H_ */
