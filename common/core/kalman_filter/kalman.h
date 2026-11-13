#pragma once
#include <Eigen/Dense>

namespace LBR
{

struct KalmanMatrix
{
    Eigen::Matrix<double, 3, 3> F;  //state-to-new-state model
    Eigen::Matrix<double, 2, 3> H;  //state-to-measure matrix (6x10 dimension)
    Eigen::Matrix<double, 3, 3>
        Q;  //process noise - how much you trust prediction
    Eigen::Matrix<double, 2, 2>
        R;  //measurement noise - how much you trust sensor
};

struct KalmanParams
{
    Eigen::Matrix<double, 3, 1> x;  //state vector
    Eigen::Matrix<double, 3, 1> x_predict;
    Eigen::Matrix<double, 3, 3> P;  //covariance matrix
    Eigen::Matrix<double, 3, 3> P_predict;
};

class EKF
{
public:
    //Constructor
    explicit EKF(const KalmanMatrix& matrix);

    //Initalize state and covariance vector -- first step
    //@return True if init, False otherwise
    bool init(const KalmanParams params);

    const Eigen::Matrix<double, 3, 1>& get_state() const;

    //Predict the current state from the previous state (aka x_hat)
    // with the current rotation velocity from gyroscope for quaternion
    void predict(double dt);

    //Update the state agaisnt the newest measurement z
    // z = [pz, az, qw, qx, qy, qz]
    void update(const Eigen::Matrix<double, 2, 1>& z);

private:
    bool initialized{false};

    //params: a structure containing internal computation parameter for Kalman
    KalmanParams params;

    //matrix: essentail matrix for Kalman filter
    KalmanMatrix matrix;
};
}  //namespace LBR

/**x = [pz, vz, az]
   z = [pz, az]*/