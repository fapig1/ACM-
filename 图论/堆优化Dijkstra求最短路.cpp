#include <bits/stdc++.h>
#define int long long
#define endl '\n'
using namespace std;

const int N = 1e5 + 5;
const int mod = 80112002;
int n, m, s;
int x, y, z;
vector<pair<int, int>>g[N];
int dist[N];
bool st[N];

//堆优化版dij,适用于稠密图
int dij()
{
    memset(dist, 0x3f, sizeof dist);
    dist[1] = 0;
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>> > q;
    q.push({ 0,1 });

    while (!q.empty())
    {
        auto t = q.top();
        q.pop();
        int dis = t.first, idx = t.second;
        if (st[idx]) continue;
        st[idx] = true;
        for (int i = 0; i < g[idx].size(); i++)
        {
            dist[g[idx][i].second] = min(dis + g[idx][i].first, dist[g[idx][i].second]);
            q.push({ dist[g[idx][i].second] ,g[idx][i].second });
        }
    }

    if (dist[n] == 0x3f3f3f3f3f3f3f3f) return -1;
    else
        return dist[n];
}

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    cin >> n >> m;
    for (int i = 1; i <= m; i++)
    {
        cin >> x >> y >> z;
        g[x].push_back({ z,y });
    }

    cout << dij();


    return 0;
}