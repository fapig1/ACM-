#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// 【速查】树的直径（两次 BFS，非递归）
// 用法: vector<vector<pair<int,ll>>> g(n + 1);  g[u].push_back({v, w}); g[v].push_back({u, w});   // 点 1-based
//       auto [len, s, t] = treeDiameter(n, g);   // 直径长度与两个端点
// 注意: 边权必须非负（有负边权用树形 DP：每个点维护向下最长、次长链）；无边权就全部填 1
// 复杂度: O(n)
// 从 s 出发求到所有点的距离（树上 BFS 即可，不需要 Dijkstra）
vector<ll> treeDist(int n, const vector<vector<pair<int, ll>>>& g, int s) {
    vector<ll> d(n + 1, -1);
    vector<int> q = {s};
    d[s] = 0;
    for (size_t i = 0; i < q.size(); i++) {
        int u = q[i];
        for (auto [v, w] : g[u]) if (d[v] < 0) { d[v] = d[u] + w; q.push_back(v); }
    }
    return d;
}
tuple<ll, int, int> treeDiameter(int n, const vector<vector<pair<int, ll>>>& g) {
    auto d1 = treeDist(n, g, 1);
    int s = max_element(d1.begin() + 1, d1.end()) - d1.begin();
    auto d2 = treeDist(n, g, s);
    int t = max_element(d2.begin() + 1, d2.end()) - d2.begin();
    return {d2[t], s, t};
}

// 示例: 读 n 和 n-1 条无权边，输出直径长度
int main() {
    int n;
    cin >> n;
    vector<vector<pair<int, ll>>> g(n + 1);
    for (int i = 1; i < n; i++) { int u, v; cin >> u >> v; g[u].push_back({v, 1}); g[v].push_back({u, 1}); }
    auto [len, s, t] = treeDiameter(n, g);
    cout << len << '\n';
    return 0;
}
