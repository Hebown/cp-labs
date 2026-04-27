#pragma once

#include "hw3_gn.h"
#include <vector>

class EllipseResidualFunction : public ResidualFunction {
    std::vector<double> points;
public:
    EllipseResidualFunction(const std::vector<double>& pts);
    virtual int nR() const override;
    virtual int nX() const override;
    virtual void eval(double* R, double* J, double* X) override;
};