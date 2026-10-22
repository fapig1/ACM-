#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// 【速查】Bellman-Ford（最多经过 k 条边的最短路）
// 用法: vector<array<ll,3>> edges;  edges.push_back({u, v, w});     // 有向边，可有负边
//       auto d = bellmanFordK(n, edges, s, k);   // d[v] = s 到 v 最多 k 条边的最短距离，到不了为 BF_INF
// 复杂度: O(k · m)；每轮必须用上一轮的 back 数组，否则会"串联"超过 k 条边
const ll BF_INF = LLONG_MAX / 4;
vector<ll> bellmanFordK(int n, const vector<array<ll, 3>>& edges, int s, int k) {
    vector<ll> d(n + 1, BF_INF), back;
    d[s] = 0;
    for (int it = 0; it < k; it++) {
        back = d;
        for (auto& [u, v, w] : edges)
            if (back[u] < BF_INF && back[u] + w < d[v]) d[v] = back[u] + w;
    }
    return d;
}

// 示例: 读 n m k 和 m 条边，输出 1 到 n 最多 k 条边的最短路，到不了输出 impossible
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n, m, k;
    cin >> n >> m >> k;
    vector<array<ll, 3>> edges(m);
    for (auto& [u, v, w] : edges) cin >> u >> v >> w;
    auto d = bellmanFordK(n, edges, 1, k);
    if (d[n] >= BF_INF) cout << "impossible" << '\n';
    else cout << d[n] << '\n';
    return 0;
}
