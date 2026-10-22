#include <bits/stdc++.h>
using namespace std;

// 【速查】Link-Cut Tree（动态树，P3690）
// 用途: 森林上动态加边 / 删边 + 路径查询 / 单点修改 / 判连通（本模板维护路径点权异或和）
// 用法: LCT t(n);  t.v[i] = 初始点权（再对每个点 t.pushup(i)，或直接用 modify）;
//       t.link(x, y) -> 不连通才连，成功返回 true;  t.cut(x, y) -> 有边 (x,y) 才断，成功返回 true;
//       t.query(x, y) -> x 到 y 路径点权异或和（要求连通）;  t.modify(x, val);  t.connected(x, y)
// 改成路径和 / 最大值: 改 pushup 里的运算（以及 s[] 的含义）
// 复杂度: 每次均摊 O(log n)；点 1-based
struct LCT {
    vector<array<int, 2>> c;
    vector<int> f, v, s;
    vector<char> r;
    vector<int> stk;

    LCT(int n) : c(n + 1, {0, 0}), f(n + 1), v(n + 1), s(n + 1), r(n + 1) {}

    bool nroot(int x) { return c[f[x]][0] == x || c[f[x]][1] == x; }   // x 不是所在 splay 的根
    void pushup(int x) { s[x] = s[c[x][0]] ^ s[c[x][1]] ^ v[x]; }
    void rev(int x) { swap(c[x][0], c[x][1]); r[x] ^= 1; }
    void pushdown(int x) {
        if (r[x]) {
            if (c[x][0]) rev(c[x][0]);
            if (c[x][1]) rev(c[x][1]);
            r[x] = 0;
        }
    }
    void rotate(int x) {
        int y = f[x], z = f[y], k = (c[y][1] == x), w = c[x][!k];
        if (nroot(y)) c[z][c[z][1] == y] = x;
        c[x][!k] = y; c[y][k] = w;
        if (w) f[w] = y;
        f[y] = x; f[x] = z;
        pushup(y);
    }
    void splay(int x) {
        int y = x;
        stk.clear(); stk.push_back(y);
        while (nroot(y)) stk.push_back(y = f[y]);
        while (!stk.empty()) { pushdown(stk.back()); stk.pop_back(); }
        while (nroot(x)) {
            y = f[x]; int z = f[y];
            if (nroot(y)) rotate(((c[y][0] == x) ^ (c[z][0] == y)) ? x : y);
            rotate(x);
        }
        pushup(x);
    }
    void access(int x) {                       // 打通根到 x 的实链
        for (int y = 0; x; x = f[y = x]) { splay(x); c[x][1] = y; pushup(x); }
    }
    void makeroot(int x) { access(x); splay(x); rev(x); }
    int findroot(int x) {
        access(x); splay(x);
        pushdown(x);
        while (c[x][0]) { x = c[x][0]; pushdown(x); }
        splay(x);
        return x;
    }
    void split(int x, int y) { makeroot(x); access(y); splay(y); }   // 之后 y 的子树就是 x-y 路径
    bool connected(int x, int y) { return findroot(x) == findroot(y); }
    bool link(int x, int y) {
        makeroot(x);
        if (findroot(y) == x) return false;
        f[x] = y;
        return true;
    }
    bool cut(int x, int y) {
        makeroot(x);
        if (findroot(y) != x || f[y] != x || c[y][0]) return false;
        f[y] = c[x][1] = 0;
        pushup(x);
        return true;
    }
    int query(int x, int y) { split(x, y); return s[y]; }
    void modify(int x, int val) { splay(x); v[x] = val; pushup(x); }
};
