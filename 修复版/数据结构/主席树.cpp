#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// 【速查】主席树（可持久化线段树）
// 用途: ① 可持久化数组：基于任意历史版本单点改 / 单点查（P3919）
//       ② 静态区间第 k 小：按前缀建权值线段树，两个版本相减（P3834）
// 用法: PST t(预计节点数);   // 节点数 ≈ 2n + 更新次数 × (log n + 2)；只是 reserve，超了也能用（会扩容）
//   ① int r0 = t.build(1, n, a);  int r1 = t.update(r0, 1, n, pos, v);  t.query(r1, 1, n, pos)
//      每个版本就是一个根编号，自己用 vector<int> root 存；"只查询也生成新版本"时 root[i] = root[ver]
//   ② 值离散化到 1..m；root[0] = 0（空树）；root[i] = t.insert(root[i-1], 1, m, id(a[i]));
//      t.kth(root[l-1], root[r], 1, m, k) -> 第 k 小的离散化编号
// 复杂度: 每次 O(log n) 时间 + O(log n) 个新节点
// 10.22: 原版是全局 tree[N*25] 数组 + 全局函数（另有一份写在注释里的第 k 小），合并为一个结构体
struct PST {
    struct Node { int l, r; ll val; };       // val: ① 中为叶子的值，② 中为该值域区间的数的个数
    vector<Node> t;

    PST(int reserveNodes = 0) { t.reserve(reserveNodes + 1); t.push_back({0, 0, 0}); }   // 0 号为空节点
    int clone(int u) { t.push_back(t[u]); return (int)t.size() - 1; }

    // ---- ① 可持久化数组 ----
    int build(int l, int r, const vector<ll>& a) {   // a 为 1-based
        int u = clone(0);
        if (l == r) { t[u].val = a[l]; return u; }
        int mid = (l + r) / 2;
        int lc = build(l, mid, a), rc = build(mid + 1, r, a);
        t[u].l = lc; t[u].r = rc;               // 先算完孩子再写，避免 push_back 扩容后引用失效
        return u;
    }
    int update(int u, int l, int r, int pos, ll v) {
        int nu = clone(u);
        if (l == r) { t[nu].val = v; return nu; }
        int mid = (l + r) / 2;
        if (pos <= mid) { int c = update(t[nu].l, l, mid, pos, v); t[nu].l = c; }
        else { int c = update(t[nu].r, mid + 1, r, pos, v); t[nu].r = c; }
        return nu;
    }
    ll query(int u, int l, int r, int pos) {
        while (l < r) {
            int mid = (l + r) / 2;
            if (pos <= mid) u = t[u].l, r = mid;
            else u = t[u].r, l = mid + 1;
        }
        return t[u].val;
    }

    // ---- ② 静态区间第 k 小 ----
    int insert(int u, int l, int r, int pos) {      // 值 pos 的个数 +1，返回新根
        int nu = clone(u);
        t[nu].val++;
        if (l == r) return nu;
        int mid = (l + r) / 2;
        if (pos <= mid) { int c = insert(t[nu].l, l, mid, pos); t[nu].l = c; }
        else { int c = insert(t[nu].r, mid + 1, r, pos); t[nu].r = c; }
        return nu;
    }
    int kth(int u, int v, int l, int r, ll k) {     // 版本 v 减版本 u 的第 k 小
        while (l < r) {
            int mid = (l + r) / 2;
            ll leftCnt = t[t[v].l].val - t[t[u].l].val;
            if (k <= leftCnt) u = t[u].l, v = t[v].l, r = mid;
            else k -= leftCnt, u = t[u].r, v = t[v].r, l = mid + 1;
        }
        return l;
    }
};

// 示例（P3919）: 读 n q 和初始数组；每个操作 "ver 1 pos v"（基于 ver 改值）或 "ver 2 pos"（查值）
//   第 i 个操作生成版本 i（查询操作的新版本 = 原版本）
void solve() {
    int n, q;
    cin >> n >> q;
    vector<ll> a(n + 1);
    for (int i = 1; i <= n; i++) cin >> a[i];
    PST t(2 * n + q * (__lg(max(n, 1)) + 2));   // 建树 2n-1 个点 + 每次修改 ⌈log n⌉+1 个点
    vector<int> root(q + 1);
    root[0] = t.build(1, n, a);
    for (int i = 1; i <= q; i++) {
        int ver, op, pos;
        cin >> ver >> op >> pos;
        if (op == 1) { ll v; cin >> v; root[i] = t.update(root[ver], 1, n, pos, v); }
        else { cout << t.query(root[ver], 1, n, pos) << '\n'; root[i] = root[ver]; }
    }
}

// 第 k 小用法示例（P3834）:
// void solveKth() {
//     int n, q; cin >> n >> q;
//     vector<ll> a(n + 1); for (int i = 1; i <= n; i++) cin >> a[i];
//     vector<ll> vals(a.begin() + 1, a.end()); sort(vals.begin(), vals.end());
//     vals.erase(unique(vals.begin(), vals.end()), vals.end()); int m = vals.size();
//     PST t(n * 20); vector<int> root(n + 1, 0);
//     for (int i = 1; i <= n; i++) root[i] = t.insert(root[i - 1], 1, m, lower_bound(vals.begin(), vals.end(), a[i]) - vals.begin() + 1);
//     while (q--) { int l, r, k; cin >> l >> r >> k; cout << vals[t.kth(root[l - 1], root[r], 1, m, k) - 1] << '\n'; }
// }
