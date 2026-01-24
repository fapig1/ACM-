#include <bits/stdc++.h>
#define int long long
#define endl '\n'
using namespace std;
const int N = 1e4 + 5;
int t;
int n, m, u, v, w;
vector<pair<int,int> > arr[N];
int st[N], dist[N], cnt[N];
queue<int> q;
//spfa判断负环
void spfa()
{
    for(int i=1;i<=n;i++)
    {
        st[i]=1;
        q.push(i);
    }
    while (!q.empty())
    {
        int tm = q.front();
        q.pop();
        st[tm] = 0;
        for (int i = 0; i < arr[tm].size(); i++)
        {
            int v = arr[tm][i].first, dis = arr[tm][i].second;
            if (dist[v] > dist[tm] + dis)
            {
                dist[v] = dist[tm] + dis;
                if (!st[v])
                {
                    cnt[v] = cnt[tm] + 1;
                    if (cnt[v] >= n)
                    {
                        cout << "Yes" << endl;
                        return;
                    }
                    st[v] = 1;
                    q.push(v);
                }
            }
        }
    }
    cout << "No" << endl;
}

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);


        cin >> n >> m;
        for (int i = 1; i <= m; i++)
        {
            cin >> u >> v >> w;
            arr[u].push_back({v,w});
        }
        spfa();
    

    return 0;
}