#include <bits/stdc++.h>
#define int long long
#define endl '\n'
using namespace std;
const int N = 205;
// 【速查】Floyd 全源最短路
// 用法: Floyd F;（放全局）  F.n = n;  F.d 先 memset 0x3f、d[i][i]=0，读边 F.d[x][y]=min(F.d[x][y], w)；
//       F.floyd() 后 F.d[i][j] 即最短路
// 判不可达: d[x][y] > 0x3f3f3f3f3f3f3f3fLL / 2（long long 下 memset 0x3f 的值）
// 可有负边，不能有负环
// 复杂度: O(n³)，n ≤ 500 左右
// 10.22: 仅把原来的全局变量 / 函数放进结构体，算法代码未改
struct Floyd{
    int n;
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
};

Floyd F;

signed main() {
    int m, k, x, y, z;
    cin >> F.n >> m >> k;
    memset(F.d, 0x3f, sizeof F.d);
    for (int i = 1; i <= F.n; i++)
    {
        F.d[i][i] = 0;
    }
    for (int i = 1; i <= m; i++)
    {
        cin >> x >> y >> z;
        F.d[x][y] = min(F.d[x][y], z);
    }
    F.floyd();
    for (int i = 1; i <= k; i++)
    {
        cin >> x >> y;
        if (F.d[x][y] > 0x3f3f3f3f3f3f3f3fLL / 2) cout << "impossible" << endl;   // 10.22 修复: 原版 0x3f3f3f3f/2，距离 > 5e8 被误判
        else
            cout << F.d[x][y] << endl;
    }

    return 0;
}
