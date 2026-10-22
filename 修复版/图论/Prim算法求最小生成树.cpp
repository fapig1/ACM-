#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// 【速查】朴素 Prim 最小生成树（稠密图）
// 用法: Prim p(n);  p.addEdge(a, b, w);（无向，重边取最小）;  ll res; bool ok = p.run(res);
// 返回: ok = false 表示图不连通；res = MST 权值和；点 1-based
// 复杂度: O(n²)，n ≤ 几千的稠密图（稀疏图用 Kruskal）
struct Prim {
    static constexpr ll INF = LLONG_MAX / 4;
    int n;
    vector<vector<ll>> g;
    Prim(int n) : n(n), g(n + 1, vector<ll>(n + 1, INF)) {}
    void addEdge(int a, int b, ll w) { g[a][b] = g[b][a] = min(g[a][b], w); }
    bool run(ll& res) {
        vector<ll> dist(n + 1, INF);
        vector<char> in(n + 1, 0);
        res = 0;
        dist[1] = 0;
        for (int it = 0; it < n; it++) {
            int t = -1;
            for (int j = 1; j <= n; j++) if (!in[j] && (t == -1 || dist[j] < dist[t])) t = j;
            if (dist[t] >= INF) return false;     // 剩下的点都连不上
            res += dist[t];
            in[t] = 1;
            for (int j = 1; j <= n; j++) if (!in[j]) dist[j] = min(dist[j], g[t][j]);
        }
        return true;
    }
};

// 示例: 读 n m 和 m 条边，输出 MST 权值和，不连通输出 impossible
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n, m;
    cin >> n >> m;
    Prim p(n);
    for (int i = 0; i < m; i++) { int a, b; ll w; cin >> a >> b >> w; p.addEdge(a, b, w); }
    ll res;
    if (!p.run(res)) cout << "impossible" << '\n';
    else cout << res << '\n';
    return 0;
}
