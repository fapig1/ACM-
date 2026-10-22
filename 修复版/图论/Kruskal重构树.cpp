#include <bits/stdc++.h>
using namespace std;

// 【速查】Kruskal 重构树（P1967 货车运输、P4197）
// 用途: "u 到 v 所有路径中，最大边权的最小值"（瓶颈路）= 重构树上 val[lca(u, v)]；
//       "从 u 出发只走边权 <= x 的边能到哪些点" = u 往上跳到 val <= x 的最高祖先，它的子树叶子就是答案
// 用法: KruskalTree t(n);  t.addEdge(u, v, w);  t.build();       // 点 1-based，按边权升序合并（最小瓶颈）
//       t.bottleneck(u, v) -> 最小化的最大边权，不连通返回 -1;
//       t.highest(u, x) -> u 只走 <= x 的边能到的连通块对应的重构树节点（子树叶子 = 能到的点）
//       求"最大化最小边"（货车运输）: build(true) 改为按边权降序合并，bottleneck 返回最小边的最大值
// 复杂度: O(m log m + n log n)；重构树共 2n-1 个点，叶子 1..n，内部点 val = 合并它的那条边
struct KruskalTree {
    int n, tot;
    vector<array<long long, 3>> edges;
    vector<int> fa, dep;
    vector<long long> val;
    vector<vector<int>> up, ch;
    int LOG;

    KruskalTree(int n) : n(n) {}
    void addEdge(int u, int v, long long w) { edges.push_back({w, u, v}); }
    int find(int x) { while (fa[x] != x) x = fa[x] = fa[fa[x]]; return x; }

    void build(bool maxBottleneck = false) {
        sort(edges.begin(), edges.end());
        if (maxBottleneck) reverse(edges.begin(), edges.end());
        int N = 2 * n;
        fa.resize(N); iota(fa.begin(), fa.end(), 0);
        val.assign(N, 0); ch.assign(N, {});
        tot = n;
        for (auto& [w, u, v] : edges) {
            int a = find(u), b = find(v);
            if (a == b) continue;
            ++tot; val[tot] = w;
            fa[a] = fa[b] = tot;
            ch[tot] = {a, b};
        }
        LOG = 1; while ((1 << LOG) <= tot) LOG++;
        up.assign(LOG, vector<int>(tot + 1, 0));
        dep.assign(tot + 1, 0);
        for (int r = tot; r >= 1; r--) {       // 父亲编号一定比儿子大，倒序就是"先父后子"
            for (int c : ch[r]) { up[0][c] = r; dep[c] = dep[r] + 1; }
        }
        for (int k = 1; k < LOG; k++)
            for (int v = 1; v <= tot; v++) up[k][v] = up[k - 1][up[k - 1][v]];
    }
    int lca(int u, int v) {
        if (dep[u] < dep[v]) swap(u, v);
        for (int k = LOG - 1; k >= 0; k--) if (dep[u] - (1 << k) >= dep[v]) u = up[k][u];
        if (u == v) return u;
        for (int k = LOG - 1; k >= 0; k--) if (up[k][u] != up[k][v]) u = up[k][u], v = up[k][v];
        return up[0][u];
    }
    long long bottleneck(int u, int v) {
        if (u == v) return 0;
        if (find(u) != find(v)) return -1;
        return val[lca(u, v)];
    }
    int highest(int u, long long x) {           // 仅最小瓶颈（升序建树）时语义成立
        for (int k = LOG - 1; k >= 0; k--) {
            int p = up[k][u];
            if (p && val[p] <= x) u = p;
        }
        return u;
    }
};
