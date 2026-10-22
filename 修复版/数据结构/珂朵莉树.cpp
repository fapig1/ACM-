#include <bits/stdc++.h>
using namespace std;

// 【速查】珂朵莉树 ODT（颜色段均摊，CF896C）
// 用途: 有"区间赋值"操作且数据随机时，用 set 维护值相同的连续段，暴力处理其余操作
// 用法: ODT t;  t.init(a, n);   // a 为 1-based，自动加哨兵 [n+1, n+1]
//       t.assign(l, r, v)  区间赋值;   t.add(l, r, v)  区间加;
//       t.kth(l, r, k) -> 区间第 k 小;   t.powsum(l, r, x, mod) -> Σ a_i^x mod mod
// 注意: 先 split(r + 1) 再 split(l)，顺序反了迭代器会失效
//       只有存在区间赋值且数据随机时复杂度才有保证，构造数据会被卡
// 复杂度: 随机数据下均摊 O(n log log n) 段操作
struct ODT {
    struct Node {
        long long l, r;
        mutable long long v;
        bool operator<(const Node& o) const { return l < o.l; }
    };
    set<Node> s;

    void init(const vector<long long>& a, int n) {
        s.clear();
        for (int i = 1; i <= n; i++) s.insert({i, i, a[i]});
        s.insert({n + 1, n + 1, 0});
    }
    // 把包含 pos 的段从 pos 处断开，返回以 pos 开头的段
    set<Node>::iterator split(long long pos) {
        auto it = s.lower_bound({pos, 0, 0});
        if (it != s.end() && it->l == pos) return it;
        --it;
        long long l = it->l, r = it->r, v = it->v;
        s.erase(it);
        s.insert({l, pos - 1, v});
        return s.insert({pos, r, v}).first;
    }
    void assign(long long l, long long r, long long v) {
        auto itr = split(r + 1), itl = split(l);
        s.erase(itl, itr);
        s.insert({l, r, v});
    }
    void add(long long l, long long r, long long v) {
        auto itr = split(r + 1), itl = split(l);
        for (auto it = itl; it != itr; ++it) it->v += v;
    }
    long long kth(long long l, long long r, long long k) {
        auto itr = split(r + 1), itl = split(l);
        vector<pair<long long, long long>> seg;   // (值, 个数)
        for (auto it = itl; it != itr; ++it) seg.push_back({it->v, it->r - it->l + 1});
        sort(seg.begin(), seg.end());
        for (auto& [v, c] : seg) { if (k <= c) return v; k -= c; }
        return -1;
    }
    static long long qpow(long long a, long long b, long long m) {
        long long res = 1 % m; a %= m; if (a < 0) a += m;
        for (; b; b >>= 1, a = (__int128)a * a % m) if (b & 1) res = (__int128)res * a % m;
        return res;
    }
    long long powsum(long long l, long long r, long long x, long long mod) {
        auto itr = split(r + 1), itl = split(l);
        long long res = 0;
        for (auto it = itl; it != itr; ++it)
            res = (res + (__int128)qpow(it->v, x, mod) * ((it->r - it->l + 1) % mod)) % mod;
        return res;
    }
};
