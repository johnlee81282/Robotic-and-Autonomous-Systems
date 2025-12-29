# Robotic-and-Autonomous-Systems
Three Projects: (i) Speed Control (ii) Navigation (iii) Autonomous Soccer Car

## Overview
This repository contains the full implementation, control logic, and evaluation criteria
for three robotic and autonomous systems projects. Each project focuses on a different
core robotics challenge, including feedback control, high-speed navigation, and
autonomous perception and decision-making. Detailed documentation for each project is provided in the
corresponding PDF report:

- `Project1_Speed_Control_Report.pdf`
- `Project2_Navigation_Report.pdf`
- `Project3_Autonomous_Soccer_Car_Report.pdf`

## Project 1: Speed Control
Our car must travel along: (i) a straight path, (ii) uphill, and (iii) downhill
at approximately 4 ft/sec. We allow a 2% margin of error for the straight path and 10% for
uphill/downhill parts. Our time needs to be within [9.8, 10.2] seconds for the straight path (40 ft) and [8.1, 9.9] seconds
for both uphill and downhill sections (36 ft).

## Project 2: Navigation
Our car must go as fast as it can around a track (~240 ft with curves). Our car should complete two laps around the track in
under 60 seconds.

## Project 3: Autonomous Soccer Car
Our car must search for and capture the target ball. Then, it must find the goal and shoot the ball after it finds the optimal distance from the goal for shooting.
