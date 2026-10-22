#include <bits/stdc++.h>
using namespace std;

// 【速查】拓扑排序（Kahn / BFS 入度法）
// 用法: vector<vector<int>> g(n + 1);  g[x].push_back(y);   // 点 1-based，有向边 x -> y
//       vector<int> order;  bool ok = topoSort(n, g, order);
// 返回: true 时 order 是一个拓扑序；false 说明有环（order.size() < n）
// 字典序最小的拓扑序: 把 queue 换成 priority_queue<int, vector<int>, greater<int>>
// 复杂度: O(n + m)
bool topoSort(int n, const vector<vector<int>>& g, vector<int>& order) {
    vector<int> deg(n + 1, 0);
    for (int u = 1; u <= n; u++) for (int v : g[u]) deg[v]++;
    queue<int> q;
    order.clear();
    for (int i = 1; i <= n; i++) if (deg[i] == 0) q.push(i);
    while (!q.empty()) {
        int u = q.front(); q.pop();
        order.push_back(u);
        for (int v : g[u]) if (--deg[v] == 0) q.push(v);
    }
    return (int)order.size() == n;
}

// 示例: 读 n m 和 m 条边，输出拓扑序，有环输出 -1
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n, m;
    cin >> n >> m;
    vector<vector<int>> g(n + 1);
    for (int i = 0; i < m; i++) { int x, y; cin >> x >> y; g[x].push_back(y); }
    vector<int> order;
    if (topoSort(n, g, order)) { for (int x : order) cout << x << ' '; }
    else cout << -1 << '\n';
    return 0;
}
