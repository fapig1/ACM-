#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// 【速查】朴素 Dijkstra（稠密图）
// 用法: vector<vector<pair<int,ll>>> g(n + 1);  g[u].push_back({v, w});   // 点 1-based，边权非负
//       auto d = dijkstraDense(n, g, s);   // 到不了为 DIJ_INF
// 复杂度: O(n² + m)，n ≤ 几千、m 接近 n² 时用；稀疏图用堆优化版
const ll DIJ_INF = LLONG_MAX / 4;
vector<ll> dijkstraDense(int n, const vector<vector<pair<int, ll>>>& g, int s) {
    vector<ll> d(n + 1, DIJ_INF);
    vector<char> done(n + 1, 0);
    d[s] = 0;
    for (int it = 0; it < n; it++) {
        int t = -1;
        for (int j = 1; j <= n; j++) if (!done[j] && (t == -1 || d[j] < d[t])) t = j;
        if (d[t] >= DIJ_INF) break;
        done[t] = 1;
        for (auto [v, w] : g[t]) d[v] = min(d[v], d[t] + w);
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
    auto d = dijkstraDense(n, g, 1);
    cout << (d[n] >= DIJ_INF ? -1 : d[n]);
    return 0;
}
