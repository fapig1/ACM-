#include <bits/stdc++.h>
using namespace std;

const int N = 200005;

// 【速查】树的重心
// 用途: 找删掉后"最大剩余连通块"最小的点（点分治的分治中心）
// 用法: Centroid T;（放全局）  T.n = n;  T.g[u].push_back(v) 建双向边;  T.dfs(1, 0);
//       T.centroid 里是重心（1 或 2 个），T.best 是最大块大小
// 复杂度: O(n)；递归，深链注意爆栈
// 10.22: 仅把原来的全局变量 / 函数放进结构体，算法代码未改
struct Centroid {
    int n;
    vector<int> g[N];

    int sz[N];
    int best = INT_MAX;
    vector<int> centroid;

    void dfs(int u, int fa) {
        sz[u] = 1;

        int max_part = 0;

        for (int v : g[u]) {
            if (v == fa) continue;

            dfs(v, u);

            sz[u] += sz[v];
            max_part = max(max_part, sz[v]);
        }

        max_part = max(max_part, n - sz[u]);

        if (max_part < best) {
            best = max_part;
            centroid.clear();
            centroid.push_back(u);
        } else if (max_part == best) {
            centroid.push_back(u);
        }
    }
};

Centroid T;

int main() {
    cin >> T.n;

    for (int i = 1; i < T.n; i++) {
        int u, v;
        cin >> u >> v;
        T.g[u].push_back(v);
        T.g[v].push_back(u);
    }

    T.dfs(1, 0);

    cout << "Centroid:";
    for (int x : T.centroid) {
        cout << ' ' << x;
    }
    cout << '\n';

    cout << "Min max component size = " << T.best << '\n';

    return 0;
}
