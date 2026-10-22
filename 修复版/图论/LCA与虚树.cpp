#include <bits/stdc++.h>
using namespace std;

// 【速查】LCA（倍增，非递归预处理）
// 用法: LCA t(n);  t.addEdge(u, v);  t.build(root);      // 点 1-based，无向树
//       t.lca(u, v);  t.dist(u, v)（边数，带权就另开前缀和 sum[u] + sum[v] - 2 sum[lca]）;
//       t.kth(u, k) -> u 往上第 k 个祖先（不存在返回 0）;  t.dep[u] 深度（根为 0）、t.tin[u] DFS 序
// 复杂度: 预处理 O(n log n)，每次查询 O(log n)
struct LCA {
    int n, LOG;
    vector<vector<int>> g, up;
    vector<int> dep, tin;

    LCA(int n) : n(n), g(n + 1), dep(n + 1, 0), tin(n + 1, 0) {
        LOG = 1;
        while ((1 << LOG) <= n) LOG++;
        up.assign(LOG, vector<int>(n + 1, 0));
    }
    void addEdge(int u, int v) { g[u].push_back(v); g[v].push_back(u); }
    void build(int root) {
        int timer = 0;
        vector<int> st = {root};
        vector<char> vis(n + 1, 0);
        vis[root] = 1; up[0][root] = 0;
        while (!st.empty()) {                    // 迭代 DFS，深链不会爆栈
            int u = st.back(); st.pop_back();
            tin[u] = ++timer;
            for (int v : g[u]) if (!vis[v]) {
                vis[v] = 1; dep[v] = dep[u] + 1; up[0][v] = u;
                st.push_back(v);
            }
        }
        for (int k = 1; k < LOG; k++)
            for (int v = 1; v <= n; v++) up[k][v] = up[k - 1][up[k - 1][v]];
    }
    int kth(int u, int k) {
        if (k > dep[u]) return 0;
        for (int i = 0; i < LOG; i++) if (k >> i & 1) u = up[i][u];
        return u;
    }
    int lca(int u, int v) {
        if (dep[u] < dep[v]) swap(u, v);
        u = kth(u, dep[u] - dep[v]);
        if (u == v) return u;
        for (int k = LOG - 1; k >= 0; k--)
            if (up[k][u] != up[k][v]) u = up[k][u], v = up[k][v];
        return up[0][u];
    }
    int dist(int u, int v) { return dep[u] + dep[v] - 2 * dep[lca(u, v)]; }
};

// 【速查】虚树（只保留关键点及其两两 LCA 的压缩树）
// 用途: 多次询问、每次给 k 个关键点（Σk 不大）做树形 DP（P2495 消耗战、P3320 等）
// 用法: auto [nodes, par] = buildVirtualTree(t, keys);   // t 为建好的 LCA
//       nodes: 虚树所有点（按 DFS 序，nodes[0] 是虚树根）; par[i]: nodes[i] 在虚树上的父亲（par[0] = 0）
//       虚树边 (par[i], nodes[i]) 对应原树上一条祖先链，边权按题意用原树信息算（如 dist、链上最小边）
// 复杂度: O(k log k + k log n)
pair<vector<int>, vector<int>> buildVirtualTree(LCA& t, vector<int> keys) {
    auto byTin = [&](int a, int b) { return t.tin[a] < t.tin[b]; };
    sort(keys.begin(), keys.end(), byTin);
    keys.erase(unique(keys.begin(), keys.end()), keys.end());
    vector<int> nodes = keys;
    for (int i = 0; i + 1 < (int)keys.size(); i++) nodes.push_back(t.lca(keys[i], keys[i + 1]));
    sort(nodes.begin(), nodes.end(), byTin);
    nodes.erase(unique(nodes.begin(), nodes.end()), nodes.end());
    vector<int> par(nodes.size(), 0);
    for (int i = 1; i < (int)nodes.size(); i++) par[i] = t.lca(nodes[i - 1], nodes[i]);   // DFS 序相邻两点的 LCA 就是父亲
    return {nodes, par};
}
