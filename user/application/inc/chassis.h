/**
 *******************************************************************************
 * @file      : chassis.h
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
#ifndef __CHASSIS_H_
#define __CHASSIS_H_

#ifdef __cplusplus

/* Includes ------------------------------------------------------------------*/
#include "pid.h"
/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
class Chassis {
 public:
  void Init();
  void Control();
  void SetYawAngle(int16_t _ang) { ang_yaw_ = _ang; };
  void SetXSpeed(float _spd) { x_target_ = _spd; };
  void SetYSpeed(float _spd) { y_target_ = _spd; };

 private:
  Pid follow_;
  int16_t ang_yaw_;
  float x_spd_, y_spd_, w_bias;
  float x_target_, y_target_, w_target_;
};
/* Exported variables --------------------------------------------------------*/
extern Chassis chassis;
/* Exported function prototypes ----------------------------------------------*/

#endif

#endif /* __CHASSIS_H_ */
