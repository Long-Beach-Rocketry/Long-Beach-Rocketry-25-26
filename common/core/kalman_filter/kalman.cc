#include <cmath>
#include "kalman.h"

using namespace Eigen;

namespace LBR 
{

//Construct system model matrix
//Internal computation component will be init in init()
EKF::EKF(const Matrix<double, 3, 3>& newF,
        const Matrix<double, 2, 3>& newH,
        const Matrix<double, 3, 3>& newQ,
        const Matrix<double, 2, 2>& newR
    ) : F(newF), H(newH), Q(newQ), R(newR) {}

const Matrix<double, 3, 1>& EKF::getXhat(){
    return x_hat;
};


void EKF::initialize(const Matrix<double, 3, 1>& x0, 
        const Matrix<double, 3, 3>& P0){
    x_hat = x0;
    x_hat_pred = x0;
    P = P0;
    P_new = P0;
    init = true;
}

void EKF::predict(double dt){
    if (!init) return; //Check whether x and P has been initliazed

    //Relationship between pz, vz, az
    F(0, 1) = dt;
    F(0, 2) = pow(dt, 2) / 2;
    F(1, 2) = dt;

    x_hat_pred = F * x_hat;
    
    //Covariance predict
    P_new = F * P * F.transpose() + Q;
}

void EKF::update(const Matrix<double, 2, 1>& z){
    if (!init) return;

    //Kalman gain
    const Matrix<double, 3, 2> K = P_new * H.transpose()
                    * (H * P_new * H.transpose() + R).inverse();

    //Raw computation
    const Matrix<double, 3, 3> I3 =  Matrix<double, 3, 3>::Identity();
    x_hat = x_hat_pred + K * (z - H * x_hat_pred);
    P = (I3 - K * H) * P_new;
}

}