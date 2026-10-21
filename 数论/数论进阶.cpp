#include <bits/stdc++.h>
using namespace std;
#define int long long

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
