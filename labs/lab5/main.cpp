#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>
#include "solver5903.hpp"
#include "ellipsoid_residual.hpp"
#include "hw3_gn.h"

void printHelp(const char* prog) {
    std::cout << "Usage: " << prog << " [options]\n"
              << "Options:\n"
              << "  --data <file>          point cloud file (default: ellipse753.txt)\n"
              << "  --init <A> <B> <C>     initial parameters (auto from bounding box if not given)\n"
              << "  --exact-line-search    use exact line search (default: false)\n"
              << "  --verbose              print iteration info (default: false)\n"
              << "  --max-iter <N>         max iterations (default: 100)\n"
              << "  --grad-tol <val>       gradient tolerance (default: 1e-6)\n"
              << "  --res-tol <val>        residual tolerance (default: 1e-6)\n"
              << "  --help                 show this help\n";
}

int main(int argc, char** argv) {
    // 默认参数
    std::string dataFile = "ellipse753.txt";
    bool exactLineSearch = false;
    bool verbose = false;
    int maxIter = 100;
    double gradTol = 1e-6, resTol = 1e-6;
    bool initGiven = false;
    double initA = 0, initB = 0, initC = 0;

    // 在这里我给你准备了非常非常多的命令行参数，你可以根据自己的喜好来试一试
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--help") == 0) {
            printHelp(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "--data") == 0 && i+1 < argc) {
            dataFile = argv[++i];
        } else if (strcmp(argv[i], "--exact-line-search") == 0) {
            exactLineSearch = true;
        } else if (strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "--max-iter") == 0 && i+1 < argc) {
            maxIter = std::stoi(argv[++i]);
        } else if (strcmp(argv[i], "--grad-tol") == 0 && i+1 < argc) {
            gradTol = std::stod(argv[++i]);
        } else if (strcmp(argv[i], "--res-tol") == 0 && i+1 < argc) {
            resTol = std::stod(argv[++i]);
        } else if (strcmp(argv[i], "--init") == 0 && i+3 < argc) {
            initA = std::stod(argv[++i]);
            initB = std::stod(argv[++i]);
            initC = std::stod(argv[++i]);
            initGiven = true;
        } else {
            std::cerr << "Unknown option: " << argv[i] << "\n";
            printHelp(argv[0]);
            return 1;
        }
    }

    // 读取点云
    std::vector<double> pts;
    std::ifstream file(dataFile);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << dataFile << std::endl;
        return 1;
    }
    double x, y, z;
    while (file >> x >> y >> z) {
        pts.push_back(x);
        pts.push_back(y);
        pts.push_back(z);
    }
    file.close();
    std::cout << "Loaded " << pts.size()/3 << " points from " << dataFile << std::endl;

    // 设置初始参数（如果命令行未指定，则用包围盒半轴）
    double X[3];
    if (initGiven) {
        X[0] = initA; X[1] = initB; X[2] = initC;
    } else {
        double xmin=1e9, xmax=-1e9, ymin=1e9, ymax=-1e9, zmin=1e9, zmax=-1e9;
        for (size_t i = 0; i < pts.size(); i += 3) {
            if (pts[i]   < xmin) xmin = pts[i];
            if (pts[i]   > xmax) xmax = pts[i];
            if (pts[i+1] < ymin) ymin = pts[i+1];
            if (pts[i+1] > ymax) ymax = pts[i+1];
            if (pts[i+2] < zmin) zmin = pts[i+2];
            if (pts[i+2] > zmax) zmax = pts[i+2];
        }
        X[0] = (xmax - xmin) / 2.0;
        X[1] = (ymax - ymin) / 2.0;
        X[2] = (zmax - zmin) / 2.0;
        if (X[0] < 0.1) X[0] = 1.0;
        if (X[1] < 0.1) X[1] = 1.0;
        if (X[2] < 0.1) X[2] = 1.0;
    }
    std::cout << "Initial parameters: A=" << X[0] << ", B=" << X[1] << ", C=" << X[2] << std::endl;

    // 设置优化参数
    GaussNewtonParams params;
    params.exact_line_search = exactLineSearch;
    params.gradient_tolerance = gradTol;
    params.residual_tolerance = resTol;
    params.max_iter = maxIter;
    params.verbose = verbose;

    // 求解
    EllipseResidualFunction residual(pts);
    Solver5903 solver;
    GaussNewtonReport report;
    double finalF = solver.solve(&residual, X, params, &report);

    // 输出结果
    std::cout << "\n=== Optimization Results ===" << std::endl;
    std::cout << "Stop reason: ";
    switch (report.stop_type) {
        case GaussNewtonReport::STOP_GRAD_TOL:
            std::cout << "gradient tolerance reached";
            break;
        case GaussNewtonReport::STOP_RESIDUAL_TOL:
            std::cout << "residual tolerance reached";
            break;
        case GaussNewtonReport::STOP_NO_CONVERGE:
            std::cout << "max iterations reached (no converge)";
            break;
        case GaussNewtonReport::STOP_NUMERIC_FAILURE:
            std::cout << "numerical failure (SVD solve failed)";
            break;
    }
    std::cout << ", iterations: " << report.n_iter << std::endl;
    std::cout << "Final parameters: A = " << X[0] << ", B = " << X[1] << ", C = " << X[2] << std::endl;
    std::cout << "Final objective value F = " << finalF << std::endl;

    // 计算残差均方根 RMS
    double *R = new double[residual.nR()];
    residual.eval(R, nullptr, X);
    double rms = 0.0;
    for (int i = 0; i < residual.nR(); ++i) rms += R[i] * R[i];
    rms = std::sqrt(rms / residual.nR());
    std::cout << "Residual RMS = " << rms << std::endl;
    delete[] R;

    return 0;
}