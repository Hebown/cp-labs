#ifndef _SPARSE_H
#define _SPARSE_H

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>
/* 如需其他库，请在此包含 */

typedef std::vector<double> Vecd;
typedef std::vector<int> Veci;

/**
 * @brief 稀疏矩阵类。
 * 假设：
 * 1. 非零元素个数在 int 范围内
 * 2. 所有元素精度为 1e-10
 *    （若 fabs(x) < 1e-10，则认为 x 为 0）
*/
class Sparse {
private:
    const double epsilon = 1e-10;   // 精度

    /* 如需其他私有成员变量和函数，请在此定义 */
    /* 我决定采用压缩行存储的朴素实现 */
    Vecd vals;
    Veci col_ind;
    Veci row_ptr;
    int m,n;
public:
    /* 如需构造函数和析构函数，请在此定义 */
    Sparse(int m,int n);
    ~Sparse();

    /**
     * @brief 读取矩阵 Matrix[row][column] 处的元素
     * 
     * @param row 行索引，从 0 开始，如 0, 1, 2, 3 ...
     * @param col 列索引，从 0 开始，如 0, 1, 2, 3 ...
    */
    double at(int row, int col) const;

    /**
     * @brief 在 Matrix[row][column] 处插入/修改元素
     * 
     * @param val 要插入/修改的值
     * @param row 行索引，从 0 开始，如 0, 1, 2, 3 ...
     * @param col 列索引，从 0 开始，如 0, 1, 2, 3 ...
    */
    void insert(double val, int row, int col);

    /**
     * @brief 使用三个等长向量初始化稀疏矩阵
     * 
     * @param rows 行索引
     * @param cols 列索引
     * @param vals 值。vals[i] = Matrix[rows[i]][cols[i]]
    */
    
    void initializeFromVector(const Veci& rows, const Veci& cols, const Vecd& input_vals);

    /* 如需其他公有成员变量和函数，请在此定义 */
    void matvec(const Vecd& x, Vecd& result) const;

    
    private:
        int getIndexOfElement(int row,int col) const;
        inline void checkInputRange(int row,int col) const;
        struct Element{
            int row,col;
            double val;
            bool operator<(const Element&e)const noexcept{
                return (this->row<e.row)||(this->row==e.row&&this->col<e.col);
            }
        };
    
};

#endif