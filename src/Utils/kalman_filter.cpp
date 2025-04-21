//
// Created by jun on 24-5-9.
//

#include "kalman_filter.h"
#include <iostream>

KalmanFilter::KalmanFilter(const Eigen::VectorXf& x0, const float& R, const float& Q)
{
    int X_N = x0.size();
    x_ = Eigen::MatrixXf::Zero(X_N, 1);
    X_k = Eigen::VectorXf(X_N);
    A_ = Eigen::MatrixXf::Identity(X_N, X_N);
    A_.block(0, X_N-1, X_N, 1) = Eigen::VectorXf::Ones(X_N, 1);
    C_ = Eigen::MatrixXf::Identity(X_N, X_N);
    R_ = Eigen::MatrixXf::Identity(X_N, X_N) * R;
    Q_ = Eigen::MatrixXf::Identity(X_N, X_N) * Q;
    P_ = Eigen::MatrixXf::Identity(X_N, X_N) * 50;
}

void KalmanFilter::predict(const Eigen::VectorXf& z)
{
    if (x_ == Eigen::MatrixXf::Zero(z.size(), 1))
        x_ = z;
    X_k = A_ * x_;
    P_ = A_ * P_ * A_.transpose() + R_;

    const Eigen::MatrixXf K_ = P_ * C_.transpose() * (C_ * P_ * C_.transpose() + Q_).inverse();
    x_ = X_k + K_ * (z - C_ * X_k);
    P_ = (Eigen::MatrixXf::Identity(x_.size(), x_.size()) - K_ * C_) * P_;
}

Eigen::VectorXf KalmanFilter::getState() const
{
    return x_;
}