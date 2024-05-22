/**
 *******************************************************************************
 * @file      : board_comm.h
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
#ifndef __BOARD_COMM_H_
#define __BOARD_COMM_H_

#ifdef __cplusplus

/* Includes ------------------------------------------------------------------*/
#include "bsp_can.h"
/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
#pragma pack(1)
typedef struct {
  int16_t x_speed;
  int16_t y_speed;
  int16_t w_speed;
  uint8_t shoot_flag : 1;
  uint8_t fric_flag : 1;
  uint8_t aim_flag : 1;
  uint8_t rotate_flag : 1;
  uint8_t cap_flag : 1;
  uint8_t ready_flag : 1;
  uint8_t jump_flag : 1;
  uint8_t enegry : 1;  //
  uint8_t long_len_flag : 1;
  uint8_t short_len_flag : 1;
  uint8_t refresh_flag : 1;
  uint8_t reserved : 5;
} send_pack;

typedef struct {
  uint16_t cooling_heat;
  uint16_t cooling_limit;
  int16_t bullet_speed;
  uint8_t robot_id;
  uint8_t shooter_output;
} rece_pack;

#pragma pack()

class BoardComm {
 public:
  void SetXSpeed(float _x_speed) { send_.x_speed = (int16_t)_x_speed; };
  void SetYSpeed(float _y_speed) { send_.y_speed = (int16_t)_y_speed; };
  void SetWSpeed(float _w_speed) { send_.w_speed = (int16_t)_w_speed; };
  void SetShootFlag(uint8_t _flag) { send_.shoot_flag = _flag; };
  void SetFricFlag(uint8_t _flag) { send_.fric_flag = _flag; };
  void SetAimFlag(uint8_t _flag) { send_.aim_flag = _flag; };
  void SetRotateFlag(uint8_t _flag) { send_.rotate_flag = _flag; };
  void SetCapFlag(uint8_t _flag) { send_.cap_flag = _flag; };
  void SetReadyFlag(uint8_t _flag) { send_.ready_flag = _flag; };
  void SetJumpFlag(uint8_t _flag) { send_.jump_flag = _flag; }
  void SetLongLenFlag(uint8_t _flag) { send_.long_len_flag = _flag; }
  void SetShortLenFlag(uint8_t _flag) { send_.short_len_flag = _flag; }
  void SetRefreshUIFlag(uint8_t _flag) { send_.refresh_flag = _flag; }
  uint8_t GetRotateFlag() { return send_.rotate_flag; };
  int16_t GetCoolingHeat() { return rece_.cooling_heat; };
  int16_t GetCoolingLimit() { return rece_.cooling_limit; };
  float GetBulletSpeed() { return rece_.bullet_speed / 10.0f; };

  uint8_t GetShooterOutput() { return rece_.shooter_output; };
  uint16_t GetRobotID() { return rece_.robot_id; }
  uint8_t GetGimbalFlag() { return gimbal_stop_; }
  void SetGimbalFlag(uint8_t _flag) { gimbal_stop_ = _flag; }
  uint8_t GetShootStop() { return shoot_stop_; }
  void SetShootStop(uint8_t _flag) { shoot_stop_ = _flag; }
  void Send();
  void Receive();
  void Init(CAN_HandleTypeDef* _phcan, uint16_t _id);

 private:
  CanInstance* p_instance_;
  send_pack send_;
  rece_pack rece_;
  uint8_t gimbal_stop_, shoot_stop_;
};

/* Exported variables --------------------------------------------------------*/
extern BoardComm board_comm;
/* Exported function prototypes ----------------------------------------------*/

#endif

#endif /* __BOARD_COMM_H_ */
