#include <bits/stdc++.h>
using namespace std;

// 【速查】树上启发式合并 dsu on tree（CF600E：每个子树里出现次数最多的颜色之和）
// 用途: 离线回答"每个点的子树里 xxx"（众数、不同颜色数、某深度的点数……），信息只能加不能快速合并时
// 用法: DsuOnTree t(n);  t.addEdge(u, v);  t.col[i] = 颜色（非负整数，大就先离散化）;  auto ans = t.solve(1);   // ans[u]
// 改题: 改 addPoint / clearAll 维护的信息，以及 solve 里记录答案的那一行
// 原理: 先算轻儿子并清空，再算重儿子并保留，最后把轻儿子子树逐点加回来；每个点被加 O(log n) 次
// 复杂度: O(n log n)；预处理是迭代的，主过程递归深度 = 树高（链状 1e6 可能爆栈）
struct DsuOnTree {
    int n;
    vector<vector<int>> g;
    vector<int> col, sz, heavy, tin, tout, ver, fa;
    vector<long long> ans;

    DsuOnTree(int n) : n(n), g(n + 1), col(n + 1), sz(n + 1, 1), heavy(n + 1, 0),
                       tin(n + 1), tout(n + 1), ver(n + 1), fa(n + 1, 0), ans(n + 1) {}
    void addEdge(int u, int v) { g[u].push_back(v); g[v].push_back(u); }

    // ---- 需要按题意修改的部分 ----
    vector<int> cnt;
    int mx = 0;
    long long sum = 0;
    void addPoint(int u) {
        int c = col[u];
        cnt[c]++;
        if (cnt[c] > mx) mx = cnt[c], sum = c;
        else if (cnt[c] == mx) sum += c;
    }
    void clearAll(int u) {                     // 清空 u 子树的贡献
        for (int i = tin[u]; i <= tout[u]; i++) cnt[col[ver[i]]]--;
        mx = 0; sum = 0;
    }
    // ------------------------------

    void dfs(int u, bool keep) {
        for (int v : g[u]) if (v != fa[u] && v != heavy[u]) dfs(v, false);
        if (heavy[u]) dfs(heavy[u], true);
        for (int v : g[u]) if (v != fa[u] && v != heavy[u])
            for (int i = tin[v]; i <= tout[v]; i++) addPoint(ver[i]);
        addPoint(u);
        ans[u] = sum;                          // 记录答案
        if (!keep) clearAll(u);
    }
    vector<long long> solve(int root) {
        // 迭代求 fa、DFS 序、子树大小、重儿子
        vector<int> order, st = {root};
        fa[root] = 0;
        vector<int> it(n + 1, 0);
        int timer = 0;
        tin[root] = ++timer; ver[timer] = root;
        while (!st.empty()) {
            int u = st.back();
            if (it[u] < (int)g[u].size()) {
                int v = g[u][it[u]++];
                if (v == fa[u]) continue;
                fa[v] = u; tin[v] = ++timer; ver[timer] = v;
                st.push_back(v);
            } else {
                st.pop_back();
                tout[u] = timer;
                if (fa[u]) { sz[fa[u]] += sz[u]; if (!heavy[fa[u]] || sz[u] > sz[heavy[fa[u]]]) heavy[fa[u]] = u; }
            }
        }
        cnt.assign(max(n, *max_element(col.begin(), col.end())) + 1, 0);   // 颜色可以超过 n（但需 >= 0）
        mx = 0; sum = 0;
        dfs(root, true);
        return ans;
    }
};
