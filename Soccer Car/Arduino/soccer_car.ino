//
// begin license header
//
// This file is part of Pixy CMUcam5 or "Pixy" for short
//
// All Pixy source code is provided under the terms of the
// GNU General Public License v2 (http://www.gnu.org/licenses/gpl-2.0.html).
// Those wishing to use Pixy source code, software and/or
// technologies under different licensing terms should contact us at
// cmucam@cs.cmu.edu. Such licensing terms are available for
// all portions of the Pixy codebase presented here.
//
// end license header
//

#include <Pixy2.h>
#include <PIDLoop.h>
#include <Wire.h>
#include <Servo.h>

Pixy2 pixy; 
// Adjust Proportional, Integral, and Derivative controls to keep the pan and tilt stable
PIDLoop panLoop(300, 100, 500, true);
PIDLoop tiltLoop(500, 0, 500, true);

int panPos = 400; // Center value for our pan
int panDir = 1; // Direction the pan rotates in
int panMin = 200; // Minimum position that the pan moves when searching
int panMax = 600; // Maximum position that the pan moves when searching

// Variables for oscillating the pan when object not detected
unsigned long lastMove = 0; 
int stepDelay = 5;
int stepSize = 5; 

Servo servo1; // Shooting mechanism
Servo servo2; // Gate that captures the ball and prevents it from escaping
bool s = false; // Shot or not

unsigned long lastTimeTiltHigh = 0; // Time captured when tilt is facing down (ball is captured)
int k = 1; // k=1 when part 1 and k=2 when part 2


const byte PSOC_ADDR = 0x08; // Match with slave address

// Helper to send data to PSoC over I2C
void sendToPsoc(bool hasObject, int16_t panCmd, int16_t tiltCmd, int16_t panCmd2, int16_t tiltCmd2, bool shoot) {
  Wire.beginTransmission(PSOC_ADDR);

  Wire.write((uint8_t)0); // Dummy because in our case, the PSoC started reading from the 2nd byte
  
  // Byte 0: ball detected or not
  Wire.write(hasObject ? 1 : 0);

  // Byte 1–2: pan position for part 1
  Wire.write((panCmd >> 8) & 0xFF);
  Wire.write(panCmd & 0xFF);

  // Byte 3–4: tilt position for part 1
  Wire.write((tiltCmd >> 8) & 0xFF);
  Wire.write(tiltCmd & 0xFF);
  
  // byte 5-6: pan position for part 2
  Wire.write((panCmd2 >> 8) & 0xFF);
  Wire.write(panCmd2 & 0xFF);

  // byte 7-8: tilt position for part 2
  Wire.write((tiltCmd2 >> 8) & 0xFF);
  Wire.write(tiltCmd2 & 0xFF);

  // byte 9: ball shot or not
  Wire.write(shoot ? 1 : 0);

  Wire.endTransmission();
}

void setup()
{
  Serial.begin(115200);
  Wire.begin(); 
  pixy.init();
  pixy.changeProg("color_connected_components");
  pixy.setLamp(1,1);

  servo1.attach(9);
  servo1.write(0);

  servo2.attach(3);
  servo2.write(120);
}

void loop()
{  
  // Part 1: capturing the ball
  if (k == 1) {
    int32_t panOffset, tiltOffset;
    
    // Get the ball detected from pixy
    pixy.ccc.getBlocks(true, CCC_SIG1);
    
    // Pixy cam detects the ball
    if (pixy.ccc.numBlocks)
    {        
      
      // Calculate pan and tilt "errors" with respect to first object (blocks[0]), 
      // which is the biggest object (they are sorted by size).  
      panOffset = (int32_t)pixy.frameWidth/2 - (int32_t)pixy.ccc.blocks[0].m_x;
      tiltOffset = (int32_t)pixy.ccc.blocks[0].m_y - (int32_t)pixy.frameHeight/2;  
    
      // Update loops
      panLoop.update(panOffset);
      tiltLoop.update(tiltOffset);
    
      // Set pan and tilt servos  
      pixy.setServos(panLoop.m_command, tiltLoop.m_command);

      // Send information to psoc
      sendToPsoc(true, (int16_t)panLoop.m_command, (int16_t)tiltLoop.m_command, 0,0,s);
      
      // If the tilt is facing down enough, close the gate to capture the ball and move onto the second part
      if (tiltLoop.m_command > 730) {
        // If first time entering this condition, capture the time
        if (lastTimeTiltHigh == 0) {
          lastTimeTiltHigh = millis();
        }

        // If it has been above 730 for over 0.2 second, close the gate
        if (millis() - lastTimeTiltHigh >= 200) {
          k=2;
          lastTimeTiltHigh = 0;
          servo2.write(60);
        }
      }
      else {
        // Reset timer when tilt is not facing down enough (the ball is not in our possession)
        lastTimeTiltHigh = 0;
      }

    }  
    // Ball is not detected
    else 
    {
      panLoop.reset();
      tiltLoop.reset();
      // Oscillate the pan (searching for the ball)
      if (millis() - lastMove > stepDelay)
      {
          lastMove = millis();
          panPos += panDir * stepSize;

          // Reverse direction at edges
          if (panPos >= panMax || panPos <= panMin)
              panDir *= -1;
      }

      // Tilt stays centered
      int tiltPos = 200;

      pixy.setServos(panPos, tiltPos);

      // Send information to psoc
      sendToPsoc(false, (int16_t)panLoop.m_command, (int16_t)tiltLoop.m_command, 0, 0,s);
    }
  }



  // Part 2: scoring the goal
  if (k == 2) {
    int32_t panOffset, tiltOffset;

    // Get the goal detected from pixy
    pixy.ccc.getBlocks(true, CCC_SIG2);

    // The goal is detected
    if (pixy.ccc.numBlocks)
    {
      
      // calculate pan and tilt "errors" with respect to first object (blocks[0]), 
      // which is the biggest object (they are sorted by size).  
      panOffset = (int32_t)pixy.frameWidth/2 - (int32_t)pixy.ccc.blocks[0].m_x;
      tiltOffset = (int32_t)pixy.ccc.blocks[0].m_y - (int32_t)pixy.frameHeight/2;  
    
      // update loops
      panLoop.update(panOffset);
      tiltLoop.update(tiltOffset);
    
      // set pan and tilt servos  
      pixy.setServos(panLoop.m_command, tiltLoop.m_command);

      // If the car is close enough to the ball, open the gate, and then once the gate fully opens, shoot the ball
      if (pixy.ccc.blocks[0].m_height > 60 && pixy.ccc.blocks[0].m_width > 180) {
        s = true;
        servo2.write(120);
        delay(500);
        servo1.write(120);
        delay(500);
      }

      // Send information to psoc
      sendToPsoc(true, 0, 0, (int16_t)panLoop.m_command, (int16_t)tiltLoop.m_command, s);
    } 
    // Goal is not detected
    else 
    {
      panLoop.reset();
      tiltLoop.reset();
      // Oscillate the pan (searching for the goal)
      if (millis() - lastMove > stepDelay)
      {
          lastMove = millis();
          panPos += panDir * stepSize;

          if (panPos >= panMax || panPos <= panMin)
              panDir *= -1;
      }

      // Tilt stays centered
      int tiltPos = 200;

      pixy.setServos(panPos, tiltPos);
      
      // Send information to psoc
      sendToPsoc(false, 0, 0, (int16_t)panLoop.m_command, (int16_t)tiltLoop.m_command,s);

      servo1.write(0);
    }

  }
}


