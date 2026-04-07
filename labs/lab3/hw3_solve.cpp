#include "hw3_solve.h"

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


Vecd Conjugate_Gradient(const Sparse& A, const Vecd& b, double error, int kmax) {
    int n = b.size();
    Vecd x(n, 0.0);      // 初始解
    Vecd r = b;          // 残差 r = b - A*x，因为 x=0，所以 r=b
    Vecd p = r;          // 初始搜索方向
    double rr = 0.0;     // r^T * r
    for (int i = 0; i < n; ++i) rr += r[i] * r[i];

    for (int k = 0; k < kmax; ++k) {
        // 1. 计算 A * p
        Vecd Ap(n, 0.0);
        A.matvec(p, Ap);

        // 2. 计算步长 alpha = (r^T r) / (p^T A p)
        double pAp = 0.0;
        for (int i = 0; i < n; ++i) pAp += p[i] * Ap[i];
        double alpha = rr / pAp;

        // 3. 更新解 x = x + alpha * p
        for (int i = 0; i < n; ++i) x[i] += alpha * p[i];

        // 4. 更新残差 r = r - alpha * A p
        for (int i = 0; i < n; ++i) r[i] -= alpha * Ap[i];

        // 5. 检查收敛：残差平方和 <= error
        double new_rr = 0.0;
        for (int i = 0; i < n; ++i) new_rr += r[i] * r[i];
        if (new_rr <= error) break;

        // 6. 计算 beta = new_rr / rr
        double beta = new_rr / rr;
        rr = new_rr;

        // 7. 更新搜索方向 p = r + beta * p
        for (int i = 0; i < n; ++i) p[i] = r[i] + beta * p[i];
    }
    return x;
}