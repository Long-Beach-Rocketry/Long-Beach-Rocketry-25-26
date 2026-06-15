                                
 1. Software loop                                              
 - Read IMU for acceleration and orientation 
   + accel, gyro, linear_accel, gravity, quat
 - Read Baro for altitude    
   + ...
                         
 - State machine:                            
 + WAITING - Locked    
 + ASCENT  - PID Active  
 + APOGEE  - Retract (Safety)      
 
                                             Peak Altitude
                                                   _
                        Target Apogee ----->      / \      
                                                 /   \
                                                /     \
                                               /       \     
                                              /         \        
         Waiting Starts        Ascent --->   /           \
 _______________v___________________________/             \
 
 
 ? Not sure if we still need to use Kalman filter to calc
   current velocity to compare with target velocity

 3. Fault tolerance (Safety)
 - Dual Trigger deploy IF (G-Force > X AND Altitude > Y)
 - Servo Watchdog cuts powers to servo motor if it draws too much current
 NOTE : Trust IMU for direction, Baro for height
 
 4. Input :
 - CAN Input for LAUNCH and TARGET APOGEE from Telem board
 - CAN Output send Servo angle and System Health to Telem board
 - PWM Output drives the servo motor
