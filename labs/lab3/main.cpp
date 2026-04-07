// main.cpp
#include "sparse.h"
#include "hw3_solve.h"


int main() {
    /* 测试稀疏矩阵 */
    Sparse S(6,6); // 这里必须指定稀疏矩阵的长和宽

    Veci rows{0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5};
    Veci cols{0, 4, 0, 1, 5, 1, 2, 3, 0, 2, 3, 4, 1, 3, 4, 5, 1, 4, 5};
    Vecd vals{10, -2, 3, 9, 3, 7, 8, 7, 3, 8, 7, 5, 8, 9, 9, 13, 4, 2, -1};
    S.initializeFromVector(rows, cols, vals);

    std::cout << "矩阵：" << std::endl;
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++)
            std::cout << S.at(i, j) << ' ';
        std::cout << std::endl;
    }

    std::cout << std::endl << "insert(3, 4, 4)" << std::endl;
    S.insert(3, 4, 4);

    std::cout << "矩阵：" << std::endl;
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++)
            std::cout << S.at(i, j) << ' ';
        std::cout << std::endl;
    }

    /* 测试高斯-赛德尔方法 */
    vals = {10, -1, 2, 0, -1, 11, -1, 3, 2, -1, 10, -1, 0, 3, -1, 8};
    cols = {0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3};
    rows = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3};
    Vecd b = {6, 25, -11, 15};
    S.initializeFromVector(rows, cols, vals);

    Vecd x = Gauss_Seidel(S, b, 1e-12);
    std::cout << std::endl << "高斯-赛德尔结果：" << std::endl;
    for (int i = 0; i < x.size(); i++)
        std::cout << x[i] << ' ';
    std::cout << std::endl;

    // /* （加分项）测试共轭梯度法 */
    // x = Conjugate_Gradient(S, b, 1e-12, 100);
    // std::cout << std::endl << "共轭梯度结果：" << std::endl;
    // for (int i = 0; i < x.size(); i++)
    //     std::cout << x[i] << ' ';
    // std::cout << std::endl;

    return 0;
}