#pragma once

#include "hw3_gn.h"

class Solver5903 : public GaussNewtonSolver {
public:
    virtual double solve(ResidualFunction* f, double* X,
                         GaussNewtonParams param,
                         GaussNewtonReport* report) override;
private:
    double exactLineSearch(ResidualFunction* f, double* X, double* d, int m, int n);
    double backtracking(ResidualFunction* f, double* X, double* d, double curF, int m, int n);
};