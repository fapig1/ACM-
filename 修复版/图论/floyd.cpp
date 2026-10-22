#include <bits/stdc++.h>
#define int long long
#define endl '\n'
using namespace std;
int n, m, k, x, y, z;
const int N = 205;
int d[N][N];
// 【速查】Floyd 全源最短路
// 用法: d 先 memset 0x3f、d[i][i]=0，读边 d[x][y]=min(d[x][y], w)；floyd() 后 d[i][j] 即最短路
// 判不可达: d[x][y] > 0x3f3f3f3f3f3f3f3fLL / 2（long long 下 memset 0x3f 的值）
// 可有负边，不能有负环
// 复杂度: O(n³)，n ≤ 500 左右
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
        if (d[x][y] > 0x3f3f3f3f3f3f3f3fLL / 2) cout << "impossible" << endl;   // 10.22 修复: 原版 0x3f3f3f3f/2，距离 > 5e8 被误判
        else
            cout << d[x][y] << endl;
    }

    return 0;
}