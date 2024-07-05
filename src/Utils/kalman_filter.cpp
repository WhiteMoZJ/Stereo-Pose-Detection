//
// Created by jun on 24-5-9.
//

#include "kalman_filter.h"

KalmanFilter::KalmanFilter(const Eigen::VectorXd& x0, const Eigen::MatrixXd& P0, const Eigen::MatrixXd& F,
                                const Eigen::MatrixXd& Q, const Eigen::MatrixXd& H, const Eigen::MatrixXd& R)
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

void KalmanFilter::update(const Eigen::VectorXd& z)
{
    Eigen::VectorXd y = z - H_ * x_;
    Eigen::MatrixXd S = H_ * P_ * H_.transpose() + R_;
    Eigen::MatrixXd K = P_ * H_.transpose() * S.inverse();

    x_ = x_ + K * y;
    P_ = (Eigen::MatrixXd::Identity(x_.size(), x_.size()) - K * H_) * P_;
}

Eigen::VectorXd KalmanFilter::getState() const
{
    return x_;
}