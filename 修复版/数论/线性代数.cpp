#include <bits/stdc++.h>
using namespace std;
#define int long long

// ============================== 线性基与矩阵 ==============================
// 内容: 异或线性基、矩阵类（加减乘、转置、快速幂）、实数/模质数高斯消元、
//       行列式、矩阵求逆、矩阵树定理。
// 约定:
//   * 矩阵与向量下标均从 0 开始；mat(n) 为 n×n，mat(n,m) 为 n 行 m 列。
//   * 取模统一用 mod = 1e9+7；mod 必须是质数（消元/求逆用费马小定理）。
//   * 实数版本 eps = 1e-9；精度不足时把 double 换成 long double。
//   * 本文件 #define int long long，与标准库函数拼接时注意重载冲突。
// =====================================================================

const int mod = 1e9 + 7;

// ============================== 异或线性基 ==============================
// 维护一组数的异或张成空间（值域 0..2^63-1）。
// p[i] 存“最高位为 i”的基向量；insert(x) 返回 x 是否增加了秩。
// 性质:
//   * 非零 p[i] 线性无关，张成空间等于所有已插入数的异或空间；
//   * 最大异或和: 从高位到低位贪心 ans = max(ans, ans ^ p[i])；
//   * 判断 x 能否被表出: 把 insert 的消元过程走一遍，看是否变成 0。
// 复杂度: 单次插入 O(64)，空间 O(64)。
// 注意: 这里只做“行阶梯”插入；若要查询第 k 小异或和，还需再消成行最简并特判 0。
vector<int> p(64);
// 【速查】异或线性基
// 用法: insert(x) 插入（返回是否让秩 +1）；最大异或和: ans = 0; for i 从 63 到 0: ans = max(ans, ans ^ p[i])
// 用途: 一堆数里选若干个异或出最大值 / 判断 x 能否被异或出来
// 复杂度: 每次 O(64)
bool insert(int x){
    for(int i = 63; i >= 0; i--){
        if(!(x >> i)) continue;
        if(!p[i]){
            p[i] = x;
            return true;
        }
        x ^= p[i];
    }
    return false;
}

// 【速查】矩阵（加减乘、转置、快速幂，mod 1e9+7）
// 用法: mat a(n) 方阵 / mat a(n, m)；a.a[i][j] 0-based；a * b、a + b；ksm(a, k) 矩阵快速幂
// 用途: 线性递推加速（如 1e18 项斐波那契）
// 复杂度: 乘法 O(n³)，快速幂 O(n³ log k)
// 矩阵类 (0-based)：mat(n) 为 n×n 方阵，mat(n,m) 为 n 行 m 列零矩阵
// 所有元素默认 0；算术运算全部对 mod 取模。
// 注意: 运算符不做形状检查，调用方需保证:
//       +、- 两边同形；* 要求 this->m == b.n；快速幂要求方阵。
struct mat{
    int n, m;
    vector<vector<int>> a;
    // 单参数 mat(n) 构造 n*n 方阵 (原版 y 默认 0, 会得到 n*0 矩阵导致越界)
    mat(int x = 0, int y = -1){
        n = x;
        m = (y < 0 ? x : y);
        a.resize(n, vector<int> (m));
    }

    // 矩阵加法，O(n*m)
    mat operator+ (const mat& b){
        mat c(n, m);
        for(int i = 0; i < n; i++){
            for(int j = 0; j < m; j++){
                c.a[i][j] = (a[i][j] + b.a[i][j]) % mod;
            }
        }
        return c;
    } 

    // 矩阵减法（结果调整到 [0,mod)），O(n*m)
    mat operator- (const mat& b){
        mat c(n, m);
        for(int i = 0; i < n; i++){
            for(int j = 0; j < m; j++){
                c.a[i][j] = ((a[i][j] - b.a[i][j]) % mod + mod) % mod;
            }
        }
        return c;
    } 

    // 矩阵乘法，O(n*m*b.m)；内层不取模会溢出 long long，这里每步都取模
    mat operator* (const mat& b){
        mat c(n, b.m);
        for(int i = 0; i < n; i++){
            for(int j = 0; j < b.m; j++){
                for(int k = 0; k < m; k++){
                    c.a[i][j] = (c.a[i][j] + a[i][k] * b.a[k][j]) % mod;
                }
            }
        }
        return c;
    } 

    // 转置，返回 m×n 矩阵，O(n*m)
    mat transpose(){
        mat res(m, n);
        for(int i = 0; i < n; i++){
            for(int j = 0; j < m; j++){
                res.a[j][i] = a[i][j];
            }
        }
        return res;
    }

    void print(){
        for(int i = 0; i < n; i++){
            for(int j = 0; j < m; j++){
                cout << a[i][j] << " ";
            }
            cout << '\n';
        }
    }

};

// 矩阵快速幂: 计算 x^a（x 必须是方阵，a >= 0），O(n^3 log a)
// 单位矩阵作为累乘初值；a=0 时返回单位矩阵（即使 x 是零矩阵）。
mat ksm(mat x, int a){
    int n = x.a.size();
    mat res(n, n);
    for(int i = 0; i < n; i++){
        res.a[i][i] = 1;
    }
    while(a){
        if(a & 1) res = res * x;
        x = x * x;
        a >>= 1;
    }
    return res;
}

// 实数高斯消元 - 解线性方程组 (0-based)
const double eps = 1e-9;

// 【速查】实数高斯消元解线性方程组
// 用法: a 为 n 行 (m+1) 列增广矩阵（最后一列是常数），gauss(a, n, m, ans)
// 返回: 1 唯一解（写进 ans）/ 0 无解 / 2 无穷多解
// 复杂度: O(n·m·min(n, m))
// 解线性方程组：a 为 n 行 m+1 列增广矩阵（n 个方程，m 个未知数）
// 返回 1 有唯一解并写入 ans，0 无解，2 有无穷多解。
// 做法: 逐列选绝对值最大的行做主元（部分主元，减小误差），
//       把主元行归一化后消去其余各行该列；r 为最终秩。
// 注意:
//   * a 按值传入，函数内交换行不影响调用方；
//   * 无穷多解时只返回标记，不给出自由变量的一组特解/基础解系；
//   * 精度不足时把 double 换成 long double 并相应调整 eps。
// 复杂度 O(n*m*min(n,m))。
int gauss(vector<vector<double>> a, int n, int m, vector<double> &ans){
    int r = 0;
    for(int c = 0; c < m && r < n; c++){
        int t = r;
        for(int i = r; i < n; i++){
            if(fabs(a[i][c]) > fabs(a[t][c])) t = i;
        }
        if(fabs(a[t][c]) < eps) continue;
        swap(a[t], a[r]);
        for(int j = m; j >= c; j--) a[r][j] /= a[r][c];
        for(int i = 0; i < n; i++){
            if(i != r && fabs(a[i][c]) > eps){
                double f = a[i][c];
                for(int j = c; j <= m; j++){
                    a[i][j] -= f * a[r][j];
                }
            }
        }
        r++;
    }
    for(int i = r; i < n; i++){
        if(fabs(a[i][m]) > eps) return 0;
    }
    if(r < m) return 2;
    ans.assign(m, 0);
    for(int i = 0; i < m; i++) ans[i] = a[i][m];
    return 1;
}

// 【速查】实数行列式 det(a)，O(n³)
// 实数行列式 (0-based)：高斯消元化为上三角，O(n^3)
// 每次选列主元；交换两行时 det 变号；det = 上三角对角线乘积。
// 主元绝对值 < eps 视为 0，直接返回 0（奇异）。
double det(vector<vector<double>> a){
    int n = a.size();
    double res = 1;
    for(int i = 0; i < n; i++){
        int t = i;
        for(int j = i + 1; j < n; j++){
            if(fabs(a[j][i]) > fabs(a[t][i])) t = j;
        }
        if(fabs(a[t][i]) < eps) return 0;
        if(t != i){
            swap(a[t], a[i]);
            res = -res;
        }
        res *= a[i][i];
        for(int j = i + 1; j < n; j++){
            double f = a[j][i] / a[i][i];
            for(int k = i; k < n; k++){
                a[j][k] -= f * a[i][k];
            }
        }
    }
    return res;
}

// 【速查】实数矩阵求逆：inv(a, b) 成功返回 true，逆矩阵写入 b；奇异返回 false。O(n³)
// 实数矩阵求逆 (0-based)：用 Gauss-Jordan 消元把 [A | I] 化成 [I | A^{-1}]。
// 矩阵奇异返回 false；否则逆矩阵写入 b（b 会被重新分配为 n×n）。
// 复杂度 O(n^3)；同样使用部分主元控制误差，eps 判定同 gauss。
bool inv(vector<vector<double>> a, vector<vector<double>> &b){
    int n = a.size();
    b.assign(n, vector<double>(n, 0));
    for(int i = 0; i < n; i++) b[i][i] = 1;
    for(int i = 0; i < n; i++){
        int t = i;
        for(int j = i + 1; j < n; j++){
            if(fabs(a[j][i]) > fabs(a[t][i])) t = j;
        }
        if(fabs(a[t][i]) < eps) return false;
        swap(a[t], a[i]);
        swap(b[t], b[i]);
        double d = a[i][i];
        for(int j = 0; j < n; j++){
            a[i][j] /= d;
            b[i][j] /= d;
        }
        for(int k = 0; k < n; k++){
            if(k != i){
                double f = a[k][i];
                for(int j = 0; j < n; j++){
                    a[k][j] -= f * a[i][j];
                    b[k][j] -= f * b[i][j];
                }
            }
        }
    }
    return true;
}

// 模质数快速幂: a^b mod mod，O(log b)；要求 b >= 0，mod 为质数（用于费马逆元）
int qpow(int a, int b){
    int res = 1;
    a %= mod;
    while(b){
        if(b & 1) res = res * a % mod;
        a = a * a % mod;
        b >>= 1;
    }
    return res;
}

// 【速查】模质数高斯消元（n 个未知数 n 个方程）
// 用法: a 为 n 行 (n+1) 列增广矩阵，元素先规整到 [0, mod)；返回 1 唯一解 / 0 无解 / 2 无穷多解
// 复杂度: O(n³)
// 模质数高斯消元解线性方程组 (0-based)：a 为 n 行 n+1 列增广矩阵
// 返回 1 有唯一解并写入 ans，0 无解，2 有无穷多解
// 与实数版相同，但主元取第一个非零元；用费马小定理求逆元，要求 mod 为质数。
// 复杂度 O(n^3)；a 按值传入。
int gauss_mod(vector<vector<int>> a, int n, vector<int> &ans){
    int r = 0;
    for(int c = 0; c < n && r < n; c++){
        int t = -1;
        for(int i = r; i < n; i++){
            if(a[i][c]){
                t = i;
                break;
            }
        }
        if(t == -1) continue;
        swap(a[t], a[r]);
        int iv = qpow(a[r][c], mod - 2);
        for(int j = c; j <= n; j++){
            a[r][j] = a[r][j] * iv % mod;
        }
        for(int i = 0; i < n; i++){
            if(i != r && a[i][c]){
                int f = a[i][c];
                for(int j = c; j <= n; j++){
                    a[i][j] = ((a[i][j] - f * a[r][j]) % mod + mod) % mod;
                }
            }
        }
        r++;
    }
    for(int i = r; i < n; i++){
        if(a[i][n]) return 0;
    }
    if(r < n) return 2;
    ans.assign(n, 0);
    for(int i = 0; i < n; i++) ans[i] = a[i][n];
    return 1;
}

// 【速查】模质数行列式 det_mod(a)，元素在 [0, mod)，O(n³)
// 模质数行列式 (0-based)：高斯消元化为上三角，O(n^3)
// 交换两行时 res 变号（写成 mod-res）；主元为 0 则行列式为 0。
int det_mod(vector<vector<int>> a){
    int n = a.size();
    int res = 1;
    for(int i = 0; i < n; i++){
        int t = -1;
        for(int j = i; j < n; j++){
            if(a[j][i]){
                t = j;
                break;
            }
        }
        if(t == -1) return 0;
        if(t != i){
            swap(a[t], a[i]);
            res = mod - res;
        }
        res = res * a[i][i] % mod;
        int iv = qpow(a[i][i], mod - 2);
        for(int j = i + 1; j < n; j++){
            if(!a[j][i]) continue;
            int f = a[j][i] * iv % mod;
            for(int k = i; k < n; k++){
                a[j][k] = ((a[j][k] - f * a[i][k]) % mod + mod) % mod;
            }
        }
    }
    return res;
}

// 【速查】矩阵树定理：无向图生成树个数 (mod)
// 用法: matrix_tree(n, edges)，点 0..n-1，edges 为无向边（可重边，自环忽略）；不连通返回 0
// 复杂度: O(n³ + m)
// ============================== 矩阵树定理 (Kirchhoff) ==============================
// 无向图生成树计数：构造 Laplace 矩阵 L = D - A，删去任意一行一列后求行列式，
// 其绝对值就是生成树个数（这里对 mod 取模，mod 为质数）。
// 参数: n 个点（0-based），edges 为无向边列表（每条边只写一次）。
// 约定:
//   * 重边按重数计入；自环 (u==u) 直接跳过（对生成树无贡献）；
//   * n <= 1 时约定只有 1 棵空树，返回 1；
//   * 带权图把边权 w 当作 w 条重边，得到的是“各生成树边权乘积之和”；
//   * 图不连通时行列式为 0。
// 复杂度 O(n^3 + m)。
int matrix_tree(int n, vector<pair<int,int>> edges){
    if(n <= 1) return 1;
    vector<vector<int>> L(n, vector<int>(n, 0));
    for(auto &e : edges){
        int u = e.first, v = e.second;
        if(u == v) continue;
        L[u][u]++, L[v][v]++;
        L[u][v]--, L[v][u]--;
    }
    //删去第0行第0列，对剩余(n-1)x(n-1)子矩阵求行列式
    int sz = n - 1;
    vector<vector<int>> a(sz, vector<int>(sz));
    for(int i = 1; i < n; i++){
        for(int j = 1; j < n; j++){
            a[i - 1][j - 1] = (L[i][j] % mod + mod) % mod;
        }
    }
    return det_mod(a);
}
