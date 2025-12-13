/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include <stdio.h>
#include <math.h>

#define TARGET_SPEED_FTPS   4.0f
/* in inches (later, convert to feet by dividing by 12) */
#define WHEEL_RADIUS        1.25f
#define MAGNET_COUNT        5
#define PI                  3.14159265f

// circumference in feet = (wheel radius in feet) * (2 * pi)
static const float WHEEL_CIRC_FT = (2*PI / 5) * (WHEEL_RADIUS / 12.0);
static const float MAX_ERROR = 100;
static const float MIN_ERROR = -100;

double error;
uint32 elapsed_time;
uint32 last_time;
uint32 curr_time;
double elapsed_cycles;
double speed;
double kp = 30;
double ki = 5;
double int_error;
double base = 30;
double pwm;

// Duty cycle:
char strbuf[16];

CY_ISR(inter)
{
    curr_time = Timer_ReadCapture();
    elapsed_cycles = last_time - curr_time;
    last_time = curr_time;
    speed = (WHEEL_CIRC_FT / elapsed_cycles) * 10000;
    
    error = TARGET_SPEED_FTPS - speed;
    int_error += error;

    pwm = base + (kp * error) + (ki * int_error);
    if (pwm <= 0) {
        pwm = 20;
    }

    if (error > MAX_ERROR) {
        PWM_WriteCompare(200);
    }
    else if (error < MIN_ERROR) {
        PWM_WriteCompare(20);
    }
    else {
        PWM_WriteCompare((uint) pwm);
    }

    sprintf(strbuf, "Speed: %3.0f", speed);
    LCD_Position(0,0);
    LCD_PrintString(strbuf);
 
    char buffer[64];

    sprintf(buffer, "%d,%d,%d,%d,%d,%d\r\n", 
        (int)(speed*100), (int)(error*100), (int)(int_error*100), (int)(pwm*100), (int)(elapsed_cycles), (int)(curr_time));
    UART_PutString(buffer);
    
    Timer_ReadStatusRegister();
    
    
}


int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
   
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    LCD_Start();
    PWM_Start();
    UART_Start();
   
    Timer_Start();
    
    HE_Interrupt_Start();
    HE_Interrupt_SetVector(inter);
   
   
    for(;;)
    {
        /* Place your application code here. */
        //UART_PutString("Hello from PSoC\r\n");
    }
}

/* [] END OF FILE */
