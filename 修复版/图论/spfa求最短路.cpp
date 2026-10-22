#include <bits/stdc++.h>
#define int long long
#define endl '\n'
using namespace std;

const int N = 1e5 + 5;
const int mod = 80112002;
// 【速查】SPFA 单源最短路（可有负边，无负环）
// 用法: SPFA S;（放全局）  S.n = n;  S.g[x].push_back({y, w});  S.spfa()
// 返回: 1 到 n 的最短路，到不了返回 0x3f3f3f3f3f3f3f3f；S.dist[] 是到所有点的距离
// 复杂度: 一般很快，最坏 O(nm)（会被卡）；没有负边请用 Dijkstra
// 10.22: 仅把原来的全局变量 / 函数放进结构体，算法代码未改
struct SPFA{
    int n, m, s;
    vector<pair<int, int>>g[N];
    int dist[N];
    bool st[N];
    //spfa求最短路
    int spfa()
    {
        memset(dist, 0x3f, sizeof dist);
        dist[1] = 0;
        queue<int> q;
        q.push(1);
        st[1]=true;
        while (!q.empty())
        {
            auto t = q.front();
            q.pop();
            st[t] = false;
            for (int i = 0; i < g[t].size(); i++)
            {
                if (dist[g[t][i].first] > dist[t] + g[t][i].second)
                {
                    dist[g[t][i].first] = dist[t] + g[t][i].second;
                    if (!st[g[t][i].first])
                    {
                        q.push(g[t][i].first);
                        st[g[t][i].first] = true;
                    }
                }
            }
        }

        if (dist[n] == 0x3f3f3f3f3f3f3f3f) return 0x3f3f3f3f3f3f3f3f;
        else
            return dist[n];
    }
};

SPFA S;

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    int x, y, z;
    cin >> S.n >> S.m;
    for (int i = 1; i <= S.m; i++)
    {
        cin >> x >> y >> z;
        S.g[x].push_back({ y,z });
    }
    int res;
    res = S.spfa();
    if (res == 0x3f3f3f3f3f3f3f3f)
        cout << "impossible" << endl;
    else
        cout << res << endl;

    return 0;
}
