#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// 【速查】堆优化 Dijkstra（稀疏图）
// 用法: vector<vector<pair<int,ll>>> g(n + 1);  g[u].push_back({v, w});   // {终点, 边权}，与其他最短路模板一致
//       auto d = dijkstra(n, g, s);   // d[v] = s 到 v 的最短路，到不了为 DIJ_INF；边权必须非负
// 复杂度: O(m log m)
// 10.22 改动: 原版存边是 {边权, 终点}，与其他模板相反，已统一为 {终点, 边权}
const ll DIJ_INF = LLONG_MAX / 4;
vector<ll> dijkstra(int n, const vector<vector<pair<int, ll>>>& g, int s) {
    vector<ll> d(n + 1, DIJ_INF);
    priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<>> pq;
    d[s] = 0; pq.push({0, s});
    while (!pq.empty()) {
        auto [du, u] = pq.top(); pq.pop();
        if (du != d[u]) continue;              // 过期的堆元素
        for (auto [v, w] : g[u])
            if (du + w < d[v]) { d[v] = du + w; pq.push({d[v], v}); }
    }
    return d;
}

// 示例: 读 n m 和 m 条边，输出 1 到 n 的最短路，到不了输出 -1
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n, m;
    cin >> n >> m;
    vector<vector<pair<int, ll>>> g(n + 1);
    for (int i = 0; i < m; i++) { int u, v; ll w; cin >> u >> v >> w; g[u].push_back({v, w}); }
    auto d = dijkstra(n, g, 1);
    cout << (d[n] >= DIJ_INF ? -1 : d[n]);
    return 0;
}
