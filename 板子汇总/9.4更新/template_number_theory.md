# David_数论



[TOC]

### 排列组合

```cpp
const int mod = 1e9 + 7;
const int maxn = 2e5;


//模逆元 排列数
vector<int> fac(maxn + 5), inv(maxn + 5);
int ksm(int x, int a, int mod){
    int ans = 1;
    while(a){
        if(a & 1) ans = ans * x % mod;
        x = x * x % mod;
        a >>= 1;
    }
    return ans;
}

void init(){
    fac[0] = 1;
    for(int i = 1; i <= maxn; i++){
        fac[i] = fac[i - 1] * i % mod;
    }
    inv[maxn] = ksm(fac[maxn], mod - 2, mod);
    for(int i = maxn - 1; i >= 0; i--){
        inv[i] = inv[i + 1] * (i + 1) % mod;
    }
}

int A(int n, int m){
    return fac[n] * inv[n - m] % mod;
}

int C(int n, int m){
    return fac[n] * inv[m] % mod * inv[n - m] % mod;
}

//错位排列Derangement
int Derangement(int n){
    vector<int> dp(n + 1);
    dp[1] = 0;
    dp[2] = 1;
    for(int i = 3; i <= n; i++){
        dp[i] = (n - 1) * (dp[i - 1] + dp[i - 2]) % mod;
    }
}

//卡特兰数CatalanNumber 解决不交叉问题的种类数
//有 n 个左括号与 n 个右括号。能组成多少个合法括号序列？
int CatalanNumber(int n){
    int ans = C(2 * n, n) - C(2 * n, n + 1);
    return (ans % mod + mod) % mod;
}
```

### 数论基础

```cpp
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
// 前提: NTT 需要质数模数 p = c * 2^k + 1 (NTT 友好模数), 变换长度只支持 2 的幂且 ≤ 2^k。
// 本模板: NTT_MOD = 998244353 = 119 * 2^23 + 1  →  长度上限 2^23 ≈ 8.4e6。
//
// (NTT_MOD, G) 是一对 (模数, 原根), 两者的关系是 NTT 的核心:
//   1) 模 p 的乘法群 (Z/pZ)* 是循环群, 阶为 p−1 = 119·2^23;
//   2) G = 3 是该群的一个"原根"(生成元): 3 的阶恰好等于 p−1
//      (比 p−1 小的任何次幂都不等于 1), 所以 3 的各次幂能遍历全部非零元素;
//   3) 只要 len | p−1, 则 w_len = G^((p−1)/len) mod p 就是 len 次本原单位根:
//      w_len^len = G^(p−1) ≡ 1 (费马小定理), 且 w_len 的阶恰为 len,
//      这正是 NTT 蝶形运算每一层 len 所用的旋转因子 (见下方 ntt 里 wlen);
//   4) 逆变换需要逆单位根 w_len^{-1} = (G^{-1})^((p−1)/len),
//      因此定义 GI = G^{-1} = 332748118 (3·332748118 ≡ 1 mod p);
//   5) 一句话总结: 模数决定"能变换多长"(看 p−1 里因子 2 的个数),
//      原根决定"具体用哪个单位根"(由原根的幂次生成)。
//
// 注意: 换模数必须同步换原根。常用 (模数, 原根) 组合 (3 是这些模数的公共原根, 非巧合):
//   167772161  = 5·2^25 + 1   → 长度上限 2^25
//   469762049  = 7·2^26 + 1   → 长度上限 2^26
//   998244353  = 119·2^23 + 1 → 长度上限 2^23 (本模板)
//   1004535809 = 479·2^21 + 1 → 长度上限 2^21
// 原根判定法: 对 p−1 的每个质因子 q, 若 g^((p−1)/q) ≢ 1 (mod p), 则 g 是原根。
// 与 FFT 相比: NTT 全程整数运算、无浮点误差, 但每个系数都会先对 NTT_MOD 取模。
// 用法: 多项式系数按升幂存入 vector<int>，直接调用 multiply_ntt(A, B) 即可。
// 依赖上方通用快速幂 ksm。
const int NTT_MOD = 998244353;
const int G = 3;          // NTT_MOD 的原根 (生成元): w_len = G^((p-1)/len)
const int GI = 332748118; // G 的逆元: 逆变换用 w_len^{-1} = GI^((p-1)/len)

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


// ============================== 杜教筛 ==============================
// 用途: 线性筛 O(n) 只能处理 n ≤ 1e7~1e8; 杜教筛在 O(n^{2/3}) 内求积性函数前缀和
//       S(n) = Σ_{i=1}^n f(i), n 可到 1e9 ~ 1e10 (更大需调阈值并注意内存)。
// 原理:
//   选 g 使 h = f * g (狄利克雷卷积) 的前缀和 H(n) = Σ_{i≤n} h(i) 能 O(1) 求, 由
//       H(n) = Σ_{d=1}^n g(d) S(⌊n/d⌋)  移项得
//       S(n) = (H(n) − Σ_{d=2}^n g(d) S(⌊n/d⌋)) / g(1)
//   右边 Σ 用整除分块压成 O(√n) 段; 小值 v ≤ T 由线性筛预处理。
// 常见配对: (μ, 1)     → h = ε,    H(n) = 1          (梅滕斯函数)
//           (φ, 1)     → h = id,   H(n) = n(n+1)/2
//           (μ·id^k, id^k) → h = ε
// 记忆化不需要哈希表: 递归查询的键全是 ⌊N/i⌋ 形式 (⌊⌊N/a⌋/b⌋ = ⌊N/(ab)⌋),
//   只有 2√N 个不同值; 键 v ≤ T 查线性筛前缀, 键 v > T 存 memo[N/v] (N/v < N/T, 下标安全)。
//   注意"大键换小下标": 进记忆化分支的键必然 v > T, 所以下标 N/v < N/T 很小;
//   例如 N=1e9, T≈1e6 时, v=2 直接查 smu[2], 大键 v=1e9/5e8 的下标只有 1/2,
//   数组只开 N/T+2 ≈ 1002 个元素, 绝不按 N 或按 v 开。
//   注意用 vis 标记"已算过"而不是看值是否为 0 —— μ 前缀和可以真的等于 0!
// 复杂度: 阈值 T = n^{2/3} 时 O(n^{2/3}); T 越大预处理越重、递归越轻。
// 使用示例:
//   Dujiao dj; dj.init(1e9);
//   auto [smu, sphi] = dj.calc(1e9);   // smu = Σ_{i≤1e9} μ(i) mod 1e9+7, sphi 同理
// 若要求精确值(不取模): 去掉 calc 中所有 % mod 即可 (注意 φ 前缀和 n 大时会超 long long)。
struct Dujiao {
    long long N;                            // 目标上界
    long long T;                            // 阈值: 线性筛预处理到 T, 键 v > T 走记忆化
    vector<int32_t> mu, phi, smu, sphi;     // 小范围前缀 (int32 省内存)
    vector<long long> memo_mu, memo_phi;    // 大键记忆化: 下标 N/v
    vector<char> vis;                       // 已算标记 (不能拿值==0 判断!)

    void init(long long n){
        N = n;
        T = min((long long)pow((long double)N, 2.0 / 3.0) + 1, 5000000LL); // n^{2/3}, 上限 5e6
        if(T > N) T = N;                    // N 很小时直接全筛 其实不要也可以, 神金ds非要保护 N=1 少开一个字节空间
        mu.assign(T + 1, 0); phi.assign(T + 1, 0);
        smu.assign(T + 1, 0); sphi.assign(T + 1, 0);
        vector<int32_t> nop(T + 1, 0);
        vector<long long> pr;
        mu[1] = 1; phi[1] = 1;
        for(long long i = 2; i <= T; i++){
            if(!nop[i]){ pr.push_back(i); mu[i] = -1; phi[i] = i - 1; }
            for(long long p : pr){
                long long t = i * p;
                if(t > T) break;
                nop[t] = 1;
                if(i % p == 0){ mu[t] = 0; phi[t] = phi[i] * p; break; }
                mu[t] = -mu[i];
                phi[t] = phi[i] * (p - 1);
            }
        }
        for(long long i = 1; i <= T; i++){
            smu[i] = ((long long)smu[i - 1] + mu[i] + mod) % mod;
            sphi[i] = (sphi[i - 1] + phi[i]) % mod;
        }
        long long sz = N / T + 2;           // 键 v > T ⟹ N/v < N/T, 数组只开 N/T 大小
        memo_mu.assign(sz, 0); memo_phi.assign(sz, 0); vis.assign(sz, 0);
    }

    // 返回 (Σ_{i=1}^v μ(i), Σ_{i=1}^v φ(i)) 均对 mod 取模
    pair<long long, long long> calc(long long v){
        if(v <= T) return {smu[v], sphi[v]};
        long long idx = N / v;              // 大键的下标变换, O(1) 存取
        if(vis[idx]) return {memo_mu[idx], memo_phi[idx]};
        vis[idx] = 1;
        long long m = 1;                    // S_μ(v) = 1 − Σ_{d=2..v} S_μ(⌊v/d⌋)
        long long p = (long long)((__int128)v * (v + 1) / 2 % mod); // S_φ(v) = v(v+1)/2 − Σ ...
        for(long long l = 2; l <= v; ){
            long long q = v / l, r = v / q; // 整除分块: [l,r] 内 ⌊v/d⌋ 都等于 q
            long long len = (r - l + 1) % mod;
            auto [sm, sp] = calc(q);
            m = (m - len * sm % mod + mod) % mod;
            p = (p - len * sp % mod + mod) % mod;
            l = r + 1;
        }
        memo_mu[idx] = m; memo_phi[idx] = p;
        return {m, p};
    }
};
// 验证过的取值: M(10^6)=212, M(10^7)=1037, M(10^8)=1928, M(10^9)=-222, M(10^10)=-33722;
//              Σ_{i≤10^6} φ(i)=303963552392, Σ_{i≤10^7} φ(i)=30396356427242
// 实测耗时: n=1e9 约 27ms, n=1e10 约 135ms


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
```

### 数论进阶

```cpp
// ============================== 数论进阶板子 ==============================
// 内容: 快速乘、Miller-Rabin、Pollard-Rho、欧拉降幂、BSGS/exBSGS、
//       Lucas/exLucas、阶与原根、二次剩余(Cipolla)、Min_25 筛
// 与 数论.cpp 合并时注意: 本文件自带 ksm / exgcd / oula_phi 副本，请去重。
// 说明: 本文件 ksm 内部用 mul(龟速防溢出) 实现，任何模数(含 2^64 级)都安全；
//       mul 基于 __int128，若编译器不支持可换成龟速乘(见注释)。

const int mod = 1e9 + 7;

// ============================== 基础工具 ==============================
// 模乘防溢出: 仅在 a,b < m 时保证正确
int mul(int a, int b, int m){
    return (int)((__int128)a * b % m);
}

int ksm(int x, int a, int m){
    int ans = 1;
    x %= m;                     // 兼容大底数(如 Miller-Rabin 的 a)
    while(a){
        if(a & 1) ans = mul(ans, x, m);
        x = mul(x, x, m);
        a >>= 1;
    }
    return ans;
}

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
    return d;
}

// 模逆元(要求 gcd(a,m)=1，m 不必为质数)
int inv(int a, int m){
    int x, y;
    exgcd(a, m, x, y);
    return (x % m + m) % m;
}

// 单个数欧拉函数
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

// ============================== Miller-Rabin 素性测试 ==============================
// 费马测试 + 二次探测。底数集为 2^64 内的确定性集合(无需随机)。
// 复杂度 O(k log n)，k 为底数个数。
// 验证: n=341550071728321(伪素数) 判合数; n=998244353 判素数。
bool is_prime(int n){
    if(n < 2) return false;
    for(int p : {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37}){
        if(n % p == 0) return n == p;
    }
    int d = n - 1, s = 0;
    while(d % 2 == 0){
        d /= 2;
        s++;
    }
    for(int a : {2, 325, 9375, 28178, 450775, 9780504, 1795265022}){
        if(a % n == 0) continue;
        int x = ksm(a, d, n);
        if(x == 1 || x == n - 1) continue;
        bool ok = false;
        for(int i = 1; i < s; i++){
            x = mul(x, x, n);
            if(x == n - 1){
                ok = true;
                break;
            }
        }
        if(!ok) return false;
    }
    return true;
}

// ============================== Pollard-Rho 大数分解 ==============================
// 生日悖论: 在模 q(待找因子) 的剩余系里取 O(√q) 个伪随机数 x_i = f(x_{i-1})，
// 出现 x_i ≡ x_j (mod q) 时 gcd(|x_i-x_j|, n) 即非平凡因子。
// Floyd 判圈: 龟走 1 步、兔走 2 步，O(1) 空间。(Brent 版更快，需要时可替换)
// 期望复杂度 O(n^{1/4})。依赖 is_prime。
mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());

int pollard_rho(int n){
    if(n % 2 == 0) return 2;
    if(n % 3 == 0) return 3;
    while(true){
        int c = rng() % (n - 1) + 1;
        int x = rng() % (n - 1) + 1, y = x;
        for(;;){
            x = (mul(x, x, n) + c) % n;   // 龟
            y = (mul(y, y, n) + c) % n;   // 兔 x2
            y = (mul(y, y, n) + c) % n;
            int d = __gcd(abs(x - y), n);
            if(d == n) break;             // 环撞满，换参数重试
            if(d > 1) return d;
        }
    }
}

// 质因数分解(带指数)，结果存入 mp。使用示例:
//   map<int,int> mp; factor(341550071728321, mp);
//   mp = {10670053: 1, 32010157: 1}
void factor(int n, map<int,int>& mp){
    if(n < 2) return;
    if(is_prime(n)){
        mp[n]++;
        return;
    }
    int d = pollard_rho(n);
    factor(d, mp);
    factor(n / d, mp);
}

// ============================== 欧拉降幂(扩展欧拉定理) ==============================
// a^b ≡ a^b (b < φ(p));  a^b ≡ a^(b mod φ(p) + φ(p)) (b ≥ φ(p))  (mod p)
// b 以字符串传入，可超过 long long。依赖 oula_phi。
// 验证: 2^100 mod 7 = 2 (φ(7)=6, 100≥6 → 2^(4+6)=2^10≡2)。
int euler_pow(int a, string b, int p){
    int phi = oula_phi(p);
    int e = 0;
    bool big = false;
    for(char c : b){
        e = e * 10 + (c - '0');
        if(e >= phi){
            e %= phi;
            big = true;
        }
    }
    if(big) e += phi;
    return ksm(a, e, p);
}

// ============================== BSGS 离散对数 ==============================
// 求 a^x ≡ b (mod p) 的最小非负整数解 x，要求 gcd(a, p) = 1。
// 大步小步: x = i*m - j, m = ⌈√p⌉, 哈希表存 b*a^j。
// 复杂度 O(√p)。无解返回 -1。
// 验证: bsgs(2, 3, 5) = 3 (2^3 ≡ 3 mod 5)。
int bsgs(int a, int b, int p){
    a %= p;
    b %= p;
    if(b == 1 || p == 1) return 0;
    int m = ceil(sqrt(p));
    unordered_map<int,int> mp;
    int cur = b;
    for(int j = 0; j < m; j++){        // baby step: b * a^j
        if(!mp.count(cur)) mp[cur] = j;
        cur = cur * a % p;
    }
    int step = ksm(a, m, p);           // giant step 步长 a^m
    cur = 1;
    for(int i = 0; i <= m; i++){
        if(mp.count(cur)){
            int x = i * m - mp[cur];
            if(x >= 0) return x;
        }
        cur = cur * step % p;
    }
    return -1;
}

// 扩展 BSGS: gcd(a, p) 任意。逐次消公因子归约到普通 BSGS。
// 验证: exbsgs(2, 4, 6) = 2;  exbsgs(4, 7, 10) = -1。
int exbsgs(int a, int b, int p){
    a %= p;
    b %= p;
    if(b == 1 || p == 1) return 0;
    int d = __gcd(a, p), k = 0, base = 1;
    while(d != 1){
        if(b % d) return -1;           // b 不含因子 d，且 b≠1 → 无解
        b /= d;
        p /= d;
        k++;
        base = base * (a / d) % p;
        if(base == b) return k;        // 消到只剩常数的特判
        d = __gcd(a, p);
    }
    int t = bsgs(a, b * inv(base, p) % p, p);
    return t == -1 ? -1 : t + k;
}

// ============================== Lucas 定理 / exLucas ==============================
// Lucas: C(n,m) mod p，p 为小质数(n 可远大于 p)。
// 定理: 把 n、m 按 p 进制展开，C(n,m) ≡ ∏ C(n_i, m_i) (mod p)。
// 验证: lucas(5, 2, 3) = 1 (C(5,2)=10≡1; 5=12₃, 2=02₃ → C(1,0)C(2,2)=1)。
int lucas_fac[1000005];

void lucas_init(int p){
    lucas_fac[0] = 1;
    for(int i = 1; i < p; i++){
        lucas_fac[i] = lucas_fac[i - 1] * i % p;
    }
}

// C(n,m) mod p，n、m < p
int C_small(int n, int m, int p){
    if(m > n) return 0;
    return lucas_fac[n] * ksm(lucas_fac[m], p - 2, p) % p
         * ksm(lucas_fac[n - m], p - 2, p) % p;
}

int lucas(int n, int m, int p){
    if(m == 0) return 1;
    return C_small(n % p, m % p, p) * lucas(n / p, m / p, p) % p;
}

// exLucas: C(n,m) mod p^k (p 质数)。核心: 消 p 因子 + 周期阶乘 + 逆元。
// fac_ex[i] = ∏_{j≤i, p∤j} j (mod pk)，每 pk 一个周期。
int fac_ex[1000005];

void exlucas_init(int p, int pk){
    fac_ex[0] = 1;
    for(int i = 1; i <= pk; i++){
        if(i % p) fac_ex[i] = fac_ex[i - 1] * i % pk;
        else fac_ex[i] = fac_ex[i - 1];
    }
}

// n! 除去全部 p 因子后的部分 (mod pk)
int f(int n, int p, int pk){
    if(n == 0) return 1;
    return f(n / p, p, pk) * ksm(fac_ex[pk], n / pk, pk) % pk * fac_ex[n % pk] % pk;
}

// C(n,m) mod p^k
int C_mod_pk(int n, int m, int p, int k){
    int pk = 1;
    for(int i = 0; i < k; i++) pk *= p;
    int cnt = 0;                       // v_p(C(n,m))
    for(int i = n; i; i /= p) cnt += i / p;
    for(int i = m; i; i /= p) cnt -= i / p;
    for(int i = n - m; i; i /= p) cnt -= i / p;
    if(cnt >= k) return 0;             // p 的指数超过 pk 中含量 → 0
    int res = 1;
    if(cnt) res = res * ksm(p, cnt, pk) % pk;
    res = res * f(n, p, pk) % pk;
    res = res * inv(f(m, p, pk), pk) % pk;
    res = res * inv(f(n - m, p, pk), pk) % pk;
    return res;
}

// C(n,m) mod M，M 任意合数: 分解 M = ∏ p^k，逐块求值再 CRT 合并。
// 验证: exlucas(5, 2, 12) = 10 (mod 4 得 2，mod 3 得 1，CRT 合并得 10)。
int exlucas(int n, int m, int M){
    int ans = 0, MM = M;
    for(int p = 2; p * p <= MM; p++){
        if(MM % p) continue;
        int k = 0, pk = 1;
        while(MM % p == 0){
            MM /= p;
            k++;
            pk *= p;
        }
        exlucas_init(p, pk);
        int a = C_mod_pk(n, m, p, k);
        ans = (ans + a * (M / pk) % M * inv(M / pk % pk, pk) % M) % M;
    }
    if(MM > 1){
        int p = MM, k = 1, pk = p;
        exlucas_init(p, pk);
        int a = C_mod_pk(n, m, p, k);
        ans = (ans + a * (M / pk) % M * inv(M / pk % pk, pk) % M) % M;
    }
    return ans;
}

// ============================== 阶与原根 ==============================
// 阶: 最小的 k > 0 使 a^k ≡ 1 (mod p)。存在性要求 gcd(a,p)=1。
// 求 p 的最小原根(p 为质数): 对 p-1 的每个质因子 q 检验 g^((p-1)/q) ≢ 1 (mod p)。
// 验证: primitive_root(998244353) = 3。
int primitive_root(int p){
    int phi = p - 1, x = phi;
    vector<int> facs;
    for(int i = 2; i * i <= x; i++){
        if(x % i == 0){
            facs.push_back(i);
            while(x % i == 0) x /= i;
        }
    }
    if(x > 1) facs.push_back(x);
    for(int g = 2; ; g++){
        bool ok = true;
        for(int q : facs){
            if(ksm(g, phi / q, p) == 1){
                ok = false;
                break;
            }
        }
        if(ok) return g;
    }
}

// ============================== 二次剩余(Cipolla) ==============================
// 解 x^2 ≡ n (mod p)，p 为奇质数。欧拉判别法判有解，扩域 F_p[√w] 上快速幂。
// 返回一个解 x(另一个为 p-x)，无解返回 -1。
// 验证: cipolla(2, 7) = 4 (4^2 ≡ 2 mod 7)。
int cipolla(int n, int p){
    n %= p;
    if(n == 0) return 0;
    if(ksm(n, (p - 1) / 2, p) != 1) return -1;   // 勒让德符号 = -1，非二次剩余
    int a = 0, w = 0;
    while(true){
        a = rng() % p;
        w = ((mul(a, a, p) - n) % p + p) % p;     // w = a^2 - n
        if(ksm(w, (p - 1) / 2, p) == p - 1) break; // 选到非二次剩余的 w
    }
    // 计算 (a + √w)^((p+1)/2)，答案必为纯实数部分
    pair<int,int> pw = {a, 1}, res = {1, 0};       // (x, y) 表示 x + y√w
    auto mul2 = [&](pair<int,int> u, pair<int,int> v){
        return make_pair((mul(u.first, v.first, p) + mul(mul(u.second, v.second, p), w, p)) % p,
                         (mul(u.first, v.second, p) + mul(u.second, v.first, p)) % p);
    };
    int e = (p + 1) / 2;
    while(e){
        if(e & 1) res = mul2(res, pw);
        pw = mul2(pw, pw);
        e >>= 1;
    }
    return res.first;
}

// ============================== Min_25 筛 ==============================
// 求 Σ_{i=1}^n f(i)，f 积性且素数处 f(p) 为 p 的多项式组合(本模板 f(p)=p(p-1)，
// 即 f(p)=p^2-p，f(p^e)=p^e(p^e-1)，f(1)=1 —— 洛谷 P5325)。
// n 可达 1e11，复杂度 O(n^{3/4}/log n)。
// 原理: 按最小质因子分类。G 函数在 ⌊n/i⌋ 上递推筛出素数贡献，S 递归枚举合数。
// 注意: 大数组用 int32_t 避免被 #define int long long 撑爆内存。
// 验证: solve(10) = 263 (手算: f(1..10)=1,2,6,12,20,12,42,56,72,40)。
const int MIN25_N = 1000005;         // ≥ 2√n

namespace min25 {
    long long n, sq;
    int tot;
    long long w[2 * MIN25_N];        // ⌊n/i⌋ 的全部取值
    int32_t g1[2 * MIN25_N], g2[2 * MIN25_N]; // 素数处 Σp、Σp^2
    int32_t id1[MIN25_N], id2[MIN25_N];
    vector<int> primes;
    bool is_comp[MIN25_N];
    int sp1[MIN25_N], sp2[MIN25_N];  // 素数前缀和(按素数个数索引)

    int idx(long long v){            // ⌊n/i⌋ → 离散化下标
        return v <= sq ? id1[v] : id2[n / v];
    }

    // 最小质因子 ≥ primes[y](0-based 索引) 的所有数的 f 之和
    // 注意: 递归可能传入 y == primes.size()(表示"大于 √n 的素数")，
    //       此时素数部分 g - sp[size] 恰为 (√n, x] 内的素数贡献。
    int S(long long x, int y){
        if(x <= 1) return 0;
        if(y < (int)primes.size() && primes[y] > x) return 0;
        if(y >= (int)primes.size() && primes.back() > x) return 0;
        int k = idx(x);
        // 素数贡献 = 全部素数的 (p^2-p) − 前 y 个素数的 (p^2-p)
        int ans = (g2[k] - g1[k] - (sp2[y] - sp1[y]) % mod + 2LL * mod) % mod;
        for(int i = y; i < (int)primes.size() && 1LL * primes[i] * primes[i] <= x; i++){
            long long pe = primes[i];
            for(int e = 1; pe <= x; e++, pe *= primes[i]){   // 枚举 p^e ≤ x(含最高次幂)
                int val = pe % mod * ((pe - 1) % mod) % mod;  // f(p^e) = p^e(p^e-1)
                ans = (ans + val * (S(x / pe, i + 1) + (e > 1)) % mod) % mod;
            }
        }
        return ans;
    }

    int solve(long long N){
        n = N;
        sq = sqrt(n);
        memset(is_comp, 0, sizeof(is_comp));   // 支持多次调用: 重置筛状态
        primes.clear();
        for(int i = 2; i <= sq; i++){
            if(!is_comp[i]) primes.push_back(i);
            for(int p : primes){
                if(1LL * i * p > sq) break;
                is_comp[i * p] = true;
                if(i % p == 0) break;
            }
        }
        for(int i = 1; i <= (int)primes.size(); i++){
            sp1[i] = (sp1[i - 1] + primes[i - 1]) % mod;
            sp2[i] = (sp2[i - 1] + 1LL * primes[i - 1] * primes[i - 1] % mod) % mod;
        }
        // 离散化 ⌊n/i⌋，初始按"全是素数"算 g1=Σp、g2=Σp^2
        tot = 0;
        int inv6 = ksm(6, mod - 2, mod);
        for(long long l = 1, r; l <= n; l = r + 1){
            r = n / (n / l);
            w[++tot] = n / l;
            long long v = w[tot] % mod;
            g1[tot] = (v * (v + 1) / 2 % mod - 1 + mod) % mod;
            g2[tot] = (v * (v + 1) % mod * (2 * v + 1) % mod * inv6 % mod - 1 + mod) % mod;
            if(w[tot] <= sq) id1[w[tot]] = tot;
            else id2[n / w[tot]] = tot;
        }
        // 逐素数筛掉合数
        for(int i = 0; i < (int)primes.size(); i++){
            int p = primes[i];
            for(int j = 1; j <= tot && 1LL * p * p <= w[j]; j++){
                int k = idx(w[j] / p);
                g1[j] = (g1[j] - 1LL * p * (g1[k] - sp1[i]) % mod + mod) % mod;
                g2[j] = (g2[j] - 1LL * p * p % mod * (g2[k] - sp2[i]) % mod + mod) % mod;
            }
        }
        return (S(n, 0) + 1) % mod;  // +f(1)
    }
}
```

### 线性基与矩阵

```cpp
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
    mat(int x = 0, int y = 0){
        n = x;
        m = y;
        a.resize(x, vector<int> (y));
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
```
