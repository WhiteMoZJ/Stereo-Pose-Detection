//
// Created by jun on 24-5-9.
//

#ifndef KALMAN_FILTER_H
#define KALMAN_FILTER_H
#include <Eigen/Dense>

/**
 * @brief The KalmanFilter class represents a Kalman filter implementation.
 */
class KalmanFilter {
public:
    /**
     * @brief Default constructor for the KalmanFilter class.
     * @param x0 The initial state vector.
     * @param Q The process noise covariance matrix.
     * @param R The process noise covariance matrix.
     */
    explicit KalmanFilter(const Eigen::VectorXf& x0, const float& R, const float& Q);

    ~KalmanFilter() = default;

    /**
     * @brief Predicts the next state of the system.
     */
    void predict();

    /**
     * @brief Updates the state estimate based on the given measurement.
     * @param z The measurement vector.
     */
    void update(const Eigen::VectorXf& z);

    /**
     * @brief Returns the current state vector.
     * @return The current state vector.
     */
    Eigen::VectorXf getState() const;

private:
    Eigen::VectorXf x_; // state vector
    Eigen::VectorXf X_k;
    Eigen::MatrixXf A_;
    Eigen::MatrixXf P_; // state covariance matrix
    Eigen::MatrixXf R_; // measurement covariance matrix
    Eigen::MatrixXf C_;
    Eigen::MatrixXf Q_; // process covariance matrix
};



#endif //KALMAN_FILTER_H
