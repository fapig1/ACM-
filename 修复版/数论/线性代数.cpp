#include <bits/stdc++.h>
using namespace std;
#define int long long

const int mod = 1e9 + 7;

//线性基LinearBasis
vector<int> p(64);
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

//矩阵 (0-based)
struct mat{
    int n, m;
    vector<vector<int>> a;
    // 单参数 mat(n) 构造 n*n 方阵 (原版 y 默认 0, 会得到 n*0 矩阵导致越界)
    mat(int x = 0, int y = -1){
        n = x;
        m = (y < 0 ? x : y);
        a.resize(n, vector<int> (m));
    }

    mat operator+ (const mat& b){
        mat c(n, m);
        for(int i = 0; i < n; i++){
            for(int j = 0; j < m; j++){
                c.a[i][j] = (a[i][j] + b.a[i][j]) % mod;
            }
        }
        return c;
    } 

    mat operator- (const mat& b){
        mat c(n, m);
        for(int i = 0; i < n; i++){
            for(int j = 0; j < m; j++){
                c.a[i][j] = ((a[i][j] - b.a[i][j]) % mod + mod) % mod;
            }
        }
        return c;
    } 

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

//实数高斯消元 (0-based)
const double eps = 1e-9;

//解线性方程组：a为n行m+1列增广矩阵（n个方程，m个未知数）
//返回1有唯一解并写入ans，0无解，2有无穷多解；精度不足时把double换成long double
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

//实数行列式 (0-based)
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

//实数矩阵求逆 (0-based)：矩阵奇异返回false，否则逆矩阵写入b
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

//模质数快速幂
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

//模质数高斯消元解线性方程组 (0-based)：a为n行n+1列增广矩阵
//返回1有唯一解并写入ans，0无解，2有无穷多解
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

//模质数行列式 (0-based)
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

//矩阵树定理：无向图生成树计数 (点0-based，重边合法，自环自动忽略)
//返回生成树个数模mod，图不连通返回0；带权图把边权当重数即得各树边权乘积之和
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
