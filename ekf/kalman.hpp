#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>
#include <fstream>
#include <Eigen/Dense>

//discrete 
class KF{
    public:
    /*
    creating matrices to represent variables:
    Q (process noise covariance matrix) (larger Q trusts sensor more because noise is unpredictable,
    while smaller Q trusts the model more) (larger time step, larger Q) (uncertainty in model)
    R (measurement noise covariance matrix) (how much you trust the sensor) (if large, rely on prediction) (uncertainty in measurement)
    H (selecting/combining part of state to form measurement)
    F 
    P (state covariance matrix) (confidence of state after seeing measurement/ should decrease
    over time)
    K (kalman gain) (smaller if R is large)
    z (The actual measurement vector from sensors at a specific time step)
    */
    

    KF(
        //function parameters (1D filter)
            double dt,
            const Eigen::DiagonalMatrix<double, 13>& Q_, //uncertainty of estimate (x)
            const Eigen::DiagonalMatrix<double, 5>& R_, //how much you trust sensor (std^2) <-- inverse
            // - diagonal: uncertainty for each state ; offdiagonal: correlation between errors in each measurement 
            const Eigen::Matrix<double, 5,13>& H_, //observation vector
            const Eigen::Matrix<double, 13,13>& F_, //state transition matrix (encodes how much state evolves over one time step)
            const Eigen::Matrix<double, 13,13>& P_ //state covariance matrix (uncertainty of state vector) 
            // - diagonal: variance of state ; offdiagonal: covariances between states (uncertain = large, certain = small)
            //state matrix = x: transpose([px,py,pz,vx,vy,vz,qx,qy,qz,qw,bx,by,bz])
    
    );

   // KF(); //default constructor
    Eigen::Matrix<double,13,1> get_xhat() const{ return x_hat; } //to access to private variable: x_hat
    //void init(); //create the function for init

    void init(double t1, const Eigen::Matrix<double, 13,1>& x0,const Eigen::Matrix<double, 13,13>& P0); //x is the estimate vector & prevents it from copying full matrix 
    //telling filter where to start
    
    //declaring predict and update
    void predict(const Eigen::Vector3d& gyromeas);
    void update(const Eigen::Matrix<double,5,1>& z);
    
    private:
        double dt;
        //Q,R,H,F,P,K;
        int m,n; //system dimensions
        double t0, t; //initial and current time
        Eigen::Matrix<double,13,13> I; //identity matrix
        Eigen::Matrix<double,13,1> x_hat, x_hat_new; 
        Eigen::Matrix<double, 13,13> P_new; //update values
        bool initialized; //to verify if the data has been initialized
        
        Eigen::DiagonalMatrix<double,13> Q; //uncertainty of estimate (x)
        Eigen::DiagonalMatrix<double, 5> R; //how much you trust sensor (std^2) <-- inverse 
        Eigen::Matrix<double, 5,13> H;
        Eigen::Matrix<double, 13,13> F;
        Eigen::Matrix<double,13, 13> P;
        Eigen::Matrix<double 13,5>K;
};
//values


//predict
//update


/*
SENSOR MAPPING
px,py,pz = position 
vx,vy,vz = velocity
qx,qy,qz,qw = orientation (quaternion) --> vector qx,qy,qz stores rotation axis and qw stores rotation amount; uses half-angle; qw at no rotation is 1
bx,by,bz = gyro bias
Barometer - pressure (altitude) -> PZ
Accelerometer - linear acceleration -> prediction to update vx,vy,vz,px,py,pz
Gyroscope - angular velocity -> prediction for qx,qy,qz,qw
Quaternion - orientation -> update to correct orientation of qx,qy,qz,qw (qw is scalar)
*/
