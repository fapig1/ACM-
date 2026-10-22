#include <bits/stdc++.h>
using namespace std;

// 【速查】最小树形图（朱刘算法，P4716）
// 用途: 有向图里以 root 为根、能到达所有点的边权和最小的"有向生成树"
// 用法: long long w = zhuLiu(n, root, edges);   // 点 0..n-1，edges 为 {u, v, w}（u -> v）
//       返回最小权和；有点到不了返回 -1
// 复杂度: O(n·m)
long long zhuLiu(int n, int root, vector<array<long long, 3>> edges) {
    const long long INF = LLONG_MAX / 4;
    long long res = 0;
    vector<long long> in(n);
    vector<int> pre(n), id(n), vis(n);
    while (true) {
        fill(in.begin(), in.end(), INF);
        for (auto& [u, v, w] : edges)               // 每个点选一条最小入边
            if (u != v && w < in[v]) in[v] = w, pre[v] = (int)u;
        for (int i = 0; i < n; i++) if (i != root && in[i] == INF) return -1;
        int cnt = 0;
        fill(id.begin(), id.end(), -1); fill(vis.begin(), vis.end(), -1);
        in[root] = 0;
        for (int i = 0; i < n; i++) {               // 找环
            res += in[i];
            int v = i;
            while (vis[v] != i && id[v] == -1 && v != root) vis[v] = i, v = pre[v];
            if (v != root && id[v] == -1) {
                for (int u = pre[v]; u != v; u = pre[u]) id[u] = cnt;
                id[v] = cnt++;
            }
        }
        if (cnt == 0) break;                        // 无环，完成
        for (int i = 0; i < n; i++) if (id[i] == -1) id[i] = cnt++;
        for (auto& [u, v, w] : edges) {             // 缩环，入环边权减去环内入边
            long long vv = v;
            u = id[u]; v = id[v];
            if (u != v) w -= in[vv];
        }
        n = cnt; root = id[root];
    }
    return res;
}

// 【速查】一般图最大匹配（带花树，UOJ79）
// 用途: 不是二分图的最大匹配（有奇环）
// 用法: Blossom b(n);  b.addEdge(u, v);  int cnt = b.solve();   // 点 1-based
//       b.match[u] = u 的匹配点（0 表示未匹配）
// 复杂度: O(n³)，n ≤ 1000 没问题
struct Blossom {
    int n;
    vector<vector<int>> g;
    vector<int> match, pre, fa, typ, vis;
    int stamp = 0;

    Blossom(int n) : n(n), g(n + 1), match(n + 1), pre(n + 1), fa(n + 1), typ(n + 1), vis(n + 1) {}
    void addEdge(int u, int v) { g[u].push_back(v); g[v].push_back(u); }
    int find(int x) { while (fa[x] != x) x = fa[x] = fa[fa[x]]; return x; }
    int lca(int x, int y) {
        ++stamp;
        while (true) {
            if (x) {
                x = find(x);
                if (vis[x] == stamp) return x;
                vis[x] = stamp;
                x = match[x] ? pre[match[x]] : 0;
            }
            swap(x, y);
        }
    }
    void blossom(int x, int y, int l, queue<int>& q) {   // 把奇环缩成花
        while (find(x) != l) {
            pre[x] = y; y = match[x];
            if (typ[y] == 2) typ[y] = 1, q.push(y);
            if (find(x) == x) fa[x] = l;
            if (find(y) == y) fa[y] = l;
            x = pre[y];
        }
    }
    bool bfs(int s) {
        for (int i = 1; i <= n; i++) fa[i] = i, typ[i] = 0, pre[i] = 0;
        queue<int> q; q.push(s); typ[s] = 1;
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (int v : g[u]) {
                if (find(u) == find(v) || typ[v] == 2) continue;
                if (!typ[v]) {
                    typ[v] = 2; pre[v] = u;
                    if (!match[v]) {                        // 找到增广路，沿 pre 翻转
                        for (int x = v, last; x; x = last) last = match[pre[x]], match[x] = pre[x], match[pre[x]] = x;
                        return true;
                    }
                    typ[match[v]] = 1; q.push(match[v]);
                } else {                                     // 两个偶点相遇：奇环
                    int l = lca(u, v);
                    blossom(u, v, l, q); blossom(v, u, l, q);
                }
            }
        }
        return false;
    }
    int solve() {
        int res = 0;
        for (int i = 1; i <= n; i++) if (!match[i] && bfs(i)) res++;
        return res;
    }
};
