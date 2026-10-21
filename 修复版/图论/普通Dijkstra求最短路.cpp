#include <bits/stdc++.h>
#define int long long
#define endl '\n'
using namespace std;

const int N = 1e5+5;
const int mod = 80112002;
int n, m, s;
int x, y, z;
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
    
    cout<<dij();

    
    return 0;
}