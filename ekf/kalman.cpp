/** 
* @author Phoebe Ho
* @date 11/20/2025
 **/

 #include <iostream>
 #include <Eigen/Dense>
 #include "kalman.hpp"
 
 using namespace std;
 KF::KF(
    double dt_,
            const Eigen::DiagonalMatrix<double, 13>& Q_, //uncertainty of estimate (x)
            const Eigen::DiagonalMatrix<double, 5>& R_, //how much you trust sensor (std^2) <-- inverse 
            const Eigen::Matrix<double, 5,13>& H_,
            const Eigen::Matrix<double, 13,13>& F_,
            const Eigen::DiagonalMatrix<double, 13>& P_
 )
 {
    dt = dt_;
    Q = Q_;
    R = R_;
    H = H_;
    F = F_;
    P = P_;

    n=P.rows(); //use for higher dimensions
    m =R.rows(); //use for higher dimensions
    //x_hat.resize(1,1);
    x_hat.setZero() ;
    x_hat(9) = 1.0;
    //x_hat_new.resize(1,1);
    x_hat_new.setZero();
    x_hat_new(9) = 1.0;
    //I.resize(1,1);
    I.setIdentity(); //sets all diagonal values to be 1 and all off diagonal values to be 0
      t0 = 0.0;
      t = 0.0;
    initialized = false;

 }

 void KF::init(double t1, const Eigen::Matrix<double, 13,1>& x0, const Eigen::Matrix<double, 13,13>& P0){
    x_hat = x0;
    x_hat_new = x0;
    t0 = t1;
    P = P0;     
    P_new = P0;
    initialized = true;
 }

 
 void KF::predict(const Eigen::Vector3d& gyromeas){
   if (!initialized) return;
   //when accessing matrix elements, use [][]
   Eigen::Vector3d omega = gyromeas - x_hat.segment<3>(10); //subtract gyro bias to prevent drift over time and prevent error
   Eigen::Matrix<double, 4,4> w;
   double wx, wy, wz;
   wx = omega(0);
   wy = omega(1);
   wz = omega(2);
   w<<0, -wx, -wy, -wz,
         wx,  0,   wz, -wy,
         wy, -wz,   0,   wx,
         wz,  wy, -wx,  0 ;
   Eigen::Vector4d q;
   /*2.*/
   q = x_hat.segment(6,4); 
   q += (0.5)*w*q*dt;
   
   
   /*3.*/
   q.normalize(); //normalize q
    x_hat_new = F*x_hat;
    x_hat_new.segment<4>(6) = q; //length is 4 starting from 6
   
    //uncertainty:
    P_new = F*P*F.transpose()+Q; //functions can access private variables from the same class
    //cout<<"Predict "<<" "<<x_hat_new<<" " <<P_new<<endl;
 }
 void KF::update(const Eigen::Matrix<double,5,1>& z){
   if (!initialized) return;
    //kalman gain 
    //for (int i = 0; i<n; i++){
    K = P_new*H.transpose()*(H*P_new*H.transpose()+R).inverse(); //kalman gain (matrix)
    //update estimate with measurement
    x_hat = x_hat_new+K*(z-H*x_hat_new);
    //update estimate uncertainty
    P = (I-K*H)*P_new; //*(I.coeff(0,0)-K.coeff(0,0)*H.coeff(0,0)).transpose()+K.coeff(0,0)*R.coeff(0,0)*K.transpose();
    //P = P_new; //update prior covariance
    //cout<<"Filterworking "<<x_hat(0,0)<<" "<<P_new(0,0)<<endl;
    }
 

    //NORMALIZE QUATERNION...

/*1. Quaternion
standard kinematic equation: q(dot on top)=(1/2)w(arrow on top)(multiplying quaternion)q
w(arrow on top) = [x,y,z,w]^T
q = [0 -wx -wy -wz
     wx 0   wz -wy
     wy -wz  0  wx
     wz wy -wx  0 ]

2. Updating values for x_hat
- use .segment(index,length)

3. Normalize Quaternion
U = q/||q||
or can be expressed as
U = w/d + (x/d)i + (y/d)j + (z/d)k
d is the norm : ((w^2)+(x^2)+(y^2)+(z^2))^(0.5)
*/