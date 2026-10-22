#include <bits/stdc++.h>
using namespace std;

// 【速查】点分治（P3806：询问树上是否存在距离恰好为 k 的点对）
// 用途: 统计树上路径（距离 = k / ≤ k 的点对数、带限制的路径计数等），每层以重心为根统计"经过重心的路径"
// 用法: CentroidDecomp cd(n);  cd.addEdge(u, v, w);   // 点 1-based，边权 >= 0
//       vector<char> res = cd.solve(queries);          // res[i] = 是否存在距离为 queries[i] 的点对
// 改题: 换掉 calc() 里"用当前子树距离去配之前子树距离"的部分即可（如计数距离 ≤ k 的点对）
// 复杂度: O(n log n · q)；求重心、算距离都用 BFS，不会爆栈
struct CentroidDecomp {
    int n;
    vector<vector<pair<int, int>>> g;
    vector<char> removed;
    vector<int> sz, par;

    CentroidDecomp(int n) : n(n), g(n + 1), removed(n + 1, 0), sz(n + 1), par(n + 1) {}
    void addEdge(int u, int v, int w) { g[u].push_back({v, w}); g[v].push_back({u, w}); }

    // BFS 收集以 s 为起点（父亲为 p）的连通块，顺序为 BFS 序
    vector<int> collect(int s, int p) {
        vector<int> order = {s};
        par[s] = p;
        for (size_t i = 0; i < order.size(); i++) {
            int u = order[i];
            for (auto [v, w] : g[u]) if (v != par[u] && !removed[v]) { par[v] = u; order.push_back(v); }
        }
        return order;
    }
    int centroid(int s) {
        vector<int> order = collect(s, 0);
        int tot = order.size();
        for (int i = tot - 1; i >= 0; i--) {
            int u = order[i]; sz[u] = 1;
            for (auto [v, w] : g[u]) if (v != par[u] && !removed[v]) sz[u] += sz[v];
        }
        for (int u : order) {
            int mx = tot - sz[u];
            for (auto [v, w] : g[u]) if (v != par[u] && !removed[v]) mx = max(mx, sz[v]);
            if (mx * 2 <= tot) return u;
        }
        return s;
    }

    vector<int> qs;
    vector<char> res;
    int maxK = 0;
    vector<char> exist;                    // exist[d] = 之前的子树里有到重心距离为 d 的点

    void calc(int c) {
        vector<int> used = {0};
        exist[0] = 1;
        for (auto [v0, w0] : g[c]) {
            if (removed[v0]) continue;
            // 当前子树里所有点到重心的距离
            vector<int> ds;
            vector<pair<int, long long>> st = {{v0, w0}};
            vector<int> from = {c};
            for (size_t i = 0; i < st.size(); i++) {
                auto [u, d] = st[i];
                if (d <= maxK) ds.push_back((int)d);
                for (auto [v, w] : g[u]) if (v != from[i] && !removed[v]) { st.push_back({v, d + w}); from.push_back(u); }
            }
            for (int d : ds)
                for (size_t i = 0; i < qs.size(); i++)
                    if (!res[i] && qs[i] >= d && exist[qs[i] - d]) res[i] = 1;
            for (int d : ds) if (!exist[d]) { exist[d] = 1; used.push_back(d); }
        }
        for (int d : used) exist[d] = 0;
    }
    vector<char> solve(const vector<int>& queries) {
        qs = queries;
        res.assign(qs.size(), 0);
        maxK = 0;
        for (int k : qs) maxK = max(maxK, k);
        exist.assign(maxK + 1, 0);
        vector<int> todo = {1};
        while (!todo.empty()) {               // 用栈代替递归分治
            int s = todo.back(); todo.pop_back();
            int c = centroid(s);
            calc(c);
            removed[c] = 1;
            for (auto [v, w] : g[c]) if (!removed[v]) todo.push_back(v);
        }
        return res;
    }
};
