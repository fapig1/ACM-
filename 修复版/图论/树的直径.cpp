#include <bits/stdc++.h>
using namespace std;

const int N = 200005;

// 【速查】树的直径（两次 DFS）
// 用法: TreeDiameter T;（放全局）  T.n = n;  T.g[u].push_back(v) 建双向边;
//       s = T.get_far(1); t = T.get_far(s);  直径 = T.dist[t]，端点是 s、t
// 注意: 边权非负才能用两次 DFS（有负边权用树形 DP）；本版无边权，有边权把 +1 改成 +w
// 复杂度: O(n)；递归，深链注意爆栈
// 10.22: 仅把原来的全局变量 / 函数放进结构体，算法代码未改
struct TreeDiameter {
    int n;
    vector<int> g[N];
    int dist[N];

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
};

TreeDiameter T;

int main() {
    cin >> T.n;

    for (int i = 1; i < T.n; i++) {
        int u, v;
        cin >> u >> v;
        T.g[u].push_back(v);
        T.g[v].push_back(u);
    }

    int s = T.get_far(1);
    int t = T.get_far(s);

    cout << T.dist[t] << '\n';


    return 0;
}
