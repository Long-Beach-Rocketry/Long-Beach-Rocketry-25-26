                                
## Software loop                                              
- Read IMU for acceleration and orientation 
  - accel, gyro, linear_accel, gravity, quat
- Read Baro for altitude    

                         
## State machine:                            
1. PRELAUNCH - Not launched yet
2. LAUNCHED - 6 second interval from launch
3. AIRBRAKES_DEPLOYING - Deploy airbrakes after 6 seconds
4. AIRBRAKES_FULLY_DEPLOYED - When at 30 degrees; not sure when to know though
5. AIRBRAKES_RETRACTING - Should do before target apogee
6. AIRBRAKES_FULLY_RETRACTED - Again, dont know how to know if retracted
7. RECOVERY_POPPED - Job finished     



## Fault tolerance (Safety)
 - Dual Trigger deploy IF (G-Force > X AND Altitude > Y)
 - Servo Watchdog cuts powers to servo motor if it draws too much current
 - Use time to hardcode transition states and servo motor angles if sensors fail?
**NOTE** : Trust IMU for direction, Baro for height
 
