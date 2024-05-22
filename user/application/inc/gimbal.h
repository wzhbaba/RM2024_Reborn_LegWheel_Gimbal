/**
 *******************************************************************************
 * @file      : gimbal.h
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
#ifndef __GIMBAL_H_
#define __GIMBAL_H_

#ifdef __cplusplus

/* Includes ------------------------------------------------------------------*/
#include "dji_motor.h"
#include "pid.h"
/* Exported macro ------------------------------------------------------------*/
const int16_t k_yaw_offset = 6801;

/* Exported constants --------------------------------------------------------*/
#define YAW 0;
#define PITCH 1;
/* Exported types ------------------------------------------------------------*/

/**
 * @brief The Gimbal class represents a gimbal system used for controlling the pitch and yaw positions.
 */
class Gimbal
{
   public:
    Pid angle_[2];          /**< Array of Pid objects for angle control of pitch and yaw. */
    Pid speed_[2], tor_[2]; /**< Array of Pid objects for speed control of pitch and yaw. */
    DjiMotor motor_[2];     /**< Array of DjiMotor objects for controlling the motors of the gimbal. */

    /**
     * @brief Controls the gimbal system.
     */
    void Control();

    /**
     * @brief Sets the pitch position of the gimbal.
     * @param _ang The desired pitch angle in degrees.
     */
    void SetPitchPosition(float _ang);

    /**
     * @brief Sets the yaw position of the gimbal.
     * @param _ang The desired yaw angle in degrees.
     */
    void SetYawPosition(float _ang);
    void SetVisionYaw(float _ang);
    void SetVisionPitch(float _ang);
    /**
     * @brief Initializes the PID controllers for angle and speed control.
     */
    void PidInit();

    /**
     * @brief Initializes the motors of the gimbal.
     */
    void MotorInit();

    int16_t output_speed_[2]; /**< Array of output speeds for the motors. */
   private:
    float set_pitch_;
    float set_yaw_;
};
/* Exported variables --------------------------------------------------------*/
extern Gimbal gimbal;
/* Exported function prototypes ----------------------------------------------*/

#endif

#endif /* __GIMBAL_H_ */
