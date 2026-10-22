#include <bits/stdc++.h>
using namespace std;
#define int long long

// ============================== 数论基础 ==============================
// 内容: 欧拉筛/最小质因子、线性筛全家桶（μ/φ/τ/σ）、快速幂、exgcd、
//       CRT/exCRT、单点欧拉函数、莫比乌斯反演、狄利克雷前缀/后缀变换、
//       康托展开、FFT/NTT、多项式求逆、FWT、整除分块、杜教筛、类欧几里得、SOSDP。
// 约定:
//   * 本文件用 #define int long long，所有 int 均为 64 位；与自带 int 的代码拼接时注意。
//   * 筛法封装在 Sieve 结构体里（Sieve sv(n)），质数表是 sv.primes，不再有全局 primes；
//     狄利克雷变换等需要质数表的函数显式传入 sv.primes。
//   * 模数统一用全局 mod = 1e9+7；NTT 单独使用 NTT_MOD = 998244353。
//   * 类欧的公式与变式推导见 数论恒等式.md 第 14 节。
// =====================================================================

const int mod = 1e9 + 7;

// 【速查】线性筛全家桶 Sieve：质数表、最小质因子、μ、φ、约数个数 τ、约数和 σ
// 用法: Sieve sv(n);   // 一次 O(n) 筛出 1..n 的全部信息
//       sv.primes（<= n 的质数，升序）  sv.minp[x]  sv.mu[x]  sv.phi[x]  sv.dcnt[x]（τ）  sv.dsum[x]（σ）
//       sv.factor(x) -> {(p, 次数)}，sv.distinctPrimes(x) -> 不同质因子；都要求 2 <= x <= n，O(log x)
// 复杂度: O(n)；数组用 int32，n = 1e7 时约 280MB，按需删掉不用的数组
// 10.22: 原版是全局数组 + init() / linear_sieve() 两个函数共用一个全局 primes（混用会重复），已合并为结构体
// 原理: 设 i 的最小质因子为 q，枚举质数 p 去筛 m = i*p：
//   p < q  : p 与 i 互质，积性函数直接相乘
//   p == q : p 是 m 的最小质因子，指数 +1，按"去掉一个 p"的商修正，然后 break
//   每个合数只在其最小质因子处被筛一次，故总复杂度 O(n)
struct Sieve {
    int n;
    vector<int> primes;
    vector<int32_t> minp, mu, phi, dcnt, dsum, mexp, mpw;   // mexp: 最小质因子的指数；mpw: 1 + p + ... + p^mexp

    Sieve(int n) : n(n), minp(n + 1, 0), mu(n + 1, 0), phi(n + 1, 0), dcnt(n + 1, 0), dsum(n + 1, 0), mexp(n + 1, 0), mpw(n + 1, 0) {
        if (n >= 1) { mu[1] = 1; phi[1] = 1; dcnt[1] = 1; dsum[1] = 1; }
        for (int i = 2; i <= n; i++) {
            if (!minp[i]) {
                minp[i] = i; primes.push_back(i);
                mu[i] = -1; phi[i] = i - 1; dcnt[i] = 2; dsum[i] = i + 1; mexp[i] = 1; mpw[i] = i + 1;
            }
            for (int p : primes) {
                if (p > minp[i] || i * p > n) break;
                int m = i * p;
                minp[m] = p;
                if (p == minp[i]) {
                    mu[m] = 0;
                    phi[m] = phi[i] * p;
                    mexp[m] = mexp[i] + 1;
                    mpw[m] = mpw[i] * p + 1;
                    dcnt[m] = dcnt[i] / (mexp[i] + 1) * (mexp[m] + 1);
                    dsum[m] = dsum[i] / mpw[i] * mpw[m];
                } else {
                    mu[m] = -mu[i];
                    phi[m] = phi[i] * (p - 1);
                    mexp[m] = 1; mpw[m] = p + 1;
                    dcnt[m] = dcnt[i] * 2;
                    dsum[m] = dsum[i] * (p + 1);
                }
            }
        }
    }
    vector<pair<int, int>> factor(int x) {
        vector<pair<int, int>> res;
        while (x > 1) {
            int p = minp[x], c = 0;
            while (x % p == 0) x /= p, c++;
            res.push_back({p, c});
        }
        return res;
    }
    vector<int> distinctPrimes(int x) {
        vector<int> res;
        for (auto [p, c] : factor(x)) res.push_back(p);
        return res;
    }
};


// 【速查】快速幂 x^a mod m
// 用法: ksm(x, a, m)；m ≤ 3e9（再大用数论进阶里带 mul 的 ksm），x 可为任意整数
// 复杂度: O(log a)
// 快速幂: 计算 x^a mod m，O(log a)
// 要求 a >= 0、m > 0；a = 0 时返回 1 % m。底数会先规整到 [0, m)（负数也可以）。
// 10.22 修复: 原版 ans 初值为 1（m = 1 时应为 0）且底数不取模（负底数得负结果、大底数溢出）。
int ksm(int x, int a, int m){
    int ans = 1 % m;
    x %= m;
    if(x < 0) x += m;
    while(a){
        if(a & 1) ans = ans * x % m;
        x = x * x % m;
        a >>= 1;
    }
    return ans;
}


// 【速查】扩展欧几里得：求 ax + by = gcd(a, b) 的一组解
// 用法: int g = exgcd(a, b, x, y);  逆元: gcd(a, m) = 1 时 (x % m + m) % m 就是 a 的逆元
// 解 ax + by = c: 需 g | c，特解 x0 = x·c/g，通解 x0 + k·b/g
// 复杂度: O(log)
// 扩展欧几里得: 求 g = gcd(a,b) 及一组整数解 (x,y)，使 a*x + b*y = g。
// 通解: x' = x + k*b/g, y' = y - k*a/g (k ∈ Z)。
// 用途: 求模逆元（gcd(a,m)=1 时 x 即 a^{-1} mod m）、解二元一次不定方程。
// 注意: a,b 建议取非负；若含负数，先取绝对值求解再按需调整符号。
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

// 【速查】中国剩余定理（模数两两互质）
// 用法: CRT(k, a, r) -> x ≡ a[i] (mod r[i]) 的最小非负解；a、r 都是 0-based，长度 k
// 要求: r 两两互质，乘积放得进 long long；不互质用 excrt
// 复杂度: O(k log)
// 中国剩余定理（模数两两互质）: 解 x ≡ a[i] (mod r[i]), i = 0..k-1。
// 返回 [0, n) 内的唯一解，n = ∏ r[i]。
// 做法: 令 m_i = n/r_i，用 exgcd 求 m_i 在模 r_i 下的逆元 b_i，
//       则 x = Σ a_i * m_i * b_i (mod n)。
// 前提: 所有 r[i] 两两互质且 > 1；乘积 n 必须放得进 long long（约 9.2e18，即最多两个 1e9 级模数）。
//       模数不互质请改用下面的 excrt。
// 10.22 修复: 原版 a[i]*m*b 直接用 long long 乘，n 只要超过约 3e9 就溢出（实测 n≈2.2e11 已错）；
//            现在中间乘法用 __int128，并先把 a[i]、b 规整到 [0, r[i])。
int CRT(int k, vector<int> a, vector<int> r){
    int n = 1;
    for(int i = 0; i < k; i++){
        n = n * r[i];
    }
    __int128 ans = 0;
    for(int i = 0; i < k; i++){
        int m = n / r[i], b, y;
        exgcd(m % r[i], r[i], b, y);
        b = (b % r[i] + r[i]) % r[i];
        int ai = (a[i] % r[i] + r[i]) % r[i];
        ans = (ans + (__int128)ai * b % n * m) % n;   // ai*b < r[i]^2，再乘 m 后 < n*r[i]，__int128 放得下
    }
    return (int)ans;
}

// 【速查】扩展中国剩余定理（模数可以不互质）
// 用法: excrt(m, a, n) -> x ≡ a[i] (mod m[i]) 的最小非负解，无解返回 -1；注意参数顺序是 (模数, 余数, 个数)
// 要求: 最终 lcm 放得进 long long
// 复杂度: O(n log)
// 扩展中国剩余定理（模数不必互质）: 解方程组 x ≡ a[i] (mod m[i])，i = 0..n-1。
// 返回最小非负解；无解返回 -1。要求 n >= 1。
// 做法: 维护已合并的解 ans（模 M），每次把 x ≡ a[i] (mod m[i]) 与它合并：
//       先解 M*t ≡ a[i]-ans (mod m[i])，有解当且仅当 gcd(M,m[i]) 整除差值；
//       合并后 M = lcm(M, m[i])。整体 O(n log)。
// 注意: 要求最终 lcm 放得进 long long；中间的 x*(c/d)、x*M 用 __int128 承接。
// 10.22 修复: 原版 x*(c/d) 用 long long 乘，量级约 (m[i]/d)^2，m[i]/d 超过约 3e9 即溢出，
//            即使 lcm <= 1e18 也会错（实测 m1~1e6、m2~1e12 时 10 万组错 94860 组，P4777 类题必错）。
int excrt(vector<int> m, vector<int> a, int n){
    int M = m[0];
    int ans = (a[0] % M + M) % M;
    for(int i = 1; i < n; i++){
        int c = ((a[i] - ans) % m[i] + m[i]) % m[i];
        int x, y;
        int d = exgcd(M, m[i], x, y);
        if(c % d) return -1;
        int md = m[i] / d;
        x = (int)((__int128)(x % md + md) % md * (c / d) % md);   // M*x ≡ c (mod m[i]) 的最小非负解
        int newM = M / d * m[i];
        ans = (int)(((__int128)x * M + ans) % newM);
        M = newM;
    }
    return ans;
}

// 【速查】单个数的欧拉函数 φ(n)（1..n 中与 n 互质的个数）
// 用法: oula_phi(n)；要很多个 φ 就用 sieve_phi / Sieve
// 复杂度: O(√n)
// 单个数欧拉函数 φ(n)，O(√n)
// 公式: φ(n) = n * ∏_{p|n} (1 - 1/p)，边分解边乘，避免除法丢精度。
// 要求 n >= 1；n = 1 时返回 1。
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

// 【速查】线性筛求 φ(1..n)
// 用法: auto phi = sieve_phi(n);  phi[i] = φ(i)
// 复杂度: O(n)
// 筛法求欧拉函数（线性筛）: 返回长度为 n+1 的 phi 数组，phi[i] = φ(i)，O(n)。
// 与 Sieve 的区别: 只求 φ，返回一个数组，更轻量。
// 转移同样按 i % p 分两类:
//   p | i     : p 已是 i 的质因子，φ(i*p) = φ(i) * p
//   p ∤ i     : p 是新质因子，φ(i*p) = φ(i) * (p-1)
vector<int> sieve_phi(int n) {
    vector<int> phi(n + 1);
    vector<bool> st(n + 1, false);
    vector<int> primes;
    
    phi[1] = 1; // 约定 φ(1) = 1
    
    for (int i = 2; i <= n; i++) {
        if (!st[i]) { // i 是质数
            primes.push_back(i);
            phi[i] = i - 1;
        }
        
        for (int j = 0; primes[j] <= n / i; j++) {
            st[primes[j] * i] = true;
            
            if (i % primes[j] == 0) {
                // primes[j] 是 i 的最小质因子，指数 +1
                phi[primes[j] * i] = phi[i] * primes[j];
                break;
            } else {
                // primes[j] 与 i 互质，φ 积性
                phi[primes[j] * i] = phi[i] * (primes[j] - 1);
            }
        }
    }
    
    return phi;
}

// 莫比乌斯反演（μ、φ 由 Sieve 预处理）
// 约数形式: g(n) = Σ_{d|n} f(d)  <=>  f(n) = Σ_{d|n} μ(d) * g(n/d)
// 倍数形式: g(n) = Σ_{n|d} f(d)  <=>  f(n) = Σ_{n|d} μ(d/n) * g(d)
// 记忆: 反演就是“卷上 μ”，因为 μ * 1 = ε。
// 常用结论:
//   [gcd(i,j) == 1] = Σ_{d | gcd(i,j)} μ(d)          （把互质判断转成约数求和）
//   φ(n) = Σ_{d|n} μ(d) * (n/d)                       （φ = μ * id）
//   n = Σ_{d|n} φ(d)                                  （id = φ * 1）
// 典型用法: Σ_{i,j} f(gcd(i,j)) 先写成 Σ_d g(d) * ⌊n/d⌋⌊m/d⌋，再整除分块。
// 完整恒等式表与证明见 数论恒等式.md 第 3~5 节。

// 狄利克雷前缀/后缀变换（快速 zeta 变换），O(n log log n)
// 前置: 传入的 primes 必须包含所有 <= n 的质数（用 Sieve(n).primes）。
// 原理: 对每个质数 p 做一遍类似“高维前缀和”的扫描，枚举顺序保证每个数被其全部质因子更新。
// 注意:
//   1) 全部原地修改，需要保留原数组请先拷贝；
//   2) 前缀 max/min 与求和方向相同，逆变换则把循环方向反过来做减法；
//   3) 下标从 1 开始使用，f[0] 不参与运算。

// 【速查】狄利克雷前缀/后缀和（按约数/倍数做"高维前缀和"）
// 用法: Sieve sv(n);  dirichlet_prefix_sum(f, n, sv.primes);   // f 为 1-based 数组，原地变换
//   prefix_sum: f[n] ← Σ_{d|n} f[d]     suffix_sum: f[d] ← Σ_{d|n} f[n]
//   prefix_max / prefix_min 同理取最值；prefix_inv / suffix_inv 是逆变换（相当于卷 μ）
// 复杂度: O(n log log n)
// 前缀和: 变换后 f[n] = Σ_{d|n} f[d]
void dirichlet_prefix_sum(vector<int>& f, int n, const vector<int>& primes){
    for(int p : primes)
        for(int i = 1; i * p <= n; i++)
            f[i * p] += f[i];
}

// 后缀和: 变换后 f[d] = Σ_{d|n} f[n]
void dirichlet_suffix_sum(vector<int>& f, int n, const vector<int>& primes){
    for(int p : primes)
        for(int i = n / p; i >= 1; i--)
            f[i] += f[i * p];
}

// 前缀最值: 变换后 f[n] = max_{d|n} f[d]
void dirichlet_prefix_max(vector<int>& f, int n, const vector<int>& primes){
    for(int p : primes)
        for(int i = 1; i * p <= n; i++)
            f[i * p] = max(f[i * p], f[i]);
}

// 前缀最值: 变换后 f[n] = min_{d|n} f[d]
void dirichlet_prefix_min(vector<int>& f, int n, const vector<int>& primes){
    for(int p : primes)
        for(int i = 1; i * p <= n; i++)
            f[i * p] = min(f[i * p], f[i]);
}

// 前缀和的逆变换: 已知 g[n] = Σ_{d|n} f[d]，由 g 还原 f
void dirichlet_prefix_inv(vector<int>& f, int n, const vector<int>& primes){
    for(int p : primes)
        for(int i = n / p; i >= 1; i--)
            f[i * p] -= f[i];
}

// 后缀和的逆变换: 已知 g[d] = Σ_{d|n} f[n]，由 g 还原 f
void dirichlet_suffix_inv(vector<int>& f, int n, const vector<int>& primes){
    for(int p : primes)
        for(int i = 1; i * p <= n; i++)
            f[i] -= f[i * p];
}

// 【速查】狄利克雷卷积快速幂 f^{*k}
// 用法: dirichlet_power(f, res, n, k);  res[1..n] = f 卷 k 次的结果（mod）
// 复杂度: O(n log n · log k)
// 狄利克雷卷积快速幂: 计算 f 的 k 次卷积幂 f^{*k}（k >= 0）
// 结果 res 长度 n+1，res[1] 为 ε（k=0 时就是单位元 ε）。
// 单次卷积是 O(n log n)（枚举 i,j 满足 i*j<=n 的约数对），共 O(log k) 轮，
// 总复杂度 O(n log n log k)；n 较大时通常改用“对每个质因子做 exp/log”的 O(n log log n) 做法。
// 注意: 卷积系数对 mod 取模，f[1] 不要求可逆（这里直接做卷积幂，不是形式幂级数求逆）。
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

// 阶乘表 0!..9!，只够康托展开 n <= 10 使用；n 更大时请自行预处理 fac[]
const int fact[10] = {1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880};

// 【速查】康托展开：排列 → 字典序排名（从 0 开始）
// 用法: cantor_expand(p)；p 是 0..n-1 或 1..n 的排列，n ≤ 10（fact 只到 9!）
// 复杂度: O(n²)
/**
 * 康托展开: 求排列在字典序中的排名（从 0 开始），O(n^2)
 * 公式: code = Σ_{i=0}^{n-1} cnt_i * (n-1-i)!，
 *       其中 cnt_i 是“排在 i 后面且比 p[i] 小的元素个数”。
 * @param permutation 排列，元素为 0..n-1 或 1..n，必须是连续不重复的整数
 * @return 比当前排列字典序小的排列个数（即 0-based 排名）
 * 注意: 依赖 fact[]，因此要求 n <= 10；n 更大时把 fact 换成预处理的阶乘表。
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

// 【速查】逆康托展开：排名 → 排列
// 用法: cantor_expand_inv(code, n)（元素 0..n-1）或 cantor_expand_inv(code, n, {1..n})；n ≤ 10
// 复杂度: O(n²)
/**
 * 逆康托展开: 由 0-based 排名 code 还原长度为 n 的排列，O(n^2)（vector 删除元素）
 * @param code 排名，要求 0 <= code < n!
 * @param n 排列长度
 * @param elements 可选元素集合，默认 0..n-1；若排列是 1..n 则传入 {1,...,n}
 * @return 对应的排列
 * 注意: 同样受 fact[] 限制，要求 n <= 10；code 越界会访问 available 之外的下标。
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


// ============================== FFT 快速傅里叶变换 ==============================
// 用途: O(n log n) 计算多项式乘法（卷积），系数为 double。
// 流程: 补到 2 的幂 -> 位逆序重排 -> 蝶形运算（系数转点值）-> 点值相乘 -> 逆变换。
// 约定:
//   * type = 1 正变换，type = -1 逆变换（逆变换最后除以 n）；
//   * 调用 fft 前必须先用相同长度调用 fft_init(len) 生成 rev；
//   * 长度 len 必须是 2 的幂。
// 精度注意:
//   * 结果四舍五入取整，系数绝对值较大（约 >1e15 / len）时误差会进位；
//   * 需要精确的模意义卷积请用下面的 NTT（998244353）或 MTT。
// ============================================================================

// 复数结构体: 只实现 FFT 需要的 +、-、*
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

// 【速查】FFT 多项式乘法（不取模，double）
// 用法: vector<int> C = multiply_fft(A, B);   // A、B 升幂系数，不能传临时对象
// 返回: 长度 |A|+|B|-1 的卷积；系数乘积和超过约 1e14 会掉精度，要取模请用 NTT
// 复杂度: O(n log n)
// FFT 多项式乘法: 返回 A*B 的系数，长度 A.size()+B.size()-1（不取模，直接四舍五入）
// 入参是非 const 引用（历史写法），不能直接传临时 vector；需要时先拷贝一份。
// 系数为 double，精度限制见上方 FFT 段说明。
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

// 【速查】NTT 多项式乘法（模 998244353）
// 用法: vector<int> C = multiply_ntt(A, B);  C[k] = Σ_{i+j=k} A[i]B[j] mod 998244353
// 限制: 结果长度 ≤ 2^23；换模数要同时换原根 G/GI（注释里有常用组合）
// 复杂度: O(n log n)
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


// 【速查】多项式求逆（模 998244353）
// 用法: B = poly_inv(A, n) -> 满足 A·B ≡ 1 (mod x^n) 的前 n 项；要求 A[0] ≠ 0
// 复杂度: O(n log n)
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
// 本质: 对下标做“高维前缀和/后缀和”，把位运算卷积变成逐点乘法。
//   or  正变换后 A[S] = Σ_{T⊆S} a[T]     （子集和）
//   and 正变换后 A[S] = Σ_{T⊇S} a[T]     （超集和）
//   xor 正变换是沃尔什-阿达玛变换，逆变换 = 正变换后再除以 n
// 注意:
//   1) 所有函数都要求 a.size() 是 2 的幂，且原地修改；
//   2) xor 的逆变换用 ksm(n, mod-2) 求 n 的逆元，要求 mod 为质数且 n 不是 mod 的倍数；
//   3) fwt_conv 会把结果补齐到 2 的幂，返回长度是补齐后的 n，不是 n+m-1。

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

// 【速查】FWT 位运算卷积：c[k] = Σ_{i op j = k} a[i]·b[j]，op 为 or / and / xor（mod）
// 用法: auto c = fwt_conv(a, b, fwt_or);  // 或 fwt_and / fwt_xor；返回长度为补齐后的 2 的幂
// 复杂度: O(n log n)，n 为 2 的幂
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

// 【速查】整除分块：Σ_{i=1}^{n} ⌊n/i⌋
// 套路: for (l = 1; l <= n; l = r + 1) { r = n / (n / l); 区间 [l, r] 内 ⌊n/i⌋ 都等于 n/l }
//       两个数时 r = min(n/(n/l), m/(m/l))；配莫比乌斯反演 / 杜教筛
// 复杂度: O(√n)
// 整除分块: 求 Σ_{i=1}^{n} floor(n/i)，O(√n)
// 关键: floor(n/i) 只有 O(√n) 种取值；固定左端 l 后，右端 r = n/(n/l)，
//       区间 [l,r] 内 floor(n/i) 恒为 n/l，可整段累加。
// 注意: 结果约为 n log n，n 到 1e9 时仍在 long long 内；再大需 __int128。
int div_block_sum(int n){
    int ans = 0;
    for(int l = 1, r; l <= n; l = r + 1){
        r = n / (n / l);
        ans += (n / l) * (r - l + 1);
    }
    return ans;
}
// 推广: Σ_{i=1}^{min(n,m)} floor(n/i) * floor(m/i)
//       每段右端取 r = min(n/(n/l), m/(m/l))，块内两个商都不变。
// 典型用途: 莫比乌斯反演 / 杜教筛里把 Σ_{d} f(d)⌊n/d⌋ 压成 O(√n) 段。


// 【速查】杜教筛：Σμ(i)、Σφ(i)，i ≤ n，n 到 1e10
// 用法: Dujiao dj; dj.init(n); auto [smu, sphi] = dj.calc(n);   // 结果对 mod 取模
// 复杂度: O(n^{2/3})；n = 1e10 约 0.15s
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
        if(T > N) T = N;                    // N 很小时直接线性筛到底
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


// ============================== 类欧几里得（Euclidean-like） ==============================
// 核心: 快速计算 f(n,m,a,b)=Σ_{i=0}^{n-1} floor((a*i+b)/m)，以及带权、平方、取模、
//       上取整、负数、区间等变式。
// 原理: 记 y_i=floor((a*i+b)/m)。当 0<=a,b<m 时，把“对 i 求和”换成“对 y 求和”
//       （数格点/交换求和次序），问题递归成 (m,a) 的形式；每轮规模按欧几里得
//       算法下降，因此 O(log max(a,m))。
// 统一约定:
//   * 下标 i=0..n-1；要求 n>=0，m>0。
//   * 参数建议 n,m,a,b <= 1e18（m+a-1-b 等中间和需落在 long long 内）。
//   * 未特别说明时 a,b>=0；负数请用 floor_sum_signed。
//   * exact 版本内部用 __int128 承接中间量，返回值仍是 long long，答案本身需放得下。
//   * 需要取模或 n 很大时用 floor_sum_mod / euclid3_mod。
// =====================================================================================

// 【速查】类欧几里得：Σ_{i=0}^{n-1} ⌊(a·i + b) / m⌋
// 用法: floor_sum(n, m, a, b)，要求 a, b ≥ 0、m > 0；答案要放得进 long long，否则用 floor_sum_mod
// 用途: 直线下整点计数、Σ⌊(ai+b)/m⌋ 类求和
// 复杂度: O(log max(a, m))
// 基础版: f(n,m,a,b)=Σ_{i=0}^{n-1} floor((a*i+b)/m)，O(log max(a,m))
// 要求 n>=0, m>0, a>=0, b>=0。
// 做法:
//   1) a>=m、b>=m 时先拆出整数部分（等差数列求和）；
//   2) 0<=a,b<m 时交换求和次序，递归到 (n'=y_max, m'=a, a'=m, b'=...)。
// 说明: 迭代写法，避免递归深度；中间量用 __int128。返回值是 long long，答案本身需放得下
//       （大致要求 n^2*a/m <= 9e18；n,a 到 1e9 时安全，更大请用 floor_sum_mod）。
// 验证: floor_sum(5,5,2,1)=3（i=0..4 的 floor 值为 0,0,1,1,1）。
long long floor_sum(long long n, long long m, long long a, long long b) {
    __int128 ans = 0;
    while (true) {
        if (a >= m) {
            ans += (__int128)(n - 1) * n / 2 * (a / m);
            a %= m;
        }
        if (b >= m) {
            ans += (__int128)n * (b / m);
            b %= m;
        }
        __int128 y = (__int128)a * n + b;
        if (y < m) break;
        n = (long long)(y / m);
        b = (long long)(y % m);
        swap(m, a);
    }
    return (long long)ans;
}

// 【速查】类欧几里得取模版：同 floor_sum，结果 mod，n 可到 1e18
// 复杂度: O(log max(a, m))
// 取模版: Σ_{i=0}^{n-1} floor((a*i+b)/m) mod mod，O(log max(a,m))
// 适用于 n 很大（1e18）而只需要模值的场景；要求 mod>0，a,b>=0。
// 注意: 每步都对 mod 取模，中间乘法用 __int128，避免 long long 溢出。
long long floor_sum_mod(long long n, long long m, long long a, long long b, long long mod) {
    __int128 ans = 0;
    while (true) {
        if (a >= m) {
            __int128 s = (__int128)(n - 1) * n / 2 % mod;
            ans = (ans + s * ((a / m) % mod)) % mod;
            a %= m;
        }
        if (b >= m) {
            ans = (ans + (__int128)n * ((b / m) % mod)) % mod;
            b %= m;
        }
        __int128 y = (__int128)a * n + b;
        if (y < m) break;
        n = (long long)(y / m);
        b = (long long)(y % m);
        swap(m, a);
    }
    return (long long)ans;
}

// 【速查】类欧变式: ceil_sum 上取整求和；floor_sum_signed 允许 a、b 为负；
//       floor_sum_range(l, r, m, a, b) 求 Σ_{i=l}^{r-1}（注意是左闭右开）
// 复杂度: 都是 O(log)
// 上取整版: Σ_{i=0}^{n-1} ceil((a*i+b)/m)，要求 a,b>=0，m>0
// 恒等式: ceil(x/m)=floor((x+m-1)/m)（x 为整数，m>0）。
// 注意: 这里直接算 b+m-1，要求 b <= LLONG_MAX-(m-1)；否则先拆分 b 再分段。
long long ceil_sum(long long n, long long m, long long a, long long b) {
    return floor_sum(n, m, a, b + m - 1);
}

// 负数版: a,b 可为任意整数（m>0），计算 Σ_{i=0}^{n-1} floor((a*i+b)/m)
// 做法: 做“向下取整”的带余除法 a=qa*m+ra, b=qb*m+rb（0<=ra,rb<m），
//       则 floor((a*i+b)/m)=qa*i+qb+floor((ra*i+rb)/m)，前两项用等差公式。
// 注意: 返回值需能放进 long long；中间量用 __int128，但 qa*n^2/2 过大时仍会溢出。
long long floor_sum_signed(long long n, long long m, long long a, long long b) {
    long long qa = a / m, ra = a % m;
    if (ra < 0) { ra += m; qa--; }
    long long qb = b / m, rb = b % m;
    if (rb < 0) { rb += m; qb--; }
    __int128 ans = (__int128)qa * n * (n - 1) / 2 + (__int128)qb * n
                 + floor_sum(n, m, ra, rb);
    return (long long)ans;
}

// 区间版: Σ_{i=l}^{r-1} floor((a*i+b)/m)，要求 0<=l<=r，a,b 可为任意整数
// 原理: 前缀和相减；区间长度为 0 时返回 0。
long long floor_sum_range(long long l, long long r, long long m, long long a, long long b) {
    return floor_sum_signed(r, m, a, b) - floor_sum_signed(l, m, a, b);
}

// 三合一（exact）: 同时求
//   f = Σ_{i=0}^{n-1} floor((a*i+b)/m)
//   g = Σ_{i=0}^{n-1} i * floor((a*i+b)/m)
//   h = Σ_{i=0}^{n-1} floor((a*i+b)/m)^2
// 要求 n>=0, m>0, a,b>=0。
// 递推（0<=a,b<m 时，令 y=floor((a(n-1)+b)/m)）:
//   f = n*y - f'
//   g = y*S1(n-1) - (h' - f')/2
//   h = n*y^2 - 2g' - f'
//   其中 f',g',h' 是参数 (y, a, m, m+a-1-b) 的子问题，S1(x)=x(x+1)/2。
// exact 版用 __int128 存 f,g,h；h 增长很快（约 a^2 n^3/m^2），
// n 很大时即使 __int128 也会溢出，此时请用 euclid3_mod。
// 验证: euclid3(5,5,2,1) = {3,9,3}。
struct Euclid3 {
    __int128 f, g, h;
};

// 【速查】类欧三合一（精确）：同时求 f = Σ⌊(ai+b)/m⌋、g = Σ i·⌊..⌋、h = Σ ⌊..⌋²，i = 0..n-1
// 用法: auto [f, g, h] = euclid3(n, m, a, b);   // 返回 __int128，a, b ≥ 0
// 注意: h 增长很快，n 大时用 euclid3_mod
// 复杂度: O(log max(a, m))
Euclid3 euclid3(long long n, long long m, long long a, long long b) {
    if (n <= 0) return {0, 0, 0};
    if (a >= m || b >= m) {
        long long qa = a / m, qb = b / m;
        Euclid3 t = euclid3(n, m, a % m, b % m);
        __int128 nn = n;
        __int128 s1 = nn * (n - 1) / 2;
        __int128 s2 = nn * (n - 1) * (2 * nn - 1) / 6;
        __int128 F = (__int128)qa * s1 + (__int128)qb * nn + t.f;
        __int128 G = (__int128)qa * s2 + (__int128)qb * s1 + t.g;
        __int128 H = (__int128)qa * qa * s2 + (__int128)qb * qb * nn + t.h
                   + 2 * (__int128)qa * qb * s1 + 2 * (__int128)qa * t.g
                   + 2 * (__int128)qb * t.f;
        return {F, G, H};
    }
    long long y = (long long)(((__int128)a * (n - 1) + b) / m);
    if (y == 0) return {0, 0, 0};
    long long nb = (long long)((__int128)m + a - 1 - b);
    Euclid3 t = euclid3(y, a, m, nb);
    __int128 nn = n, yy = y;
    __int128 s1 = nn * (n - 1) / 2;
    __int128 F = nn * yy - t.f;
    __int128 G = yy * s1 - (t.h - t.f) / 2;
    __int128 H = nn * yy * yy - 2 * t.g - t.f;
    return {F, G, H};
}

// 三合一（取模）: f,g,h 均对 mod 取模，要求 mod 为 >3 的质数（如 1e9+7）。
// 适用于 n 到 1e18 的大范围；递推与 exact 版相同，除法用 inv2、inv3。
// 注意: mod 不是质数或 <=3 时，2、3 不一定可逆，不能直接用本函数；
//       mod 建议取 1e9 级（如 1e9+7），mod 接近 1e18 时中间乘积可能超过 __int128。
struct Euclid3Mod {
    long long f, g, h;
};

namespace euclid_detail {
Euclid3Mod euclid3_mod_rec(long long n, long long m, long long a, long long b,
                           long long mod, long long inv2, long long inv3) {
    if (n <= 0) return {0, 0, 0};
    if (a >= m || b >= m) {
        long long qa = a / m, qb = b / m;
        Euclid3Mod t = euclid3_mod_rec(n, m, a % m, b % m, mod, inv2, inv3);
        long long nn = n % mod;
        long long s1 = (long long)((__int128)nn * ((n - 1) % mod) % mod * inv2 % mod);
        long long s2 = (long long)((__int128)s1 * ((2 * (__int128)n - 1) % mod) % mod * inv3 % mod);
        long long qam = qa % mod, qbm = qb % mod;
        long long F = (long long)(((__int128)qam * s1 + (__int128)qbm * nn + t.f) % mod);
        long long G = (long long)(((__int128)qam * s2 + (__int128)qbm * s1 + t.g) % mod);
        long long H = (long long)(((__int128)qam * qam % mod * s2
                                 + (__int128)qbm * qbm % mod * nn
                                 + t.h
                                 + 2 * (__int128)qam % mod * qbm % mod * s1
                                 + 2 * (__int128)qam % mod * t.g
                                 + 2 * (__int128)qbm % mod * t.f) % mod);
        return {F, G, H};
    }
    long long y = (long long)(((__int128)a * (n - 1) + b) / m);
    if (y == 0) return {0, 0, 0};
    long long nb = (long long)((__int128)m + a - 1 - b);
    Euclid3Mod t = euclid3_mod_rec(y, a, m, nb, mod, inv2, inv3);
    long long nn = n % mod, yy = y % mod;
    long long s1 = (long long)((__int128)nn * ((n - 1) % mod) % mod * inv2 % mod);
    long long F = (long long)(((__int128)nn * yy - t.f + mod) % mod);
    long long G = (long long)(((__int128)yy * s1
                              - (__int128)(t.h - t.f + mod) % mod * inv2 % mod
                              + mod) % mod);
    long long H = (long long)(((__int128)nn * yy % mod * yy
                              - 2 * (__int128)t.g - t.f + 3 * mod) % mod);
    return {F, G, H};
}
} // namespace euclid_detail

// 【速查】类欧三合一取模版：f、g、h 都对质数 mod (>3) 取模，n 可到 1e18
// 用法: auto r = euclid3_mod(n, m, a, b, 1e9+7);  r.f / r.g / r.h
// 复杂度: O(log max(a, m))
Euclid3Mod euclid3_mod(long long n, long long m, long long a, long long b, long long mod) {
    long long inv2 = (mod + 1) / 2;              // mod 为奇质数
    long long inv3 = ksm(3, mod - 2, mod);       // 费马小定理
    return euclid_detail::euclid3_mod_rec(n, m, a, b, mod, inv2, inv3);
}

// 【速查】SOSDP（子集和 / 高维前缀和）
// 用途: dp[mask] = Σ_{sub ⊆ mask} A[sub]；超集和把条件改成 !((j>>i)&1) 并加 dp[j | (1<<i)]
// 用法: 照抄下面两重循环（外层枚举位，内层枚举 mask）
// 复杂度: O(n · 2^n)
// SOSDP（Sum Over Subsets）示例: O(n * 2^n) 求每个 mask 的子集权值和。
// 自检示例: 原版示例 n=50 会分配 2^50 个 int，一运行即崩溃，现已改为可运行的 4 位示例。
// 注意: 下面循环算的是“子集和” dp[mask] = Σ{ A[i] : (i & mask) == i }（与 FWT-or 正变换同向），
//       不是超集和。要求超集和请把条件改成 if(!((j>>i)&1)) dp[j] += dp[j | (1<<i)]。
signed main() {
    int n = 4;                     // 位数, 状态数 1<<n
    int total_states = (1 << n);
    vector<int> A(total_states, 0);
    A[0b0001] = 1; A[0b0110] = 2; A[0b1010] = 3; A[0b1111] = 4;

    vector<int> dp = A;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < total_states; ++j) {
            if ((j >> i) & 1) {
                dp[j] += dp[j ^ (1 << i)];
            }
        }
    }

    // 自检: 与暴力子集求和逐掩码对比
    for (int mask = 0; mask < total_states; mask++) {
        int brute = 0;
        for (int sub = mask; ; sub = (sub - 1) & mask) {
            brute += A[sub];
            if (sub == 0) break;
        }
        if (dp[mask] != brute) {
            cout << "SOSDP SELF-CHECK FAILED at mask " << mask << endl;
            return 1;
        }
    }
    cout << "SOSDP subset-sum self-check passed (16 masks)" << endl;
    return 0;
}