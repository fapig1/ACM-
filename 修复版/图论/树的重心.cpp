#include <bits/stdc++.h>
using namespace std;

// 【速查】树的重心（非递归）
// 用途: 找删掉后"最大剩余连通块"最小的点（点分治的分治中心）
// 用法: vector<vector<int>> g(n + 1);  建双向边;   int best;  auto c = findCentroids(n, g, best);
// 返回: 重心（1 个或 2 个，升序）；best = 删去重心后最大连通块的大小
// 复杂度: O(n)；点 1-based
vector<int> findCentroids(int n, const vector<vector<int>>& g, int& best) {
    vector<int> order = {1}, fa(n + 1, 0), sz(n + 1, 1);
    for (size_t i = 0; i < order.size(); i++) {
        int u = order[i];
        for (int v : g[u]) if (v != fa[u]) { fa[v] = u; order.push_back(v); }
    }
    for (int i = n - 1; i > 0; i--) sz[fa[order[i]]] += sz[order[i]];   // BFS 序倒过来累加子树大小
    best = INT_MAX;
    vector<int> res;
    for (int u = 1; u <= n; u++) {
        int mx = n - sz[u];
        for (int v : g[u]) if (v != fa[u]) mx = max(mx, sz[v]);
        if (mx < best) best = mx, res = {u};
        else if (mx == best) res.push_back(u);
    }
    return res;
}

// 示例: 读 n 和 n-1 条边，输出重心与最大连通块大小
int main() {
    int n;
    cin >> n;
    vector<vector<int>> g(n + 1);
    for (int i = 1; i < n; i++) { int u, v; cin >> u >> v; g[u].push_back(v); g[v].push_back(u); }
    int best;
    auto c = findCentroids(n, g, best);
    cout << "Centroid:";
    for (int x : c) cout << ' ' << x;
    cout << '\n';
    cout << "Min max component size = " << best << '\n';
    return 0;
}
