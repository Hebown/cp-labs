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