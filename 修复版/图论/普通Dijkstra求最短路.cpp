#include <bits/stdc++.h>
#define int long long
#define endl '\n'
using namespace std;

const int N = 1e5+5;
const int mod = 80112002;
// 【速查】朴素 Dijkstra（稠密图）
// 用法: Dijkstra D;（放全局）  D.n = n;  D.g[x].push_back({y, w});  D.dij()
// 返回: 1 到 n 的最短路，到不了返回 -1；边权必须非负
// 复杂度: O(n² + m)，n ≤ 几千、m 接近 n² 时用
// 10.22: 仅把原来的全局变量 / 函数放进结构体，算法代码未改
struct Dijkstra{
    int n, m, s;
    vector<pair<int, int> >g[N];
    int dist[N];
    bool st[N];
    //普通版dijkstra求单源最短路
    int dij()
    {
        memset(dist, 0x3f, sizeof dist);
        dist[1] = 0;
        for (int p = 0; p < n; p++)
        {
            int t = -1;
            for (int j = 1; j <= n; j++)
            {
                if (!st[j] && (t == -1 || dist[t] > dist[j]))
                {
                    t = j;
                }
            }
            st[t] = true;
            for (int i = 0; i < g[t].size(); i++)
            {
                int a = g[t][i].first, b = g[t][i].second;
                dist[a] = min(dist[a], dist[t] + b);
            }

        }
        if(dist[n]==0x3f3f3f3f3f3f3f3f) return -1;
        else
        return dist[n];
    }
};

Dijkstra D;

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    int x, y, z;
    cin >> D.n >> D.m;
    for (int i = 1; i <= D.m; i++)
    {
        cin >> x >> y >> z;
        D.g[x].push_back({ y,z });
    }

    cout<<D.dij();


    return 0;
}
