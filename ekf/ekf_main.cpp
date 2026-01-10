#include <iostream>
#include <Eigen/Dense>
#include "kalman.hpp"
#include <vector>
#include "board.h"
using namespace std;
 int main(){
  bsp_init() ;

   LBR::Board& hw = LBR::get_board();

  cout<<"main "<<endl;
  double dt = 0.01;
    Eigen::DiagonalMatrix<double,13> Q, P, P0; //diagonalMatrix makes all non diagonal values 0
    Eigen::DiagonalMatrix<double, 5> R; // [pz,qx,qy,qz,qw]
    // Q,R,H,F,P,P0;
    Eigen::Matrix<double,13,1> x0;
    Eigen::Matrix<double,13,13> F;
    Eigen::Matrix<double,5,13> H = Eigen::Matrix<double,5,13>::Zero(); //set values to 0 to change later
    Eigen::Matrix<double,5,1> z;

    /*1.*/
    Q.diagonal()<<6.75e-5*pow(dt,4), 6.75e-5*pow(dt,4),6.75e-5*pow(dt,4), 
        1.35e-4*pow(dt,2),1.35e-4*pow(dt,2),1.35e-4*pow(dt,2), 
        7.5e-7*pow(dt,2)/4, 7.5e-7*pow(dt,2)/4, 7.5e-7*pow(dt,2)/4, 7.5e-7*pow(dt,2)/4,
        3.0e-6, 3.0e-6, 3.0e-6; //13x13 where left->right diagonal is variances
    /*2.*/
    R.diagonal()<<0.0056,
                  7.5e-7, 7.5e-7, 7.5e-7, 7.5e-7; //if you trust the sensor more, R is smaller such as R<<0.1;  
    /*3.*/
    H(0,2)=1.0; //observation matrix (number of measurements)*(number of states) -> 5x13
    H(1,6)=1.0;
    H(2,7)=1.0;
    H(3,8)=1.0;
    H(4,9)=1.0; //each value corresponds to row and column of the same measurement and state
    /*4.*/
    F.setZero(); //set all values initially to 0
    for (int i=0; i<13; i++){
      F(i,i) = 1.0;
    }
    F(0,3) = dt;
    F(1,4) = dt;
    F(2,5) = dt;
    /*F(3,0) = dt;
    F(4,1) = dt;
    F(5,2) = dt;*/
    /*5.*/
    P0.diagonal()<<1.0, 1.0, 1.0,
                   0.01, 0.01, 0.01,
                   0.0025,0.0025,0.0025,1e-6,
                   1e-4,1e-4,1e-4; //initial covariance
    /*6.*/
    P=P0; 
    x0.setZero();
    x0(9) = 1; //set qw to 1 because cos(0/2)=1
    /*7.*/
    

   KF kfilter(0.01, Q,R,H,F,P); //calling on the KF class and naming the object kfilter
   Bno055Data out; //getting sensor values from imu
   Bmp390 data; // different board structs
   kfilter.init(0,x0,P0);
   while(true){ //loops for a while to allow all values of the sensor to be entered
   
   double gx,gy,gz; //gyro bias
   hw.imu.read_all(out);
   gx = out.gyro.x;
   gy = out.gyro.y;
   gz = out.gyro.z;

   double qx,qy,qz,qw; //quaternion
   qw = out.quat.w;
   qx = out.quat.x;
   qy = out.quat.y;
   qz = out.quat.z;

   double baro_meas = hw.Bmp390.get_pressure();
    //parameters: 0.01, Q,R,H,F,P for kfilter
   //reading values from quat sensor (page 51/105)
   Eigen::Vector3d gyro;
   gyro<< gx,gy,gz;

   Eigen::Vector3d quats;
   quats<<qx,qy,qz,qw;
       /*8. reading baro measurements*/
    z(0) = baro_meas;
    z(1) = qx;
    z(2) = qy;
    z(3) = qz;
    z(4) = qw; //bex put it as w,x,y,z 
   

  

    kfilter.predict(gyro);
    kfilter.update(z);
    cout<<"Filter Step: "<<kfilter.get_xhat()(0,0)<<endl;
   
   
   cout<<"Filtered: "<<kfilter.get_xhat()(0,0)<<endl; //(90,0) because it's a matrix
   
   }
  }
 //g++ main.cpp kalman.cpp -I /mnt/c/Users/pho20/Documents/EigenLib -o kalman.exe
 //./kalman.exe

 /*1 obtaining noise values for Q:
accelerometer:
- convert from ug to m/s^2 (noise density = 150 ug/sqrt(hz) -> 0.00147 m/s^2 / sqrt(hz) )
- convert to RMS noise : ^ * sqrt(bandwidth = sampling rate/2) ->  0.0116213704
- convert to variance: (RMS noise)^2 -> 1.35*10^(-4)
- this is for VELOCITY Q : ^*dt^(2) (assume time step is 0.01 for now) -> 1.35*10^(-8)
- this is for POSITION Q : 2 * 1.35*10^(-4) * dt^(4) -> 6.75e-5*dt^(-4)
GYRO -> Quaternion:
- convert from degrees to radians : 0.1 deg/s * pi/180 -> 0.001745 rad/s
- convert to variance: (^)^2 - > 3.0*10^(-6)
- this is for QUATERNION Q : ^ * (dt^(2)/4) -> 7.5*10^(-11)

2. obtaining values for R - what is directly measured: Barometer for altitude and IMU for quaternion
PRESSURE -> Altitude:
- only affects pz
- RMS value of noise = 0.9 -> (0.9)^2
- altitude from barometer: h = 44330 * (1-(P/Po)^(1/5.255)) 
- altitude from barometer : P = P0*e^(-gh/RT) or altitude variance = deltaP/(rho*g) -> 0.9/(1.225*9.81) = 0.075 m
- convert to variance: (^)^(2) = 0.0056

3. obtaining values for H
    0	   1	 2	 3	 4	 5 	 6	 7	 8	 9	10	11	12
    px	py	pz	vx	vy	vz	qx	qy	qz	qw	bx	by	bz
0 pz         1
1 qx                         1
2 qy                             1
3 qz                                 1
4 qw                                     1

4. obtaining values for F
- evolution of position per time step
- position = position + velocity*dt
- writing Fq = 
- block layout : how group of states affect another group of states
      0	   1	 2	 3	 4	 5 	 6	 7	 8	 9	10	11	12
      px	py	pz	vx	vy	vz	qx	qy	qz	qw	bx	by	bz
0 px  1           dt
1 py      1           dt
2 pz          1           dt
3 vx  dt          1
4 vy      dt          1
5 vz          dt          1
6 qx                          1                       
7 qy                              1  
8 qz                                  1  
9 qw                                      1
10bx                                          1
11by                                              1
12bz                                                  1
+ position is constant (stays 1)
+ velocity affects position (multiply by dt)
+ position doesn't affect velocity
+ velocity is constant
+ quaternion is constant
+ gyro bias affects quaternion (ignore rn)
+ gyro bias is constant

5. obtaining values for initial covariance P0
- uncertain about uncertainty, so make assumptions with larger values being better
- position: assume 1 m variance -> 1  <- variance^2
- velocity: assume 0.1 m/s variance -> 0.01
- quaternion: assume 0.05 rad -> 0.0025
- angular quaternion (qw) : -> 1e-6
- gyro biases: assume 0.01 rad/s -> 1e-4

6. obtaining values for P
- P is dynamic since it changes based on state
- P follows the initial value at first

7. reading sensor values

8. obtaining values for z (5x1)
- measurement: pz,qx,qy,qz,qw
- using measurements from the sensors (baro_alt, imuquat.x(), imuquat.y(), imuquat.z(), imuquat.w()) --> taken in from update()

*/