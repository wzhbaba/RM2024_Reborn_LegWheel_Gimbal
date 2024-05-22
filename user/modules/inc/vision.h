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

enum BULLET_TYPE { BULLET_17 = 0, BULLET_42 = 1 };

/* Exported types ------------------------------------------------------------*/
struct rece_packet {
  uint8_t header = 0xA5;
  bool tracking : 1;
  uint8_t can_shoot : 1;
  uint8_t armor_id : 4;
  uint8_t reserved : 2;
  float pitch;
  float yaw;
  uint16_t checksum = 0;
} __attribute__((packed));

struct send_packet {
  uint8_t header = 0x5A;
  uint8_t detect_color : 1;  // 0-red 1-blue
  bool reset_tracker : 1;
  uint8_t mode : 3;
  uint8_t reserved : 3;
  float pitch;
  float yaw;
  float bullet_speed;
  int8_t pitch_bias;
  int8_t yaw_bias;
  int8_t pretime_bias;
  uint16_t checksum = 0;
} __attribute__((packed));

class Vision {
 public:
  void Init();
  void Ctrl();
  void Send();
  void SetReSetTracker(bool _data) { send_.reset_tracker = _data; };
  bool GetTrack() { return rece_.tracking; };
  float GetTargetPitch() { return -rece_.pitch * RAD_2_DEGREE; };
  float GetTargetYaw() { return rece_.yaw * RAD_2_DEGREE; };
  uint8_t GetCanShoot() { return rece_.can_shoot; };

 private:
  void Receive();
  void Encode();
  send_packet send_;
  rece_packet rece_;
};

/* Exported variables --------------------------------------------------------*/
extern Vision vision;
/* Exported function prototypes ----------------------------------------------*/

#endif

#endif /* __VISION_H_ */
