#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// 【速查】KM 算法（二分图最大权完美匹配，BFS 版）
// 用法: KM km(n);  km.w[i][j] = 左 i 连右 j 的权（1-based，默认 KM::NO_EDGE 表示无边）;  ll ans = km.solve();
// 返回: 最大权和；km.match[j] = 与右点 j 匹配的左点；返回值 < KM::NO_EDGE / 2 表示不存在完美匹配
// 最小权: 权取负，答案取 -solve()；左右部规模不等时补虚点（权 0）补成方阵
// 复杂度: O(n³)，n ≤ 500 没问题
// 原理: 维护顶标 la[x] + lb[y] >= w[x][y]，只走相等边。每轮从一个左部点出发，
//   像 Dijkstra 一样用 slack 记录未访问右部点的最小差值，每步把整棵交错树的顶标
//   平移 d（树内相等边不变，树外 slack 减 d），O(n) 步找到增广路，总 O(n³)。
// 10.22: 原 DFS 版最坏 O(n⁴)（n=500、w=i·j 实测 10s），已换成 BFS 版；本次再封装成结构体
struct KM {
    static constexpr ll NO_EDGE = -(ll)1e15;
    int n;
    vector<vector<ll>> w;
    vector<int> match;                         // match[y]: 与右部点 y 匹配的左部点

    KM(int n) : n(n), w(n + 1, vector<ll>(n + 1, NO_EDGE)), match(n + 1, 0) {}

    ll solve() {
        const ll BIG = LLONG_MAX / 4;
        vector<ll> la(n + 1, NO_EDGE), lb(n + 1, 0), slk(n + 1);
        vector<int> pre(n + 1);
        vector<char> visb(n + 1);
        for (int i = 1; i <= n; i++) for (int j = 1; j <= n; j++) la[i] = max(la[i], w[i][j]);
        fill(match.begin(), match.end(), 0);
        for (int s = 1; s <= n; s++) {
            fill(visb.begin(), visb.end(), 0); fill(slk.begin(), slk.end(), BIG); fill(pre.begin(), pre.end(), 0);
            int y = 0;
            match[0] = s;                      // 右部虚点 0 挂在起点 s 上
            do {
                visb[y] = 1;
                int x = match[y], ny = 0;
                ll d = BIG;
                for (int j = 1; j <= n; j++) {
                    if (visb[j]) continue;
                    ll t = la[x] + lb[j] - w[x][j];
                    if (t < slk[j]) { slk[j] = t; pre[j] = y; }
                    if (slk[j] < d) { d = slk[j]; ny = j; }
                }
                for (int j = 0; j <= n; j++) {
                    if (visb[j]) { la[match[j]] -= d; lb[j] += d; }
                    else slk[j] -= d;
                }
                y = ny;
            } while (match[y]);                // 走到未匹配的右部点即找到增广路
            while (y) { match[y] = match[pre[y]]; y = pre[y]; }   // 沿前驱翻转
        }
        ll ans = 0;
        for (int j = 1; j <= n; j++) ans += w[match[j]][j];
        return ans;
    }
};

// 示例: 读 n 和 n×n 权值矩阵，输出最大权完美匹配
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n;
    cin >> n;
    KM km(n);
    for (int i = 1; i <= n; i++) for (int j = 1; j <= n; j++) cin >> km.w[i][j];
    cout << km.solve() << "\n";
    return 0;
}
