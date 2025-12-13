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

// Speed Control Variables
// Target speed: 3.6 feet per second
#define TARGET_SPEED_FTPS   3.6f
// Cheel_radius: 1.25 inches (later, convert to feet by dividing by 12) 
#define WHEEL_RADIUS        1.25f
#define PI                  3.14159265f

// Circumference in feet = (wheel radius in feet) * (2 * pi)
// Circumference of 1/5 of the wheel in feet (because there are 5 magnets
static const float WHEEL_CIRC_FT = (2*PI / 5) * (WHEEL_RADIUS / 12.0);

// Speed difference between the actual speed and target speed
double error;

// Captured time on the previous falling edge of the hall effect
uint32 last_time;

// Captured time on the current falling edge of the hall effect
uint32 curr_time;

// Time difference between last_time and curr_time
double elapsed_cycles;

// Speed of the car in feet per second
double speed;

// Proportional control constant
double kp = 30;

// Integral control constant 
double ki = 5;

// Accumulated error over time
double int_error;

// Base compare value 
double base = 50;

// Adjusted compare value
double pwm;

// Navigation Variables
// Captured time on the falling edge of the camera input
uint32 nav_capture_time;


// Target time elapsed from the start of the current CSYNC row until the sensor detects the black line
double nav_time = 623;

// Actual time elapsed from the start of the current CSYNC row until the sensor detects the black line
double difference_time;

// Time difference between the measured time (difference_time) and the target time (nav_time)
double nav_error = 0;

// Proportional control constant
double kp_nav = 0.4;

// 
double pwm_nav;
double new_nav_time;

// Maximum cycle (timer set to 16 bits)
double max_cycle = 65536;


double conversion_constant = 7.3194;
double prev_nav_error = 0;
double derivative;

// Derivative control constant
double kd_nav = 0.08;

CY_ISR(inter)
{
    // Captures current time (in cycles)
    curr_time = Timer_ReadCapture();
    
    // Time difference between last falling edge and this falling edge of the hall effect
    elapsed_cycles = last_time - curr_time;
    last_time = curr_time;
    
    // Car's current speed in feet per second 
    speed = (WHEEL_CIRC_FT / elapsed_cycles) * 10000;
    
    // Proportional control error
    error = TARGET_SPEED_FTPS - speed;
    
    // Integral control error
    int_error += error;
    
    // Adjusting the compare value using proportional and integral control 
    pwm = base + (kp * error) + (ki * int_error);
    
    // Applying new compare value
    PWM_WriteCompare((uint) pwm);
    
    // Clears the timer interrupt flag
    Timer_ReadStatusRegister();
}

CY_ISR(navigation)
{
    // Captures current time (in cycles)
    nav_capture_time = Timer_Nav_ReadCapture();
    
    // Time elapsed from the start of the current CSYNC row until the sensor detects the black line
    difference_time = max_cycle - nav_capture_time;
    prev_nav_error = nav_error;
    
    // Proportional control
    nav_error = difference_time - nav_time;
   
    // Derivative control 
    derivative = (nav_error - prev_nav_error) / difference_time;
    
    // Converts timing output to a compare value for steering servo.
    new_nav_time = nav_time + (kp_nav * nav_error) + (kd_nav * derivative);
    pwm_nav = conversion_constant * new_nav_time;
    
    // Applying new compare value
    PWM_1_WriteCompare((uint) pwm_nav);
    
    // Writing the live error values to the UART
    char buffer[64];
    sprintf(buffer, "%d\r\n", (int) nav_error);
    UART_PutString(buffer);
    
    // Clears the timer interrupt flag
    Timer_Nav_ReadStatusRegister();
    
}


int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
   
    /* Initializations */
    UART_Start();
    
    // For speed control
    PWM_Start();
    Timer_Start();
    
    // For navigation
    Timer_Nav_Start();
    VDAC8_1_Start();
    Comp_1_Start();
    PWM_1_Start();
    
    // To navigation interrupt
    NAV_Interrupt_Start();
    NAV_Interrupt_SetVector(navigation);
    
    // To hall effect interrupt
    HE_Interrupt_Start();
    HE_Interrupt_SetVector(inter);
   
   
    for(;;)
    {
    }
}

/* [] END OF FILE */
