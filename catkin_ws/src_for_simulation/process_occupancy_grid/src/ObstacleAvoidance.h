#ifndef OBSTACLEAVOIDANCE_H_INCLUDED
#define OBSTACLEAVOIDANCE_H_INCLUDED

#include <iostream> // In-Out streams
#include <fstream>  // To write data into files
#include <eigen3/Eigen/Dense> // For Linear Algebra
#include <cmath> // Basic math functions
// #include "SolverFunctions.h" // For now the specific point is always the center of the ellipse so solver not needed
// Functions of SolverFunctions are used if you have a point P0 inside an ellipse and a point P1 outside and you want to
// know the position of the surface point that is on the [P0, P1] segment

const int number_states = 3;   // States are [x, y, phi]
const int method_weights = 2;  // 1 to consider all the obstacles, 2 to consider only the obstacles within the limit_dist range
const float limit_dist = 26 ;  // limit distance for method_weights=2, set it to -1 if you use method_weights=1

using State = Eigen::Matrix<float, number_states, 1>; // State is an alias to represent a column vector with three components
using Obstacle = Eigen::Matrix<float, 10, 1>; // Obstacle is an alias to represent ellipses [x_c, y_c, phi, a1, a2, p1, p2, v_x, v_y, w_rot]

// The following functions are defined in the paper "Avoidance of Convex and Concave Obstacles with Convergence ensured through Contraction"
// Each step has its own function. The function that regroups all the other is next_step_single_obstacle

State f_epsilon(State const& state_robot, State const& state_attractor); // compute attractor function

// The reference point is always the center point of the ellipse (simplification) so this function is not used
//float specific_radius(State state_robot, State center_point, State reference_point, Obstacle obs, int p); // compute specific radius

/** Disclaimer: there may be some references to epsilon because initially I thought it was one instead of a xi */

/** Latex equation of an ellipse: \dfrac {((x-h)\cos(A)+(y-k)\sin(A))^2}{(a^2)}+\dfrac{((x-h) \sin(A)-(y-k) \cos(A))^2}{(b^2)}=1
    where h,k and a,b are the shifts and semi-axis in the x and y directions respectively and A is the angle measured from x axis. */

/**
 * Compute gamma(xi) for an obstacle and a given position of the robot
 *
 * @param state_robot Eigen matrix of size (3,1) containing the (x,y,theta) state vector of a point/robot.
 * @param obs Eigen matrix of size (10,1) containing information about the elliptic obstacle with the [x_c, y_c, phi, a1, a2, p1, p2, v_x, v_y, w_rot] format
              x_c, y_c, phi are the position and orientation
              a1, a2 are the length of the half-axes of the ellipse
              p1, p2 are the power in the equation of the ellipse
              v_x, v_y, w_rot are the linear and angular velocities of the ellipse
 * @return Value of Gamma distance from the ellipse
 */
float gamma(State const& state_robot, Obstacle const& obs, bool is_radius=false);

/**
 * Compute lambda_r(xi) for an obstacle and a given position of the robot, used to computed D(xi) matrix
 *
 * @param state_robot Eigen matrix of size (3,1) containing the (x,y,theta) state vector of a point/robot.
 * @param obs Eigen matrix of size (10,1) containing information about the elliptic obstacle with the [x_c, y_c, phi, a1, a2, p1, p2, v_x, v_y, w_rot] format
              x_c, y_c, phi are the position and orientation
              a1, a2 are the length of the half-axes of the ellipse
              p1, p2 are the power in the equation of the ellipse
              v_x, v_y, w_rot are the linear and angular velocities of the ellipse
 * @param limit_distance Optionnal value when we want to consider an obstacle only when the Gamma value of the robot for this obstacle is inferior to limit_distance
 * @return Value of the lambda_r coefficient
 */
float lambda_r(State const& state_robot, Obstacle const& obs, float limit_distance=-1);

/**
 * Compute lambda_e(xi) for an obstacle and a given position of the robot, used to computed D(xi) matrix
 *
 * @param state_robot Eigen matrix of size (3,1) containing the (x,y,theta) state vector of a point/robot
 * @param obs Eigen matrix of size (10,1) containing information about an elliptic obstacle with the [x_c, y_c, phi, a1, a2, p1, p2, v_x, v_y, w_rot] format
              x_c, y_c, phi are the position and orientation
              a1, a2 are the length of the half-axes of the ellipse
              p1, p2 are the power in the equation of the ellipse
              v_x, v_y, w_rot are the linear and angular velocities of the ellipse
 * @param limit_distance optionnal value when we want to consider an obstacle only when the Gamma value of the robot for this obstacle is inferior to limit_distance
 * @return Value of the lambda_e coefficient
 */
float lambda_e(State const& state_robot, Obstacle const& obs, float limit_distance=-1);

/**
 * Compute a D(xi) matrix as defined in the method of (Huber and al., 2019)
 *
 * @param lambda_r Value of the lambda_r coefficient for the considered obstacle
 * @param lambda_e Value of the lambda_e coefficient for the considered obstacle
 * @return D(xi) Eigen matrix of size (number_states, number_states)
 */
Eigen::Matrix<float, number_states, number_states> D_epsilon( float const& lamb_r, float const& lamb_e);

/**
 * Compute a r(xi) vector as defined in the method of (Huber and al., 2019) that is used to get the E(xi) matrix
 *
 * @param state_robot Eigen matrix of size (3,1) containing the (x,y,theta) state vector of a point/robot
 * @param obs Eigen matrix of size (10,1) containing information about an elliptic obstacle with the [x_c, y_c, phi, a1, a2, p1, p2, v_x, v_y, w_rot] format
              x_c, y_c, phi are the position and orientation
              a1, a2 are the length of the half-axes of the ellipse
              p1, p2 are the power in the equation of the ellipse
              v_x, v_y, w_rot are the linear and angular velocities of the ellipse
 * @return Eigen matrix of size (3,1) containing the (x,y,theta) data of the r(xi) vector.
 */
State r_epsilon(State const& state_robot, Obstacle const& obs);

/**
 * Compute the Gamma-distance between two Eigen matrices of size (3,1) containing the (x,y,theta) information about two points of the workspace
 *
 * @param state1 Eigen matrix of size (3,1) containing the (x,y,theta) state vector of a point/robot
 * @param state2 Eigen matrix of size (3,1) containing the (x,y,theta) state vector of a point/robot
 * @return Gamma-distance between the two input state vectors
 */
float distance(State const& state1, State const& state2);

float partial_derivative(State const& state_robot, Obstacle const& obs, int const& direction); // partial finite derivative of order 4

State gradient(State const& state_robot, Obstacle const& obs); // compute gradient of gamma(epsilon)

Eigen::Matrix<float, number_states, number_states-1> gram_schmidt(State const& gradient_vector); // orthogonal basis of an hyperplane with Gram-Schmidt

State projection(State const& vec1, State const& vec2); // projection of vec1 onto vec2

Eigen::Matrix<float, number_states, number_states> E_epsilon(State const& r_eps_vector, Eigen::Matrix<float, number_states, number_states-1> const& ortho_basis); // create E(epsilon) matrix

Eigen::Matrix<float, number_states, number_states> M_epsilon(Eigen::Matrix<float, number_states, number_states> const& D_eps, Eigen::Matrix<float, number_states, number_states> const& E_eps); // create M(epsilon) matrix

State epsilon_dot(Eigen::Matrix<float, number_states, number_states> const& M_eps, State const& f_eps, State const& state_robot, Obstacle const& obs); // compute epsilon_dot with M(epsilon) and f(epsilon)

State next_step_single_obstacle(State const& state_robot, State const& state_attractor, Obstacle const& obs); // use all previous functions to compute the next velocity state

// Functions to handle several obstacles -> basically reuse the next_step_single_obstacle for each obstacles and compute the weight of each obstacle

// Because the number of detected obstacles can vary I use dynamic-size matrices
Eigen::MatrixXf velocities(State const& state_robot, State const& state_attractor, Eigen::MatrixXf const& mat_obs); // compute the next velocity for all obstacles

Eigen::MatrixXf weights(State const& state_robot, Eigen::MatrixXf const& mat_obs, int const& method, float const& limit_distance=-1); // compute the weight of each obstacle

float weighted_magnitude(Eigen::MatrixXf const& mat_weights, Eigen::MatrixXf const& mat_magnitudes); // compute the magnitude of the epsilon velocity

State n_bar_2D(Eigen::MatrixXf const& mat_norm_velocities, Eigen::MatrixXf const& mat_weights); // compute the weighted directional interpolation (simplification: 2D case)

State one_step_2D(State const& state_robot, State const& state_attractor, Eigen::MatrixXf const& mat_obs); // compute one step of the simulation taking into account all the obstacles

State direct_multiplication_2D(State const& state_robot, State const& state_attractor, Eigen::MatrixXf const& mat_obs); // direct multiplication of the deformation matrices of all obstacles

// Functions R_matrix, kappa_matrix and n_bar_matrix can be found in the paper (d-dimensional case)
// However, as we limited ourselves to the 2D case, I did not finished the d-dimensional implementation (need to finish n_bar_matrix)
Eigen::Matrix<float, number_states, number_states> R_matrix(State const& f_eps); // compute the R matrix to align the initial DS with the first axis (not used)

Eigen::MatrixXf kappa_matrix(Eigen::MatrixXf const& mat_n_hat); // compute the Kappa matrix (not used)

Eigen::MatrixXf n_bar_matrix(Eigen::MatrixXf const& mat_kappa_bar, Eigen::Matrix<float, number_states, number_states> const& R_mat); // compute n_bar with matrices R and kappa (not used, not finished)

// Other functions

/**
 * Update the position of elliptic obstacles based on their linear and angular velocities
 *
 * @param mat_obs Eigen matrix of size (N,10) containing the [x_c, y_c, phi, a1, a2, p1, p2, v_x, v_y, w_rot] information about N obstacles.
              x_c, y_c, phi are the position and orientation
              a1, a2 are the length of the half-axes of the ellipse
              p1, p2 are the power in the equation of the ellipse
              v_x, v_y, w_rot are the linear and angular velocities of the ellipse
 * @param time_step Time step that has to be used to update the obstacles [s]
 */
void update_obstacles(Eigen::MatrixXf & mat_obs, float const& time_step);

void compute_quiver(Eigen::Matrix<float, 5, 1> const& limits, State const& state_attractor, Eigen::MatrixXf const& mat_obs); // compute data to plot a quiver graph with matplotlib

void compute_quiver_multiplication(Eigen::Matrix<float, 5, 1> const& limits, State const& state_attractor, Eigen::MatrixXf const& mat_obs); // quiver for direct_multiplication_2D

/**
 * Return a velocity command whose linear and angular velocities have been limited for security reasons. The aim of this function is to limit the norm of the linear and angular velocities that are sent to the real robot. Limits are set in the function code.
 *
 * @param input_speed Eigen matrix of size (3,1) containing the (x_dot,y_dot,theta_dot) velocity command that will be sent to the robot
 * @return Eigen matrix of size (3,1) containing the input (x_dot,y_dot,theta_dot) velocity command whose linear and angular velocities may have been reduced if they were above the limits.
 */
State speed_limiter(State const& input_speed);

// Polygon functions -> basically the same functions than for ellipses but for polygons defined by a list of (x,y) points

float polygon_gamma(State const& state_robot, Eigen::Matrix<float, 2, 1> closest_point); // gamma function for polygons

void compute_quiver_polygon(Eigen::Matrix<float, 5, 1> const& limits, State const& state_attractor, Eigen::MatrixXf const& mat_points); // compute data to plot a quiver graph with matplotlib

State polygon_next_step_single_obstacle(State const& state_robot, State const& state_attractor, Eigen::MatrixXf const& mat_points);

State polygon_r_epsilon(State const& state_robot, Eigen::Matrix<float, 2, 1> closest_point);

float polygon_partial_derivative(State const& state_robot, Eigen::Matrix<float, 2, 1> const& closest_point, int const& direction);

State polygon_gradient(State const& state_robot, Eigen::Matrix<float, 2, 1> const& closest_point);

Eigen::Matrix<float, 2, 1> polygon_get_closest(State const& state_robot, Eigen::MatrixXf const& mat_points);

// Functions to make the link with ROS

float test_ros();

#endif // OBSTACLEAVOIDANCE_H_INCLUDED
