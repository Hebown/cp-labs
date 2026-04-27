#include "solver5903.hpp"
#include <opencv2/opencv.hpp>
#include <cmath>
#include <cstdio>

double Solver5903::solve(ResidualFunction* f, double* X,
                         GaussNewtonParams param, GaussNewtonReport* report) {
    int n = f->nX();                // 变量个数
    int m = f->nR();                // 残差个数
    double *R = new double[m];      // 残差向量
    double *J = new double[m * n];  // 雅可比矩阵（行优先）
    double F = 0.0;
    int iter = 0;
    auto stop = GaussNewtonReport::STOP_NO_CONVERGE;

    while (iter < param.max_iter) {
        f->eval(R, J, X);           // 计算当前残差和雅可比

        // 计算目标函数值 F = 0.5 * ||R||^2
        double normR2 = 0.0;
        for (int i = 0; i < m; ++i) normR2 += R[i] * R[i];
        F = 0.5 * normR2;
        double normR = std::sqrt(normR2);

        // 终止条件1：残差小
        if (normR < param.residual_tolerance) {
            stop = GaussNewtonReport::STOP_RESIDUAL_TOL;
            break;
        }

        // 梯度 g = J^T * R
        cv::Mat Jm(m, n, CV_64F, J);
        cv::Mat Rm(m, 1, CV_64F, R);
        cv::Mat g = Jm.t() * Rm;
        double normG = cv::norm(g, cv::NORM_L2);

        // 终止条件2：梯度小
        if (normG < param.gradient_tolerance) {
            stop = GaussNewtonReport::STOP_GRAD_TOL;
            break;
        }

        // 解正规方程 (J^T J) delta = -g
        cv::Mat JTJ = Jm.t() * Jm;
        cv::Mat delta;
        if (!cv::solve(JTJ, -g, delta, cv::DECOMP_SVD)) {
            stop = GaussNewtonReport::STOP_NUMERIC_FAILURE;
            break;
        }

        // 线性搜索确定步长 alpha
        double alpha = 1.0;
        double *d = delta.ptr<double>();
        if (param.exact_line_search)
            alpha = exactLineSearch(f, X, d, m, n);
        else
            alpha = backtracking(f, X, d, F, m, n);

        // 更新参数
        for (int i = 0; i < n; ++i) X[i] += alpha * d[i];

        if (param.verbose) {
            printf("iter %d: F=%g, |R|=%g, |grad|=%g, alpha=%g\n",
                   iter, F, normR, normG, alpha);
        }
        ++iter;
    }

    delete[] R;
    delete[] J;
    if (report) {
        report->stop_type = stop;
        report->n_iter = iter;
    }
    return F;
}

// 精确线性搜索：黄金分割法（区间 [0,1]）
double Solver5903::exactLineSearch(ResidualFunction* f, double* X, double* d,
                                   int m, int n) {
    double a = 0.0, b = 1.0;
    double gr = (std::sqrt(5) - 1) / 2;   // 黄金分割比 0.618
    double c = b - gr * (b - a);
    double d_ = a + gr * (b - a);
    double *tmpX = new double[n];
    double *tmpR = new double[m];

    auto phi = [&](double alpha) -> double {
        for (int i = 0; i < n; ++i) tmpX[i] = X[i] + alpha * d[i];
        f->eval(tmpR, nullptr, tmpX);          // 只需残差
        double s = 0.0;
        for (int i = 0; i < m; ++i) s += tmpR[i] * tmpR[i];
        return 0.5 * s;
    };

    double fc = phi(c), fd = phi(d_);
    while (b - a > 1e-6) {
        if (fc < fd) {
            b = d_; d_ = c; c = b - gr * (b - a);
            fd = fc; fc = phi(c);
        } else {
            a = c; c = d_; d_ = a + gr * (b - a);
            fc = fd; fd = phi(d_);
        }
    }
    delete[] tmpX; delete[] tmpR;
    return (a + b) * 0.5;
}

// 近似线性搜索：回溯法（alpha 从1开始，若不下降则减半）
double Solver5903::backtracking(ResidualFunction* f, double* X, double* d,
                                double curF, int m, int n) {
    double alpha = 1.0;
    double rho = 0.5;               // 衰减因子
    double *Xnew = new double[n];
    double *Rnew = new double[m];

    while (alpha > 1e-10) {
        for (int i = 0; i < n; ++i) Xnew[i] = X[i] + alpha * d[i];
        f->eval(Rnew, nullptr, Xnew);
        double s = 0.0;
        for (int i = 0; i < m; ++i) s += Rnew[i] * Rnew[i];
        if (0.5 * s < curF) break;   // 下降成功
        alpha *= rho;
    }
    delete[] Xnew; delete[] Rnew;
    return alpha;
}