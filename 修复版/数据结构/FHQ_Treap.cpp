#include <bits/stdc++.h>
using namespace std;

// 【速查】FHQ Treap（无旋 Treap）· 权值平衡树
// 用途: 动态可重集合：插入、删除、查排名、查第 k 小、前驱、后继（P3369 / P6136）
// 用法: Treap t(最大插入次数);  t.insert(x);  t.erase(x)（只删一个）;
//       t.rank(x) -> 比 x 小的个数 + 1;  t.kth(k) -> 第 k 小（1-based，k 越界返回 LLONG_MIN）;
//       t.prev(x) -> 严格小于 x 的最大值（没有返回 LLONG_MIN）;  t.next(x) -> 严格大于 x 的最小值（没有返回 LLONG_MAX）
// 复杂度: 每次期望 O(log n)；节点数 = 插入次数
struct Treap {
    struct Node { int l, r, sz; unsigned pri; long long key; };
    vector<Node> t;
    int root = 0;
    mt19937 rng{20251022};

    Treap(int maxn = 0) { t.reserve(maxn + 1); t.push_back({0, 0, 0, 0, 0}); }

    int newNode(long long k) { t.push_back({0, 0, 1, (unsigned)rng(), k}); return (int)t.size() - 1; }
    void pull(int u) { t[u].sz = t[t[u].l].sz + t[t[u].r].sz + 1; }

    // 按值分裂: a 中 key <= k，b 中 key > k
    void split(int u, long long k, int &a, int &b) {
        if (!u) { a = b = 0; return; }
        if (t[u].key <= k) { a = u; split(t[u].r, k, t[u].r, b); }
        else { b = u; split(t[u].l, k, a, t[u].l); }
        pull(u);
    }
    // 合并: 要求 a 的所有 key <= b 的所有 key
    int merge(int a, int b) {
        if (!a || !b) return a | b;
        if (t[a].pri > t[b].pri) { t[a].r = merge(t[a].r, b); pull(a); return a; }
        t[b].l = merge(a, t[b].l); pull(b); return b;
    }

    void insert(long long x) {
        int a, b; split(root, x, a, b);
        int c = newNode(x);
        root = merge(merge(a, c), b);
    }
    void erase(long long x) {
        int a, b, c;
        split(root, x, a, c);
        split(a, x - 1, a, b);              // b 里全是等于 x 的点
        if (b) b = merge(t[b].l, t[b].r);   // 删掉一个
        root = merge(merge(a, b), c);
    }
    int rank(long long x) {
        int a, b; split(root, x - 1, a, b);
        int res = t[a].sz + 1;
        root = merge(a, b);
        return res;
    }
    long long kth(int k) {
        int u = root;
        if (k < 1 || k > t[root].sz) return LLONG_MIN;
        while (u) {
            int ls = t[t[u].l].sz;
            if (k <= ls) u = t[u].l;
            else if (k == ls + 1) return t[u].key;
            else k -= ls + 1, u = t[u].r;
        }
        return LLONG_MIN;
    }
    long long prev(long long x) {
        long long ans = LLONG_MIN;
        for (int u = root; u; ) {
            if (t[u].key < x) ans = t[u].key, u = t[u].r;
            else u = t[u].l;
        }
        return ans;
    }
    long long next(long long x) {
        long long ans = LLONG_MAX;
        for (int u = root; u; ) {
            if (t[u].key > x) ans = t[u].key, u = t[u].l;
            else u = t[u].r;
        }
        return ans;
    }
    int size() { return t[root].sz; }
};

// 【速查】FHQ Treap · 序列版（按位置分裂，区间翻转，文艺平衡树 P3391）
// 用途: 维护一个序列：任意位置插入 / 删除、区间翻转，最后按顺序输出
// 用法: SeqTreap s(最大元素数);  s.push_back(v);  s.insert(pos, v)（插到第 pos 个之后，pos=0 插最前）;
//       s.erase(pos)（删第 pos 个，1-based）;  s.reverse(l, r);  s.toVector() -> 当前序列
// 扩展: 区间加 / 区间和 等，在 Node 里加字段，push 里下传，pull 里汇总
// 复杂度: 每次期望 O(log n)
struct SeqTreap {
    struct Node { int l, r, sz; unsigned pri; long long val; bool rev; };
    vector<Node> t;
    int root = 0;
    mt19937 rng{1022};

    SeqTreap(int maxn = 0) { t.reserve(maxn + 1); t.push_back({0, 0, 0, 0, 0, false}); }

    int newNode(long long v) { t.push_back({0, 0, 1, (unsigned)rng(), v, false}); return (int)t.size() - 1; }
    void pull(int u) { t[u].sz = t[t[u].l].sz + t[t[u].r].sz + 1; }
    void push(int u) {
        if (!t[u].rev) return;
        swap(t[u].l, t[u].r);
        if (t[u].l) t[t[u].l].rev ^= 1;
        if (t[u].r) t[t[u].r].rev ^= 1;
        t[u].rev = false;
    }
    // 按大小分裂: a 为前 k 个，b 为其余
    void split(int u, int k, int &a, int &b) {
        if (!u) { a = b = 0; return; }
        push(u);
        if (t[t[u].l].sz < k) { a = u; split(t[u].r, k - t[t[u].l].sz - 1, t[u].r, b); }
        else { b = u; split(t[u].l, k, a, t[u].l); }
        pull(u);
    }
    int merge(int a, int b) {
        if (!a || !b) return a | b;
        if (t[a].pri > t[b].pri) { push(a); t[a].r = merge(t[a].r, b); pull(a); return a; }
        push(b); t[b].l = merge(a, t[b].l); pull(b); return b;
    }

    void push_back(long long v) { int c = newNode(v); root = merge(root, c); }
    void insert(int pos, long long v) {
        int a, b; split(root, pos, a, b);
        int c = newNode(v);
        root = merge(merge(a, c), b);
    }
    void erase(int pos) {
        int a, b, c;
        split(root, pos - 1, a, b);
        split(b, 1, b, c);
        root = merge(a, c);
    }
    void reverse(int l, int r) {
        int a, b, c;
        split(root, l - 1, a, b);
        split(b, r - l + 1, b, c);
        if (b) t[b].rev ^= 1;
        root = merge(merge(a, b), c);
    }
    vector<long long> toVector() {
        vector<long long> res;
        vector<int> st;
        int u = root;
        while (u || !st.empty()) {          // 迭代中序遍历，避免深递归
            while (u) { push(u); st.push_back(u); u = t[u].l; }
            u = st.back(); st.pop_back();
            res.push_back(t[u].val);
            u = t[u].r;
        }
        return res;
    }
    int size() { return t[root].sz; }
};
