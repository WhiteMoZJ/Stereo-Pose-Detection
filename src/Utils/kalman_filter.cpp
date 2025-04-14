//
// Created by jun on 24-5-9.
//

#include "kalman_filter.h"

KalmanFilter::KalmanFilter(const Eigen::VectorXf& x0, const float& R, const float& Q)
{
    x_ = x0;
    X_k = Eigen::VectorXf(x0.size());
    A_ = Eigen::MatrixXf::Identity(x0.size(), x0.size());
    C_ = Eigen::MatrixXf::Identity(x0.size(), x0.size());
    R_ = Eigen::MatrixXf::Identity(x0.size(), x0.size()) * R;
    Q_ = Eigen::MatrixXf::Identity(x0.size(), x0.size()) * Q;
    P_ = Eigen::MatrixXf::Identity(x0.size(), x0.size()) * 50;
}

void KalmanFilter::predict()
{
    X_k = A_ * x_;
    P_ = A_ * P_ * A_.transpose() + R_;
}

void KalmanFilter::update(const Eigen::VectorXf& z)
{
    const Eigen::MatrixXf K_ = P_ * C_.transpose() * (C_ * P_ * C_.transpose() + Q_).inverse();
    x_ = X_k + K_ * (z - C_ * X_k);
    P_ = (Eigen::MatrixXf::Identity(x_.size(), x_.size()) - K_ * C_) * P_;
}

Eigen::VectorXf KalmanFilter::getState() const
{
    return x_;
}