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
    KalmanFilter(const Eigen::VectorXf& x0, const Eigen::MatrixXf& P0, const Eigen::MatrixXf& F,
                 const Eigen::MatrixXf& Q, const Eigen::MatrixXf& H, const Eigen::MatrixXf& R);

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
    void update(const Eigen::VectorXf& z);

    /**
     * @brief Returns the current state vector.
     * @return The current state vector.
     */
    Eigen::VectorXf getState() const;

private:
    Eigen::VectorXf x_; // state vector
    Eigen::MatrixXf P_; // state covariance matrix
    Eigen::MatrixXf F_; // state transition matrix
    Eigen::MatrixXf Q_; // process covariance matrix
    Eigen::MatrixXf H_; // measurement matrix
    Eigen::MatrixXf R_; // measurement covariance matrix
};



#endif //KALMAN_FILTER_H
