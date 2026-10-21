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