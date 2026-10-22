#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// 【速查】SPFA 判负环
// 用法: vector<vector<pair<int,ll>>> g(n + 1);  g[u].push_back({v, w});   // 点 1-based
//       bool neg = hasNegativeCycle(n, g);
// 说明: 所有点一开始都入队（相当于超级源点），能判整张图任意位置的负环；
//       只想判"从 s 出发能到的负环"就只把 s 入队、dist 初始化为 INF、dist[s] = 0
// 复杂度: 最坏 O(nm)
bool hasNegativeCycle(int n, const vector<vector<pair<int, ll>>>& g) {
    vector<ll> dist(n + 1, 0);
    vector<int> cnt(n + 1, 0);                 // cnt[v] = 当前最短路的边数
    vector<char> inq(n + 1, 1);
    queue<int> q;
    for (int i = 1; i <= n; i++) q.push(i);
    while (!q.empty()) {
        int u = q.front(); q.pop(); inq[u] = 0;
        for (auto [v, w] : g[u]) {
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                cnt[v] = cnt[u] + 1;
                if (cnt[v] >= n) return true;  // 最短路边数 >= n 说明有环
                if (!inq[v]) { inq[v] = 1; q.push(v); }
            }
        }
    }
    return false;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n, m;
    cin >> n >> m;
    vector<vector<pair<int, ll>>> g(n + 1);
    for (int i = 0; i < m; i++) { int u, v; ll w; cin >> u >> v >> w; g[u].push_back({v, w}); }
    cout << (hasNegativeCycle(n, g) ? "Yes" : "No") << '\n';
    return 0;
}
