#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// 【速查】Floyd 全源最短路
// 用法: Floyd f(n);  f.addEdge(x, y, w);（有向，重边自动取最小）;  f.run();
//       f.d[i][j] 即 i 到 j 的最短路，f.reachable(i, j) 判可达；点 1-based
// 可有负边，不能有负环（跑完后 f.d[i][i] < 0 说明有负环）
// 复杂度: O(n³)，n ≤ 500 左右
struct Floyd {
    static constexpr ll INF = LLONG_MAX / 4;
    int n;
    vector<vector<ll>> d;
    Floyd(int n) : n(n), d(n + 1, vector<ll>(n + 1, INF)) { for (int i = 1; i <= n; i++) d[i][i] = 0; }
    void addEdge(int x, int y, ll w) { d[x][y] = min(d[x][y], w); }
    void run() {
        for (int k = 1; k <= n; k++)
            for (int i = 1; i <= n; i++) {
                if (d[i][k] >= INF) continue;
                for (int j = 1; j <= n; j++)
                    if (d[k][j] < INF) d[i][j] = min(d[i][j], d[i][k] + d[k][j]);
            }
    }
    bool reachable(int i, int j) { return d[i][j] < INF / 2; }
};

// 示例: 读 n m k、m 条边、k 个询问，输出最短路或 impossible
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n, m, k;
    cin >> n >> m >> k;
    Floyd f(n);
    for (int i = 0; i < m; i++) { int x, y; ll z; cin >> x >> y >> z; f.addEdge(x, y, z); }
    f.run();
    while (k--) {
        int x, y; cin >> x >> y;
        if (!f.reachable(x, y)) cout << "impossible" << '\n';
        else cout << f.d[x][y] << '\n';
    }
    return 0;
}
