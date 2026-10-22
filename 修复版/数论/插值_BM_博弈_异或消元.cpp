#include <bits/stdc++.h>
using namespace std;
using ll = long long;

static ll qpowm(ll a, ll b, ll m) {
    ll r = 1 % m; a %= m; if (a < 0) a += m;
    for (; b; b >>= 1, a = (__int128)a * a % m) if (b & 1) r = (__int128)r * a % m;
    return r;
}

// 【速查】拉格朗日插值（P4781）
// 用途: 已知 n 个点 (x_i, y_i) 确定的 n-1 次多项式，求它在 k 处的值（模质数）
// 用法: lagrange(xs, ys, k, mod)      // 任意互不相同的 x，O(n²)
//       lagrangeConsecutive(ys, k, mod) // x = 0, 1, ..., n-1 时，O(n)；常用于"答案是 k 的多项式"（如 Σi^m）
// 套路: 自然数幂和 Σ_{i=1}^{k} i^m 是 m+1 次多项式，算出前 m+2 项直接插值
ll lagrange(const vector<ll>& xs, const vector<ll>& ys, ll k, ll mod) {
    int n = xs.size();
    ll res = 0;
    for (int i = 0; i < n; i++) {
        ll num = ys[i] % mod, den = 1;
        for (int j = 0; j < n; j++) if (j != i) {
            num = (__int128)num * (((k - xs[j]) % mod + mod) % mod) % mod;
            den = (__int128)den * (((xs[i] - xs[j]) % mod + mod) % mod) % mod;
        }
        res = (res + (__int128)num * qpowm(den, mod - 2, mod)) % mod;
    }
    return (res + mod) % mod;
}
ll lagrangeConsecutive(const vector<ll>& ys, ll k, ll mod) {
    int n = ys.size();
    if (k >= 0 && k < n) return (ys[k] % mod + mod) % mod;
    vector<ll> pre(n + 1, 1), suf(n + 1, 1), fac(n, 1), ifac(n, 1);
    ll km = (k % mod + mod) % mod;
    for (int i = 0; i < n; i++) pre[i + 1] = (__int128)pre[i] * ((km - i + mod) % mod) % mod;
    for (int i = n - 1; i >= 0; i--) suf[i] = (__int128)suf[i + 1] * ((km - i + mod) % mod) % mod;
    for (int i = 1; i < n; i++) fac[i] = (__int128)fac[i - 1] * i % mod;
    ifac[n - 1] = qpowm(fac[n - 1], mod - 2, mod);
    for (int i = n - 1; i > 0; i--) ifac[i - 1] = (__int128)ifac[i] * i % mod;
    ll res = 0;
    for (int i = 0; i < n; i++) {
        ll t = (__int128)ys[i] % mod * pre[i] % mod * suf[i + 1] % mod * ifac[i] % mod * ifac[n - 1 - i] % mod;
        if ((n - 1 - i) & 1) t = (mod - t) % mod;
        res = (res + t) % mod;
    }
    return (res + mod) % mod;
}

// 【速查】Berlekamp-Massey + 线性递推求第 k 项（P5487、P4723）
// 用途: 只知道数列前若干项（猜它满足某个 d 阶线性递推），求递推式或第 k 项（k 到 1e18）
// 用法: auto c = BM(a, mod);      // 返回 c，满足 a[i] = Σ_{j=1}^{d} c[j-1]·a[i-j]（d = c.size()）
//       ll v = linearRec(a, c, k, mod);   // a 的第 k 项（0-based），用前 d 项 + 递推式
// 注意: 给的项数至少要 2d 才能唯一确定 d 阶递推；mod 为质数
// 复杂度: BM O(n²)，求第 k 项 O(d² log k)
vector<ll> BM(const vector<ll>& a, ll mod) {
    vector<ll> C = {1}, B = {1};                 // C(x) = 1 - c1 x - c2 x^2 ...
    ll L = 0, m = 1, b = 1;
    for (size_t n = 0; n < a.size(); n++) {
        ll d = 0;
        for (ll i = 0; i <= L && i < (ll)C.size(); i++) d = (d + (__int128)C[i] * a[n - i]) % mod;
        if (d == 0) { m++; continue; }
        vector<ll> T = C;
        ll coef = (__int128)d * qpowm(b, mod - 2, mod) % mod;
        if (C.size() < B.size() + m) C.resize(B.size() + m, 0);
        for (size_t i = 0; i < B.size(); i++) C[i + m] = ((C[i + m] - (__int128)coef * B[i]) % mod + mod) % mod;
        if (2 * L <= (ll)n) { L = n + 1 - L; B = T; b = d; m = 1; }
        else m++;
    }
    C.resize(L + 1);
    vector<ll> res(L);
    for (int i = 1; i <= L; i++) res[i - 1] = (mod - C[i]) % mod;
    return res;
}
ll linearRec(const vector<ll>& a, const vector<ll>& c, ll k, ll mod) {
    int d = c.size();
    if (d == 0) return 0;
    if (k < d) return (a[k] % mod + mod) % mod;
    auto mul = [&](const vector<ll>& p, const vector<ll>& q) {   // p*q mod 特征多项式
        vector<ll> r(2 * d - 1, 0);
        for (int i = 0; i < d; i++) if (p[i]) for (int j = 0; j < d; j++) r[i + j] = (r[i + j] + (__int128)p[i] * q[j]) % mod;
        for (int i = 2 * d - 2; i >= d; i--) if (r[i])
            for (int j = 1; j <= d; j++) r[i - j] = (r[i - j] + (__int128)r[i] * c[j - 1]) % mod;
        r.resize(d);
        return r;
    };
    vector<ll> res(d, 0), base(d, 0);
    res[0] = 1;
    if (d == 1) base[0] = c[0] % mod; else base[1] = 1;   // base = x mod 特征多项式
    for (ll e = k; e; e >>= 1, base = mul(base, base)) if (e & 1) res = mul(res, base);
    ll ans = 0;
    for (int i = 0; i < d; i++) ans = (ans + (__int128)res[i] * (a[i] % mod)) % mod;
    return (ans + mod) % mod;
}

// 【速查】SG 函数（公平组合游戏）
// 用途: 多个独立子游戏的和：先手必胜 <=> 各子游戏 SG 值异或 != 0
// 用法: auto sg = sgTable(N, moves);  // 一堆 x 个石子每次可取 moves 里的某个数，sg[0..N]
//       Nim: 每堆 SG = 石子数，直接异或；其他游戏把 mex 的集合换成"一步能到的状态的 SG"
// 常见结论: 取 1..m 个 -> sg = x % (m+1)；Nim 必胜 <=> 异或和 != 0；阶梯 Nim -> 只看奇数阶
// 复杂度: O(N × |moves|)
vector<int> sgTable(int N, const vector<int>& moves) {
    vector<int> sg(N + 1, 0);
    for (int x = 1; x <= N; x++) {
        vector<char> seen(moves.size() + 2, 0);
        for (int mv : moves) if (mv <= x && sg[x - mv] <= (int)moves.size()) seen[sg[x - mv]] = 1;
        while (seen[sg[x]]) sg[x]++;              // mex
    }
    return sg;
}

// 【速查】异或方程组高斯消元（bitset，P2447）
// 用途: n 个 0/1 变量、m 个方程 (a_i1 x_1 ^ ... ^ a_in x_n = b_i)，判断解的情况并求一组解
// 用法: XorGauss<最大变量数+1> g(n);  g.addEq(row);   // row[0..n-1] 系数，row[n] 是右边的常数
//       int r = g.solve();  // -1 无解；否则返回自由元个数（解的个数 = 2^r），g.x[] 是一组解（自由元取 0）
// 复杂度: O(n² m / 64)
template <int MAXN>
struct XorGauss {
    int n;
    vector<bitset<MAXN>> rows;
    vector<int> x;
    XorGauss(int n) : n(n) {}
    void addEq(const bitset<MAXN>& row) { rows.push_back(row); }
    int solve() {
        int m = rows.size(), r = 0;
        vector<int> where(n, -1);
        for (int col = 0; col < n && r < m; col++) {
            int p = -1;
            for (int i = r; i < m; i++) if (rows[i][col]) { p = i; break; }
            if (p == -1) continue;
            swap(rows[p], rows[r]);
            for (int i = 0; i < m; i++) if (i != r && rows[i][col]) rows[i] ^= rows[r];
            where[col] = r++;
        }
        for (int i = r; i < m; i++) if (rows[i][n]) return -1;
        x.assign(n, 0);
        for (int col = 0; col < n; col++) if (where[col] != -1) x[col] = rows[where[col]][n];
        return n - r;
    }
};
