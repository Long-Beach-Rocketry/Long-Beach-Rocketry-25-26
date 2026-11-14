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
    /**
     * @brief Constructs the filter with the specified model matrices.
     * @param matrix State transition, measurement, and noise matrices.
     */
    explicit EKF(const KalmanMatrix& matrix);

    /**
     * @brief Initializes the filter state and covariance.
     * @param params Initial state and covariance parameters.
     * @return true if initialization succeeds, false otherwise.
     */
    bool init(const KalmanParams params);

    /**
     * @brief Returns the current estimated state.
     * @return Constant reference to the state vector
     *         [vertical position (m), vertical velocity (m/s),
     *          vertical acceleration (m/s²)].
     */
    const Eigen::Matrix<double, 3, 1>& get_state() const;

    /**
     * @brief Predicts the state and covariance at the next sample time.
     * @param dt Elapsed time since the previous sample, in seconds.
    * @return true if initialization has happened before this step, false otherwise.
     */
    bool predict(double dt);

    /**
     * @brief Corrects the predicted state and covariance using measurements.
     * @param z Measurement vector containing
     *              [vertical position (m), vertical acceleration (m/s²)].
    * @return true if initialization has happened before this step, false otherwise.
     */
    bool update(const Eigen::Matrix<double, 2, 1>& z);

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