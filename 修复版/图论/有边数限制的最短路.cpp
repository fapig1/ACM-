#include <bits/stdc++.h>
#define int long long
#define endl '\n'
using namespace std;

const int N = 1e5 + 5;
const int mod = 80112002;
// 【速查】Bellman-Ford（最多经过 k 条边的最短路）
// 用法: BellmanFord B;（放全局）  B.n = n; B.k = k;  B.g[x].push_back({y, w});  B.bell()
// 返回: 1 到 n 最多 k 条边的最短距离；到不了返回 0x3f3f3f3f3f3f3f3f（可有负边）
// 复杂度: O(k · m)；每轮必须用上一轮的 back 数组，否则会"串联"超过 k 条边
// 10.22: 仅把原来的全局变量 / 函数放进结构体，算法代码未改
struct BellmanFord{
    int n, m, s, k, t;
    vector<pair<int, int>>g[N];
    int dist[N];
    int back[N];
    bool st[N];
    //bellman-ford算法，有边数限制为k的单源最短路
    int bell()
    {
        memset(dist, 0x3f, sizeof dist);
        dist[1] = 0;
        for (int p = 0; p < k; p++)
        {
            memcpy(back, dist, sizeof dist);
            for (int j = 1; j <= n; j++)
            {
                for (auto& t : g[j])
                {
                    int a = t.first, b = t.second;

                    dist[a] = min(back[j] + b, dist[a]);
                }
            }
        }
        if (dist[n] > 0x3f3f3f3f3f3f3f3f / 2) return 0x3f3f3f3f3f3f3f3f;
        else
            return dist[n];
    }
};

BellmanFord B;

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    int x, y, z;
    cin >> B.n >> B.m >> B.k;
    for (int i = 1; i <= B.m; i++)
    {
        cin >> x >> y >> z;
        B.g[x].push_back({ y,z });
    }
    int res = B.bell();
    if (res == 0x3f3f3f3f3f3f3f3f) cout << "impossible" << endl;
    else
        cout << res << endl;

    return 0;
}
