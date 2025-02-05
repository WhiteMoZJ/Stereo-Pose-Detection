//
// Created by jun on 24-5-9.
//

#include "kalman_filter.h"

KalmanFilter::KalmanFilter(const Eigen::VectorXf& x0, const Eigen::MatrixXf& P0, const Eigen::MatrixXf& F,
                                const Eigen::MatrixXf& Q, const Eigen::MatrixXf& H, const Eigen::MatrixXf& R)
{
    x_ = x0;
    P_ = P0;
    F_ = F;
    Q_ = Q;
    H_ = H;
    R_ = R;
}

void KalmanFilter::predict()
{
    x_ = F_ * x_;
    P_ = F_ * P_ * F_.transpose() + Q_;
}

void KalmanFilter::update(const Eigen::VectorXf& z)
{
    Eigen::VectorXf y = z - H_ * x_;
    Eigen::MatrixXf S = H_ * P_ * H_.transpose() + R_;
    Eigen::MatrixXf K = P_ * H_.transpose() * S.inverse();

    x_ = x_ + K * y;
    P_ = (Eigen::MatrixXf::Identity(x_.size(), x_.size()) - K * H_) * P_;
}

Eigen::VectorXf KalmanFilter::getState() const
{
    return x_;
}