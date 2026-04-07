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
Vecd Gauss_Seidel(const Sparse& A, const Vecd& b, double error){
    Vecd current_x(b.size(), 0.0);
    Vecd last_x(b.size(), 0.0);
    auto get_max=[=](const Vecd& current_x,const Vecd&last_x){
        double max_diff=-1.0;
        for(int i=0;i<current_x.size();i++){
            double cur_diff=std::fabs(current_x[i]-last_x[i]);
            if(cur_diff>max_diff){
                max_diff=cur_diff;
            }
        }return max_diff;
    };
    do {
        last_x = current_x;
        for(int i=0;i<current_x.size();i++){
            // 计算第一部分
            double new_sum=0;
            for(int j=0;j<i;j++){
                new_sum+=A.at(i, j)*current_x[j];
            }
            // 计算第二部分
            double old_sum=0;
            for(int j=i+1;j<current_x.size();j++){
                old_sum+=A.at(i, j)*last_x[j];
            }
            current_x[i]=(b[i]-new_sum-old_sum)/A.at(i, i);
        }
    } while(get_max(current_x, last_x)>=error);
    return current_x;
}

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