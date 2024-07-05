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
     * @param P0 The initial state covariance matrix.
     * @param F The state transition matrix.
     * @param Q The process covariance matrix.
     * @param H The measurement matrix.
     * @param R The measurement covariance matrix.
     */
    KalmanFilter(const Eigen::VectorXd& x0, const Eigen::MatrixXd& P0, const Eigen::MatrixXd& F,
                 const Eigen::MatrixXd& Q, const Eigen::MatrixXd& H, const Eigen::MatrixXd& R);

    /**
     * @brief Default destructor for the KalmanFilter class.
     */
    ~KalmanFilter() = default;

    /**
     * @brief Predicts the next state of the system.
     */
    void predict();

    /**
     * @brief Updates the state estimate based on the given measurement.
     * @param z The measurement vector.
     */
    void update(const Eigen::VectorXd& z);

    /**
     * @brief Returns the current state vector.
     * @return The current state vector.
     */
    Eigen::VectorXd getState() const;

private:
    Eigen::VectorXd x_; // state vector
    Eigen::MatrixXd P_; // state covariance matrix
    Eigen::MatrixXd F_; // state transition matrix
    Eigen::MatrixXd Q_; // process covariance matrix
    Eigen::MatrixXd H_; // measurement matrix
    Eigen::MatrixXd R_; // measurement covariance matrix
};



#endif //KALMAN_FILTER_H
