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
// 【速查】SPFA 判负环
// 用法: arr[u].push_back({v, w}); spfa() 直接输出 Yes(有负环) / No
// 说明: 所有点一开始都入队（相当于超级源点），能判整张图任意位置的负环；多组数据记得清空 dist/cnt/st
// 复杂度: 最坏 O(nm)
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