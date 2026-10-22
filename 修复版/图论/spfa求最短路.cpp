#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// 【速查】SPFA 单源最短路（可有负边，无负环）
// 用法: vector<vector<pair<int,ll>>> g(n + 1);  g[u].push_back({v, w});   // 点 1-based
//       auto d = spfa(n, g, s);   // d[v] = s 到 v 的最短路，到不了为 SPFA_INF
// 复杂度: 一般很快，最坏 O(nm)（会被卡）；没有负边请用 Dijkstra
const ll SPFA_INF = LLONG_MAX / 4;
vector<ll> spfa(int n, const vector<vector<pair<int, ll>>>& g, int s) {
    vector<ll> d(n + 1, SPFA_INF);
    vector<char> inq(n + 1, 0);
    queue<int> q;
    d[s] = 0; q.push(s); inq[s] = 1;
    while (!q.empty()) {
        int u = q.front(); q.pop(); inq[u] = 0;
        for (auto [v, w] : g[u]) {
            if (d[u] + w < d[v]) {
                d[v] = d[u] + w;
                if (!inq[v]) { inq[v] = 1; q.push(v); }
            }
        }
    }
    return d;
}

// 示例: 读 n m 和 m 条边，输出 1 到 n 的最短路，到不了输出 impossible
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n, m;
    cin >> n >> m;
    vector<vector<pair<int, ll>>> g(n + 1);
    for (int i = 0; i < m; i++) { int u, v; ll w; cin >> u >> v >> w; g[u].push_back({v, w}); }
    auto d = spfa(n, g, 1);
    if (d[n] >= SPFA_INF) cout << "impossible" << '\n';
    else cout << d[n] << '\n';
    return 0;
}
