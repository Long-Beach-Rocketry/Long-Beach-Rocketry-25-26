#include <vector>
#include <Eigen/Dense?

namespace LBR {
class EKF {
public: 
    //Constructor
    EKF(
        double dt, 
        const Eigen::Matrix<double, 10, 10>& P,
        const Eigen::Matrix<double, 10, 10>& F,
        const Eigen::Matrix<double, 6, 10>& H,
        const Eigen::Matrix<double, 10, 10>& Q,
        const Eigen::Matrix<double, 6, 6>& R
    );

    //Initalize state and covariance vector -- first step
    //@return True if init, False otherwise
    bool init(const Eigen::Matrix<10, 1>& x0, 
        const Eigen::Matrix<double, 10, 10>& P0);
    
    const Eigen::Matrix Vector13d& getXhat();  
    //Predict the current state from the previous state (aka x_hat)
    // with the current rotation velocity from gyroscope for quaternion
    void predict(const Eigen::Vector3d& gyromeasure);

    //Update the state agaisnt the newest measurement z 
    // z = [pz, az, qw, qx, qy, qz]
    void update(const Eigen::Matrix<double, 6, 1>& z);
private:
    double dt; //Time-step
    bool init = false;
    Eigen::Matrix<double, 10, 10> I //identity matrix of 13x13
    //Vector of 13 states of x hat and x_hat generated from the prediction step 
    Eigen::Matrix<double, 10, 1> x_hat, x_hat_pred;
    Eigen::Matrix<double, 10, 10> P, P_new; //covariance matrix
    Eigen::Matrix<double, 10, 10> F; //state-to-new-state model
    Eigen::Matrix<double, 6, 10> H; //state-to-measure matrix (6x10 dimension)
    Eigen::Matrix<double, 10, 10> Q; //process noise
    Eigen::Matrix<double, 6, 6> R; //measurement noise
};
}//namespace LBR

/**x = [pz, vz, az, qw, qx, qy, qz, bx, by, bz] 
   z = [pz, az, qw, qx, qy, qz]*/