//
// Created by jun on 24-5-9.
//

#ifndef KALMAN_FILTER_H
#define KALMAN_FILTER_H
#include <Eigen/Dense>


class KalmanFilter
{
public:
    KalmanFilter(double R1_value, double R2_value, double Q_value);
    static void update();
    static void predict();

private:
    static Eigen::Matrix3d X;
    const int X_N = 3, Z_N = 1;

};



#endif //KALMAN_FILTER_H
