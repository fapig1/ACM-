#include <bits/stdc++.h>
using namespace std;

// 【速查】线段树合并 / 分裂（动态开点权值线段树，P5494）
// 用途: 很多棵权值线段树之间整体合并、按值域切出一段、查第 k 小 / 区间计数
//       典型: 树上每个点一棵树，DFS 时把儿子的树合并上来（子树信息统计）
// 用法: SegMerge sm(值域上界 n, 节点池大小);  int rt = 0;
//       sm.add(rt, 1, n, pos, v)            // 值 pos 的个数 += v（rt 为 0 时自动建树）
//       rt = sm.merge(rt, rt2, 1, n)       // 把 rt2 并进 rt（rt2 之后不能再用）
//       int nr = 0; sm.split(rt, nr, 1, n, l, r)   // 把 rt 中值域 [l, r] 的部分切到新树 nr
//       sm.query(rt, 1, n, l, r) -> 值域 [l, r] 的个数;  sm.kth(rt, 1, n, k) -> 第 k 小的值（不足 k 个返回 -1）
// 复杂度: add / query / kth / split 都是 O(log n)，merge 总复杂度均摊 O(总节点数)
// 节点池: 约 (插入次数 + 分裂次数) × log n
struct SegMerge {
    int n, tot = 0;
    vector<int> L, R;
    vector<long long> cnt;

    SegMerge(int n, int maxNodes) : n(n), L(maxNodes, 0), R(maxNodes, 0), cnt(maxNodes, 0) {}

    int newNode() { ++tot; assert(tot < (int)L.size()); L[tot] = R[tot] = 0; cnt[tot] = 0; return tot; }
    void pull(int u) { cnt[u] = cnt[L[u]] + cnt[R[u]]; }

    void add(int &u, int l, int r, int pos, long long v) {
        if (!u) u = newNode();
        if (l == r) { cnt[u] += v; return; }
        int mid = (l + r) / 2;
        if (pos <= mid) add(L[u], l, mid, pos, v);
        else add(R[u], mid + 1, r, pos, v);
        pull(u);
    }
    long long query(int u, int l, int r, int ql, int qr) {
        if (!u || qr < l || r < ql) return 0;
        if (ql <= l && r <= qr) return cnt[u];
        int mid = (l + r) / 2;
        return query(L[u], l, mid, ql, qr) + query(R[u], mid + 1, r, ql, qr);
    }
    int merge(int a, int b, int l, int r) {
        if (!a || !b) return a | b;
        if (l == r) { cnt[a] += cnt[b]; return a; }
        int mid = (l + r) / 2;
        L[a] = merge(L[a], L[b], l, mid);
        R[a] = merge(R[a], R[b], mid + 1, r);
        pull(a);
        return a;
    }
    void split(int &u, int &v, int l, int r, int ql, int qr) {
        if (!u || qr < l || r < ql) return;
        if (ql <= l && r <= qr) { v = u; u = 0; return; }   // 整块直接摘走
        if (!v) v = newNode();
        int mid = (l + r) / 2;
        split(L[u], L[v], l, mid, ql, qr);
        split(R[u], R[v], mid + 1, r, ql, qr);
        pull(u); pull(v);
    }
    int kth(int u, int l, int r, long long k) {
        if (!u || k < 1 || cnt[u] < k) return -1;
        while (l < r) {
            int mid = (l + r) / 2;
            if (cnt[L[u]] >= k) u = L[u], r = mid;
            else k -= cnt[L[u]], u = R[u], l = mid + 1;
        }
        return l;
    }
};
