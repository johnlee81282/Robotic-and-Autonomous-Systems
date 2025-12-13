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

// Pixy pan range: usually 0–1000; we found that our center is 400
#define PAN_CENTER 400

// Steering
#define SERVO_CENTER 4560 // Straight
#define SERVO_MIN 3000 // Left
#define SERVO_MAX 5990 // Right; we noticed that 6000 did not work as expected

// This affects how strongly pan error affects steering
#define STEER_GAIN 10 // adjust this whenever we change speed (higher speed --> lower steer gain)

// Buffer that stores information received from Arduino through I2C
/* 0: object seen or not, 1-2: pan when focused on ball, 3-4: tilt when focused on ball
5-6: pan when focused on goal, 7-8: tilt when focused on goal, 9: ball shot or not

From now on: part 1 is when the car is focused on the ball and part 2 is when the car is focused on the goal*/
uint8 i2cBuffer[10];          

volatile uint8 ballVisible = 0;   
volatile int16 panCmd = 0;   
volatile int16 tiltCmd = 0;   
volatile int16 panCmd2 = 0;   
volatile int16 tiltCmd2 = 0;
volatile uint8 shot = 0;


int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
   
    /* Initializations */
    UART_Start();
    
    // Start I2C slave
    EZI2C_1_Start();
    EZI2C_1_SetBuffer1(sizeof(i2cBuffer), sizeof(i2cBuffer), i2cBuffer);
    
    // For speed
    PWM_Start();
    Timer_Start();
    
    // For navigation
    PWM_1_Start();   
   
    for(;;)
    {
        // Description for each variable here described above in i2c buffer
        ballVisible = i2cBuffer[0]; 
        CyDelay(100); // 0.1 sec delay
        panCmd  = (int16)(( (int16)i2cBuffer[1] << 8 ) | i2cBuffer[2]);
        tiltCmd = (int16)(( (int16)i2cBuffer[3] << 8 ) | i2cBuffer[4]);
        panCmd2  = (int16)(( (int16)i2cBuffer[5] << 8 ) | i2cBuffer[6]);
        tiltCmd2 = (int16)(( (int16)i2cBuffer[7] << 8 ) | i2cBuffer[8]);
        shot = i2cBuffer[9];
        
        // UART
        char buf[32];
        sprintf(buf, "%d %d %d %d %d %d\r\n", (int) ballVisible, (int) panCmd, (int) tiltCmd, (int) panCmd2, (int) tiltCmd2, (int) shot);
        UART_PutString(buf);
        
        // If ball is shot, do a celebration
        if (shot) 
        {
            PWM_WriteCompare2(0); // For moving forward
            PWM_WriteCompare1(0); // For moving backward
            CyDelay(1000);
            PWM_WriteCompare1(80);
            PWM_1_WriteCompare(3000);
            CyDelay(1000);
            PWM_1_WriteCompare(5990);
            CyDelay(1000);
            PWM_1_WriteCompare(3000);
            CyDelay(1000);
            PWM_1_WriteCompare(5990);
            CyDelay(1000);
        }
        
        // Part 1: if the ball is detected, go towards the ball
        else if(ballVisible && !panCmd2 && !tiltCmd2)
        {
            PWM_WriteCompare2(60);
            PWM_WriteCompare1(0);
            
            // How far the pan is from center
            double panError = (double)panCmd - (double)PAN_CENTER;

            // Proportional control
            double compare = SERVO_CENTER - STEER_GAIN * panError;

            // Edge cases 
            if (compare < SERVO_MIN) compare = SERVO_MIN;
            if (compare > SERVO_MAX) compare = SERVO_MAX;
            

            PWM_1_WriteCompare((uint16)compare);
        }
        
        // Part 1: if the ball is not detected, search for the ball (rotate moving back with the wheel facing left)
        else if(!ballVisible && !panCmd2 && !tiltCmd2)
        {
            PWM_1_WriteCompare(3000);
            PWM_WriteCompare1(60);
            PWM_WriteCompare2(0);
        }
        
        // Part 2: if the goal is visible, go towards the goal 
        else if(ballVisible && !panCmd && !tiltCmd)
        {
            // Same code as part 1 when the ball is detected
           
            PWM_WriteCompare2(60);
            PWM_WriteCompare1(0);
            
            double panError = (double)panCmd2 - (double)PAN_CENTER;

            double compare = SERVO_CENTER - STEER_GAIN * panError;

            if (compare < SERVO_MIN) compare = SERVO_MIN;
            if (compare > SERVO_MAX) compare = SERVO_MAX;
            
            PWM_1_WriteCompare((uint16)compare);
        }
        
        // Part 2: if the goal is not visible, search for the goal (rotate moving forward with the wheel facing left)
        else 
        {
            PWM_1_WriteCompare(3000);
            PWM_WriteCompare2(50); // Speed is slowed down as the pixy cam did not detect the goal as well
            PWM_WriteCompare1(0);
        }
    }
}

/* [] END OF FILE */
