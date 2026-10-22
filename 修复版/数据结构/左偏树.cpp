#include <bits/stdc++.h>
using namespace std;

// 【速查】左偏树（可并堆，P3377）
// 用途: 很多个小根堆，支持 O(log n) 合并两个堆、删除堆顶
// 用法: LeftistHeap h(n);  h.val[i] = 第 i 个数（开始时每个数自成一个堆）;
//       h.join(x, y)  -> 合并 x、y 所在的堆（任一已被删除或已同堆则忽略）
//       h.pop(x)      -> 删除 x 所在堆的最小值并返回；x 已被删除返回 -1（值相同时编号小的先出）
//       h.top(x)      -> 只看不删
// 改大根堆: 把 less 里的比较反过来
// 复杂度: 合并 / 删除 O(log n)；1-based
struct LeftistHeap {
    vector<int> l, r, d, fa;
    vector<long long> val;
    vector<char> del;

    LeftistHeap(int n) : l(n + 1), r(n + 1), d(n + 1, 0), fa(n + 1), val(n + 1), del(n + 1, 0) {
        d[0] = -1;
        for (int i = 0; i <= n; i++) fa[i] = i;
    }
    bool less(int x, int y) { return val[x] < val[y] || (val[x] == val[y] && x < y); }
    int merge(int x, int y) {
        if (!x || !y) return x | y;
        if (less(y, x)) swap(x, y);
        r[x] = merge(r[x], y);
        if (d[l[x]] < d[r[x]]) swap(l[x], r[x]);
        d[x] = d[r[x]] + 1;
        return x;
    }
    int find(int x) {                         // x 所在堆的堆顶（路径压缩）
        while (fa[x] != x) x = fa[x] = fa[fa[x]];
        return x;
    }
    void join(int x, int y) {
        if (del[x] || del[y]) return;
        x = find(x), y = find(y);
        if (x == y) return;
        fa[x] = fa[y] = merge(x, y);
    }
    long long top(int x) { return del[x] ? -1 : val[find(x)]; }
    long long pop(int x) {
        if (del[x]) return -1;
        x = find(x);
        del[x] = 1;
        fa[x] = fa[l[x]] = fa[r[x]] = merge(l[x], r[x]);   // 被删点指向新堆顶，旧路径仍能找到
        fa[0] = 0;                                          // 孩子为空时上一行会写到 fa[0]，复原
        return val[x];
    }
};
