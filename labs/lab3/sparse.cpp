#include "sparse.h"
#include <execution>
#include <omp.h>
Sparse::Sparse(int m,int n):m(m),n(n){
    row_ptr.resize(m + 1, 0);
}

Sparse::~Sparse() {}

double Sparse::at(int row, int col) const{
    int index=getIndexOfElement(row, col);
    return index==-1?0:vals[index];
}

void Sparse::insert(double val, int row, int col){
    int index = getIndexOfElement(row, col);
    if (std::fabs(val) < epsilon) {// 添加精度检查
        if (index != -1) {
            vals.erase(vals.begin() + index);
            col_ind.erase(col_ind.begin() + index);
            for (int i = row + 1; i <= m; ++i) {
                row_ptr[i]--;
            }
        }
        return;
    }
    if(index==-1){// 说明是插入
        // 主要修改三个内容：vals/col_ind/row_ptr
        // 步骤如下：插入到某一行，检查相应的 row_ptr，然后配套修改对应的 vals/col_ind 即可
        // 这里不需要再检查 row/col 是否合法，因为 getIndex 已经检查过了一遍
        int start=row_ptr[row];
        int end=row_ptr[row+1];
        int pos=end; 
        // 如何插入到 row_ptr 中呢，应该插入到第一个大于这个 index 的值的前面（顺序遍历时）
        // 这个值如果是 row 的第一个，需要修改 row_ptr
        // 用默认插入到行尾的方式可以简化判断
        for (int i = start; i < end; ++i) {
            if (col_ind[i] > col) {
                pos = i;
                break;
            }
        }
        col_ind.insert(col_ind.begin() + pos, col);
        vals.insert(vals.begin() + pos, val);
        for(int i=row+1;i<=m;i++){
            row_ptr[i]++;
        }
    }else{
        vals[index]=val; // 说明是修改，这里什么都不用动，改个数值就行了
    }
}

void Sparse::initializeFromVector(const Veci& rows, const Veci& cols, const Vecd& input_vals){
    /* 
        以向量初始化的思路还是去填充那三个数组
        如果这些向量按照行优先的方式来存储的话，做起来会方便很多，我们只需要一个一个push_back即可
        然而这个函数并没有保证这一点，所以我们的第一步是把这几个东西改进，而不是全部insert
    */
    assert(rows.size()==cols.size()&&cols.size()==input_vals.size()&& rows.size()>0);
    row_ptr.assign(m+1, 0);
    vals.clear();
    col_ind.clear();
    const int nz = rows.size();
    
    std::vector<int> indices(nz);
    for (int i = 0; i < nz; ++i) indices[i] = i;
    std::sort(std::execution::par,indices.begin(), indices.end(), [&](int i, int j) {
        if (rows[i] != rows[j]) return rows[i] < rows[j];
        return cols[i] < cols[j];
    });
    
    // 按照字典序排序完之后就可以线性填入了
    // 这里我们使用了row_ptr的一个语义：前i行共有多少非0元素
    // 统计每行非零元个数
    std::vector<int> row_count(m, 0);
    for (int idx : indices) {
        row_count[rows[idx]]++;
    }
    // 计算 row_ptr（前缀和）
    row_ptr[0] = 0;
    for (int i = 1; i <= m; ++i) {
        row_ptr[i] = row_ptr[i - 1] + row_count[i - 1];
    }
    // 由此我们就可以填充了
    // 填充 col_ind 和 vals，使用临时指针记录当前行填充位置
    std::vector<int> row_pos = row_ptr; // 拷贝，用于记录当前行的下一个填充位置
    vals.resize(nz);
    col_ind.resize(nz);
    for (int idx : indices) {
        int row = rows[idx];
        int col = cols[idx];
        double val = input_vals[idx];
        int pos = row_pos[row]++;
        vals[pos] = val;
        col_ind[pos] = col;
    }
}

int Sparse::getIndexOfElement(int row,int col) const{
    checkInputRange(row, col);
    /* CRS 的矩阵的 row_ptr 指定了第 row 行的第一个元素，因此往后查找即可 */
    
    int lower_bound=row_ptr[row];// 下界由此决定
    int upper_bound=row_ptr[row+1]; // 上界总是存在，因为 m+1 索引处存放总非零元素个数
    for(int i=lower_bound;i<upper_bound;i++){
        if(col_ind[i]==col){
            return i; // 找到说明是非零值
        }
    }return -1; // 未找到说明是零值
}

inline void Sparse::checkInputRange(int row,int col) const{
    if(row >= m||row < 0){ // 超出矩阵范围
        throw std::runtime_error("at 所接受的参数 row 不合法");
    }
    if(col >= n||col < 0){ // 超出矩阵范围
        throw std::runtime_error("at 所接受的参数 col 不合法");
    }
}


void Sparse::matvec(const Vecd& x, Vecd& result) const {
    result.assign(m, 0.0);
    #pragma omp parallel for
    for (int i = 0; i < m; ++i) {
        double sum = 0.0;
        int start = row_ptr[i];
        int end = row_ptr[i+1];
        for (int k = start; k < end; ++k) {
            sum += vals[k] * x[col_ind[k]];
        }
        result[i] = sum;
    }
}