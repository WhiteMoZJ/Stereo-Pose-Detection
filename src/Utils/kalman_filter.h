//
// Created by jun on 24-5-9.
//

#ifndef KALMAN_FILTER_H
#define KALMAN_FILTER_H
#include <Eigen/Dense>


class KalmanFilter
{
public:
    explicit KalmanFilter(float R1_value, float R2_value, float Q_value);
    KalmanFilter() = delete;
    void predict(Eigen::Vector3f &Z);

private:
    Eigen::Matrix<float, 4, 1> X;  //
    Eigen::Matrix<float, 4, 4> A;
    Eigen::Matrix<float, 4, 4> P;
    Eigen::Matrix<float, 4, 4> R;
    Eigen::Matrix<float, 4, 3> K;
    Eigen::Matrix<float, 3, 4> C;
    Eigen::Matrix<float, 4, 4> Q;

    const int X_N = 4, Z_N = 3;
};



#endif //KALMAN_FILTER_H
