#include <bits/stdc++.h>
#define int long long
#define endl '\n'
using namespace std;

const int N = 1e5 + 5;

// 【速查】拓扑排序（Kahn / BFS 入度法）
// 用法: Topo T;（放全局）  T.n = n;  T.g[x].push_back(y); T.d[y]++;  然后 T.top()
// 返回: true 时 T.arr 是一个拓扑序；false 说明有环（arr.size() < n）
// 复杂度: O(n + m)；想要字典序最小的拓扑序把 queue 换成小根堆
// 10.22: 仅把原来的全局变量 / 函数放进结构体，算法代码未改
struct Topo{
    int n, m;
    vector<int> arr;
    vector<int> g[N];
    int d[N];
    queue<int> q;

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
};

Topo T;

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    cin >> T.n >> T.m;
    for (int i = 1; i <= T.m; i++)
    {
        int x, y;
        cin >> x >> y;
        T.g[x].push_back(y);
        T.d[y]++;
    }

    if (T.top())
    {
        for (auto x : T.arr)
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
