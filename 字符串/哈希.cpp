#include <bits/stdc++.h>
using namespace std;
#define int long long

//字符串哈希
struct StringHash {
    static const int MAXN = 1e6 + 5;
    static int p1[MAXN], p2[MAXN];
    static int base, mod1, mod2;
    static bool is_initialized;

    vector<int> h1, h2;
    int n;

    // 静态初始化：全局只跑一次，预处理幂次和随机基数
    static void init() {
        if (is_initialized) return;
        is_initialized = true;
        
        // 使用时间戳生成随机基数，防止被 Hack
        mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
        base = uniform_int_distribution<int>(300, 1000000)(rng);
        
        mod1 = 1e9 + 7;
        mod2 = 1e9 + 9; 

        p1[0] = p2[0] = 1;
        for (int i = 1; i < MAXN; i++) {
            p1[i] = p1[i - 1] * base % mod1;
            p2[i] = p2[i - 1] * base % mod2;
        }
    }

    // 构造函数：传入字符串，自动计算前缀哈希
    StringHash(const string& s) {
        init();
        n = s.size();
        h1.assign(n + 1, 0);
        h2.assign(n + 1, 0);
        for (int i = 0; i < n; i++) {
            h1[i + 1] = (h1[i] * base + s[i]) % mod1;
            h2[i + 1] = (h2[i] * base + s[i]) % mod2;
        }
    }

    // 获取子串 [l, r] 的双哈希值 (0-indexed)
    // 例如 s = "abcde", get(1, 3) 返回 "bcd" 的哈希
    pair<int, int> get(int l, int r) {
        int len = r - l + 1;
        int res1 = (h1[r + 1] - h1[l] * p1[len] % mod1 + mod1) % mod1;
        int res2 = (h2[r + 1] - h2[l] * p2[len] % mod2 + mod2) % mod2;
        return {res1, res2};
    }
};
