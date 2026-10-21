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

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    cin >> n >> m;
    for (int i = 1; i <= m; i++)
    {
        cin >> x >> y >> z;
        g[x].push_back({ y,z });
    }
    int res;
    res = spfa();
    if (res == 0x3f3f3f3f3f3f3f3f)
        cout << "impossible" << endl;
    else
        cout << res << endl;

    return 0;
}