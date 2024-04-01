/**
 *******************************************************************************
 * @file      : dji_motor.h
 * @brief     :
 * @history   :
 *  Version     Date            Author          Note
 *  V0.9.0      yyyy-mm-dd      <author>        1. <note>
 *******************************************************************************
 * @attention :
 *******************************************************************************
 *  Copyright (c) 2023 Reborn Team, USTB.
 *  All Rights Reserved.
 *******************************************************************************
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DJI_MOTOR_H_
#define __DJI_MOTOR_H_

#ifdef __cplusplus

/* Includes ------------------------------------------------------------------*/
#include "bsp_can.h"
/* Exported macro ------------------------------------------------------------*/
#define RPM_2_ANGLE_PER_SEC 6.0f        // ×360°/60sec
#define RPM_2_RAD_PER_SEC 0.104719755f  // ×2pi/60sec
#define ECD_ANGLE_COEF_DJI 0.043945f    // (360/8192),将编码器值转化为角度制

/* Exported constants --------------------------------------------------------*/
typedef enum {
    RELATIVE_FLAG = 0,
    ABSOLUTE_FLAG = 1,
} Pos_Flag;

/* Exported types ------------------------------------------------------------*/

class DjiMotor
{
   public:
    CanInstance *pdji_motor_instance;
    void Update();
    void Init(uint32_t _idx, CAN_HandleTypeDef *_phcan, uint8_t _init);
    float GetAngle()
    {
        return ang_real_;
    }
    int GetEncode()
    {
        return encode_;
    }
    int GetSpd()
    {
        return spd_;
    }
    int GetTor()
    {
        return tor_;
    }
    void SetOffest(int16_t _offest)
    {
        encode_offest_ = _offest;
    }

   private:
    int16_t round_cnt_;
    int16_t encode_;
    int16_t encode_offest_;
    int16_t last_encode_;
    int16_t spd_;
    int16_t tor_;
    int16_t temp_;
    uint8_t init_;
    float ang_real_;
};
/* Exported variables --------------------------------------------------------*/
extern DjiMotor dji_motor;
/* Exported function prototypes ----------------------------------------------*/

void DjiMotorSend(CAN_HandleTypeDef *_phcan, uint32_t _idx, int16_t _data1, int16_t _data2, int16_t _data3, int16_t _data4);

#endif

#endif /* __DJI_MOTOR_H_ */
