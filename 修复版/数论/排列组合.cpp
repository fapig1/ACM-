#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// ============================== 排列组合 ==============================
// 内容: 阶乘 / 阶乘逆元预处理，O(1) 求排列数 A(n,m)、组合数 C(n,m)，以及错排数、卡特兰数。
// 前提与注意:
//   1) mod 必须是质数，且所有阶乘下标 < mod，费马小定理求逆元才成立；
//   2) n >= mod 时 n! 含因子 mod 变成 0，必须改用 Lucas（见 数论进阶.cpp）；
//   3) 多组数据只构造一次即可；fac / ifac 各占 8 字节 / 项。
// 组合恒等式（帕斯卡、范德蒙德、曲棍球棒等）的证明与验算见 数论恒等式速查。
// 10.22: 原版是全局 fac / inv 数组 + init()，已封装为结构体，并让 A / C 越界时返回 0

// 【速查】组合数学工具 Comb：阶乘逆元 → C(n,m)、A(n,m)、错排、卡特兰（模质数）
// 用法: Comb cb(上限 N, mod);   // 预处理 0..N 的阶乘与逆元
//       cb.C(n, m)  cb.A(n, m)   // m < 0 或 m > n 返回 0；要求 n <= N
//       cb.catalan(n)            // 要求 2n <= N
//       Comb::derangement(n, mod) // 错排数 D(n)，不需要预处理
// 复杂度: 预处理 O(N)，C / A / catalan O(1)，derangement O(n)
struct Comb {
    ll mod;
    vector<ll> fac, ifac;

    static ll qpow(ll a, ll b, ll m) {
        ll r = 1 % m; a %= m;
        for (; b; b >>= 1, a = a * a % m) if (b & 1) r = r * a % m;
        return r;
    }
    Comb(int N, ll mod = 1000000007) : mod(mod), fac(N + 1), ifac(N + 1) {
        fac[0] = 1;
        for (int i = 1; i <= N; i++) fac[i] = fac[i - 1] * i % mod;
        ifac[N] = qpow(fac[N], mod - 2, mod);            // 费马小定理: (N!)^(mod-2)
        for (int i = N; i > 0; i--) ifac[i - 1] = ifac[i] * i % mod;   // (i-1)!^-1 = i!^-1 * i
    }
    // 排列数 A(n,m) = n!/(n-m)!：从 n 个不同元素中有序取 m 个
    ll A(int n, int m) { return (m < 0 || m > n) ? 0 : fac[n] * ifac[n - m] % mod; }
    // 组合数 C(n,m) = n!/(m!(n-m)!)：从 n 个不同元素中无序取 m 个
    ll C(int n, int m) { return (m < 0 || m > n) ? 0 : fac[n] * ifac[m] % mod * ifac[n - m] % mod; }

    // 卡特兰数 Cat(n) = C(2n,n) - C(2n,n+1) = C(2n,n)/(n+1)
    // 计数对象（都满足 Cat(n) = Σ_{i=0}^{n-1} Cat(i)Cat(n-1-i)）:
    //   n 对括号的合法序列 / n 个结点的二叉树形态 / 凸 (n+2) 边形三角剖分 /
    //   1..n 依次入栈的出栈序列 / (0,0) 到 (n,n) 不越过对角线的单调路径
    ll catalan(int n) { return ((C(2 * n, n) - C(2 * n, n + 1)) % mod + mod) % mod; }

    // 错排数 D(n)：1..n 的排列中 p_i != i 的个数；D(0)=1、D(1)=0
    // 递推: D(n) = (n-1)(D(n-1) + D(n-2))——元素 n 放到位置 k（n-1 种），
    //       若 k 也放到位置 n 剩下 n-2 个错排（D(n-2)），否则相当于 n-1 个错排（D(n-1)）
    // 通项: D(n) = n! · Σ_{k=0}^{n} (-1)^k / k!
    static ll derangement(int n, ll mod = 1000000007) {
        if (n == 0) return 1;
        ll a = 1, b = 0;                         // D(0), D(1)
        for (int i = 2; i <= n; i++) { ll c = (ll)(i - 1) % mod * ((a + b) % mod) % mod; a = b; b = c; }
        return b;
    }
};

// 范德蒙德卷积（证明与变形见 数论恒等式速查 第 9 节）:
//   Σ_{k=0}^{n} C(r,k) * C(s,n-k) = C(r+s,n)
// 需要时直接枚举 k 累加即可；前提是 r+s <= N 且 r,s < mod。
