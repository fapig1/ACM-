# David_数论（10.21 修复版，10.22 注释补充）

> **通用前导**：本文档所有代码块为片段，需配合以下前导使用：

```cpp
#include <bits/stdc++.h>
using namespace std;
```

标注 **【本次修复】** 的模板经过 2026-10-21 全量审查修复，并全部通过随机对拍/压测回归验证；各修复点在代码注释中亦有说明。

> **2026-10-22 补充**：四个数论模板增加了原理、复杂度、前提与常见坑的注释；
> 组合数学恒等式（范德蒙德、曲棍球棒、错排/卡特兰等）见
> [数论恒等式.md](../../修复版/数论/数论恒等式.md) 第 8~10 节。

[TOC]

### 排列组合

> 【本次修复】Derangement 递推 (n-1)→(i-1) 并补 return（原版为 UB）；D(0)=1。注意 C/A 无越界检查（n≤maxn）。
> 注释补充：A/C 的取模前提、错排递推的组合解释与通项、卡特兰数的计数对象与递推。

```cpp
#define int long long

// ============================== 排列组合 ==============================
// 内容: 阶乘/阶乘逆元预处理，O(1) 求排列数 A(n,m)、组合数 C(n,m)，以及错排数、卡特兰数。
// 复杂度: init() 为 O(maxn)，之后 A/C 单次 O(1)；Derangement/Catalan 为 O(n)。
// 前提与注意:
//   1) mod = 1e9+7 是质数，且所有阶乘下标均 < mod，费马小定理求逆元才成立；
//   2) A/C 不做越界检查，调用前保证 0 <= m <= n <= maxn；
//   3) 若 n >= mod，n! 中会出现因子 mod 使阶乘为 0，必须改用 Lucas（见 数论进阶.cpp）；
//   4) 多组数据只调用一次 init() 即可；maxn 按题目上限设置，fac/inv 各占 8 字节/项。
// 组合恒等式（帕斯卡、范德蒙德、曲棍球棒等）的证明与验算见 数论恒等式.md。

const int mod = 1e9 + 7;
const int maxn = 2e5;

// fac[i] = i! mod
// inv[i] = (i!)^{-1} mod：先对 maxn! 用一次费马小定理，再倒推所有阶乘逆元
vector<int> fac(maxn + 5), inv(maxn + 5);

// 快速幂: 计算 x^a mod m，O(log a)
int ksm(int x, int a, int mod){
    int ans = 1;
    while(a){
        if(a & 1) ans = ans * x % mod;
        x = x * x % mod;
        a >>= 1;
    }
    return ans;
}

// 预处理阶乘与阶乘逆元；使用 A/C 前必须调用一次
void init(){
    fac[0] = 1;
    for(int i = 1; i <= maxn; i++){
        fac[i] = fac[i - 1] * i % mod;
    }
    inv[maxn] = ksm(fac[maxn], mod - 2, mod);          // 费马小定理: (maxn!)^(mod-2)
    for(int i = maxn - 1; i >= 0; i--){
        inv[i] = inv[i + 1] * (i + 1) % mod;           // (i!)^-1 = ((i+1)!)^-1 * (i+1)
    }
}

// 排列数 A(n,m) = n!/(n-m)!：从 n 个不同元素中有序取 m 个
int A(int n, int m){
    return fac[n] * inv[n - m] % mod;
}

// 组合数 C(n,m) = n!/(m!(n-m)!)：从 n 个不同元素中无序取 m 个
int C(int n, int m){
    return fac[n] * inv[m] % mod * inv[n - m] % mod;
}

// 错排数 D(n)：1..n 的排列中满足 p_i != i 的排列个数
// 约定 D(0)=1（空排列算一种）、D(1)=0、D(2)=1
// 递推: D(n) = (n-1) * (D(n-1) + D(n-2))
// 理解: 先放元素 n 到位置 k（n-1 种选法）；
//       若 k 恰好放到位置 n，剩下 n-2 个元素错排，贡献 D(n-2)；
//       否则把位置 n 视为“不能放 k”，剩下 n-1 个元素构成错排，贡献 D(n-1)。
// 通项: D(n) = n! * Σ_{k=0}^{n} (-1)^k / k!（见 数论恒等式.md）
int Derangement(int n){
    vector<int> dp(n + 1);
    dp[0] = 1;
    if(n >= 1) dp[1] = 0;
    if(n >= 2) dp[2] = 1;
    for(int i = 3; i <= n; i++){
        dp[i] = (i - 1) * (dp[i - 1] + dp[i - 2]) % mod;
    }
    return dp[n];
}

// 卡特兰数 Cat(n) = C(2n,n)/(n+1) = C(2n,n) - C(2n,n-1)
// 计数对象（都满足同一递推 Cat(n) = Σ_{i=0}^{n-1} Cat(i)Cat(n-1-i)）:
//   * n 对括号的合法序列数；
//   * n 个结点的不同二叉树形态数；
//   * 凸 (n+2) 边形的三角剖分数；
//   * 1..n 依次入栈的合法出栈序列数；
//   * 从 (0,0) 到 (n,n) 且不越过对角线 y=x 的单调路径数。
// 注意: 减法可能得到负数，最后统一调整到 [0, mod)
int CatalanNumber(int n){
    int ans = C(2 * n, n) - C(2 * n, n + 1);
    return (ans % mod + mod) % mod;
}

// 范德蒙德卷积（证明与变形见 数论恒等式.md 第 9 节）:
//   Σ_{k=0}^{n} C(r,k) * C(s,n-k) = C(r+s,n)
// 需要时直接枚举 k 累加即可；前提是 r+s <= maxn 且 r,s < mod。
// 若 r+s >= mod，阶乘会出现 0，C 失效，请改用 Lucas（数论进阶.cpp）。
```

### 数论基础

> 【本次修复】main 由 2^50 分配的坏示例改为 16 掩码自检示例；并纠正 SOSDP 注释（该循环算的是子集和/FWT-or 方向，非超集和）。含线性筛全家桶/EXGCD/CRT/EXCRT/floor_sum/NTT/FFT/FWT/康托展开(注意 fact 只到 9!)/整除分块。
> 注释补充：欧拉筛只被最小质因子筛一次、线性筛各数组的转移、exgcd 通解、CRT 互质前提、exCRT 合并与溢出、杜教筛下标变换、类欧的适用条件。

```cpp
#define int long long

// ============================== 数论基础 ==============================
// 内容: 欧拉筛/最小质因子、线性筛全家桶（μ/φ/τ/σ）、快速幂、exgcd、
//       CRT/exCRT、单点欧拉函数、莫比乌斯反演、狄利克雷前缀/后缀变换、
//       康托展开、FFT/NTT、多项式求逆、FWT、整除分块、杜教筛、类欧几里得、SOSDP。
// 约定:
//   * 本文件用 #define int long long，所有 int 均为 64 位；与自带 int 的代码拼接时注意。
//   * 全局数组按 maxn = 1e6 开；linear_sieve 的内存约为 6 个 long long 数组，按题目缩小 maxn。
//   * init 与 linear_sieve 都会向全局 primes 里 push，二者不要混用（否则质数表重复）。
//   * 模数统一用全局 mod = 1e9+7；NTT 单独使用 NTT_MOD = 998244353。
// =====================================================================

//欧拉筛
const int mod = 1e9 + 7;
const int maxn = 1e6;
vector<int> primes;
// 欧拉筛（线性筛）: O(maxn) 求出每个数的最小质因子 min_primes[i]，同时得到全局质数表 primes。
// 原理: 每个合数只被它的最小质因子筛掉一次，因此总复杂度是线性的。
// 注意: 只筛到 maxn，getPrimes(x) 要求 x <= maxn；x = 0 或 1 时 min_primes 无效。
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
// 分解质因数（不重复）: 返回 x 的所有不同质因子，升序，O(log x)
// 依赖 init() 先筛出 min_primes；x 必须 <= maxn。
vector<int> getPrimes(int x){
    vector<int> res;
    while(x > 1){
        int p = min_primes[x];
        res.push_back(p);
        while(x % p == 0) x /= p;
    }
    return res;
}
// 分解质因数（带指数）: 返回 {(p, cnt)}，满足 x = ∏ p^cnt，升序，O(log x)
// 例: 360 = 2^3 * 3^2 * 5 -> {(2,3),(3,2),(5,1)}
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


// 线性筛综合版：质数表 + 最小质因子（is_comp/minp_exp）+ 莫比乌斯函数 + 欧拉函数
//              + 约数个数 + 约数和，全部 O(n)。
// 调用 linear_sieve(n) 后直接使用全局数组；n 可以小于 maxn。
//
// 设 i 的最小质因子为 q（由质数表从小到大枚举隐式确定），枚举质数 p 去筛 m = i*p：
//   p < q  : p 与 i 互质，各积性函数直接相乘
//   p == q : p 是 m 的最小质因子，指数 +1，按“去掉一个 p”的商修正
// 每个合数只在其最小质因子处被筛一次，故总复杂度 O(n)。
//
// 各数组含义:
//   mu[i]        莫比乌斯函数 μ(i)
//   phi_arr[i]   欧拉函数 φ(i)
//   div_cnt[i]   约数个数 τ(i)
//   div_sum[i]   约数和 σ(i)
//   minp_exp[i]  i 的最小质因子指数
//   minp_pwsum[i] 1 + p + ... + p^e（p 为最小质因子，e = minp_exp[i]）
//
// 注意: primes 是全局共享的，若之前调用过 init() 会重复；本函数假定 primes 为空。
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


// 快速幂: 计算 x^a mod m，O(log a)
// 要求 a >= 0、m > 0；a = 0 时返回 1 % m。底数 x 不取模，调用前保证 x 不溢出。
int ksm(int x, int a, int m){
    int ans = 1;
    while(a){
        if(a & 1) ans = ans * x % m;
        x = x * x % m;
        a >>= 1;
    }
    return ans;
}


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

// 中国剩余定理（模数两两互质）: 解 x ≡ a[i] (mod r[i]), i = 0..k-1。
// 返回 [0, n) 内的唯一解，n = ∏ r[i]。
// 做法: 令 m_i = n/r_i，用 exgcd 求 m_i 在模 r_i 下的逆元 b_i，
//       则 x = Σ a_i * m_i * b_i (mod n)。
// 前提: 所有 r[i] 两两互质且 > 1；n 必须在 long long 范围内（k 个 1e9 级模数最多约 6 个）。
//       模数不互质请改用下面的 excrt。
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

// 扩展中国剩余定理（模数不必互质）: 解方程组 x ≡ a[i] (mod m[i])，i = 0..n-1。
// 返回最小非负解；无解返回 -1。要求 n >= 1。
// 做法: 维护已合并的解 ans（模 M），每次把 x ≡ a[i] (mod m[i]) 与它合并：
//       先解 M*t ≡ a[i]-ans (mod m[i])，有解当且仅当 gcd(M,m[i]) 整除差值；
//       合并后 M = lcm(M, m[i])。整体 O(n log)。
// 注意: 结果范围由 lcm 决定，lcm 超出 long long 时会溢出（此时需 __int128 或取模版）。
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

// 筛法求欧拉函数（线性筛）: 返回长度为 n+1 的 phi 数组，phi[i] = φ(i)，O(n)。
// 与 linear_sieve 的区别: 这里只需要 φ，函数内部自带局部质数表，不污染全局 primes。
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

// 莫比乌斯反演（μ 由 linear_sieve 预处理，φ 同）
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
// 前置: primes 必须包含所有 <= n 的质数（调用 init() 或 linear_sieve(n) 均可）。
// 原理: 对每个质数 p 做一遍类似“高维前缀和”的扫描，枚举顺序保证每个数被其全部质因子更新。
// 注意:
//   1) 全部原地修改，需要保留原数组请先拷贝；
//   2) 前缀 max/min 与求和方向相同，逆变换则把循环方向反过来做减法；
//   3) 下标从 1 开始使用，f[0] 不参与运算。

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


// 类欧几里得算法: 计算 Σ_{i=0}^{n-1} floor((a*i + b) / m)，O(log max(a,m))
// 要求: n >= 0, m > 0, a >= 0, b >= 0（负数需先把 a,b 调整到非负）。
// 思路:
//   1) 先把 a >= m、b >= m 的整数部分拆出来（等差数列求和）；
//   2) 剩下 0 <= a,b < m，把“对 i 求和”换成“对纵坐标 y 求和”，
//      交换 n 与 m 的角色后递归，规模按欧几里得算法下降，故 O(log)。
// 注意: 中间量 (n-1)*n/2*(a/m) 可能很大，n,a 到 1e9 时需 __int128；
//       本实现用 long long（#define int long long），约可撑到 1e18 的结果。
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
```

### 数论进阶

> Miller-Rabin/Pollard-Rho/字符串指数欧拉降幂/BSGS/卢卡斯/EX卢卡斯/Cipolla/原根。
> 注释补充：Miller-Rabin 的 d·2^s 分解与确定底数、Pollard-Rho 的期望复杂度、扩展欧拉定理的分段条件、exBSGS 的消因子流程、Lucas/exLucas 的模数限制、Cipolla 的期望尝试次数、Min_25 的复杂度与状态含义。

```cpp
#define int long long

// ============================== 数论进阶板子 ==============================
// 内容: 快速乘、Miller-Rabin、Pollard-Rho、欧拉降幂、BSGS/exBSGS、
//       Lucas/exLucas、阶与原根、二次剩余(Cipolla)、Min_25 筛。
// 与 数论.cpp 合并时注意: 本文件自带 ksm / exgcd / oula_phi 副本，请去重。
// 说明: 本文件 ksm 内部用 mul(基于 __int128) 实现，任何模数(含 2^64 级)都安全；
//       若编译器不支持 __int128，可把 mul 换成龟速乘（按位加）。
// 编译: GCC/Clang，-std=c++17；MSVC 不支持 __int128。
// 复杂度速查:
//   Miller-Rabin   O(k log n)，k=7 个底数，2^64 内确定；
//   Pollard-Rho    期望 O(n^{1/4})；
//   BSGS/exBSGS    O(√p) 时间与空间；
//   Lucas          O(p) 预处理 + O(log p · log_p n) 单次；
//   exLucas        O(∑ p^k) 预处理 + O(log n · log p^k) 单次；
//   Cipolla        期望 O(log p)；
//   Min_25 筛      O(n^{3/4} / log n)。
// ==========================================================================

const int mod = 1e9 + 7;

// ============================== 基础工具 ==============================
// 模乘防溢出: 计算 a*b mod m，要求 0 <= a,b < m（否则先取模）
// 用 __int128 承接 64 位乘积；m 可以接近 2^63。
int mul(int a, int b, int m){
    return (int)((__int128)a * b % m);
}

// 快速幂: x^a mod m，内部用 mul 防溢出；要求 a >= 0、m > 1。
// 因为先做了 x %= m，底数可以很大（如 Miller-Rabin 的底数）。
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

// 扩展欧几里得: 返回 g = gcd(a,b)，并求出一组 (x,y) 使 a*x + b*y = g。
// 通解 x' = x + k*b/g, y' = y - k*a/g；用于求逆元和解不定方程。
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

// 模逆元: 返回 a^{-1} mod m，要求 gcd(a,m)=1（m 不必为质数）。
// 结果为 [0,m) 内的最小非负解；m = 1 时无意义。
int inv(int a, int m){
    int x, y;
    exgcd(a, m, x, y);
    return (x % m + m) % m;
}

// 单个数欧拉函数 φ(n)，O(√n)，要求 n >= 1。
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
// 原理: 把 n-1 写成 d * 2^s（d 为奇数），对底数 a 检查
//       a^d ≡ 1 (mod n) 或存在 0 <= r < s 使 a^{d*2^r} ≡ -1 (mod n)；
//       两者都不满足则 n 一定合数。若 n 是奇素数，上述必成立（费马小定理 + 二次探测定理）。
// 底数集 {2,325,9375,28178,450775,9780504,1795265022} 对 n < 2^64 是确定性的。
// 复杂度 O(k log n)，k 为底数个数（这里 7），每次乘法走 mul 的 __int128。
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
// 目标: 对合数 n 找一个非平凡因子（之后再递归分解）。
// 生日悖论: 在模 q(待找因子) 的剩余系里取 O(√q) 个伪随机数 x_i = f(x_{i-1})，
// 出现 x_i ≡ x_j (mod q) 时 gcd(|x_i-x_j|, n) 即非平凡因子。
// 本实现取 f(x) = x^2 + c（c 随机），Floyd 判圈: 龟走 1 步、兔走 2 步，O(1) 空间。
// (Brent 版常数更小，需要时可替换。)
// 期望复杂度 O(n^{1/4})。依赖 is_prime；n 必须是大于 3 的奇数合数。
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

// 质因数分解(带指数)，结果存入 mp（可累加多次调用）。使用示例:
//   map<int,int> mp; factor(341550071728321, mp);
//   mp = {10670053: 1, 32010157: 1}
// 做法: 若 n 是质数直接计入；否则 Pollard-Rho 找一个因子 d，递归分解 d 和 n/d。
// 注意: n 必须能用 long long 表示；递归深度是因子个数级别，不会爆栈。
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
// 对任意 a, p（不要求 gcd(a,p)=1）:
//   b <  φ(p) 时  a^b ≡ a^b (mod p)
//   b >= φ(p) 时  a^b ≡ a^(b mod φ(p) + φ(p)) (mod p)
// b 以十进制字符串传入，可超过 long long。依赖 oula_phi。
// 实现: 逐位累加 e = b mod φ(p)，并记录“真实 b 是否曾 >= φ(p)”（big）；
//       若 big 为真，指数用 e + φ(p)，而不是只取 e。
// 验证: 2^100 mod 7 = 2 (φ(7)=6, 100≥6 → 2^(4+6)=2^10≡2)。
// 要求 p >= 2（p = 1 时取模无意义，ksm 会返回 1 而不是 0）。
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
// 求 a^x ≡ b (mod p) 的最小非负整数解 x，要求 gcd(a, p) = 1（p 不必为质数）。
// 大步小步: 令 m = ⌈√p⌉，把 x 写成 x = i*m - j（0 <= j < m），
//           则 a^{i*m} ≡ b*a^j；先枚举 j 把 b*a^j 丢进哈希表，
//           再枚举 i 查表，命中即得 x = i*m - j。
// 复杂度 O(√p) 时间与空间；无解返回 -1。
// 注意: p 很大时 sqrt(p) 的 double 精度可能差 1，必要时用整数开方；
//       unordered_map 最坏 O(√p) 但常数较大，卡常可换手写哈希。
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

// 扩展 BSGS: gcd(a, p) 任意。逐次消去公因子，把方程归约到普通 BSGS。
// 推导: 设 d = gcd(a,p)。若 d ∤ b 且 b != 1，则无解；
//       否则两边同除以 d，得到 (a/d)*a^{x-1} ≡ b/d (mod p/d)，累计系数 base。
//       重复直到 gcd(a,p)=1，再解 a^t ≡ b * base^{-1} (mod p)，答案 x = t + k。
// 注意: 归约过程中若 base == b 可直接返回 k（对应 x = k 的提前命中）。
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
// 直观: 在 (1+x)^n 上做 Frobenius 同余，(1+x)^p ≡ 1+x^p (mod p)，逐位相乘。
// 前提: p 为质数，n,m >= 0；lucas_fac 只开到 1e6，因此 p <= 1e6。
// 验证: lucas(5, 2, 3) = 1 (C(5,2)=10≡1; 5=12₃, 2=02₃ → C(1,0)C(2,2)=1)。
int lucas_fac[1000005];

// 预处理 0!..(p-1)! mod p；换模数需重新调用
void lucas_init(int p){
    lucas_fac[0] = 1;
    for(int i = 1; i < p; i++){
        lucas_fac[i] = lucas_fac[i - 1] * i % p;
    }
}

// C(n,m) mod p，要求 0 <= n,m < p（调用方已取位，故这里不会越界）
// 用费马小定理求分母逆元，单次 O(log p)。
int C_small(int n, int m, int p){
    if(m > n) return 0;
    return lucas_fac[n] * ksm(lucas_fac[m], p - 2, p) % p
         * ksm(lucas_fac[n - m], p - 2, p) % p;
}

// Lucas 主递归: 每层剥掉 p 进制最低位，深度 O(log_p n)
int lucas(int n, int m, int p){
    if(m == 0) return 1;
    return C_small(n % p, m % p, p) * lucas(n / p, m / p, p) % p;
}

// exLucas: C(n,m) mod p^k (p 质数)。核心: 消去 p 因子 + 周期阶乘 + 逆元。
// 思路: 把 n! 写成 p^{v} * (与 p 互质的部分)，组合数三部分相除，
//       互质部分可逆，p 的指数用 Legendre 公式算；最后乘回 p^{v}。
// fac_ex[i] = ∏_{j<=i, p∤j} j (mod pk)，以 pk 为周期。
// 注意: fac_ex 只开到 1e6，因此要求 pk <= 1e6；换模数必须重新 exlucas_init。
int fac_ex[1000005];

// 预处理一个周期内的“去 p 阶乘”前缀积，O(pk)
void exlucas_init(int p, int pk){
    fac_ex[0] = 1;
    for(int i = 1; i <= pk; i++){
        if(i % p) fac_ex[i] = fac_ex[i - 1] * i % pk;
        else fac_ex[i] = fac_ex[i - 1];
    }
}

// n! 去掉全部 p 因子后的部分 (mod pk)
// 递推: f(n) = f(n/p) * fac_ex[pk]^{n/pk} * fac_ex[n%pk] (mod pk)
// 第一项收集所有 p 的倍数贡献，后两项是完整周期与零头。
int f(int n, int p, int pk){
    if(n == 0) return 1;
    return f(n / p, p, pk) * ksm(fac_ex[pk], n / pk, pk) % pk * fac_ex[n % pk] % pk;
}

// C(n,m) mod p^k，要求 n,m >= 0 且 n >= m（否则按 0 处理需自行加判断）
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

// C(n,m) mod M，M 任意合数: 分解 M = ∏ p^k，逐块用 exLucas 求值，再 CRT 合并。
// 合并: 对每个 p^k 算 a，贡献 a * (M/p^k) * inv(M/p^k mod p^k, p^k)（模 M）。
// 复杂度: 对 M 的每个质因子做一次 O(p^k) 预处理，总 O(∑ p^k + log M * log n)。
// 适用: M <= 1e6 左右（受 fac_ex 大小限制）；M 很大时需动态分配或换算法。
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
// 阶: 最小的 k > 0 使 a^k ≡ 1 (mod p)，记 ord_p(a)。存在性要求 gcd(a,p)=1，
//     且由拉格朗日定理 ord_p(a) | φ(p) = p-1（p 为质数时）。
// 原根: 阶恰为 p-1 的元素，其幂遍历 1..p-1。模质数 p 的原根一定存在。
// 判定: g 是原根 <=> 对 p-1 的每个不同质因子 q，都有 g^{(p-1)/q} ≢ 1 (mod p)。
// 求最小原根: 分解 p-1，从 g=2 开始逐个检验；最小原根通常很小，期望 O(log) 次尝试。
// 复杂度: 分解 p-1 为 O(√p)，检验为 O(ω(p-1) log p)。
// 注意: 要求 p 为奇质数（p=2 时原根应为 1，本函数会返回 2，需特判）。
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
// 解 x^2 ≡ n (mod p)，p 为奇质数。
// 判存在: 欧拉判别法，n^{(p-1)/2} ≡ 1 时有解，≡ -1 时无解（n=0 解为 0）。
// 做法: 取 a 使 w = a^2 - n 为非二次剩余，在扩域 F_p[√w] 中计算
//       (a + √w)^{(p+1)/2}，其虚部必为 0，实部就是一个解（另一个为 p-x）。
// 返回一个解 x，无解返回 -1；期望尝试 2 次即可找到合适的 a，复杂度 O(log p)。
// 注意: p=2 时本函数不适用（需单独返回 n）；sqrt(w) 只是形式符号，不实际开方。
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

> 【本次修复】mat(n) 单参构造由 n×0 改为 n×n（原版直接赋值即越界）。含高斯消元/行列式/求逆(实数与取模)/矩阵快速幂/矩阵树定理。
> 注释补充：线性基的插入/最大异或查询原理、矩阵运算的形状前提、部分主元与 eps 判定、矩阵树定理的 Laplace 构造与重边/自环约定。

```cpp
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
```
