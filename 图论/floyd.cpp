#include <bits/stdc++.h>
#define int long long
#define endl '\n'
using namespace std;
int n, m, k, x, y, z;
const int N = 205;
int d[N][N];
//floyd求多源最短路
void floyd()
{
    for (int k = 1; k <= n; k++)
    {
        for (int i = 1; i <= n; i++)
        {
            for (int j = 1; j <= n; j++)
            {
                d[i][j] = min(d[i][j], d[i][k] + d[k][j]);
            }
        }
    }
}

signed main() {
    cin >> n >> m >> k;
    memset(d, 0x3f, sizeof d);
    for (int i = 1; i <= n; i++)
    {
        d[i][i] = 0;
    }
    for (int i = 1; i <= m; i++)
    {
        cin >> x >> y >> z;
        d[x][y] = min(d[x][y], z);
    }
    floyd();
    for (int i = 1; i <= k; i++)
    {
        cin >> x >> y;
        if (d[x][y] > 0x3f3f3f3f/2) cout << "impossible" << endl;
        else
            cout << d[x][y] << endl;
    }

    return 0;
}