#include <bits/stdc++.h>
#define int long long
#define endl '\n'
using namespace std;

const int N = 1e5 + 5;
int n, m;
vector<int> arr;
vector<int> g[N];
int d[N];
queue<int> q;

// 【速查】拓扑排序（Kahn / BFS 入度法）
// 用法: g[x].push_back(y); d[y]++;  然后 top()
// 返回: true 时 arr 是一个拓扑序；false 说明有环（arr.size() < n）
// 复杂度: O(n + m)；想要字典序最小的拓扑序把 queue 换成小根堆
bool top()
{
    for (int i = 1; i <= n; i++)
    {
        if (d[i] == 0)
        {
            q.push(i);
            arr.push_back(i);
        }
    }
    while (!q.empty())
    {
        int t = q.front();
        q.pop();

        for (int i = 0; i < g[t].size(); i++)
        {
            d[g[t][i]]--;
            if (d[g[t][i]] == 0)
            {
                q.push(g[t][i]);
                arr.push_back(g[t][i]);
            }
        }
    }
    if (arr.size() == n) return true;
    else return false;
}

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    cin >> n >> m;
    for (int i = 1; i <= m; i++)
    {
        int x, y;
        cin >> x >> y;
        g[x].push_back(y);
        d[y]++;
    }

    if (top())
    {
        for (auto x : arr)
        {
            cout << x << ' ';
        }
    }
    else
    {
        cout << -1 << endl;
    }
    return 0;
}