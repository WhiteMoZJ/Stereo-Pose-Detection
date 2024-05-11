//
// Created by jun on 24-5-9.
//

#include "kalman_filter.h"

KalmanFilter::KalmanFilter(float R1_value, float R2_value, float Q_value)
{

}

void KalmanFilter::predict(Eigen::Vector3f &Z)
{
    const Eigen::Matrix<float, 4, 1> X_k = A * X;
    P = A * P * A.transpose() + R;

    K = P * C.transpose() * (C * P * C.transpose() + Q).inverse();
    X = X_k + K * (Z - C * X_k);
    P = (Eigen::Matrix<float, 4, 4>::Identity() - K * C) + P;

    Z = Eigen::Vector3f{X[0], X[1], X[3]};
}
