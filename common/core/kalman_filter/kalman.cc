#include "kalman.h"
#include "Eigen/Dense"

using namespace Eigen;
using namespace std;

namespace LBR 
{
//Construct system model matrix
//Internal computation component will be init in init()
EKF::EKF(double newDt, 
        const Eigen::Matrix<double, 10, 10>& newF,
        const Eigen::Matrix<double, 6, 10>& newH,
        const Eigen::Matrix<double, 10, 10>& newQ,
        const Eigen::Matrix<double, 6, 6>& newR
    ) : dt(newDt), F(newF), H(newH), Q(newQ), R(newR) {}

Eigen::Matrix Vector10d& EKF::getXhat const(){
    return x_hat;
};


void EKF::init(const Matrix<double, 10, 1>& x0, 
        const Matrix<double, 10, 10>& P0){
    x_hat = x0;
    x_hat_pred = x0;
    P = P0;
    P_new = P0;
    init = true;
}

void EKF::predict(const Vector3d& gyromeasure){
    if (!init) return; //Check whether x and P has been initliazed

    //Angular velocity minus the gyroscope bias 
    Vector3d omega = gyromeasure - x_hat.segment<3>(7);
    double wx, wy, wz; 
    wx = omega(0);
    wy = omega(1);
    wz = omega(2);
    //Define a matrix that transform qk -> qk+1 use omega
    Matrix<double, 4, 4> transform;
    transform << 0, -wx, -wy, -wz,
                wx, 0, wz, -wy,
                wy, -wz, 0, wx,
                wz, wy, -wx, 0;

    const double alpha = dt / 2;
    const Matrix<double, 4, 4> I4 = Matrix4d::Identity();

    //Convert matrix for quaternion
    // q = q * (I + dt/2 * transform)
    // A  = (I + dt/2 * transform)
    Matrix<double, 4, 4> Aq;
    Aq = I4 + alpha * transform;

    Vector4d q = x_hat.segment<4>(3);

    //Construct a matrix4d to use when computing
    //Jacobian term for Jqb, use q at time k
    double qw, qx, qy, qz;
    qw = q(0);
    qx = q(1); 
    qy = q(2); 
    qz = q(3);

    //Use previous quaternion state before prediction
    Matrix<double, 4, 3> B;
    B << qx, qy, qz,
        -qw, qz, -qy,
        -qz, -qw, qx,
        qy, -qx, -qw;
    B *= alpha;

    //Compute new q raw
    q = Aq * q;
    //Compute r using the raw quaternion
    const double r = q.norm();
    //Normalize
    q.normalize();

    //F is identity matrix, except 
    // F(0, 3), F(1, 4), F(2, 5) = dt --> Position relate to velocity
    //Will be changed later with Jqq and Jqb
    x_hat_pred = F * x_hat;
    x_hat_pred.segment<4>(3) = q; //Append q_new_value to prediction

    //Construct Jacobian matric for Jqq and Jqb
    //Jqq: Relationship between qK+1 / qk
    //Jqb: Relationship between qK+1 / b (gyro-bias)

    //Use normalized q
    const Matrix<double, 4, 4> N = 
        1/r * (I4 - q * q.transpose());
    
    const Matrix<double, 4, 4> Jqq = N * Aq;
    const Matrix<double, 4, 3> Jqb = N * B;

    //Jacobian matrix
    Matrix<double, 10, 10> J;
    J.setIdentity();

    J(0,2) = dt^2 / 2; //Pos - Accel relation
    J(0,1) = dt; //Pos-vel
    J(1,2) = dt; //Vel- accel
    J.block<4, 4>(3, 3) = Jqq;
    J.block<4, 3>(3, 7) = Jqb;
    
    //Covariance predict
    P_new = J * P * J.transpose() + Q;
}

void EKF::update(const Vector6d& z){
    if (!init) return;

    //Kalman gain
    const Matrix<double, 10, 6> K = P_new * H.transpose() 
                    * (H * P_new * H.transpose() + R).inverse();
    
    x_hat = x_hat_pred + K * (z - H * x_hat_pred);
    P = (I - K * H) * P_new;
}
}