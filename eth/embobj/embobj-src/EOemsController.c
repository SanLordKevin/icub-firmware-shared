
/*  @file       EOdecoupler.c
    @brief      This file implements internal implementation of motor-joint decoupling matrix.
    @author     alessandro.scalzo@iit.it
    @date       07/05/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "EoCommon.h"

#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"
#include "EOVtheSystem.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOemsController.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOemsController_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOemsController";

static EOemsController *s_emsc = NULL;

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOemsController* eo_emsController_Init(uint8_t nmotors, emsBoardType_t board_type) 
{
    if (nmotors > MAX_MOTORS) return NULL;

    s_emsc = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOemsController), 1);

    if (s_emsc)
    {
        s_emsc->nmotors = nmotors;
        
        while (nmotors)
        {
            s_emsc->axis_controller[--nmotors] = eo_axisController_New();     
        }

        s_emsc->decoupler[DECOUPLER_POS] = NULL;
        s_emsc->decoupler[DECOUPLER_TRQ] = NULL;
        s_emsc->decoupler[DECOUPLER_PWM] = NULL;

        switch (board_type)
        {
            case EMS_GENERIC:
                break;

            case EMS_WAIST:
                eo_emsController_SetDecoupler(DECOUPLER_PWM, decoupler_waist_pwm);
                break;

            case EMS_SHOULDER:
                eo_emsController_SetDecoupler(DECOUPLER_POS, decoupler_shoulder_pos);
                eo_emsController_SetDecoupler(DECOUPLER_TRQ, decoupler_shoulder_trq);
                eo_emsController_SetDecoupler(DECOUPLER_PWM, decoupler_shoulder_pwm);
                break;
        }
    }

    return s_emsc;
}

extern void eo_emsController_ReadEncoders(int32_t *enc)
{
    static float axis_pos[MAX_MOTORS];

    float *pos = s_emsc->encoder_pos;

    for (int i=0; i<s_emsc->nmotors; ++i)
    {
        s_emsc->encoder_pos[i] = (float)enc[i];
    }

    if (s_emsc->decoupler[DECOUPLER_POS])
    {
        eo_decoupler_Mult(s_emsc->decoupler[DECOUPLER_POS], s_emsc->encoder_pos, axis_pos);
     
        pos = axis_pos;       
    }

    for (uint8_t i=0; i<s_emsc->nmotors; ++i)
    {
        eo_axisController_ReadEncPos(s_emsc->axis_controller[i], pos[i]);
    }
}

extern void eo_emsController_SkipEncoders(void)
{
    for (uint8_t i=0; i<s_emsc->nmotors; ++i)
    {
        eo_axisController_SkipEncPos(s_emsc->axis_controller[i]);
    }
}

extern float eo_emsController_GetSpeed(uint8_t joint)
{
    return eo_axisController_GetSpeed(s_emsc->axis_controller[joint]);                
}

extern void eo_emsController_ReadTorques(int32_t *torque)
{
    for (int i=0; i<s_emsc->nmotors; ++i)
    {
        s_emsc->torque_meas[i] = (float)torque[i];
    }

    float *trq = s_emsc->torque_meas;
    static float axis_trq[MAX_MOTORS];

    if (s_emsc->decoupler[DECOUPLER_TRQ])
    {
        eo_decoupler_Mult(s_emsc->decoupler[DECOUPLER_TRQ], s_emsc->torque_meas, axis_trq);
        trq = axis_trq;
    }

    for (uint8_t i=0; i<s_emsc->nmotors; ++i)
    {
        eo_axisController_ReadTorque(s_emsc->axis_controller[i], trq[i]);
    }
}

extern void eo_emsController_SetPosRef(uint8_t joint, float pos, float vel)
{
    eo_axisController_SetPosRef(s_emsc->axis_controller[joint], pos, vel);
}

extern void eo_emsController_SetVelRef(uint8_t joint, float vel, float acc)
{
    eo_axisController_SetVelRef(s_emsc->axis_controller[joint], vel, acc);
}

extern void eo_emsController_SetTrqRef(uint8_t joint, float trq)
{
    eo_axisController_SetTrqRef(s_emsc->axis_controller[joint], trq);
}

extern float* eo_emsController_PWM()
{
    static float pwm_axis[MAX_MOTORS];
    static float pwm_motor[MAX_MOTORS];

    for (uint8_t i=0; i<s_emsc->nmotors; ++i)
    {
        pwm_axis[i] = eo_axisController_PWM(s_emsc->axis_controller[i]);
    }

    if (s_emsc->decoupler[DECOUPLER_PWM])
    {
        eo_decoupler_Mult(s_emsc->decoupler[DECOUPLER_PWM], pwm_axis, pwm_motor);
        
        return pwm_motor;    
    }

    return pwm_axis;
}


extern void eo_emsController_SetControlMode(uint8_t joint, control_mode_t mode)
{
    eo_axisController_SetControlMode(s_emsc->axis_controller[joint], mode);
}

extern void eo_emsController_ResetPosPid(uint8_t joint)
{
    eo_pid_Reset(eo_axisController_GetPosPidPtr(s_emsc->axis_controller[joint]));
}

extern void eo_emsController_Stop(uint8_t joint)
{
    eo_axisController_Stop(s_emsc->axis_controller[joint]);
}

extern void eo_emsController_ResetTrqPid(uint8_t joint)
{
    eo_pid_Reset(eo_axisController_GetTrqPidPtr(s_emsc->axis_controller[joint]));
}

extern void eo_emsGetActivePidStatus(uint8_t joint, float *pwm, float *err)
{
    eo_axisController_GetActivePidStatus(s_emsc->axis_controller[joint], pwm, err);    
}

extern void eo_emsController_SetDecoupler(emsMotorDecoupler_t dec_type, float matrix[4][4])
{
    s_emsc->decoupler[dec_type] = eo_decoupler_New(s_emsc->nmotors, matrix);
}

extern void eo_emsController_SetPosPid(uint8_t joint, float kp, float kd, float ki)
{
    eo_axisController_SetVelMin(s_emsc->axis_controller[joint], (kd==0.0f ? 0.0f : 1024.0f/kd));
    
    eo_pid_SetPid(eo_axisController_GetPosPidPtr(s_emsc->axis_controller[joint]), kp, kd, ki);    
}
    
extern void eo_emsController_SetPosPidLimits(uint8_t joint, float Ymax, float Imax)
{
    eo_pid_SetPidLimits(eo_axisController_GetPosPidPtr(s_emsc->axis_controller[joint]), Ymax, Imax);    
}

extern void eo_emsController_SetTrqPid(uint8_t joint, float kp, float ki, float kd, float Ymax, float Imax)
{
    eo_pid_SetPid(eo_axisController_GetTrqPidPtr(s_emsc->axis_controller[joint]), kp, ki, kd);
    eo_pid_SetPidLimits(eo_axisController_GetTrqPidPtr(s_emsc->axis_controller[joint]), Ymax, Imax); 
}

extern void eo_emsController_SetStiffness(uint8_t joint, float stiffness)
{
    eo_axisController_SetStiffness(s_emsc->axis_controller[joint], stiffness);
}

extern void eo_emsController_SetVelMax(uint8_t joint, float vel_max)
{
    eo_axisController_SetVelMax(s_emsc->axis_controller[joint], vel_max);
}

extern void eo_emsController_SetPosLimits(uint8_t joint, float pos_min, float pos_max)
{
    eo_axisController_SetPosLimits(s_emsc->axis_controller[joint], pos_min, pos_max);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




