#include <bits/stdc++.h>
using namespace std;
using ll = long long;
// 二维树状数组，下标 1-based，单次修改/查询复杂度 O(log n * log m)，空间 O(n * m)
// 注意：树状数组靠前缀和相减得到区间信息，只能维护可减的运算（如和），
// 不能像线段树那样维护 max/min（除非修改只在某一维上单调）
// 变体一：单点加 + 矩形求和
// 变体二：矩形加 + 单点求和（二维差分，套变体一）
// 变体三：矩形加 + 矩形求和（四个树状数组维护差分及其系数）

// 【速查】二维树状数组 · 单点加 + 矩形求和
// 用法: BIT2D b(n, m);  b.add(x, y, v);  b.ask(x, y) -> (1,1)~(x,y) 前缀和;  b.ask(x1, y1, x2, y2) -> 矩形和
// 复杂度: O(log n · log m)，空间 O(nm)；1-based
// 变体一：单点加 + 矩形求和
struct BIT2D {
    int n, m;
    vector<vector<ll>> tr;

    BIT2D(int n, int m) : n(n), m(m),
    tr(n + 1, vector<ll>(m + 1, 0)) {}

    void clear() {
        for(auto& row : tr) fill(row.begin(), row.end(), 0);
    }

    // 单点 (x, y) 加 val
    void add(int x, int y, ll val) {
        for(int i = x; i <= n; i += i & -i)
            for(int j = y; j <= m; j += j & -j)
                tr[i][j] += val;
    }

    // 查询左上角 (1, 1) 到 (x, y) 的前缀和
    ll ask(int x, int y) {
        ll res = 0;
        for(int i = x; i > 0; i -= i & -i)
            for(int j = y; j > 0; j -= j & -j)
                res += tr[i][j];
        return res;
    }

    // 查询矩形 (x1, y1) - (x2, y2) 的和（要求 x1 <= x2, y1 <= y2）
    ll ask(int x1, int y1, int x2, int y2) {
        return ask(x2, y2) - ask(x1 - 1, y2) - ask(x2, y1 - 1) + ask(x1 - 1, y1 - 1);
    }
};

// 【速查】二维树状数组 · 矩形加 + 单点查
// 用法: BIT2D_RangeAddPointQuery b(n, m);  b.add(x1, y1, x2, y2, v);  b.ask(x, y) -> 该点当前值
// 复杂度: O(log n · log m)
// 变体二：矩形加 + 单点求和
// 原理：维护二维差分，矩形加等价于差分数组上的 4 个单点加，
// 单点查询等价于差分数组的前缀和，内部直接套变体一
struct BIT2D_RangeAddPointQuery {
    int n, m;
    BIT2D bit;

    BIT2D_RangeAddPointQuery(int n, int m) : n(n), m(m), bit(n, m) {}

    void clear() { bit.clear(); }

    // 矩形 (x1, y1) - (x2, y2) 内所有点加 val
    void add(int x1, int y1, int x2, int y2, ll val) {
        bit.add(x1, y1, val);
        bit.add(x1, y2 + 1, -val);
        bit.add(x2 + 1, y1, -val);
        bit.add(x2 + 1, y2 + 1, val);
    }

    // 查询单点 (x, y) 的值
    ll ask(int x, int y) {
        return bit.ask(x, y);
    }
};

// 【速查】二维树状数组 · 矩形加 + 矩形求和
// 用法: BIT2D_RangeAddRangeSum b(n, m);  b.add(x1, y1, x2, y2, v);  b.ask(x1, y1, x2, y2) -> 矩形和
// 复杂度: O(log n · log m)，常数是普通版的 4 倍，空间 4·nm
// 变体三：矩形加 + 矩形求和
// 原理：对差分数组 d 维护前缀和时会出现 i、j 的系数，展开后需要 4 个树状数组：
// 分别维护 d[i][j]、d[i][j] * i、d[i][j] * j、d[i][j] * i * j
struct BIT2D_RangeAddRangeSum {
    int n, m;
    // t[0]: d, t[1]: d*i, t[2]: d*j, t[3]: d*i*j
    vector<BIT2D> t;

    BIT2D_RangeAddRangeSum(int n, int m) : n(n), m(m), t(4, BIT2D(n, m)) {}

    void clear() {
        for(auto& bit : t) bit.clear();
    }

    // 内部：在差分意义上做单点累加，val 的四个系数分量
    void add(int x, int y, ll val) {
        t[0].add(x, y, val);
        t[1].add(x, y, val * x);
        t[2].add(x, y, val * y);
        t[3].add(x, y, val * x * y);
    }

    // 矩形 (x1, y1) - (x2, y2) 内所有点加 val
    void add(int x1, int y1, int x2, int y2, ll val) {
        add(x1, y1, val);
        add(x1, y2 + 1, -val);
        add(x2 + 1, y1, -val);
        add(x2 + 1, y2 + 1, val);
    }

    // 查询 (1, 1) 到 (x, y) 的前缀和，公式由差分前缀和展开得到
    ll ask(int x, int y) {
        ll res = (x + 1) * (y + 1) * t[0].ask(x, y)
            - (y + 1) * t[1].ask(x, y)
            - (x + 1) * t[2].ask(x, y)
            + t[3].ask(x, y);
        return res;
    }

    // 查询矩形 (x1, y1) - (x2, y2) 的和（要求 x1 <= x2, y1 <= y2）
    ll ask(int x1, int y1, int x2, int y2) {
        return ask(x2, y2) - ask(x1 - 1, y2) - ask(x2, y1 - 1) + ask(x1 - 1, y1 - 1);
    }
};

// 使用示例
// int main() {
//     int n = 5, m = 5;
//
//     // ---- 变体一：单点加 + 矩形求和 ----
//     BIT2D bit(n, m);
//     bit.add(2, 3, 7);                      // 点 (2,3) 加 7
//     ll s = bit.ask(1, 1, 3, 4);            // 矩形 (1,1)-(3,4) 的和 = 7
//
//     // ---- 变体二：矩形加 + 单点求和 ----
//     BIT2D_RangeAddPointQuery b1(n, m);
//     b1.add(1, 1, 3, 3, 5);                 // 矩形 (1,1)-(3,3) 全部加 5
//     ll v = b1.ask(2, 2);                   // 点 (2,2) 的值 = 5
//
//     // ---- 变体三：矩形加 + 矩形求和 ----
//     BIT2D_RangeAddRangeSum b2(n, m);
//     b2.add(1, 1, 3, 3, 5);                 // 矩形 (1,1)-(3,3) 全部加 5
//     b2.add(2, 2, 4, 4, 2);                 // 矩形 (2,2)-(4,4) 全部加 2
//     ll sum = b2.ask(2, 2, 3, 3);           // 矩形 (2,2)-(3,3) 的和 = 4*5 + 4*2 = 28
//
//     return 0;
// }
