#include "kalman.h"

namespace LBR
{

EKF::EKF(const KalmanMatrix& matrix) : matrix{matrix}
{
}

bool EKF::init(const KalmanParams kalmanParams)
{
    params = kalmanParams;
    initialized = true;
    return true;
}

const Eigen::Matrix<double, 3, 1>& EKF::get_state() const
{
    return params.x;
}

void EKF::predict(double dt)
{
    if (!initialized)
        return;

    // State: [position, velocity, acceleration].
    matrix.F(0, 1) = dt;
    matrix.F(0, 2) = dt * dt / 2.0;
    matrix.F(1, 2) = dt;

    // Predict state and covariance.
    params.x_predict = matrix.F * params.x;
    params.P_predict = matrix.F * params.P * matrix.F.transpose() + matrix.Q;
}

void EKF::update(const Eigen::Matrix<double, 2, 1>& z)
{
    if (!initialized)
        return;

    // Measurement: [position, acceleration].
    const Eigen::Matrix<double, 3, 2> K =
        params.P_predict * matrix.H.transpose() *
        (matrix.H * params.P_predict * matrix.H.transpose() + matrix.R)
            .inverse();

    // Updated state and covariance.
    params.x = params.x_predict + K * (z - matrix.H * params.x_predict);

    const Eigen::Matrix<double, 3, 3> I3 =
        Eigen::Matrix<double, 3, 3>::Identity();

    params.P = (I3 - K * matrix.H) * params.P_predict;
}

}  // namespace LBR