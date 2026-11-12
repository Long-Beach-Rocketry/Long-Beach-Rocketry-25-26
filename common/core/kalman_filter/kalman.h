#pragma once
#include <Eigen/Dense>

namespace LBR {
class EKF {
public: 
    //Constructor
    //Note: size of state and measurement are same = 3x3
    EKF(
        const Eigen::Matrix<double, 3, 3>& F,
        const Eigen::Matrix<double, 2, 3>& H,
        const Eigen::Matrix<double, 3, 3>& Q,
        const Eigen::Matrix<double, 2, 2>& R
    );

    //Initalize state and covariance vector -- first step
    //@return True if init, False otherwise
    void initialize(const Eigen::Matrix<double, 3, 1>& x0, 
        const Eigen::Matrix<double, 3, 3>& P0);
    
    const Eigen::Matrix<double, 3, 1>& getXhat();  
    //Predict the current state from the previous state (aka x_hat)
    // with the current rotation velocity from gyroscope for quaternion
    void predict(double dt);

    //Update the state agaisnt the newest measurement z 
    // z = [pz, az, qw, qx, qy, qz]
    void update(const Eigen::Matrix<double, 2, 1>& z);
private:
    bool init = false;
    //Vector of 13 states of x hat and x_hat generated from the prediction step 
    Eigen::Matrix<double, 3, 1> x_hat, x_hat_pred;
    Eigen::Matrix<double, 3, 3> P, P_new; //covariance matrix
    Eigen::Matrix<double, 3, 3> F; //state-to-new-state model
    Eigen::Matrix<double, 2, 3> H; //state-to-measure matrix (6x10 dimension)
    Eigen::Matrix<double, 3, 3> Q; //process noise - how much you trust prediction
    Eigen::Matrix<double, 2, 2> R; //measurement noise - how much you trust sensor
};
}//namespace LBR

/**x = [pz, vz, az]
   z = [pz, az]*/