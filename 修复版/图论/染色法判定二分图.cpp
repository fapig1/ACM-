#include <bits/stdc++.h>
using namespace std;

// 【速查】染色法判二分图（BFS，非递归）
// 用法: vector<vector<int>> g(n + 1);  建双向边;   vector<int> color;  bool ok = isBipartite(n, g, color);
// 结果: ok 时 color[i] ∈ {1, 2} 是两侧的划分（每个连通块独立染色）
// 复杂度: O(n + m)；点 1-based
bool isBipartite(int n, const vector<vector<int>>& g, vector<int>& color) {
    color.assign(n + 1, 0);
    for (int s = 1; s <= n; s++) {
        if (color[s]) continue;
        color[s] = 1;
        vector<int> q = {s};
        for (size_t i = 0; i < q.size(); i++) {
            int u = q[i];
            for (int v : g[u]) {
                if (!color[v]) { color[v] = 3 - color[u]; q.push_back(v); }
                else if (color[v] == color[u]) return false;
            }
        }
    }
    return true;
}

// 示例: 读 n m 和 m 条无向边，输出 Yes / No
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n, m;
    cin >> n >> m;
    vector<vector<int>> g(n + 1);
    for (int i = 0; i < m; i++) { int u, v; cin >> u >> v; g[u].push_back(v); g[v].push_back(u); }
    vector<int> color;
    cout << (isBipartite(n, g, color) ? "Yes" : "No") << '\n';
    return 0;
}
