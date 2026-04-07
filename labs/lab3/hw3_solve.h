// hw3_solve.h
#ifndef _HW3_SOLVE_H
#define _HW3_SOLVE_H

#include "sparse.h"

/**
 * @brief 使用高斯-赛德尔方法求解 Ax = b
 * 
 * @param A 系数矩阵
 * @param b 标签向量（仅昵称）
 * @param error 当 |x_{k+1} - x_k| 的无穷范数 <（或 <=）error 时，停止迭代
 * @return x: Vecd，方程 Ax = b 的解
 */
Vecd Gauss_Seidel(const Sparse& A, const Vecd& b, double error);

/**
 * @brief 使用共轭梯度法求解 Ax = b
 * 
 * @param A 系数矩阵
 * @param b 标签向量（仅昵称）
 * @param error 当 ||r||_2^2 <（或 <=）error 时停止迭代，r = b - Ax 为残差
 * @param kmax 最大迭代次数
 * @return x: Vecd，方程 Ax = b 的解
 */
Vecd Conjugate_Gradient(const Sparse& A, const Vecd& b, double error, int kmax);

#endif