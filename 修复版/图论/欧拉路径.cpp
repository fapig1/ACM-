#include <bits/stdc++.h>
using namespace std;

// 【速查】欧拉路径 / 欧拉回路（Hierholzer，迭代版）
// 用途: 一笔画：每条边恰好走一次（P7771 有向字典序最小、P2731 无向）
// 用法: EulerDirected e(n);  e.addEdge(u, v);  auto path = e.solve();
//       EulerUndirected e(n); e.addEdge(u, v);  auto path = e.solve();
//       返回经过的点序列（长度 m+1）；不存在欧拉路径返回空 vector；点 1-based
// 字典序最小: 有向版已按出边终点排序（从最小可行起点出发，每次走最小的边）
// 判定: 有向——所有点 出度-入度 ∈ {0}，或恰好一个 +1（起点）一个 -1（终点）；无向——奇度点 0 或 2 个；且边连通
// 复杂度: O(m log m)（排序），不排序 O(n + m)
struct EulerDirected {
    int n, m = 0;
    vector<vector<int>> g;
    EulerDirected(int n) : n(n), g(n + 1) {}
    void addEdge(int u, int v) { g[u].push_back(v); m++; }
    vector<int> solve() {
        vector<int> in(n + 1, 0), out(n + 1, 0);
        for (int u = 1; u <= n; u++) { sort(g[u].begin(), g[u].end()); out[u] = g[u].size(); for (int v : g[u]) in[v]++; }
        int start = 0, plus = 0, minus = 0;
        for (int u = 1; u <= n; u++) {
            int d = out[u] - in[u];
            if (d == 1) plus++, start = u;
            else if (d == -1) minus++;
            else if (d != 0) return {};
        }
        if (!(plus == 0 && minus == 0) && !(plus == 1 && minus == 1)) return {};
        if (!start) for (int u = 1; u <= n; u++) if (out[u]) { start = u; break; }
        if (!start) return {1};                   // 没有边
        vector<int> ptr(n + 1, 0), st = {start}, path;
        while (!st.empty()) {
            int u = st.back();
            if (ptr[u] < (int)g[u].size()) st.push_back(g[u][ptr[u]++]);
            else { path.push_back(u); st.pop_back(); }
        }
        if ((int)path.size() != m + 1) return {};   // 边不连通
        reverse(path.begin(), path.end());
        return path;
    }
};

struct EulerUndirected {
    int n, m = 0;
    vector<vector<pair<int, int>>> g;              // (邻点, 边编号)
    EulerUndirected(int n) : n(n), g(n + 1) {}
    void addEdge(int u, int v) { g[u].push_back({v, m}); g[v].push_back({u, m}); m++; }
    vector<int> solve() {
        int start = 0, odd = 0;
        for (int u = 1; u <= n; u++) if (g[u].size() % 2) { odd++; if (!start) start = u; }
        if (odd != 0 && odd != 2) return {};
        if (!start) for (int u = 1; u <= n; u++) if (!g[u].empty()) { start = u; break; }
        if (!start) return {1};
        for (int u = 1; u <= n; u++) sort(g[u].begin(), g[u].end());
        vector<char> used(m, 0);
        vector<int> ptr(n + 1, 0), st = {start}, path;
        while (!st.empty()) {
            int u = st.back();
            while (ptr[u] < (int)g[u].size() && used[g[u][ptr[u]].second]) ptr[u]++;
            if (ptr[u] < (int)g[u].size()) {
                auto [v, id] = g[u][ptr[u]++];
                used[id] = 1;
                st.push_back(v);
            } else { path.push_back(u); st.pop_back(); }
        }
        if ((int)path.size() != m + 1) return {};
        reverse(path.begin(), path.end());
        return path;
    }
};
