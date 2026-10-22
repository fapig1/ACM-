#include <bits/stdc++.h>
using namespace std;
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
