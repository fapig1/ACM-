#include <bits/stdc++.h>
using namespace std;

// 【速查】2-SAT（P4782）
// 用途: n 个布尔变量，若干条"a 或 b"形式的约束，判断是否有解并给出一组解
// 用法: TwoSat s(n);                       // 变量 0..n-1
//       s.addClause(x, a, y, b)            // 约束: (x == a) 或 (y == b)，a、b ∈ {0, 1}
//       常用翻译:  x 必须为 a      -> addClause(x, a, x, a)
//                 x、y 不能同时为 1 -> addClause(x, 0, y, 0)
//                 x 为 1 则 y 为 1  -> addClause(x, 0, y, 1)
//       bool ok = s.solve();  ok 时 s.ans[x] ∈ {0, 1} 是一组解
// 复杂度: O(n + m)；Tarjan 用迭代写法，2e6 个点也不会爆栈
struct TwoSat {
    int n;
    vector<vector<int>> g;
    vector<int> ans;

    TwoSat(int n) : n(n), g(2 * n) {}
    int id(int x, int v) { return 2 * x + v; }          // 点 2x+v 表示 "x == v"
    void addClause(int x, int a, int y, int b) {
        g[id(x, !a)].push_back(id(y, b));             // x != a  =>  y == b
        g[id(y, !b)].push_back(id(x, a));             // y != b  =>  x == a
    }
    bool solve() {
        int N = 2 * n, timer = 0, cc = 0;
        vector<int> dfn(N, -1), low(N), comp(N, -1), it(N, 0), st, call;
        for (int s = 0; s < N; s++) {
            if (dfn[s] != -1) continue;
            dfn[s] = low[s] = timer++; st.push_back(s); call.push_back(s);
            while (!call.empty()) {
                int u = call.back();
                if (it[u] < (int)g[u].size()) {
                    int v = g[u][it[u]++];
                    if (dfn[v] == -1) { dfn[v] = low[v] = timer++; st.push_back(v); call.push_back(v); }
                    else if (comp[v] == -1) low[u] = min(low[u], dfn[v]);
                } else {
                    call.pop_back();
                    if (!call.empty()) low[call.back()] = min(low[call.back()], low[u]);
                    if (low[u] == dfn[u]) {
                        while (true) { int w = st.back(); st.pop_back(); comp[w] = cc; if (w == u) break; }
                        cc++;
                    }
                }
            }
        }
        ans.assign(n, 0);
        for (int x = 0; x < n; x++) {
            if (comp[2 * x] == comp[2 * x + 1]) return false;
            ans[x] = comp[2 * x + 1] < comp[2 * x];     // Tarjan 编号小 = 拓扑序靠后，选它
        }
        return true;
    }
};
