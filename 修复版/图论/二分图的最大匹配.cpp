#include <bits/stdc++.h>
using namespace std;

// 【速查】匈牙利算法（二分图最大匹配）
// 用法: Hungarian h(n1, n2);  h.addEdge(u, v);   // 左部 1..n1，右部 1..n2，只建 左 -> 右
//       int ans = h.solve();   h.matchR[v] = 与右点 v 匹配的左点（0 为未匹配），h.matchL[u] 同理
// 常用: 最小点覆盖 = 最大匹配；最大独立集 = 点数 - 最大匹配；DAG 最小路径覆盖 = n - 最大匹配（拆点）
// 复杂度: O(n1 · m)；规模到 1e5 级改用 Dinic；递归深度 ≤ n1
struct Hungarian {
    int n1, n2, stamp = 0;
    vector<vector<int>> g;
    vector<int> matchL, matchR, vis;

    Hungarian(int n1, int n2) : n1(n1), n2(n2), g(n1 + 1), matchL(n1 + 1, 0), matchR(n2 + 1, 0), vis(n2 + 1, 0) {}
    void addEdge(int u, int v) { g[u].push_back(v); }
    bool dfs(int u) {
        for (int v : g[u]) {
            if (vis[v] == stamp) continue;
            vis[v] = stamp;
            if (!matchR[v] || dfs(matchR[v])) { matchR[v] = u; matchL[u] = v; return true; }
        }
        return false;
    }
    int solve() {
        int res = 0;
        for (int u = 1; u <= n1; u++) { ++stamp; if (dfs(u)) res++; }   // 时间戳代替每轮清空 vis
        return res;
    }
};

/*
补充定义和定理：
最大匹配数：最大匹配的匹配边的数目
最小点覆盖数：选取最少的点，使任意一条边至少有一个端点被选择
最大独立集：选取最多的点，使任意所选两点均不相连
最小路径覆盖数：对于一个 DAG（有向无环图），选取最少条路径，使得每个顶点属于且仅属于一条路径。路径长可以为 0（即单个点）。
定理1：最大匹配数 = 最小点覆盖数（这是 Konig 定理）
定理2：最大独立集 = 顶点数 - 最大匹配数
定理3：最小路径覆盖数 = 顶点数 - 最大匹配数
*/

// 示例: 读 n1 n2 m 和 m 条 左->右 边，输出最大匹配数
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n1, n2, m;
    cin >> n1 >> n2 >> m;
    Hungarian h(n1, n2);
    for (int i = 0; i < m; i++) { int u, v; cin >> u >> v; h.addEdge(u, v); }
    cout << h.solve() << "\n";
    return 0;
}
