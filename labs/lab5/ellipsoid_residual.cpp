#include "ellipsoid_residual.hpp"

EllipseResidualFunction::EllipseResidualFunction(const std::vector<double>& pts)
    : points(pts) {}

int EllipseResidualFunction::nR() const {
    return points.size() / 3;
}

int EllipseResidualFunction::nX() const {
    return 3;   // A, B, C
}

void EllipseResidualFunction::eval(double* R, double* J, double* X) {
    double A = X[0], B = X[1], C = X[2];
    double invA2 = 1.0/(A*A), invB2 = 1.0/(B*B), invC2 = 1.0/(C*C);
    int m = nR();

    for (int i = 0; i < m; ++i) {
        double x = points[3*i], y = points[3*i+1], z = points[3*i+2];
        double val = x*x*invA2 + y*y*invB2 + z*z*invC2;
        R[i] = 1.0 - val;

        if (J != nullptr) {
            J[i*3 + 0] = 2.0 * x*x / (A*A*A);
            J[i*3 + 1] = 2.0 * y*y / (B*B*B);
            J[i*3 + 2] = 2.0 * z*z / (C*C*C);
        }
    }
}