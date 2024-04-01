/**
 *******************************************************************************
 * @file      : user_lib.h
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
#ifndef __USER_LIB_H_
#define __USER_LIB_H_

#ifdef __cplusplus

/* Includes ------------------------------------------------------------------*/
#include <stm32f407xx.h>

#include "arm_math.h"
#include "stdint.h"
/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define RAD_2_DEGREE 57.2957795f     // 180/pi
#define DEGREE_2_RAD 0.01745329252f  // pi/180

/* Exported types ------------------------------------------------------------*/
namespace math
{
float Sqrt(float _x);
float AbsLimit(float _num, float _limit);
float Sign(float _value);
float FloatDeadband(float _value, float _min_value, float _max_value);
float FloatConstrain(float _value, float _min_value, float _max_value);
int16_t Int16Constrain(int16_t _value, int16_t _min_value, int16_t _max_value);
float LoopFloatConstrain(float _value, float _min_value, float _max_value);
float ThetaFormat(float _ang);
int FloatRounding(float _raw);
float *Norm3d(float *v);
float NormOf3d(float *v);
void Cross3d(float *v1, float *v2, float *res);
float Dot3d(float *v1, float *v2);
};  // namespace math
/* Exported variables --------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif

#endif /* __USER_LIB_H_ */
