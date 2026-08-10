#include <bits/stdc++.h>
using namespace std;
#define int long long

//欧拉筛
const int mod = 1e9 + 7;
const int maxn = 1e6;
vector<int> primes;
//欧拉筛 同时 预处理最小质因数 与 目标数分解质因数

int min_primes[maxn + 5];
void init(){
    for(int i = 2; i <= maxn; i++){
        if(!min_primes[i]){
            min_primes[i] = i;
            primes.push_back(i);
        }
        for(int p : primes){
            if(p > min_primes[i] || i * p > maxn) break;
            min_primes[i * p] = p;
        }
    }
}
//分解质因数
vector<int> getPrimes(int x){
    vector<int> res;
    while(x > 1){
        int p = min_primes[x];
        res.push_back(p);
        while(x % p == 0) x /= p;
    }
    return res;
}
//分解质因数带次数（幂）{p, cnt}
vector<pair<int,int>> getPrimesCnt(int x){
    vector<pair<int,int>> res;
    while(x > 1){
        int p = min_primes[x];
        int cnt = 0;
        while(x % p == 0){
            x /= p;
            cnt++;
        }
        res.push_back({p, cnt});
    }
    return res;
}


// 线性筛综合版：质数表 + 最小质因子 + 莫比乌斯函数 + 欧拉函数 + 约数个数 + 约数和
// 调用 linear_sieve(maxn) 后直接使用全局数组
bool is_comp[maxn + 5];
int mu[maxn + 5];          // 莫比乌斯函数
int phi_arr[maxn + 5];     // 欧拉函数
int div_cnt[maxn + 5];     // 约数个数
int div_sum[maxn + 5];     // 约数和
int minp_exp[maxn + 5];    // 最小质因子的指数
int minp_pwsum[maxn + 5];  // 1 + p + p^2 + ... + p^(最小质因子指数)

void linear_sieve(int n){
    mu[1] = 1; phi_arr[1] = 1; div_cnt[1] = 1; div_sum[1] = 1;
    for(int i = 2; i <= n; i++){
        if(!is_comp[i]){
            primes.push_back(i);
            mu[i] = -1;
            phi_arr[i] = i - 1;
            div_cnt[i] = 2;
            div_sum[i] = i + 1;
            minp_exp[i] = 1;
            minp_pwsum[i] = i + 1;
        }
        for(int p : primes){
            int m = i * p;
            if(m > n) break;
            is_comp[m] = true;
            if(i % p == 0){
                mu[m] = 0;
                phi_arr[m] = phi_arr[i] * p;
                minp_exp[m] = minp_exp[i] + 1;
                minp_pwsum[m] = minp_pwsum[i] * p + 1;
                div_cnt[m] = div_cnt[i] / (minp_exp[i] + 1) * (minp_exp[m] + 1);
                div_sum[m] = div_sum[i] / minp_pwsum[i] * minp_pwsum[m];
                break;
            }else{
                mu[m] = -mu[i];
                phi_arr[m] = phi_arr[i] * (p - 1);
                minp_exp[m] = 1;
                minp_pwsum[m] = p + 1;
                div_cnt[m] = div_cnt[i] * 2;
                div_sum[m] = div_sum[i] * (p + 1);
            }
        }
    }
}


// 旧欧拉筛单独函数
// void init(){
//     vector<bool> nop(maxn + 5);
//     nop[0] = nop[1] = true;
//     for(int i = 2; i <= maxn; i++){
//         if(!nop[i]){
//             primes.push_back(i);
//             if(i > sqrt(maxn)) continue;
//             for(int j = i * i; j <= maxn; j++){
//                 nop[j] = true;
//             }
//         }
//     }
// }


int ksm(int x, int a, int m){
    int ans = 1;
    while(a){
        if(a & 1) ans = ans * x % m;
        x = x * x % m;
        a >>= 1;
    }
    return ans;
}


//扩展欧几里得算法
int exgcd(int a, int b, int &x, int &y){
    if(!b){
        x = 1;
        y = 0;
        return a;
    }
    int d = exgcd(b, a % b, x, y);
    int t = x;
    x = y;
    y = t - (a / b) * y;
    return d;//最大公约数
}

//中国剩余定理(x === a (mod r) )
int CRT(int k, vector<int> a, vector<int> r){
    int n = 1, ans = 0;
    for(int i = 0; i < k; i++){
        n = n * r[i];
    }
    for(int i = 0; i < k; i++){
        int m = n / r[i], b, y;
        exgcd(m, r[i], b, y);
        ans = (ans + a[i] * m * b % n) % n;
    }
    return (ans % n + n) % n;
}

//扩展中国剩余定理
//求解方程组 x ≡ a[i] (mod m[i])
int excrt(vector<int> m, vector<int> a, int n){
    int M = m[0];
    int ans = a[0];
    for(int i = 1; i < n; i++){
        int c = (a[i] - ans % m[i] + m[i]) % m[i];
        int x, y;
        int d = exgcd(M, m[i], x, y);
        if(c % d) return -1;
        x = x * (c / d) % (m[i] / d);
        ans += x * M;
        M = M / d * m[i];
        ans = (ans % M + M) % M;
    }
    return ans;
}

//一个数的欧拉函数
int oula_phi(int n){
    int ans = n;
    for(int i = 2; i * i <= n; i++){
        if(n % i == 0){
            ans = ans / i * (i - 1);
            while(n % i == 0) n /= i;
        }
    }
    if(n > 1) ans = ans / n * (n - 1);
    return ans;
}

//筛法求欧拉函数
vector<int> sieve_phi(int n) {
    vector<int> phi(n + 1);
    vector<bool> st(n + 1, false);
    vector<int> primes;
    
    phi[1] = 1; // 特殊情况：1的欧拉函数值为1[1,3,5](@ref)
    
    for (int i = 2; i <= n; i++) {
        if (!st[i]) { // i是质数
            primes.push_back(i);
            phi[i] = i - 1; // 质数的欧拉函数值为i-1[1,2](@ref)
        }
        
        for (int j = 0; primes[j] <= n / i; j++) {
            st[primes[j] * i] = true;
            
            if (i % primes[j] == 0) {
                // primes[j]是i的最小质因子
                phi[primes[j] * i] = phi[i] * primes[j]; // 情况1[2,5](@ref)
                break;
            } else {
                // primes[j]与i互质
                phi[primes[j] * i] = phi[i] * (primes[j] - 1); // 情况2[1,3](@ref)
            }
        }
    }
    
    return phi;
}

// 莫比乌斯反演
// 若 g(n) = Σ_{d|n} f(d)，则 f(n) = Σ_{d|n} μ(d) * g(n/d)
// 若 g(n) = Σ_{n|d} f(d)，则 f(n) = Σ_{n|d} μ(d/n) * g(d)
// 常用结论:
//   [gcd(i, j) == 1] = Σ_{d | gcd(i,j)} μ(d)
//   φ(n) = Σ_{d|n} μ(d) * (n / d)
//   线性筛求 μ / φ 见上方 linear_sieve

// 狄利克雷前缀/后缀 变换（快速 zeta 变换，O(n log log n)）
// 需要先调用 linear_sieve(maxn) 得到全局质数表 primes
// 注意: 变换是原地进行的，若需保留原数组请先拷贝

// 前缀和: 变换后 f[n] = Σ_{d|n} f[d]
void dirichlet_prefix_sum(vector<int>& f, int n){
    for(int p : primes)
        for(int i = 1; i * p <= n; i++)
            f[i * p] += f[i];
}

// 后缀和: 变换后 f[d] = Σ_{d|n} f[n]
void dirichlet_suffix_sum(vector<int>& f, int n){
    for(int p : primes)
        for(int i = n / p; i >= 1; i--)
            f[i] += f[i * p];
}

// 前缀最值: 变换后 f[n] = max_{d|n} f[d]
void dirichlet_prefix_max(vector<int>& f, int n){
    for(int p : primes)
        for(int i = 1; i * p <= n; i++)
            f[i * p] = max(f[i * p], f[i]);
}

// 前缀最值: 变换后 f[n] = min_{d|n} f[d]
void dirichlet_prefix_min(vector<int>& f, int n){
    for(int p : primes)
        for(int i = 1; i * p <= n; i++)
            f[i * p] = min(f[i * p], f[i]);
}

// 前缀和的逆变换: 已知 g[n] = Σ_{d|n} f[d]，由 g 还原 f
void dirichlet_prefix_inv(vector<int>& f, int n){
    for(int p : primes)
        for(int i = n / p; i >= 1; i--)
            f[i * p] -= f[i];
}

// 后缀和的逆变换: 已知 g[d] = Σ_{d|n} f[n]，由 g 还原 f
void dirichlet_suffix_inv(vector<int>& f, int n){
    for(int p : primes)
        for(int i = 1; i * p <= n; i++)
            f[i] -= f[i * p];
}

// 狄利克雷卷积快速幂 - 计算f的k次卷积幂
void dirichlet_power(const vector<int>& f, vector<int>& res, int n, int k) {
    // 初始化结果为狄利克雷卷积的单位元ε
    res.assign(n + 1, 0);
    res[1] = 1;  // ε(1)=1, ε(n)=0(n>1)
    
    vector<int> temp(f), base(f);
    
    while (k) {
        if (k & 1) {
            vector<int> new_res(n + 1, 0);
            // 计算res = res * base
            for (int i = 1; i <= n; i++) {
                for (int j = 1; i * j <= n; j++) {
                    new_res[i * j] = (new_res[i * j] + res[i] * base[j]) % mod;
                }
            }
            res = new_res;
        }
        
        // 计算base = base * base
        vector<int> new_base(n + 1, 0);
        for (int i = 1; i <= n; i++) {
            for (int j = 1; i * j <= n; j++) {
                new_base[i * j] = (new_base[i * j] + base[i] * base[j]) % mod;
            }
        }
        base = new_base;
        
        k >>= 1;
    }
}

// 阶乘表，通常n不会太大，可以预先计算好
// 实际使用时可以根据需要动态计算或传入
const int fact[10] = {1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880};

/**
 * 康托展开，求排列的字典序排第几，O(n^2)
 * @param permutation 排列，元素为0..n-1或1..n均可，但需是连续不重复的整数
 * @return 康托展开值（即比当前排列小的排列个数）
 */
int cantor_expand(const std::vector<int>& permutation) {
    int n = permutation.size();
    int code = 0;
    
    for (int i = 0; i < n; ++i) {
        int smaller = 0;  // 统计未出现的元素中有多少个比当前元素小
        for (int j = i + 1; j < n; ++j) {
            if (permutation[j] < permutation[i]) {
                smaller++;
            }
        }
        code += smaller * fact[n - i - 1];
    }
    
    return code;
}

/**
 * 逆康托展开
 * @param code 康托展开值（比目标排列小的排列个数）
 * @param n 排列长度
 * @param elements 可选的元素集合，默认使用0..n-1；如果是1..n则需传入对应集合
 * @return 对应的排列
 */
std::vector<int> cantor_expand_inv(int code, int n, const std::vector<int>& elements = {}) {
    std::vector<int> result;
    
    // 确定可用的元素集合
    std::vector<int> available;
    if (elements.empty()) {
        // 默认使用0..n-1
        for (int i = 0; i < n; ++i) {
            available.push_back(i);
        }
    } else {
        available = elements;
    }
    
    for (int i = n; i > 0; --i) {
        int fact_val = fact[i - 1];
        int index = code / fact_val;      // 确定应该取第几小的数
        code %= fact_val;                  // 更新余数
        
        result.push_back(available[index]);
        available.erase(available.begin() + index);
    }
    
    return result;
}


// FFT 需要用 double，所以局部不建议用全局的 int int 覆盖所有变量
struct Complex {
    double r, i;
    Complex(double r = 0, double i = 0) : r(r), i(i) {}
    Complex operator + (const Complex& t) const { return {r + t.r, i + t.i}; }
    Complex operator - (const Complex& t) const { return {r - t.r, i - t.i}; }
    Complex operator * (const Complex& t) const { return {r * t.r - i * t.i, r * t.i + i * t.r}; }
};

const double PI = acos(-1.0);
vector<int> rev; // 预处理位逆序

void fft_init(int len) {
    rev.assign(len, 0);
    for (int i = 0; i < len; i++)
        rev[i] = (rev[i >> 1] >> 1) | ((i & 1) ? (len >> 1) : 0);
}

void fft(vector<Complex>& a, int type) {
    int n = a.size();
    for (int i = 0; i < n; i++) if (i < rev[i]) swap(a[i], a[rev[i]]);
    for (int mid = 1; mid < n; mid <<= 1) {
        Complex wn(cos(PI / mid), type * sin(PI / mid));
        for (int i = 0; i < n; i += (mid << 1)) {
            Complex w(1, 0);
            for (int j = 0; j < mid; j++, w = w * wn) {
                Complex x = a[i + j], y = w * a[i + j + mid];
                a[i + j] = x + y;
                a[i + j + mid] = x - y;
            }
        }
    }
    if (type == -1) for (auto& x : a) x.r /= n;
}

// 多项式乘法封装
vector<int> multiply_fft(vector<int>& A, vector<int>& B) {
    int n = A.size(), m = B.size(), total = n + m - 1;
    int len = 1; while (len < total) len <<= 1;
    fft_init(len);
    vector<Complex> fa(len), fb(len);
    for (int i = 0; i < n; i++) fa[i].r = A[i];
    for (int i = 0; i < m; i++) fb[i].r = B[i];
    fft(fa, 1); fft(fb, 1);
    for (int i = 0; i < len; i++) fa[i] = fa[i] * fb[i];
    fft(fa, -1);
    vector<int> res(total);
    for (int i = 0; i < total; i++) res[i] = (int)(fa[i].r + 0.5);
    return res;
}

// // 假设输入两个多项式的系数
//     vector<int> a = {1, 2, 1}; // x^2 + 2x + 1
//     vector<int> b = {1, 1};    // x + 1
    
//     int n = a.size(), m = b.size();
//     int len = 1;
//     while (len < n + m - 1) len <<= 1; // 补齐到2的幂次

//     vector<Complex> fa(len), fb(len);
//     for (int i = 0; i < n; i++) fa[i] = Complex(a[i], 0);
//     for (int i = 0; i < m; i++) fb[i] = Complex(b[i], 0);

//     fft(fa, len, 1);
//     fft(fb, len, 1);
    
//     // 点值相乘
//     for (int i = 0; i < len; i++) fa[i] = fa[i] * fb[i];

//     fft(fa, len, -1);

//     // 输出结果 (四舍五入)
//     for (int i = 0; i < n + m - 1; i++) {
//         cout << (int)(fa[i].r + 0.5) << " ";
//     }
//     return 0;


// ============================== NTT 快速数论变换 ==============================
// 模数 NTT_MOD = 998244353 = 119 * 2^23 + 1 是质数，
//   它减 1 后含足够多的因子 2，因此支持长度不超过 2^23 的 NTT。
// G = 3 是该模数的一个原根，GI 是 G 的逆元（给逆变换当单位根）。
// 与 FFT 相比: NTT 全程整数运算、无浮点误差，但每个系数都会先对 NTT_MOD 取模。
// 用法: 多项式系数按升幂存入 vector<int>，直接调用 multiply_ntt(A, B) 即可。
// 依赖上方通用快速幂 ksm。
const int NTT_MOD = 998244353;
const int G = 3;          // 998244353 的一个原根
const int GI = 332748118; // G 在模 NTT_MOD 下的逆元

// 位逆序表: ntt_rev[i] = i 按 log2(n) 位反转后的下标，变换前按当前长度 n 更新
vector<int> ntt_rev;
void ntt_rev_init(int n) {
    ntt_rev.resize(n);
    for (int i = 0; i < n; i++)
        ntt_rev[i] = (ntt_rev[i >> 1] >> 1) | ((i & 1) ? (n >> 1) : 0);
}

// 原地 NTT 变换（蝶形运算）
// type =  1: 正变换，系数表示 -> 点值表示
// type = -1: 逆变换，点值表示 -> 系数表示（结尾自动乘 1/n）
// 前置条件: a.size() 为 2 的幂（且 ≤ 2^23），并已按该长度调用过 ntt_rev_init
void ntt(vector<int>& a, int type) {
    int n = a.size();
    // 1. 位逆序重排，把系数放到分治正确的位置
    for (int i = 0; i < n; i++)
        if (i < ntt_rev[i]) swap(a[i], a[ntt_rev[i]]);
    // 2. 自底向上合并，len 为当前合并块的长度（2, 4, 8, ...）
    for (int len = 2; len <= n; len <<= 1) {
        int wlen = ksm(type == 1 ? G : GI, (NTT_MOD - 1) / len, NTT_MOD); // len 次单位根
        for (int i = 0; i < n; i += len) {      // 遍历每个块
            int w = 1;
            for (int j = 0; j < len / 2; j++, w = w * wlen % NTT_MOD) { // 块内蝶形
                int x = a[i + j];               // 左半边
                int y = w * a[i + j + len / 2] % NTT_MOD; // 右半边乘旋转因子
                a[i + j] = (x + y) % NTT_MOD;
                a[i + j + len / 2] = (x - y + NTT_MOD) % NTT_MOD;
            }
        }
    }
    if (type == -1) { // 3. 逆变换最后整体除以 n
        int inv_n = ksm(n, NTT_MOD - 2, NTT_MOD);
        for (int& x : a) x = x * inv_n % NTT_MOD;
    }
}

// NTT 加速的多项式乘法，返回系数均在 [0, NTT_MOD) 内的结果
// 返回 C，长度 n + m - 1，满足 C[k] = Σ_{i+j=k} A[i] * B[j] (mod NTT_MOD)
// 限制: n + m - 1 ≤ 2^23；超出需换大模数（如 469762049, 1004535809）或拆系数 MTT
// 使用示例: vector<int> C = multiply_ntt(A, B);
vector<int> multiply_ntt(const vector<int>& A, const vector<int>& B) {
    if (A.empty() || B.empty()) return {};      // 空多项式
    int n = A.size(), m = B.size(), total = n + m - 1;
    int len = 1;                                // 结果长度为 total，补到不小于它的 2 的幂
    while (len < total) len <<= 1;
    vector<int> fa(A), fb(B);                   // 拷贝一份，不修改入参
    fa.resize(len); fb.resize(len);
    ntt_rev_init(len);
    ntt(fa, 1); ntt(fb, 1);                     // 系数 -> 点值
    for (int i = 0; i < len; i++) fa[i] = fa[i] * fb[i] % NTT_MOD; // 点值相乘
    ntt(fa, -1);                                // 点值 -> 系数
    fa.resize(total);                           // 高次项系数均为 0，截掉
    return fa;
}

// 使用示例:
//   vector<int> A = {1, 2, 1};        // A(x) = 1 + 2x + x^2
//   vector<int> B = {1, 1};           // B(x) = 1 + x
//   vector<int> C = multiply_ntt(A, B); // C = {1, 3, 3, 1}


// ============================== 多项式求逆（NTT 加速） ==============================
// 求 B(x) 使得 A(x)·B(x) ≡ 1 (mod x^n)，所有系数均取模 NTT_MOD（998244353）
// 依赖: ksm + ntt + ntt_rev_init + multiply_ntt
// 要求: A[0] != 0（常数项在模意义下存在逆元）
// 原理: 牛顿迭代。若 B 满足 A·B ≡ 1 (mod x^m)，则
//         B' = B·(2 − A·B) 满足 A·B' ≡ 1 (mod x^{2m})
//       长度每次翻倍，总复杂度 O(n log n)。len = 4m 保证卷积无循环串位
vector<int> poly_inv(const vector<int>& A, int n){
    vector<int> B(1, ksm(A[0], NTT_MOD - 2, NTT_MOD)); // 初始 B0 = A[0]^{-1}
    int m = 1;                                         // 当前精度: 模 x^m
    while(m < n){
        int len = m << 2;                              // NTT 长度 4m
        vector<int> a(min((int)A.size(), m << 1)), b(B); // A 截断到 2m 项
        for(int i = 0; i < (int)a.size(); i++) a[i] = A[i];
        a.resize(len); b.resize(len);
        ntt_rev_init(len);
        ntt(a, 1); ntt(b, 1);
        for(int i = 0; i < len; i++)                   // 点值域: B(2 − A·B)
            a[i] = b[i] * (2 - a[i] * b[i] % NTT_MOD + NTT_MOD) % NTT_MOD;
        ntt(a, -1);
        B.assign(a.begin(), a.begin() + (m << 1));     // 精度翻倍到 2m
        m <<= 1;
    }
    B.resize(n);                                       // 截断到所需长度
    return B;
}
// 使用示例:
//   vector<int> A = {1, 1, 1};          // A(x) = 1 + x + x^2
//   vector<int> B = poly_inv(A, 3);     // B = {1, mod-1, 0}，即 1 − x
//   验证: vector<int> C = multiply_ntt(A, B); C.resize(3); // C = {1, 0, 0}


// 快速沃尔什变换 FWT（与/或/异或 卷积，O(n log n)，n 为 2 的幂，模 mod 运算）
// 用法: a、b 长度补成 2 的幂 → 各自正变换 → 点乘 → 逆变换

// 或卷积: c[k] = Σ_{i|j==k} a[i]*b[j]
void fwt_or(vector<int>& a, bool inv){
    int n = a.size();
    for(int len = 1; len < n; len <<= 1)
        for(int i = 0; i < n; i += len << 1)
            for(int j = 0; j < len; j++){
                int &x = a[i + j], &y = a[i + j + len];
                if(inv) y = (y - x + mod) % mod;
                else y = (y + x) % mod;
            }
}

// 与卷积: c[k] = Σ_{i&j==k} a[i]*b[j]
void fwt_and(vector<int>& a, bool inv){
    int n = a.size();
    for(int len = 1; len < n; len <<= 1)
        for(int i = 0; i < n; i += len << 1)
            for(int j = 0; j < len; j++){
                int &x = a[i + j], &y = a[i + j + len];
                if(inv) x = (x - y + mod) % mod;
                else x = (x + y) % mod;
            }
}

// 异或卷积: c[k] = Σ_{i^j==k} a[i]*b[j]
void fwt_xor(vector<int>& a, bool inv){
    int n = a.size();
    for(int len = 1; len < n; len <<= 1)
        for(int i = 0; i < n; i += len << 1)
            for(int j = 0; j < len; j++){
                int x = a[i + j], y = a[i + j + len];
                a[i + j] = (x + y) % mod;
                a[i + j + len] = (x - y + mod) % mod;
            }
    if(inv){
        int in = ksm(n, mod - 2, mod);
        for(int& v : a) v = v * in % mod;
    }
}

// FWT 卷积封装，传入 fwt_or / fwt_and / fwt_xor
vector<int> fwt_conv(vector<int> a, vector<int> b, void (*f)(vector<int>&, bool)){
    int n = 1, mx = max(a.size(), b.size());
    while(n < mx) n <<= 1;
    a.resize(n); b.resize(n);
    f(a, false); f(b, false);
    for(int i = 0; i < n; i++) a[i] = a[i] * b[i] % mod;
    f(a, true);
    return a;
}

// 整除分块: Σ_{i=1}^{n} floor(n/i) 取值只有 O(√n) 段
// 每段 [l, r] 内 floor(n/i) 相同，r = n / (n / l)
int div_block_sum(int n){
    int ans = 0;
    for(int l = 1, r; l <= n; l = r + 1){
        r = n / (n / l);
        ans += (n / l) * (r - l + 1);
    }
    return ans;
}
// 推广: Σ_{i=1}^{min(n,m)} floor(n/i) * floor(m/i) 同理按 r = min(n/(n/l), m/(m/l)) 分段


// 类欧几里得算法 log求和 sum = i(0 - n-1) floor [(a*i + b) / c]
// 求i从0到n-1的ai+b/c向下取整的总和
// n >= 0, m > 0, a >= 0, b >= 0
int floor_sum(int n, int m, int a, int b) {
    int ans = 0;
    if (a >= m) {
        ans += (n - 1) * n / 2 * (a / m);
        a %= m;
    }
    if (b >= m) {
        ans += n * (b / m);
        b %= m;
    }

    int y_max = (a * n + b) / m;
    int x_max = (y_max * m - b);
    if (y_max == 0) return ans;

    ans += (n - (x_max + a - 1) / a) * y_max;
    ans += floor_sum(y_max, a, m, (a - x_max % a) % a);
    return ans;
}

//SOSDP
signed main() {
    int n = 50;
    int total_states = (1 << n);
    vector<int> dp(total_states, 0); // dp数组初始化，根据题意可能初始为A[i]

    // SOS DP 计算超集和
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < total_states; ++j) {
            if ((j >> i) & 1) { // 如果状态j的第i位是1
                dp[j] += dp[j ^ (1 << i)]; // 累加不含第i位的状态的值
            }
        }
    }

    // 此时dp[mask]存储了所有满足 (i & mask) == mask 的A[i]之和
    return 0;
}