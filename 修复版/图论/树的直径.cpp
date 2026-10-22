#include <bits/stdc++.h>
using namespace std;

const int N = 200005;

int n;
vector<int> g[N];
int dist[N];

// 【速查】树的直径（两次 DFS）
// 用法: s = get_far(1); t = get_far(s);  直径 = dist[t]，端点是 s、t
// 注意: 边权非负才能用两次 DFS（有负边权用树形 DP）；本版无边权，有边权把 +1 改成 +w
// 复杂度: O(n)；递归，深链注意爆栈
void dfs(int u, int fa) {
    for (int v : g[u]) {
        if (v == fa) continue;
        dist[v] = dist[u] + 1;
        dfs(v, u);
    }
}

int get_far(int start) {
    memset(dist, 0, sizeof dist);
    dfs(start, 0);

    int far = start;
    for (int i = 1; i <= n; i++) {
        if (dist[i] > dist[far]) {
            far = i;
        }
    }
    return far;
}

int main() {
    cin >> n;

    for (int i = 1; i < n; i++) {
        int u, v;
        cin >> u >> v;
        g[u].push_back(v);
        g[v].push_back(u);
    }

    int s = get_far(1);
    int t = get_far(s);

    cout << dist[t] << '\n';


    return 0;
}
