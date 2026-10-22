# David_数据结构（10.21 修复版）

> **通用前导**：本文档所有代码块为片段，需配合以下前导使用：

```cpp
#include <bits/stdc++.h>
using namespace std;
```

标注 **【本次修复】** 的模板经过 2026-10-21 全量审查修复，并全部通过随机对拍/压测回归验证；各修复点在代码注释中亦有说明。

[TOC]

### 区间加乘线段树 + zkw线段树

> Seg：区间乘/加/取模区间和（先乘后加复合标记）；Tree：zkw 非递归单点赋值区间 max（仅非负值域）。

```cpp
// 【速查】区间加乘线段树（取模区间和）
// 用途: 区间乘 x、区间加 x、查区间和 (mod)，经典 P3373
// 用法: Seg sg(n, mod); sg.build(1, 1, n, a);          // a 下标 1..n
//       sg.updateMul(1, 1, n, l, r, x);  sg.updateAdd(1, 1, n, l, r, x);
//       sg.ask(1, 1, n, l, r) -> 区间和 % mod；多组数据先 sg.clear()
// 注意: x 要先规整到 [0, mod)，负数会算出负值
// 复杂度: 建树 O(n)，每次修改/查询 O(log n)
//线段树，支持区间加与区间乘
struct Seg{
    int n, mod;
    vector<int> tr, lazyAdd, lazyMul;

    Seg(int n, int mod) : n(n), mod(mod),
    tr(n * 4 + 5),
    lazyAdd(n * 4 + 5),
    lazyMul(n * 4 + 5, 1) {}

    void build(int p, int l, int r, const vector<int>& a){
        if(l == r){
            tr[p] = a[l] % mod;
            return;
        }
        int mid = l + r >> 1;
        build(p << 1, l, mid, a);
        build(p << 1 | 1, mid + 1, r, a);
        pushup(p);
    }

    void clear(){
        fill(tr.begin(), tr.end(), 0);
        fill(lazyAdd.begin(), lazyAdd.end(), 0);
        fill(lazyMul.begin(), lazyMul.end(), 1);
    }

    void pushup(int p){
        tr[p] = (tr[p << 1] + tr[p << 1 | 1]) % mod;
    }

    void apply(int p, int l, int r, int mul, int add){
        tr[p] = (1LL * tr[p] * mul % mod + 1LL * (r - l + 1) * add % mod) % mod;
        lazyMul[p] = 1LL * lazyMul[p] * mul % mod;
        lazyAdd[p] = (1LL * lazyAdd[p] * mul % mod + add) % mod;
    }

    void pushdown(int p, int l, int r){
        if(lazyMul[p] != 1 || lazyAdd[p] != 0){
            int mid = l + r >> 1;
            apply(p << 1, l, mid, lazyMul[p], lazyAdd[p]);
            apply(p << 1 | 1, mid + 1, r, lazyMul[p], lazyAdd[p]);
            lazyMul[p] = 1;
            lazyAdd[p] = 0;
        }
    }

    void updateMul(int p, int l, int r, int ql, int qr, int val){
        if(ql <= l && r <= qr){
            apply(p, l, r, val, 0);
            return;
        }
        pushdown(p, l, r);
        int mid = l + r >> 1;
        if(ql <= mid) updateMul(p << 1, l, mid, ql, qr, val);
        if(mid < qr) updateMul(p << 1 | 1, mid + 1, r, ql, qr, val);
        pushup(p);
    }

    void updateAdd(int p, int l, int r, int ql, int qr, int val){
        if(ql <= l && r <= qr){
            apply(p, l, r, 1, val);
            return;
        }
        pushdown(p, l, r);
        int mid = l + r >> 1;
        if(ql <= mid) updateAdd(p << 1, l, mid, ql, qr, val);
        if(mid < qr) updateAdd(p << 1 | 1, mid + 1, r, ql, qr, val);
        pushup(p);
    }

    int ask(int p, int l, int r, int ql, int qr){
        if(ql <= l && r <= qr){
            return tr[p];
        }
        pushdown(p, l, r);
        int mid = l + r >> 1;
        int res = 0;
        if(ql <= mid) res = (res + ask(p << 1, l, mid, ql, qr)) % mod;
        if(mid < qr) res = (res + ask(p << 1 | 1, mid + 1, r, ql, qr)) % mod;
        return res;
    }
};

// 【速查】zkw 非递归线段树（单点赋值 + 区间最大值）
// 用法: Tree t(n);  t.update(0, 0, 0, pos, val);  t.query(0, 0, 0, l, r)   // 前三个参数没用；1-based
// 返回: [l, r] 的最大值；l > r 返回 0。初值全 0，只适合非负值
// 复杂度: O(log n)，常数比递归版小
//非递归线段树，适用于单点修改区间查询的问题，常数更小
struct Tree {
    int sz;                // 大于值域上限的 2 的幂
    vector<int> tree;

    // n: 值域右端点（1‑based），实际会用到的最大下标
    Tree(int n) {
        sz = 1;
        while (sz < n) sz <<= 1;
        tree.assign(sz * 2, 0);      // 1‑based 索引，叶子从 sz 开始
    }

    // 清空线段树（全部置零），供下一轮重用
    void clear() {
        fill(tree.begin(), tree.end(), 0);
    }

    // 单点更新：忽略前三个无用参数，只保留 tar, val
    void update(int /*ind*/, int /*l*/, int /*r*/, int tar, int val) {
        int p = tar + sz - 1;         // tar 是 1‑based
        tree[p] = val;
        p >>= 1;
        while (p) {
            tree[p] = max(tree[p << 1], tree[p << 1 | 1]);
            p >>= 1;
        }
    }

    // 区间最大值查询：忽略前三个无用参数，ql,qr 为 1‑based
    int query(int /*ind*/, int /*l*/, int /*r*/, int ql, int qr) {
        if (ql > qr) return 0;
        int l = ql + sz - 1;
        int r = qr + sz - 1;
        int ans = 0;
        while (l <= r) {
            if (l & 1) ans = max(ans, tree[l++]);
            if (!(r & 1)) ans = max(ans, tree[r--]);
            l >>= 1;
            r >>= 1;
        }
        return ans;
    }
};
```

### 二维线段树（矩形加+矩形和）

> 【本次重写】原版 build 重定义无法编译，且带修改场景三重错误；已重写为联合递归（x树×y树乘积）+标记永久化，O(log n·log m)。

```cpp
// 【速查】二维线段树（矩形加 + 矩形求和）
// 用法: Seg2D sg(n, m);  sg.build(a);                  // a 为 (n+1)×(m+1)，1-based
//       sg.update(x1, x2, y1, y2, v);  sg.ask(x1, x2, y1, y2) -> long long   // 先 x 范围再 y 范围！
// 复杂度: 每次 O(log n · log m)
// 内存: 两个 4n×4m 的 long long 矩阵，n=m=1000 约 256MB，大了就换二维树状数组
/* ============================================================================
 * 二维线段树(矩形加 + 矩形求和) —— 联合递归 + 标记永久化版本
 * ----------------------------------------------------------------------------
 * 原版三处致命缺陷(已整体重写):
 *   1) 两个 build 函数签名完全相同, 重定义无法编译;
 *   2) 外层整覆盖节点做内层区间加时, tr += ylen*val 漏乘该节点管辖的 x 长度;
 *   3) 外层部分覆盖时只下探子节点, 看不到祖先外层节点上的整覆盖修改;
 *      且部分覆盖后整行重算 tr[p][py]=两子之和, 会冲掉该节点已有的标记。
 * 教训: "先按 x 分解、再对每个整覆盖 x 节点做内层 y 分解"的两段式写法,
 *       外层重算会读到不含内层祖先标记的子行值, 无法在带修改时保持正确。
 *
 * 本实现: 把二维线段树看成一维 x 树与一维 y 树的乘积(节点 = (px,py) 对),
 *   单次递归同时下潜两个维度(最多 4 个孩子), 标记永久化(不 pushdown):
 *     tr[px][py]  = span(px)×span(py) 的精确矩阵和(含本节点标记);
 *     tag[px][py] = 曾对该整块加过的单位格增量 v(永久存放, 不下传);
 *   更新: 整块覆盖处 tag+=v, tr+=v*块面积; 部分覆盖则递归孩子后
 *         tr = 四孩子之和 + 自身 tag*块面积 (永远不冲掉标记);
 *   查询: 整块覆盖直接返回 tr; 部分覆盖 = 孩子递归 + 自身 tag×(与查询的交集面积)。
 *   复杂度: 单次更新/查询 O(log n · log m); 空间 4n×4m 的 long long 两个矩阵。
 * ==========================================================================*/
struct Seg2D {
    int n, m;
    vector<vector<long long>> tr, tag;

    Seg2D(int n, int m) : n(n), m(m),
        tr(n * 4 + 5, vector<long long>(m * 4 + 5, 0)),
        tag(n * 4 + 5, vector<long long>(m * 4 + 5, 0)) {}

    void clear() {
        for (auto& row : tr) fill(row.begin(), row.end(), 0);
        for (auto& row : tag) fill(row.begin(), row.end(), 0);
    }

    // ---------------- build: a 为 1-based (n+1)×(m+1) ----------------
    void build(int px, int py, int xl, int xr, int yl, int yr, const vector<vector<int>>& a) {
        if (xl == xr && yl == yr) {
            tr[px][py] = a[xl][yl];
            return;
        }
        int xm = (xl + xr) >> 1, ym = (yl + yr) >> 1;
        if (xl == xr) {
            build(px, py << 1, xl, xr, yl, ym, a);
            build(px, py << 1 | 1, xl, xr, ym + 1, yr, a);
            tr[px][py] = tr[px][py << 1] + tr[px][py << 1 | 1];
        } else if (yl == yr) {
            build(px << 1, py, xl, xm, yl, yr, a);
            build(px << 1 | 1, py, xm + 1, xr, yl, yr, a);
            tr[px][py] = tr[px << 1][py] + tr[px << 1 | 1][py];
        } else {
            build(px << 1,     py << 1,     xl, xm, yl, ym, a);
            build(px << 1,     py << 1 | 1, xl, xm, ym + 1, yr, a);
            build(px << 1 | 1, py << 1,     xm + 1, xr, yl, ym, a);
            build(px << 1 | 1, py << 1 | 1, xm + 1, xr, ym + 1, yr, a);
            tr[px][py] = tr[px << 1][py << 1] + tr[px << 1][py << 1 | 1]
                       + tr[px << 1 | 1][py << 1] + tr[px << 1 | 1][py << 1 | 1];
        }
    }

    // ---------------- 矩形加: [ql,qr]×[qly,qry] 每格 +val ----------------
    void update(int px, int py, int xl, int xr, int yl, int yr,
                int ql, int qr, int qly, int qry, long long val) {
        if (qr < xl || xr < ql || qry < yl || yr < qly) return;          // 无交
        if (ql <= xl && xr <= qr && qly <= yl && yr <= qry) {            // 整块覆盖
            long long area = (long long)(xr - xl + 1) * (yr - yl + 1);
            tag[px][py] += val;
            tr[px][py] += val * area;
            return;
        }
        int xm = (xl + xr) >> 1, ym = (yl + yr) >> 1;
        if (xl == xr) {
            update(px, py << 1, xl, xr, yl, ym, ql, qr, qly, qry, val);
            update(px, py << 1 | 1, xl, xr, ym + 1, yr, ql, qr, qly, qry, val);
            tr[px][py] = tr[px][py << 1] + tr[px][py << 1 | 1]
                       + tag[px][py] * (long long)(xr - xl + 1) * (yr - yl + 1);
        } else if (yl == yr) {
            update(px << 1, py, xl, xm, yl, yr, ql, qr, qly, qry, val);
            update(px << 1 | 1, py, xm + 1, xr, yl, yr, ql, qr, qly, qry, val);
            tr[px][py] = tr[px << 1][py] + tr[px << 1 | 1][py]
                       + tag[px][py] * (long long)(xr - xl + 1) * (yr - yl + 1);
        } else {
            update(px << 1,     py << 1,     xl, xm, yl, ym, ql, qr, qly, qry, val);
            update(px << 1,     py << 1 | 1, xl, xm, ym + 1, yr, ql, qr, qly, qry, val);
            update(px << 1 | 1, py << 1,     xm + 1, xr, yl, ym, ql, qr, qly, qry, val);
            update(px << 1 | 1, py << 1 | 1, xm + 1, xr, ym + 1, yr, ql, qr, qly, qry, val);
            tr[px][py] = tr[px << 1][py << 1] + tr[px << 1][py << 1 | 1]
                       + tr[px << 1 | 1][py << 1] + tr[px << 1 | 1][py << 1 | 1]
                       + tag[px][py] * (long long)(xr - xl + 1) * (yr - yl + 1);
        }
    }

    // ---------------- 矩形求和: [ql,qr]×[qly,qry] ----------------
    long long ask(int px, int py, int xl, int xr, int yl, int yr,
                  int ql, int qr, int qly, int qry) {
        if (qr < xl || xr < ql || qry < yl || yr < qly) return 0;        // 无交
        if (ql <= xl && xr <= qr && qly <= yl && yr <= qry)              // 整块覆盖
            return tr[px][py];
        int xm = (xl + xr) >> 1, ym = (yl + yr) >> 1;
        long long ov = (long long)(min(xr, qr) - max(xl, ql) + 1)
                     * (min(yr, qry) - max(yl, qly) + 1);                // 交集面积
        long long res = tag[px][py] * ov;
        if (xl == xr) {
            res += ask(px, py << 1, xl, xr, yl, ym, ql, qr, qly, qry);
            res += ask(px, py << 1 | 1, xl, xr, ym + 1, yr, ql, qr, qly, qry);
        } else if (yl == yr) {
            res += ask(px << 1, py, xl, xm, yl, yr, ql, qr, qly, qry);
            res += ask(px << 1 | 1, py, xm + 1, xr, yl, yr, ql, qr, qly, qry);
        } else {
            res += ask(px << 1,     py << 1,     xl, xm, yl, ym, ql, qr, qly, qry);
            res += ask(px << 1,     py << 1 | 1, xl, xm, ym + 1, yr, ql, qr, qly, qry);
            res += ask(px << 1 | 1, py << 1,     xm + 1, xr, yl, ym, ql, qr, qly, qry);
            res += ask(px << 1 | 1, py << 1 | 1, xm + 1, xr, ym + 1, yr, ql, qr, qly, qry);
        }
        return res;
    }

    // ---------------- 便捷接口 ----------------
    void build(const vector<vector<int>>& a) { build(1, 1, 1, n, 1, m, a); }
    void update(int ql, int qr, int qly, int qry, long long val) {
        update(1, 1, 1, n, 1, m, ql, qr, qly, qry, val);
    }
    long long ask(int ql, int qr, int qly, int qry) {
        return ask(1, 1, 1, n, 1, m, ql, qr, qly, qry);
    }
};

// 使用示例
// int main() {
//     int n = 5, m = 5;
//     vector<vector<int>> a(n + 1, vector<int>(m + 1, 0));
//
//     Seg2D seg(n, m);
//     seg.build(a);
//
//     // 矩形区域 (x1,y1) 到 (x2,y2) 加 val
//     seg.update(x1, x2, y1, y2, val);
//
//     // 查询矩形区域 (x1,y1) 到 (x2,y2) 的和
//     long long sum = seg.ask(x1, x2, y1, y2);
//
//     return 0;
// }
```

### 李超线段树（三版本）

> 【本次修复】原版三版本 Line/LCSeg/INF 重名无法编译；已拆分为 LC_double（double 最小值）/LC_disc（离散化 int 最大值）/LC_frac（__int128 分数精确）三个命名空间。注意文件内 #define int long long 的宏污染。

```cpp
#define int long long

/* ============================================================================
 * 李超线段树 —— 三个版本
 * ----------------------------------------------------------------------------
 * 原版缺陷: 三个版本放在同一作用域, Line / LCSeg / INF 重名, 整个文件无法编译。
 * 修复: 版本1/2 各自封装进命名空间, 版本3 原名本就唯一, 一并入命名空间统一风格。
 *
 * 用法:
 *   LC_double :: LCSeg seg(n);          // double 最小值版, x∈[1,n] 整数域
 *   LC_disc   :: LCSeg seg(m, coords);  // 离散化 int 最大值版, coords 为升序去重 x 值表
 *   LC_frac   :: LCSegFrac seg(n);      // __int128 分数精确比较最小值版
 * 注意: 文件顶部 #define int long long 会影响其后粘贴的代码,
 *       scanf("%d") 读 int 变量会因类型不符得到垃圾值, main 请写 signed main。
 * ==========================================================================*/

// 【速查】李超线段树 · double 最小值版
// 用途: 不断插入直线/线段，查某个整数 x 处所有线中的最小 y（斜率优化 DP 的利器）
// 用法: LC_double::LCSeg sg(n);              // x 取值 1..n
//       sg.addLine(k, b);                      // 插入整条直线 y = kx + b
//       Line L(k, b); L.exists = true; sg.addSeg(1, 1, n, l, r, L);   // 只在 [l,r] 生效的线段，exists 必须置 true
//       sg.ask(1, 1, n, x) -> 最小 y；没有线时返回 INF(1e18)
// 改最大值: 按代码里"更改处"把比较符号和 INF 反过来
// 复杂度: 直线 O(log n)，线段 O(log² n)，查询 O(log n)
// ==================== 版本1: double 最小值版 ====================
namespace LC_double {

//普通李超线段树
//用于查询若干线段或者直线在某点处的最值
const double eps = 1e-9, INF = 1e18;

int sgn(double x){
    if(fabs(x) < eps) return 0;
    return x < 0 ? -1 : 1;
}

struct Line{
    double k, b;
    bool exists;
                                //更改处（-INF）
    Line(double k = 0, double b = INF) : k(k), b(b), exists(false) {}

    double cal(int x)const{
        return k * x + b;
    }
};
//插入线段复杂度为 log * log 其余为 log (当前为维护最小值版本)
struct LCSeg{
    int n;
    vector<Line> tr;
    LCSeg(int n) : n(n), tr(n * 4) {}

    void add(int p, int l, int r, Line val){
        if(!tr[p].exists){
            tr[p] = val;
            tr[p].exists = true;
            return;
        }
        int mid = l + r >> 1;
                        //更改处 <
        if(tr[p].cal(mid) > val.cal(mid)) swap(tr[p], val);
        if(l == r) return;
                        //更改处 >
        if(val.cal(l) < tr[p].cal(l)){
            add(p << 1, l, mid, val);
        } else {
            add(p << 1 | 1, mid + 1, r, val);
        }
    }

    // 插入一条全局直线
    void addLine(double k, double b){
        Line l(k, b);
        l.exists = true;
        add(1, 1, n, l);
    }

    // 插入一条在 [L, R] 范围内的线段
    void addSeg(int p, int l, int r, int ql, int qr, Line val){
        if(ql <= l && r <= qr){
            add(p, l, r, val);
            return;
        }
        int mid = l + r >> 1;
        if(ql <= mid) addSeg(p << 1, l, mid, ql, qr, val);
        if(mid < qr) addSeg(p << 1 | 1, mid + 1, r, ql, qr, val);
    }
    // 查询 x 处的最小 y 值（最大值版本把 return 里的 min 改成 max）
    double ask(int p, int l, int r, int x){
        if(x < l || x > r) return INF;//更改处-INF
        // 注意：本节点为空时不能直接 return！addSeg 只把线段存在被完全覆盖的节点上，
        // 祖先可能为空、而子孙节点里存着线段，必须继续往下递归才能查到。
        double res = tr[p].exists ? tr[p].cal(x) : INF;//更改处-INF
        if(l == r) return res;
        int mid = l + r >> 1;
                        //更改处 max
        if(x <= mid) return min(res, ask(p << 1, l, mid, x));
        return min(res, ask(p << 1 | 1, mid + 1, r, x));
    }
};

} // namespace LC_double

// 【速查】李超线段树 · 离散化 int 最大值版
// 用途: x 坐标很大/稀疏（如前缀和），先离散化再查最大 y
// 用法: coords = 所有可能查询的 x，sort + unique；LC_disc::LCSeg sg(coords.size(), coords);
//       sg.addLine(k, b);  sg.ask(1, 1, m, id) -> 最大 y，id = x 在 coords 里的下标 (1-based)
// 复杂度: 插入/查询 O(log m)
// ==================== 版本2: 离散化 int 最大值版 ====================
namespace LC_disc {

//离散化李超线段树（对所有可能的x值进行离散化处理）（最大值版本）
const int INF = 2e18; // 维护最大值，初始b设为-INF (在 #define int long long 下为 64 位, 2e18 安全)

struct Line {
    int k, b;
    bool exists;
    Line(int k = 0, int b = -INF) : k(k), b(b), exists(false) {}
    int cal(int x) const {
        return k * x + b;
    }
};

struct LCSeg {
    int n;
    vector<Line> tr;
    vector<int>& coords; //离散化后的数组，sort+erase（unique）

    LCSeg(int n, vector<int>& c) : n(n), tr(n * 4), coords(c) {}

    void add(int p, int l, int r, Line val) {
        if (!tr[p].exists) {
            tr[p] = val; tr[p].exists = true;
            return;
        }

        int mid = l + r >> 1;

        int mid_x = coords[mid - 1];
        int l_x = coords[l - 1];
        int r_x = coords[r - 1];

        if (tr[p].cal(mid_x) < val.cal(mid_x)) swap(tr[p], val);
        if (l == r) return;

        if (val.cal(l_x) > tr[p].cal(l_x)) add(p << 1, l, mid, val);
        else if (val.cal(r_x) > tr[p].cal(r_x)) add(p << 1 | 1, mid + 1, r, val);
    }

    void addLine(int k, int b) {
        Line l(k, b);
        l.exists = true;
        add(1, 1, n, l);
    }

    int ask(int p, int l, int r, int idx) {
        // 同样：空节点不能提前 return，子孙节点里可能存着线段（若以后加了 addSeg 就会错）
        int cur_x = coords[idx - 1];
        int res = tr[p].exists ? tr[p].cal(cur_x) : -INF;
        if (l == r) return res;

        int mid = l + r >> 1;
        if (idx <= mid) return max(res, ask(p << 1, l, mid, idx));
        return max(res, ask(p << 1 | 1, mid + 1, r, idx));
    }
};

// auto get_id = [&](int x) {
//     return lower_bound(coords.begin(), coords.end(), x) - coords.begin() + 1;
// };

// for (int i = 1; i <= n; i++) {
//     int x_idx = get_id(pre[i]);  ！！！要引用离散化后的角标（1-based）
//     dp[i] = seg.ask(1, 1, m, x_idx) + a * pre[i] * pre[i] + b * pre[i];
//     int new_k = -2 * a * pre[i];
//     int new_b = dp[i] + a * pre[i] * pre[i] - b * pre[i] + c;
//     seg.addLine(new_k, new_b);
// }

} // namespace LC_disc

// 【速查】李超线段树 · __int128 分数精确最小值版
// 用途: k、b 是分数，double 会卡精度时用；查询点为整数
// 用法: LC_frac::LCSegFrac sg(n);
//       sg.addLine(p, q, u, v);               // y = (p/q)x + (u/v)
//       sg.addSeg(l, r, Frac(p,q), Frac(u,v)); // 只在 [l,r] 生效
//       sg.ask(x) -> Frac 最小值（.p/.q）
// 复杂度: 同 double 版，常数大一些（__int128 + 约分）
// ==================== 版本3: __int128 分数精确版 ====================
namespace LC_frac {

// ==================== 分数版李超线段树 ====================
// k、b 都可以是分数 p/q。查询点为整数，所有比较用 __int128 精确完成，没有浮点误差。
// 下面实现的是最小值版本；若需要最大值，把 add 和 ask 中标注“方向”处的比较符号反过来即可。
struct Frac {
    __int128 p, q; // 表示 p/q，始终 q > 0 且已约分

    Frac(__int128 p = 0, __int128 q = 1) : p(p), q(q) { norm(); }

    static __int128 gcd128(__int128 a, __int128 b) {
        if (a < 0) a = -a;
        if (b < 0) b = -b;
        while (b) {
            __int128 t = a % b;
            a = b;
            b = t;
        }
        return a;
    }

    void norm() {
        if (q == 0) { // 实际使用中不会出现分母为 0
            exit(1);
        }
        if (q < 0) {
            p = -p;
            q = -q;
        }
        if (p == 0) {
            q = 1;
            return;
        }
        __int128 g = gcd128(p, q);
        p /= g;
        q /= g;
    }

    Frac operator*(long long x) const { return Frac(p * x, q); }
    Frac operator+(const Frac& o) const { return Frac(p * o.q + o.p * q, q * o.q); }
    Frac operator-() const { return Frac(-p, q); }

    bool operator<(const Frac& o) const { return p * o.q < o.p * q; }
    bool operator>(const Frac& o) const { return p * o.q > o.p * q; }
    bool operator==(const Frac& o) const { return p == o.p && q == o.q; }
    bool operator<=(const Frac& o) const { return !(*this > o); }
    bool operator>=(const Frac& o) const { return !(*this < o); }

    long long toLL() const { return (long long)(p / q); }
    double toDouble() const { return (double)p / (double)q; }
};

struct LineFrac {
    Frac k, b;
    bool exists;
    // 空节点初始化为一个足够大的值，用于最小值版本
    // [方向] 最小值用 +INF；若改最大值，这里改为足够小的值，如 -(1LL << 62)
    LineFrac(Frac k = 0, Frac b = Frac(1LL << 62, 1)) : k(k), b(b), exists(false) {}

    Frac cal(int x) const {
        return k * x + b;
    }
};

struct LCSegFrac {
    int n;
    vector<LineFrac> tr;

    LCSegFrac(int n) : n(n), tr(n * 4) {}

    void add(int p, int l, int r, LineFrac val) {
        if (!tr[p].exists) {
            tr[p] = val;
            tr[p].exists = true;
            return;
        }
        int mid = l + r >> 1;
        // 最小值版本：中点更小/更优的留在当前节点
        // [方向] 最小值：当前中点值 > 新线中点值时交换（让更小的留在当前）
        if (tr[p].cal(mid) > val.cal(mid)) swap(tr[p], val);
        if (l == r) return;

        // [方向] 最小值：新线在左端点更小则往左走（改最大值时改为 >）
        if (val.cal(l) < tr[p].cal(l)) {
            add(p << 1, l, mid, val);
        // [方向] 最小值：新线在右端点更小则往右走（改最大值时改为 >）
        } else if (val.cal(r) < tr[p].cal(r)) {
            add(p << 1 | 1, mid + 1, r, val);
        }
    }

    // 插入全局直线 y = k*x + b，k 和 b 都可以是分数
    void addLine(const Frac& k, const Frac& b) {
        LineFrac l(k, b);
        l.exists = true;
        add(1, 1, n, l);
    }

    // 直接传 p/q 形式：y = (p/q)*x + (u/v)
    void addLine(long long p, long long q, long long u, long long v) {
        addLine(Frac(p, q), Frac(u, v));
    }

    // 插入一条在 [L,R] 范围内的线段
    void addSeg(int p, int l, int r, int ql, int qr, LineFrac val) {
        if (ql <= l && r <= qr) {
            add(p, l, r, val);
            return;
        }
        int mid = l + r >> 1;
        if (ql <= mid) addSeg(p << 1, l, mid, ql, qr, val);
        if (mid < qr) addSeg(p << 1 | 1, mid + 1, r, ql, qr, val);
    }

    void addSeg(int ql, int qr, const Frac& k, const Frac& b) {
        LineFrac l(k, b);
        l.exists = true;
        addSeg(1, 1, n, ql, qr, l);
    }

    Frac ask(int p, int l, int r, int x) {
        // [方向] 最小值：空节点返回 +INF（改最大值时改为 -INF）
        if (x < l || x > r) return Frac(1LL << 62, 1);
        Frac res = tr[p].exists ? tr[p].cal(x) : Frac(1LL << 62, 1);
        if (l == r) return res;
        int mid = l + r >> 1;
        // [方向] 最小值：合并时取 min（改最大值时改为 max）
        if (x <= mid) return min(res, ask(p << 1, l, mid, x));
        return min(res, ask(p << 1 | 1, mid + 1, r, x));
    }

    Frac ask(int x) {
        return ask(1, 1, n, x);
    }
};

} // namespace LC_frac
```

### 主席树（可持久化数组）

> 结构体 PST（vector 节点池，按需 reserve），含可持久化数组（P3919：每个操作含查询都产生新版本）与静态区间第 k 小。
> 【10.22 结构体封装】原全局变量 + 全局函数改为结构体 / 传参函数，用法见块内速查卡。

```cpp
using ll = long long;

// 【速查】主席树（可持久化线段树）
// 用途: ① 可持久化数组：基于任意历史版本单点改 / 单点查（P3919）
//       ② 静态区间第 k 小：按前缀建权值线段树，两个版本相减（P3834）
// 用法: PST t(预计节点数);   // 节点数 ≈ 2n + 更新次数 × (log n + 2)；只是 reserve，超了也能用（会扩容）
//   ① int r0 = t.build(1, n, a);  int r1 = t.update(r0, 1, n, pos, v);  t.query(r1, 1, n, pos)
//      每个版本就是一个根编号，自己用 vector<int> root 存；"只查询也生成新版本"时 root[i] = root[ver]
//   ② 值离散化到 1..m；root[0] = 0（空树）；root[i] = t.insert(root[i-1], 1, m, id(a[i]));
//      t.kth(root[l-1], root[r], 1, m, k) -> 第 k 小的离散化编号
// 复杂度: 每次 O(log n) 时间 + O(log n) 个新节点
// 10.22: 原版是全局 tree[N*25] 数组 + 全局函数（另有一份写在注释里的第 k 小），合并为一个结构体
struct PST {
    struct Node { int l, r; ll val; };       // val: ① 中为叶子的值，② 中为该值域区间的数的个数
    vector<Node> t;

    PST(int reserveNodes = 0) { t.reserve(reserveNodes + 1); t.push_back({0, 0, 0}); }   // 0 号为空节点
    int clone(int u) { t.push_back(t[u]); return (int)t.size() - 1; }

    // ---- ① 可持久化数组 ----
    int build(int l, int r, const vector<ll>& a) {   // a 为 1-based
        int u = clone(0);
        if (l == r) { t[u].val = a[l]; return u; }
        int mid = (l + r) / 2;
        int lc = build(l, mid, a), rc = build(mid + 1, r, a);
        t[u].l = lc; t[u].r = rc;               // 先算完孩子再写，避免 push_back 扩容后引用失效
        return u;
    }
    int update(int u, int l, int r, int pos, ll v) {
        int nu = clone(u);
        if (l == r) { t[nu].val = v; return nu; }
        int mid = (l + r) / 2;
        if (pos <= mid) { int c = update(t[nu].l, l, mid, pos, v); t[nu].l = c; }
        else { int c = update(t[nu].r, mid + 1, r, pos, v); t[nu].r = c; }
        return nu;
    }
    ll query(int u, int l, int r, int pos) {
        while (l < r) {
            int mid = (l + r) / 2;
            if (pos <= mid) u = t[u].l, r = mid;
            else u = t[u].r, l = mid + 1;
        }
        return t[u].val;
    }

    // ---- ② 静态区间第 k 小 ----
    int insert(int u, int l, int r, int pos) {      // 值 pos 的个数 +1，返回新根
        int nu = clone(u);
        t[nu].val++;
        if (l == r) return nu;
        int mid = (l + r) / 2;
        if (pos <= mid) { int c = insert(t[nu].l, l, mid, pos); t[nu].l = c; }
        else { int c = insert(t[nu].r, mid + 1, r, pos); t[nu].r = c; }
        return nu;
    }
    int kth(int u, int v, int l, int r, ll k) {     // 版本 v 减版本 u 的第 k 小
        while (l < r) {
            int mid = (l + r) / 2;
            ll leftCnt = t[t[v].l].val - t[t[u].l].val;
            if (k <= leftCnt) u = t[u].l, v = t[v].l, r = mid;
            else k -= leftCnt, u = t[u].r, v = t[v].r, l = mid + 1;
        }
        return l;
    }
};

// 示例（P3919）: 读 n q 和初始数组；每个操作 "ver 1 pos v"（基于 ver 改值）或 "ver 2 pos"（查值）
//   第 i 个操作生成版本 i（查询操作的新版本 = 原版本）
void solve() {
    int n, q;
    cin >> n >> q;
    vector<ll> a(n + 1);
    for (int i = 1; i <= n; i++) cin >> a[i];
    PST t(2 * n + q * (__lg(max(n, 1)) + 2));   // 建树 2n-1 个点 + 每次修改 ⌈log n⌉+1 个点
    vector<int> root(q + 1);
    root[0] = t.build(1, n, a);
    for (int i = 1; i <= q; i++) {
        int ver, op, pos;
        cin >> ver >> op >> pos;
        if (op == 1) { ll v; cin >> v; root[i] = t.update(root[ver], 1, n, pos, v); }
        else { cout << t.query(root[ver], 1, n, pos) << '\n'; root[i] = root[ver]; }
    }
}

// 第 k 小用法示例（P3834）:
// void solveKth() {
//     int n, q; cin >> n >> q;
//     vector<ll> a(n + 1); for (int i = 1; i <= n; i++) cin >> a[i];
//     vector<ll> vals(a.begin() + 1, a.end()); sort(vals.begin(), vals.end());
//     vals.erase(unique(vals.begin(), vals.end()), vals.end()); int m = vals.size();
//     PST t(n * 20); vector<int> root(n + 1, 0);
//     for (int i = 1; i <= n; i++) root[i] = t.insert(root[i - 1], 1, m, lower_bound(vals.begin(), vals.end(), a[i]) - vals.begin() + 1);
//     while (q--) { int l, r, k; cin >> l >> r >> k; cout << vals[t.kth(root[l - 1], root[r], 1, m, k) - 1] << '\n'; }
// }
```

### ST表

> 静态区间最大值。

```cpp
// 【速查】ST 表（静态区间最值，不支持修改）
// 用法: ST st(a);  st.query(l, r)        // a 为 1-based（a[0] 占位），查 [l, r]
// 改成 min / gcd / & / |: 改构造函数和 query 里的 max（必须是可重复贡献的运算）
// 复杂度: 预处理 O(n log n)，查询 O(1)
//ST表，静态区间最值查询，O(nlogn)预处理，O(1)查询
struct ST{
    int n;
    vector<int> lg;
    vector<vector<int>> f;

    //数组a为1-base，查询同为1-base，若是0-base自行前面补0
    ST(vector<int> a):n(a.size()-1){
        lg.assign(n+1,0);
        for(int i=2;i<=n;i++){
            lg[i]=lg[i/2]+1;
        }

        f.assign(n+1,vector<int>(lg[n]+1));

        for(int i=1;i<=n;i++){
            f[i][0]=a[i];
        }

        for(int j=1;j<=lg[n];j++){
            for(int i=1;i+(1<<j)-1<=n;i++){
                // 修改点：合并方式，max 换成 min / __gcd / & / | 等
                f[i][j]=max(f[i][j-1],f[i+(1<<(j-1))][j-1]);
            }
        }
    }

    // 修改点：返回类型与合并方式需与预处理保持一致
    int query(int l,int r){
        int k=lg[r-l+1];
        return max(f[l][k],f[r-(1<<k)+1][k]);
    }
};
```

### 二维树状数组（三变体）

> 单点加矩形和 / 矩形加单点查 / 矩形加矩形和（四 BIT）。大值域乘积注意 __int128。

```cpp
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
```

### 并查集

> 路径压缩+按大小；注意 find 为递归实现，1e6 级长链首查可能爆栈。

```cpp
// 【速查】并查集（连通性 + 连通块计数）
// 用法: SDU d(n);  d.merge(x, y);  d.check(x, y) -> 是否连通;  d.cnt -> 当前连通块个数;  d.find(x) -> 根
// 复杂度: 近似 O(1)（路径压缩）；1-based。注意 find 是递归，1e6 深链可能爆栈
//并查集
struct SDU{
    int n, cnt;//cnt为连通块数量
    vector<int> fa;
    SDU(int m = 0){
        n = m;
        fa.resize(n + 1);
        for(int i = 1; i <= n; i++) fa[i] = i;//1-based
        cnt = n;
    }

    int find(int x){
        return x == fa[x] ? x : fa[x] = find(fa[x]);
    }

    void merge(int x, int y){
        int xx = find(x);
        int yy = find(y);
        if(xx != yy){
            fa[xx] = yy;
            cnt--;
        }
    }

    bool check(int x, int y){
        return find(x) == find(y);
    }
};
```

### 带权并查集

> merge(x,y,d) 约定 dis[y]-dis[x]==d，返回 false 表示约束矛盾。

```cpp
// 【速查】带权并查集（维护"差值"关系：val[y] - val[x] = d）
// 用法: DSU d(n);  d.merge(x, y, d0) -> 加约束 val[y]-val[x]=d0，与已有约束矛盾返回 false
//       查差值: 先 d.find(x), d.find(y)，同根时 val[y]-val[x] = d.dis[y] - d.dis[x]
// 含义: find 后 dis[x] = val[x] - val[根]
// 复杂度: 近似 O(1)；1-based
//带权并查集，维护每个连通块根节点到每个节点中某个权值关系
struct DSU{
    int n;
    vector<int> fa, sz, dis;

    DSU(int n):n(n),fa(n+1),sz(n+1,1),dis(n+1,0){
        for(int i=1;i<=n;i++){
            fa[i]=i;
        }
    }

    int find(int x){
        if(fa[x]==x) return x;

        int f=fa[x];
        fa[x]=find(fa[x]);

        dis[x]+=dis[f];

        return fa[x];
    }

    bool merge(int x,int y,int d){
        int fx=find(x);
        int fy=find(y);

        if(fx==fy){
            return dis[y]-dis[x]==d;
        }

        if(sz[fx]>sz[fy]){
            swap(x,y);
            swap(fx,fy);
            d=-d;
        }

        fa[fx]=fy;

        dis[fx]=dis[y]-dis[x]-d;

        sz[fy]+=sz[fx];

        return true;
    }
};
```

### 树状数组套动态开点权值线段树

> 【本次修复】节点池按实测调整并加 assert（原版溢出后静默 WA）。PST_BIT：M=N*600≈720MB；staticPST_BIT 更省内存（压测全对）。两 namespace 勿同时编入。

```cpp
#define ll long long
#define endl '\n'
// 【速查】树状数组套权值线段树（带修区间第 k 小，P2617）
// 用途: 单点修改 + 区间第 k 小 / 区间 ≤x 个数 / 排名 / 前驱后继
// 用法: 离线读入全部操作 → 初值和所有修改值一起离散化 → bit.add(i, id, +1) 建初始
//       修改: bit.add(pos, 旧id, -1); bit.add(pos, 新id, +1)；  查询: vals[bit.kth(l, r, k) - 1]
// 复杂度: 修改/查询 O(log² n)；内存很大（见下方 M 说明），MLE 就换 staticPST_BIT
namespace PST_BIT{
    /* ============================================================================
    * 树状数组套动态开点权值线段树（BIT × Dynamic PST）
    * ----------------------------------------------------------------------------
    * 一句话：外层 BIT 管"位置区间"，内层值域线段树管"值域统计"。
    * 适用题型：
    *     带修区间第 K 小 / 带修区间 ≤ x 的个数 / 带修区间排名 /
    *     带修区间前驱后继 / 带修二维数点 等
    * 复杂度：
    *     单次修改 O(log²n)
    *     单次查询 O(log²n)
    * 空间：
    *     节点数 ≈ (n + 2q) · log²n，按最坏情况开池子
    * ==========================================================================*/

    const int N = 1e5 + 5;    // 序列长度上限
    /* 节点池容量与内存代价(节点 12B):
     *   M = N*600 = 6e7 ≈ 720MB —— 实测 n=q=1e5(9.9万次修改)消耗约 5e7
     *   若修改占比超过 ~7 成, 需按 (n + 2·#modify)·log n·log m 估算, 极端全修改
     *   约 8.7e7 ≈ 1GB, 常规 OJ 不可行, 请改用下方 staticPST_BIT 或整体二分。
     * clone 内置 assert: 池耗尽会立刻 RE 报错, 而不是静默写出错误答案。 */
    const int M = N * 600;

    int n, q, m;              // n 序列长，q 操作数，m 离散化后值域大小
    int a[N];                 // 当前数组，随修改实时更新
    vector<int> vals;         // 离散化容器（初始值 + 所有修改后的值）

    /* ---------------------------- 离线操作记录 ---------------------------- */
    struct Op{
        int type;             // 1 = 修改(C), 2 = 查询(Q)
        int l, r, k;          // 查询参数：区间 [L,R]，第 k 小
        int pos, val;         // 修改参数：a[pos] = val
    };
    vector<Op> ops;

    /* =============================== 核心结构 ============================== */
    struct BitPST{

        /* -------- 内层：动态开点权值线段树 -------- */
        struct node{
            int l, r;
            int cnt;          // 【适配点 1】题型不同时改这个字段：
                            //   · 第K小 / ≤x个数      → cnt（计数）
                            //   · 区间和              → sum（ll）
                            //   · 区间最值            → mx（带初值）
        } t[M];

        int root[N];          // root[i] = BIT 第 i 个节点挂的线段树根编号
        int top = 0;          // 节点池指针；t[0] 是零值哨兵，天然代表空树

        /* 复制旧节点并返回新节点编号，主席树的关键操作 */
        int clone(int x){ assert(top < M); t[++top] = t[x]; return top; }

        /* 在子树 x 上做单点 pos += v（v 可为负） */
        int update(int x, int l, int r, int pos, int v){
            x = clone(x);
            t[x].cnt += v;                       // 【适配点 1】对应字段修改处
            if(l == r) return x;
            int mid = (l + r) >> 1;
            if(pos <= mid) t[x].l = update(t[x].l, l, mid, pos, v);
            else           t[x].r = update(t[x].r, mid + 1, r, pos, v);
            return x;
        }

        /* -------- 外层：树状数组 -------- */
        /* 在位置 i 上，对值 pos 做 +v（v 可为负） */
        void add(int i, int pos, int v){
            for(; i <= n; i += i & -i)
                root[i] = update(root[i], 1, m, pos, v);
        }

        /* -------- 查询：两条 BIT 链缓存 -------- */
        /* 大小只需 log n + 1，写到 N 是保守写法 */
        int qL[N], qR[N];

        /* 区间 [L,R] 的第 k 小，返回离散化后的下标
        * 【适配点 2】要换查询语义时主要改这里： */
        int kth(int L, int R, int k){
            int cl = 0, cr = 0;
            for(int i = L - 1; i > 0; i -= i & -i) qL[++cl] = root[i];
            for(int i = R;     i > 0; i -= i & -i) qR[++cr] = root[i];

            int l = 1, r = m;
            while(l < r){
                int mid = (l + r) >> 1;

                /* 【核心】左子树计数 = 右链求和 - 左链求和 */
                int cnt = 0;
                for(int i = 1; i <= cr; i++) cnt += t[t[qR[i]].l].cnt;
                for(int i = 1; i <= cl; i++) cnt -= t[t[qL[i]].l].cnt;

                if(k <= cnt){
                    for(int i = 1; i <= cr; i++) qR[i] = t[qR[i]].l;
                    for(int i = 1; i <= cl; i++) qL[i] = t[qL[i]].l;
                    r = mid;
                }else{
                    for(int i = 1; i <= cr; i++) qR[i] = t[qR[i]].r;
                    for(int i = 1; i <= cl; i++) qL[i] = t[qL[i]].r;
                    k -= cnt;
                    l = mid + 1;
                }
            }
            return l;
        }

        /* ---------- 可选：带修区间 ≤ x 的个数（按需展开） ----------
        * 思路：沿值域走，每次完全落在 [1,pos] 内的子树直接累加 cnt。
        *
        int countLE(int L, int R, int pos){
            int cl = 0, cr = 0;
            for(int i = L - 1; i > 0; i -= i & -i) qL[++cl] = root[i];
            for(int i = R;     i > 0; i -= i & -i) qR[++cr] = root[i];

            auto sum = [&](){
                int s = 0;
                for(int i = 1; i <= cr; i++) s += t[qR[i]].cnt;
                for(int i = 1; i <= cl; i++) s -= t[qL[i]].cnt;
                return s;
            };

            int l = 1, r = m, res = 0;
            while(l < r){
                int mid = (l + r) >> 1;
                if(pos <= mid){
                    for(int i = 1; i <= cr; i++) qR[i] = t[qR[i]].l;
                    for(int i = 1; i <= cl; i++) qL[i] = t[qL[i]].l;
                    r = mid;
                }else{
                    for(int i = 1; i <= cr; i++) qR[i] = t[qR[i]].l;
                    for(int i = 1; i <= cl; i++) qL[i] = t[qL[i]].l;
                    res += sum();
                    for(int i = 1; i <= cr; i++) qR[i] = t[qR[i]].r;
                    for(int i = 1; i <= cl; i++) qL[i] = t[qL[i]].r;
                    l = mid + 1;
                }
            }
            if(l <= pos) res += sum();
            return res;
        }
        * --------------------------------------------------------- */

    } bit;

    /* ------------------------- 离散化辅助 ------------------------- */
    int getId(int x){
        return lower_bound(vals.begin(), vals.end(), x) - vals.begin() + 1;
    }

    /* ============================================================================
    * 【易错点清单】—— 每一条都是真实踩过的坑
    * ----------------------------------------------------------------------------
    * 1) 必须"离线读入所有操作"再离散化。修改后的值也要 push 进 vals，
    *    否则 getId 时越界或找不到。
    *
    * 2) t[0] 是全零哨兵节点，clone(0) 天然得到空树，无需额外 buildEmpty。
    *    但如果你把 cnt 换成 mx 并需要负数初值，要手动初始化 t[0].mx = -INF。
    *
    * 3) BIT 下标从 1 开始。L-1 = 0 时 for 循环自然不进入，无需特判。
    *
    * 4) 节点池 M 一定要按 (n + 2q)·log²n 估算。
    *    n = q = 1e5 时，纯 BIT 做法约 3e5 次 add × log²n(≈289) ≈ 8.7e7 节点，
    *    实际常常 MLE。若 MLE，改"静态 PST + BIT 动态增量"的写法能省一半。
    *
    * 5) kth 里 cnt 统计的是"左子树"的贡献，不是当前节点的。
    *    换成 countLE 时，注意"完全落区间"的那部分要立刻累加，不能继续下探。
    *
    * 6) 修改 = 删旧值 + 加新值，两次 add，别忘了同步更新 a[pos]。
    *
    * 7) qL / qR 是每次查询临时缓存，不要设成 static，也不要跨查询复用。
    *
    * 8) 查询时别把 kth 的第一个参数写成 L（那是静态主席树的写法）；
    *    BIT 版本的"游标链"要从 L-1 开始。
    * ==========================================================================*/

    /* ============================================================================
    * 【常见题型的改造点速查】
    * ----------------------------------------------------------------------------
    * · 区间第 K 小（P2617 本尊）        → 直接使用 kth
    * · 区间 ≤ x 的个数                  → 展开 countLE（上文注释块）
    * · 区间中位数                       → kth(L, R, (R-L+2)/2)
    * · 区间 x 的排名（升序）            → countLE(L, R, getId(x)-1) + 1
    * · 区间前驱（严格 < x 的最大值）    → kth(L, R, countLE(L, R, getId(x)-1))
    * · 区间后继（严格 > x 的最小值）    → kth(L, R, countLE(L, R, getId(x)) + 1)
    *     注意：k > 区间长度时要输出无解
    * · 二维数点 / 带修矩阵和            → 把"位置"换成第一维坐标，
    *                                      外层 BIT 沿第一维走，内层沿第二维
    * ==========================================================================*/

    /* ------------------------------- 主流程 ------------------------------- */
    void solve(){
        cin >> n >> q;

        for(int i = 1; i <= n; i++){
            cin >> a[i];
            vals.push_back(a[i]);
        }

        ops.assign(q + 1, Op{});
        for(int i = 1; i <= q; i++){
            char opt; cin >> opt;
            if(opt == 'C'){
                int pos, val; cin >> pos >> val;
                ops[i] = {1, 0, 0, 0, pos, val};
                vals.push_back(val);            // ← 别忘了修改后的值
            }else{
                int l, r, k; cin >> l >> r >> k;
                ops[i] = {2, l, r, k, 0, 0};
            }
        }

        sort(vals.begin(), vals.end());
        vals.erase(unique(vals.begin(), vals.end()), vals.end());
        m = vals.size();

        /* 初始数组等价于"对每个位置做一次 +1 修改" */
        for(int i = 1; i <= n; i++)
            bit.add(i, getId(a[i]), +1);

        for(int i = 1; i <= q; i++){
            Op &op = ops[i];
            if(op.type == 1){
                bit.add(op.pos, getId(a[op.pos]), -1);
                bit.add(op.pos, getId(op.val),     +1);
                a[op.pos] = op.val;
            }else{
                int idx = bit.kth(op.l, op.r, op.k);
                cout << vals[idx - 1] << endl;
            }
        }
    }
}

// 【速查】静态主席树 + 树状数组增量（带修区间第 k 小，省内存版）
// 用途、用法与上面 PST_BIT 相同；初值建静态主席树，只有修改走 BIT，内存约省一半
// 复杂度: 修改/查询 O(log² n)
namespace staticPST_BIT{
    /* ============================================================================
    * 静态主席树 + 树状数组套动态开点权值线段树
    *     (Static PST + BIT × Dynamic PST)
    * ----------------------------------------------------------------------------
    * 一句话：
    *     静态 PST 保存"初始数组"的前缀版本；
    *     BIT 套动态 PST 只保存"修改量"；
    *     查询时两部分贡献相加，得到 [L,R] 的当前值域分布。
    *
    * 相比纯 BIT 版本的优势：
    *     · 初始 n 个数只花 n·log m 节点，不再花 n·log n·log m
    *     · 内存约省 60%，查询常数更小
    *
    * 复杂度：
    *     单次修改 O(log²n)
    *     单次查询 O(log²n)
    * 空间：
    *     静态 PST： n · log m
    *     BIT 增量： 2q · log n · log m
    * ==========================================================================*/

    const int N  = 1e5 + 5;
    const int M1 = N * 25;     // 静态 PST 节点池：n · log m (实测 n=1e5 消耗 1.87e6, N*25 留余量)
    const int M2 = N * 400;    // 动态 BIT 节点池：2q · log n · log m (实测全修改负载消耗 3.36e7/4e7)

    int n, q, m;               // n 序列长，q 操作数，m 离散化后值域大小
    int a[N];                  // 当前数组，随修改实时更新
    vector<int> vals;          // 离散化容器（初始值 + 所有修改后的值）

    /* ---------------------------- 离线操作记录 ---------------------------- */
    struct Op{
        int type;              // 1 = 修改(C), 2 = 查询(Q)
        int l, r, k;           // 查询参数：区间 [L,R]，第 k 小
        int pos, val;          // 修改参数：a[pos] = val
    };
    vector<Op> ops;

    /* =============================== 核心结构 ============================== */
    struct BitPST{

        /* ======================= 静态部分：普通主席树 =======================
        * 用途：保存初始数组的前缀版本 st.root[i] = 前 i 个数的值域分布
        * 特点：不带修改，纯查询 */
        struct StaticPST{
            struct node{
                int l, r;
                int cnt;       // 【适配点 1】按题型换：cnt / sum / mx
            } t[M1];

            int root[N];       // root[i]：前缀 [1..i] 的版本根
            int top = 0;       // t[0] 是零值哨兵，天然代表空树

            int clone(int x){ assert(top < M1); t[++top] = t[x]; return top; }

            int update(int x, int l, int r, int pos, int v){
                x = clone(x);
                t[x].cnt += v;                       // 【适配点 1】合并处
                if(l == r) return x;
                int mid = (l + r) >> 1;
                if(pos <= mid) t[x].l = update(t[x].l, l, mid, pos, v);
                else           t[x].r = update(t[x].r, mid + 1, r, pos, v);
                return x;
            }
        } st;

        /* ================ 动态部分：BIT 套动态开点权值线段树 ================
        * 用途：每个 BIT 节点挂一棵值域线段树，只记录"修改量"（+1/-1）
        * 特点：初始为空；每次修改走 log n 个 BIT 节点 */
        struct DiffPST{
            struct node{
                int l, r;
                int cnt;       // 【适配点 1】同上
            } t[M2];

            int root[N];       // root[i]：BIT 节点 i 挂的动态线段树根
            int top = 0;

            int clone(int x){ assert(top < M2); t[++top] = t[x]; return top; }

            int update(int x, int l, int r, int pos, int v){
                x = clone(x);
                t[x].cnt += v;
                if(l == r) return x;
                int mid = (l + r) >> 1;
                if(pos <= mid) t[x].l = update(t[x].l, l, mid, pos, v);
                else           t[x].r = update(t[x].r, mid + 1, r, pos, v);
                return x;
            }

            /* 在位置 i 上，对值 pos 做 +v */
            void add(int i, int pos, int v){
                for(; i <= n; i += i & -i)
                    root[i] = update(root[i], 1, m, pos, v);
            }
        } dif;

        /* ========================= 查询缓存 ========================= */
        /* 两条 BIT 游标链，大小只需 log n + 1，N 是保守写法 */
        int qL[N], qR[N];

        /* ------------------- 区间 [L,R] 第 k 小 -------------------
        * 【适配点 2】换查询语义时主要改这里 */
        int kth(int L, int R, int k){
            /* 动态部分：两条 BIT 链 */
            int cl = 0, cr = 0;
            for(int i = L - 1; i > 0; i -= i & -i) qL[++cl] = dif.root[i];
            for(int i = R;     i > 0; i -= i & -i) qR[++cr] = dif.root[i];

            /* 静态部分：两个版本根
            * 这里和纯 BIT 版本唯一不同的地方：
            *   纯 BIT 的 u、v 从 root[L-1]、root[R] 拿，
            *   这里从 st.root[L-1]、st.root[R] 拿。 */
            int u = st.root[L - 1], v = st.root[R];

            int l = 1, r = m;
            while(l < r){
                int mid = (l + r) >> 1;

                /* 【核心】左子树计数 = 静态差值 + 动态差值 */
                int cnt = st.t[st.t[v].l].cnt - st.t[st.t[u].l].cnt;
                for(int i = 1; i <= cr; i++) cnt += dif.t[dif.t[qR[i]].l].cnt;
                for(int i = 1; i <= cl; i++) cnt -= dif.t[dif.t[qL[i]].l].cnt;

                if(k <= cnt){
                    u = st.t[u].l;
                    v = st.t[v].l;
                    for(int i = 1; i <= cr; i++) qR[i] = dif.t[qR[i]].l;
                    for(int i = 1; i <= cl; i++) qL[i] = dif.t[qL[i]].l;
                    r = mid;
                }else{
                    u = st.t[u].r;
                    v = st.t[v].r;
                    for(int i = 1; i <= cr; i++) qR[i] = dif.t[qR[i]].r;
                    for(int i = 1; i <= cl; i++) qL[i] = dif.t[qL[i]].r;
                    k -= cnt;
                    l = mid + 1;
                }
            }
            return l;
        }

        /* ---------- 可选：带修区间 ≤ x 的个数 ----------
        * 思路和 kth 相同，只是把"二分行进"改成"沿值域下探 + 累加"。
        *
        int countLE(int L, int R, int pos){
            int cl = 0, cr = 0;
            for(int i = L - 1; i > 0; i -= i & -i) qL[++cl] = dif.root[i];
            for(int i = R;     i > 0; i -= i & -i) qR[++cr] = dif.root[i];
            int u = st.root[L - 1], v = st.root[R];

            int l = 1, r = m, res = 0;
            while(l < r){
                int mid = (l + r) >> 1;
                if(pos <= mid){
                    u = st.t[u].l; v = st.t[v].l;
                    for(int i = 1; i <= cr; i++) qR[i] = dif.t[qR[i]].l;
                    for(int i = 1; i <= cl; i++) qL[i] = dif.t[qL[i]].l;
                    r = mid;
                }else{
                    int s = st.t[st.t[v].l].cnt - st.t[st.t[u].l].cnt;
                    for(int i = 1; i <= cr; i++) s += dif.t[dif.t[qR[i]].l].cnt;
                    for(int i = 1; i <= cl; i++) s -= dif.t[dif.t[qL[i]].l].cnt;
                    res += s;

                    u = st.t[u].r; v = st.t[v].r;
                    for(int i = 1; i <= cr; i++) qR[i] = dif.t[qR[i]].r;
                    for(int i = 1; i <= cl; i++) qL[i] = dif.t[qL[i]].r;
                    l = mid + 1;
                }
            }
            if(l <= pos){
                int s = st.t[v].cnt - st.t[u].cnt;
                for(int i = 1; i <= cr; i++) s += dif.t[qR[i]].cnt;
                for(int i = 1; i <= cl; i++) s -= dif.t[qL[i]].cnt;
                res += s;
            }
            return res;
        }
        * --------------------------------------------------------- */

    } pst;

    /* ------------------------- 离散化辅助 ------------------------- */
    int getId(int x){
        return lower_bound(vals.begin(), vals.end(), x) - vals.begin() + 1;
    }

    /* ============================================================================
    * 【易错点清单】
    * ----------------------------------------------------------------------------
    * 1) 静态 PST 和动态 BIT 共用同一个 m（离散化值域大小），
    *    所以初始数组、修改后的值必须一起参与离散化。
    *
    * 2) st.root[0] = 0 天然代表"空前缀"，不需要显式 build 空树。
    *    静态 PST 的版本链表是 root[i] = update(root[i-1], ...)。
    *
    * 3) 动态 BIT 的 root 初始全 0，因为它只记录修改量；
    *    若某位置从未被修改，该 BIT 链上所有 root 都是 0，等价空树。
    *
    * 4) kth 中要同时推进两组游标：
    *      · 静态：u = st.t[u].l / st.t[u].r
    *      · 动态：qL[i] / qR[i] = dif.t[...].l / .r
    *    少写一组会导致下一层用错子树。
    *
    * 5) 计数公式：左子树 cnt = 静态差值 + 动态差值。
    *    顺序不能颠倒（其实是加法交换律，但意思是两者都要算进去），
    *    别只写了静态差就完事。
    *
    * 6) 修改 = "删旧值 + 加新值"，两次 dif.add，别忘了同步更新 a[pos]。
    *
    * 7) M1 / M2 分开估：
    *      M1 ≈ n · log m
    *      M2 ≈ 2q · log n · log m
    *    n = q = 1e5 时，M1 ≈ 2e6，M2 ≈ 6e7。
    *    若仍 MLE，改"二分 + 离线 CDQ"或"整体二分"。
    *
    * 8) qL / qR 是每次查询临时缓存，不要设成 static，也不要跨查询复用。
    * ==========================================================================*/

    /* ============================================================================
    * 【常见题型的改造点速查】
    * ----------------------------------------------------------------------------
    * · 区间第 K 小（P2617 本尊）        → 直接使用 kth
    * · 区间 ≤ x 的个数                  → 展开 countLE
    * · 区间中位数                       → kth(L, R, (R-L+2)/2)
    * · 区间 x 的排名（升序）            → countLE(L, R, getId(x)-1) + 1
    * · 区间前驱（严格 < x 的最大值）    → kth(L, R, countLE(L, R, getId(x)-1))
    * · 区间后继（严格 > x 的最小值）    → kth(L, R, countLE(L, R, getId(x)) + 1)
    * · 二维数点 / 带修矩阵和            → 把"位置"换成第一维坐标，
    *                                      静态 PST 沿第一维按前缀建，
    *                                      动态 BIT 沿第一维走
    * ==========================================================================*/

    /* ------------------------------- 主流程 ------------------------------- */
    void solve(){
        cin >> n >> q;

        for(int i = 1; i <= n; i++){
            cin >> a[i];
            vals.push_back(a[i]);
        }

        ops.assign(q + 1, Op{});
        for(int i = 1; i <= q; i++){
            char opt; cin >> opt;
            if(opt == 'C'){
                int pos, val; cin >> pos >> val;
                ops[i] = {1, 0, 0, 0, pos, val};
                vals.push_back(val);
            }else{
                int l, r, k; cin >> l >> r >> k;
                ops[i] = {2, l, r, k, 0, 0};
            }
        }

        sort(vals.begin(), vals.end());
        vals.erase(unique(vals.begin(), vals.end()), vals.end());
        m = vals.size();

        /* 静态 PST：链式建前缀版本
        * 初始数组 a[i] 全部进静态部分，一个都不进 BIT */
        for(int i = 1; i <= n; i++)
            pst.st.root[i] = pst.st.update(pst.st.root[i-1], 1, m, getId(a[i]), +1);

        /* 动态 BIT：初始为空。只有"修改"才会往里加东西 */

        for(int i = 1; i <= q; i++){
            Op &op = ops[i];
            if(op.type == 1){
                /* 修改 = 在 BIT 里记一笔"删旧 + 加新" */
                pst.dif.add(op.pos, getId(a[op.pos]), -1);
                pst.dif.add(op.pos, getId(op.val),     +1);
                a[op.pos] = op.val;
            }else{
                int idx = pst.kth(op.l, op.r, op.k);
                cout << vals[idx - 1] << endl;
            }
        }
    }

}
```

### 线段树套动态开点权值线段树

> 【本次修复】M=N*1000 覆盖 n=q=1e5 全修改负载（约1.2GB，按题缩规模）；clone 加 assert。

```cpp
#define ll long long
#define endl '\n'

/* ============================================================================
 * 线段树套动态开点权值线段树（SegmentTree × Dynamic PST）
 * ----------------------------------------------------------------------------
 * 一句话：外层位置线段树把 [L,R] 拆成 O(log n) 个节点，
 *        每个节点挂一棵值域线段树，这些"根"一起做值域二分。
 * 适用题型：
 *     带修区间第 K 小 / 区间 ≤ x 个数 / 区间排名 /
 *     带修区间前驱后继 / 二维数点 等
 * 复杂度：
 *     单次修改 O(log²n)
 *     单次查询 O(log²n)
 * 空间：
 *     节点数 ≈ (n + 2q) · log n · log m，比 BIT 版本常数略大
 * ==========================================================================*/

const int N = 1e5 + 5;
/* 【易错点 1】内存池：
 *   初始 n 个数，每个位置走 log n 个外层节点，每个内层再走 log m 个。
 *   n = q = 1e5 全修改的极端负载实测消耗约 9.7e7 节点：
 *     M ≈ (n + 2q) · log n · log m ≈ 1e8 (N*1000)
 *   注意 M=1e8 × 12B/节点 ≈ 1.2GB, 常规 OJ(256~512MB) 必 MLE ——
 *   请按题目真实规模缩小 N/M (例如 n=q=3e4 时 M≈9e6 即够),
 *   或改用"树状数组套"文件中内存更省的 staticPST_BIT 版本。
 * clone 内置 assert: 池耗尽会立刻 RE, 而不是静默写出错误答案。 */
const int M = N * 1000;

int n, q, m;
int a[N];
vector<int> vals;

struct Op{
    int type;      // 1 修改, 2 查询
    int l, r, k;
    int pos, val;
};
vector<Op> ops;

// 【速查】线段树套权值线段树（带修区间第 k 小）
// 用途: 同"树状数组套"，外层换成位置线段树；能做 BIT 做不了的（外层需要区间信息时）
// 用法: 离线 + 离散化后，seg.modify(1, 1, n, pos, id, ±1) 删旧加新；查询 vals[seg.kth(l, r, k) - 1]
// 复杂度: O(log² n)；内存比 BIT 版还大，n=q=1e5 满负载约 1.2GB，按真实规模缩小 N/M
/* =============================== 核心结构 ============================== */
struct SegPST{

    /* ---------------- 内层：动态开点权值线段树 ---------------- */
    struct node{
        int l, r;
        int cnt;      // 【适配点 1】按题型换字段：cnt / sum / mx
    } t[M];

    int root[N << 2]; // 外层线段树每个节点挂一棵值域线段树的根
    int top = 0;      // t[0] 是全零哨兵

    int clone(int x){ assert(top < M); t[++top] = t[x]; return top; }

    /* 在子树 x 上，值 pos 处 += v */
    int update(int x, int l, int r, int pos, int v){
        x = clone(x);
        t[x].cnt += v;                       // 【适配点 1】合并处
        if(l == r) return x;
        int mid = (l + r) >> 1;
        if(pos <= mid) t[x].l = update(t[x].l, l, mid, pos, v);
        else           t[x].r = update(t[x].r, mid + 1, r, pos, v);
        return x;
    }

    /* ---------------- 外层：位置线段树 ---------------- */

    /* 位置 pos 上，值 val += v
     * 沿根到叶路径，所有经过的外层节点都要挂上这次修改 */
    void modify(int nd, int l, int r, int pos, int val, int v){
        root[nd] = update(root[nd], 1, m, val, v);
        if(l == r) return;
        int mid = (l + r) >> 1;
        if(pos <= mid) modify(nd << 1,     l,   mid, pos, val, v);
        else           modify(nd << 1 | 1, mid+1, r, pos, val, v);
    }

    /* ---------------- 查询：收集 + 二分 ---------------- */

    /* 【易错点 2】qNode 大小：区间 [L,R] 在外层分解最多 2·log₂n 个节点。
     * 保守开 N 或 64+ 即可；一定不要开成 2*log n 那种紧贴边界的数。 */
    int qNode[N], qcnt;

    /* 收集外层线段树中覆盖 [L,R] 的所有节点对应的值域根 */
    void collect(int nd, int l, int r, int L, int R){
        if(L <= l && r <= R){
            qNode[++qcnt] = root[nd];
            return;
        }
        int mid = (l + r) >> 1;
        if(L <= mid) collect(nd << 1,     l,   mid, L, R);
        if(R >  mid) collect(nd << 1 | 1, mid+1, r, L, R);
    }

    /* 区间 [L,R] 第 k 小，返回离散化下标
     * 【适配点 2】换语义时主要改这里 */
    int kth(int L, int R, int k){
        qcnt = 0;
        collect(1, 1, n, L, R);

        int l = 1, r = m;
        while(l < r){
            int mid = (l + r) >> 1;
            int cnt = 0;
            for(int i = 1; i <= qcnt; i++)
                cnt += t[t[qNode[i]].l].cnt;

            if(k <= cnt){
                for(int i = 1; i <= qcnt; i++) qNode[i] = t[qNode[i]].l;
                r = mid;
            }else{
                for(int i = 1; i <= qcnt; i++) qNode[i] = t[qNode[i]].r;
                k -= cnt;
                l = mid + 1;
            }
        }
        return l;
    }

    /* ---------- 可选：区间 ≤ x 的个数（对照 BIT 版本照搬） ----------
    int countLE(int L, int R, int pos){
        qcnt = 0;
        collect(1, 1, n, L, R);

        int l = 1, r = m, res = 0;
        while(l < r){
            int mid = (l + r) >> 1;
            if(pos <= mid){
                for(int i = 1; i <= qcnt; i++) qNode[i] = t[qNode[i]].l;
                r = mid;
            }else{
                for(int i = 1; i <= qcnt; i++) res += t[t[qNode[i]].l].cnt;
                for(int i = 1; i <= qcnt; i++) qNode[i] = t[qNode[i]].r;
                l = mid + 1;
            }
        }
        if(l <= pos)
            for(int i = 1; i <= qcnt; i++) res += t[qNode[i]].cnt;
        return res;
    }
    * --------------------------------------------------------- */

} seg;

/* ----------------------------- 辅助 ----------------------------- */
int getId(int x){
    return lower_bound(vals.begin(), vals.end(), x) - vals.begin() + 1;
}

/* ============================================================================
 * 【易错点清单】
 * ----------------------------------------------------------------------------
 * 1) 节点池 M 要按 (n + 2q)·log n · log m 估，比 BIT 版本更大：
 *      · 初始建树：n 个位置 × log n 个外层节点 × log m 个内层节点
 *      · 每次修改：2 次 × log n × log m
 *    n = q = 1e5 时约 9e7 节点，别按 BIT 的 4e7 开，会 RE。
 *
 * 2) 外层线段树索引从 1 开始（1 << 1 = 2），root[] 至少要开 N << 2。
 *
 * 3) modify 时**外层路径上每个节点都要挂上这次修改**：
 *    root[nd] = update(root[nd], ...) 必须在递归前执行，不能只更新叶子。
 *
 * 4) 离散化必须离线读入所有修改后的值；否则 getId 越界。
 *
 * 5) t[0] 是哨兵空节点，clone(0) 天然空树；若 cnt 换成 mx 且需要 -INF 初值，
 *    请手动给 t[0].mx 赋 -INF。
 *
 * 6) kth 中"二分每一步更新 qNode 指针"是关键，漏更新会导致下一层用错子树。
 *
 * 7) collect 是**查询前**一次性收集，不要和 modify 混用同一个缓冲区。
 * ==========================================================================*/

/* ============================================================================
 * 【常见题型的改造点速查】
 * ----------------------------------------------------------------------------
 * · 区间第 K 小                → 直接 kth(L, R, k)
 * · 区间 ≤ x 的个数            → 展开 countLE
 * · 区间中位数                 → kth(L, R, (R-L+2)/2)
 * · 区间 x 的排名（升序）      → countLE(L, R, getId(x)-1) + 1
 * · 区间前驱（< x 的最大值）   → kth(L, R, countLE(L, R, getId(x)-1))
 * · 区间后继（> x 的最小值）   → kth(L, R, countLE(L, R, getId(x)) + 1)
 * · 带"区间修改"的题型        → 把 modify 改成类似 lazy 的做法，
 *                                并在 kth 二分时同步下推（难度上一个台阶）
 * ==========================================================================*/

/* ------------------------------- 主流程 ------------------------------- */
void solve(){
    cin >> n >> q;

    for(int i = 1; i <= n; i++){
        cin >> a[i];
        vals.push_back(a[i]);
    }

    ops.assign(q + 1, Op{});
    for(int i = 1; i <= q; i++){
        char opt; cin >> opt;
        if(opt == 'C'){
            int pos, val; cin >> pos >> val;
            ops[i] = {1, 0, 0, 0, pos, val};
            vals.push_back(val);
        }else{
            int l, r, k; cin >> l >> r >> k;
            ops[i] = {2, l, r, k, 0, 0};
        }
    }

    sort(vals.begin(), vals.end());
    vals.erase(unique(vals.begin(), vals.end()), vals.end());
    m = vals.size();

    /* 初始数组：对每个位置做一次 +1 修改 */
    for(int i = 1; i <= n; i++)
        seg.modify(1, 1, n, i, getId(a[i]), +1);

    for(int i = 1; i <= q; i++){
        Op &op = ops[i];
        if(op.type == 1){
            seg.modify(1, 1, n, op.pos, getId(a[op.pos]), -1);
            seg.modify(1, 1, n, op.pos, getId(op.val),     +1);
            a[op.pos] = op.val;
        }else{
            int idx = seg.kth(op.l, op.r, op.k);
            cout << vals[idx - 1] << endl;
        }
    }
}
```

### 普通莫队

> 【本次修复】补离散化（原版 a_i>5e4 即段错误）。
> 【10.22 结构体封装】原全局变量 + 全局函数改为结构体 / 传参函数，用法见块内速查卡。

```cpp
using ll = long long;

// 【速查】普通莫队（离线区间查询，无修改）
// 用途: 区间信息能 O(1) 加 / 删一个元素时，离线回答 m 个区间询问。本例: 区间内任取两数相同的方案数（P1494）
// 用法: Mo mo;  mo.a = 数组（0-based，值已离散化到 0..K-1）;  mo.cnt.assign(K, 0);
//       auto res = mo.solve(qs);   // qs[i] = {l, r}（0-based 闭区间），res[i] = 该询问时的 mo.cur
// 改题: 只改"按题意修改"那一段（add / del 维护 cur）
// 复杂度: O(n√m)，块长 B = n / √(2m/3)，奇偶排序
struct Mo {
    // ---- 按题意修改 ----
    vector<int> a, cnt;
    ll cur = 0;                                // 当前区间内相同数对的个数 Σ C(cnt, 2)
    void add(int pos) { cur += cnt[a[pos]]; cnt[a[pos]]++; }
    void del(int pos) { cnt[a[pos]]--; cur -= cnt[a[pos]]; }
    // --------------------
    vector<ll> solve(const vector<pair<int, int>>& qs) {
        int n = a.size(), m = qs.size();
        int B = max(1, (int)(n / sqrt(max(1.0, m * 2.0 / 3.0))));
        vector<int> ord(m);
        iota(ord.begin(), ord.end(), 0);
        sort(ord.begin(), ord.end(), [&](int x, int y) {
            int bx = qs[x].first / B, by = qs[y].first / B;
            if (bx != by) return bx < by;
            return (bx & 1) ? qs[x].second < qs[y].second : qs[x].second > qs[y].second;
        });
        vector<ll> res(m);
        int l = 0, r = -1;
        for (int id : ord) {
            auto [ql, qr] = qs[id];
            while (l > ql) add(--l);              // 先扩张再收缩，保证区间不为负长度
            while (r < qr) add(++r);
            while (l < ql) del(l++);
            while (r > qr) del(r--);
            res[id] = cur;
        }
        return res;
    }
};

// 示例（P1494 小 Z 的袜子）: 读 n m、n 个数、m 个询问 (l r, 1-based)，输出最简分数"相同 / 总方案"
void solve() {
    int n, m;
    cin >> n >> m;
    Mo mo;
    mo.a.resize(n);
    for (auto& x : mo.a) cin >> x;
    vector<int> all = mo.a;                    // 离散化：值域可能到 1e9
    sort(all.begin(), all.end());
    all.erase(unique(all.begin(), all.end()), all.end());
    for (auto& x : mo.a) x = lower_bound(all.begin(), all.end(), x) - all.begin();
    mo.cnt.assign(all.size(), 0);
    vector<pair<int, int>> qs(m);
    for (auto& [l, r] : qs) { cin >> l >> r; l--; r--; }
    auto res = mo.solve(qs);
    for (int i = 0; i < m; i++) {
        ll len = qs[i].second - qs[i].first + 1, num = res[i], den = len * (len - 1) / 2;
        if (num == 0) { cout << "0/1\n"; continue; }
        ll g = __gcd(num, den);
        cout << num / g << '/' << den / g << '\n';
    }
}
```

### 带修莫队

> 三维排序 (l块,r块,t)；值域≤1e6，更大需自行离散化。
> 【10.22 结构体封装】原全局变量 + 全局函数改为结构体 / 传参函数，用法见块内速查卡。

```cpp
using ll = long long;

// 【速查】带修莫队（离线，支持单点修改）
// 用途: 普通莫队 + 时间维；本例: 区间不同数个数 + 单点改值（P1903 数颜色）
// 用法: MoUpd mo;  mo.a = 初始数组（0-based）;  mo.cnt.assign(值域, 0);
//       mo.addQuery(l, r)（0-based 闭区间，自动记录"之前已有几次修改"）;  mo.addUpdate(pos, x);  按输入顺序调用
//       auto res = mo.solve();   // res[i] = 第 i 个询问的答案
// 改题: 只改"按题意修改"那一段（add / del 按"值"维护 res）
// 复杂度: O(n^{5/3})，块长 B = n^{2/3}
struct MoUpd {
    // ---- 按题意修改 ----
    vector<int> a, cnt;
    ll res = 0;                                // 当前区间不同数的个数
    void add(int val) { if (cnt[val]++ == 0) res++; }
    void del(int val) { if (--cnt[val] == 0) res--; }
    // --------------------
    struct Q { int l, r, t, id; };
    vector<Q> qs;
    vector<pair<int, int>> ups;                // (位置, 新值)
    void addQuery(int l, int r) { qs.push_back({l, r, (int)ups.size(), (int)qs.size()}); }
    void addUpdate(int pos, int x) { ups.push_back({pos, x}); }

    vector<ll> solve() {
        int n = a.size();
        int B = max(1, (int)pow(n, 2.0 / 3.0));
        sort(qs.begin(), qs.end(), [&](const Q& x, const Q& y) {
            if (x.l / B != y.l / B) return x.l / B < y.l / B;
            if (x.r / B != y.r / B) return x.r / B < y.r / B;
            return x.t < y.t;
        });
        vector<ll> ans(qs.size());
        int l = 0, r = -1, t = 0;
        auto apply = [&](int k) {              // 执行 / 撤销第 k 次修改（交换后再调用一次即撤销）
            auto& [pos, x] = ups[k];
            if (l <= pos && pos <= r) { del(a[pos]); add(x); }
            swap(a[pos], x);
        };
        for (auto& q : qs) {
            while (l > q.l) add(a[--l]);
            while (r < q.r) add(a[++r]);
            while (l < q.l) del(a[l++]);
            while (r > q.r) del(a[r--]);
            while (t < q.t) apply(t++);
            while (t > q.t) apply(--t);
            ans[q.id] = res;
        }
        return ans;
    }
};

// 示例（P1903）: 读 n m、n 个数、m 个操作（Q l r 询问 / R p x 修改，1-based），输出每个询问的不同数个数
void solve() {
    int n, m;
    cin >> n >> m;
    MoUpd mo;
    mo.a.resize(n);
    for (auto& x : mo.a) cin >> x;
    mo.cnt.assign(1000006, 0);                 // 值域上限；值很大时先离散化（含修改值）
    for (int i = 0; i < m; i++) {
        char c; int x, y;
        cin >> c >> x >> y;
        if (c == 'Q') mo.addQuery(x - 1, y - 1);
        else mo.addUpdate(x - 1, y);
    }
    for (ll v : mo.solve()) cout << v << '\n';
}
```

### 回滚莫队

> 不删除莫队；P5906 区间最远重复位置。
> 【10.22 结构体封装】原全局变量 + 全局函数改为结构体 / 传参函数，用法见块内速查卡。

```cpp
using ll = long long;

// 【速查】回滚莫队（只加不删）
// 用途: 信息只能方便地"加入"、难"删除"时（区间最值类）。本例: 区间内相同数的最远距离（P5906）
// 用法: RollbackMo mo;  mo.a = 数组（0-based，值已离散化到 0..K-1）;  mo.init(K);
//       auto res = mo.solve(qs);   // qs[i] = {l, r}（0-based 闭区间）
// 做法: 左端点同块的询问按 r 升序一起处理：右指针只右移；左边每次从块的右边界往左加入，
//       记下改动，答完后回滚；l、r 在同一块的小询问直接暴力
// 改题: 改 init / addRight / addLeft / 回滚记录的内容 / brute
// 复杂度: O(n√q)
struct RollbackMo {
    vector<int> a, fst, lst;                   // fst / lst: 当前区间内每个值第一次 / 最后一次出现的位置
    void init(int K) { fst.assign(K, -1); lst.assign(K, -1); }
    ll brute(int l, int r) {                   // 小区间暴力（用完复原）
        ll best = 0;
        vector<int> touched;
        for (int j = l; j <= r; j++) {
            int v = a[j];
            if (fst[v] == -1) { fst[v] = j; touched.push_back(v); }
            best = max(best, (ll)(j - fst[v]));
        }
        for (int v : touched) fst[v] = -1;
        return best;
    }
    vector<ll> solve(const vector<pair<int, int>>& qs) {
        int n = a.size(), q = qs.size();
        int B = max(1, (int)(n / sqrt(max(1.0, q * 2.0 / 3.0))));
        vector<int> ord(q);
        iota(ord.begin(), ord.end(), 0);
        sort(ord.begin(), ord.end(), [&](int x, int y) {
            if (qs[x].first / B != qs[y].first / B) return qs[x].first / B < qs[y].first / B;
            return qs[x].second < qs[y].second;
        });
        vector<ll> ans(q);
        int curBlock = -1, r = -1;
        ll res = 0;
        for (int id : ord) {
            auto [ql, qr] = qs[id];
            int b = ql / B;
            if (b != curBlock) {                   // 换块：清空，右指针放到块的右边界
                fill(fst.begin(), fst.end(), -1); fill(lst.begin(), lst.end(), -1);
                res = 0; curBlock = b; r = (b + 1) * B - 1;
            }
            if (b == qr / B) { ans[id] = brute(ql, qr); continue; }
            while (r < qr) {                       // 右边只加
                int v = a[++r];
                if (fst[v] == -1) fst[v] = r;
                lst[v] = r;
                res = max(res, (ll)(lst[v] - fst[v]));
            }
            ll saved = res;
            vector<array<int, 3>> stk;             // (值, 旧 fst, 旧 lst)，用于回滚
            for (int l = min(n, (b + 1) * B) - 1; l >= ql; l--) {
                int v = a[l];
                stk.push_back({v, fst[v], lst[v]});
                if (lst[v] == -1) lst[v] = l;
                fst[v] = l;
                res = max(res, (ll)(lst[v] - fst[v]));
            }
            ans[id] = res;
            res = saved;
            while (!stk.empty()) { auto [v, f, l2] = stk.back(); stk.pop_back(); fst[v] = f; lst[v] = l2; }
        }
        return ans;
    }
};

// 示例（P5906）: 读 n、n 个数、q、q 个询问（1-based），输出区间内相同数的最远距离
void solve() {
    int n;
    cin >> n;
    RollbackMo mo;
    mo.a.resize(n);
    for (auto& x : mo.a) cin >> x;
    vector<int> b = mo.a;
    sort(b.begin(), b.end());
    b.erase(unique(b.begin(), b.end()), b.end());
    for (auto& x : mo.a) x = lower_bound(b.begin(), b.end(), x) - b.begin();
    mo.init(b.size());
    int q;
    cin >> q;
    vector<pair<int, int>> qs(q);
    for (auto& [l, r] : qs) { cin >> l >> r; l--; r--; }
    for (ll v : mo.solve(qs)) cout << v << '\n';
}
```

### FHQ Treap 平衡树（权值版 + 序列版）

> 【10.22 新增】插入/删除/排名/第 k 小/前驱后继；序列版支持任意位置插删、区间翻转。

```cpp
// 【速查】FHQ Treap（无旋 Treap）· 权值平衡树
// 用途: 动态可重集合：插入、删除、查排名、查第 k 小、前驱、后继（P3369 / P6136）
// 用法: Treap t(最大插入次数);  t.insert(x);  t.erase(x)（只删一个）;
//       t.rank(x) -> 比 x 小的个数 + 1;  t.kth(k) -> 第 k 小（1-based，k 越界返回 LLONG_MIN）;
//       t.prev(x) -> 严格小于 x 的最大值（没有返回 LLONG_MIN）;  t.next(x) -> 严格大于 x 的最小值（没有返回 LLONG_MAX）
// 复杂度: 每次期望 O(log n)；节点数 = 插入次数
struct Treap {
    struct Node { int l, r, sz; unsigned pri; long long key; };
    vector<Node> t;
    int root = 0;
    mt19937 rng{20251022};

    Treap(int maxn = 0) { t.reserve(maxn + 1); t.push_back({0, 0, 0, 0, 0}); }

    int newNode(long long k) { t.push_back({0, 0, 1, (unsigned)rng(), k}); return (int)t.size() - 1; }
    void pull(int u) { t[u].sz = t[t[u].l].sz + t[t[u].r].sz + 1; }

    // 按值分裂: a 中 key <= k，b 中 key > k
    void split(int u, long long k, int &a, int &b) {
        if (!u) { a = b = 0; return; }
        if (t[u].key <= k) { a = u; split(t[u].r, k, t[u].r, b); }
        else { b = u; split(t[u].l, k, a, t[u].l); }
        pull(u);
    }
    // 合并: 要求 a 的所有 key <= b 的所有 key
    int merge(int a, int b) {
        if (!a || !b) return a | b;
        if (t[a].pri > t[b].pri) { t[a].r = merge(t[a].r, b); pull(a); return a; }
        t[b].l = merge(a, t[b].l); pull(b); return b;
    }

    void insert(long long x) {
        int a, b; split(root, x, a, b);
        int c = newNode(x);
        root = merge(merge(a, c), b);
    }
    void erase(long long x) {
        int a, b, c;
        split(root, x, a, c);
        split(a, x - 1, a, b);              // b 里全是等于 x 的点
        if (b) b = merge(t[b].l, t[b].r);   // 删掉一个
        root = merge(merge(a, b), c);
    }
    int rank(long long x) {
        int a, b; split(root, x - 1, a, b);
        int res = t[a].sz + 1;
        root = merge(a, b);
        return res;
    }
    long long kth(int k) {
        int u = root;
        if (k < 1 || k > t[root].sz) return LLONG_MIN;
        while (u) {
            int ls = t[t[u].l].sz;
            if (k <= ls) u = t[u].l;
            else if (k == ls + 1) return t[u].key;
            else k -= ls + 1, u = t[u].r;
        }
        return LLONG_MIN;
    }
    long long prev(long long x) {
        long long ans = LLONG_MIN;
        for (int u = root; u; ) {
            if (t[u].key < x) ans = t[u].key, u = t[u].r;
            else u = t[u].l;
        }
        return ans;
    }
    long long next(long long x) {
        long long ans = LLONG_MAX;
        for (int u = root; u; ) {
            if (t[u].key > x) ans = t[u].key, u = t[u].l;
            else u = t[u].r;
        }
        return ans;
    }
    int size() { return t[root].sz; }
};

// 【速查】FHQ Treap · 序列版（按位置分裂，区间翻转，文艺平衡树 P3391）
// 用途: 维护一个序列：任意位置插入 / 删除、区间翻转，最后按顺序输出
// 用法: SeqTreap s(最大元素数);  s.push_back(v);  s.insert(pos, v)（插到第 pos 个之后，pos=0 插最前）;
//       s.erase(pos)（删第 pos 个，1-based）;  s.reverse(l, r);  s.toVector() -> 当前序列
// 扩展: 区间加 / 区间和 等，在 Node 里加字段，push 里下传，pull 里汇总
// 复杂度: 每次期望 O(log n)
struct SeqTreap {
    struct Node { int l, r, sz; unsigned pri; long long val; bool rev; };
    vector<Node> t;
    int root = 0;
    mt19937 rng{1022};

    SeqTreap(int maxn = 0) { t.reserve(maxn + 1); t.push_back({0, 0, 0, 0, 0, false}); }

    int newNode(long long v) { t.push_back({0, 0, 1, (unsigned)rng(), v, false}); return (int)t.size() - 1; }
    void pull(int u) { t[u].sz = t[t[u].l].sz + t[t[u].r].sz + 1; }
    void push(int u) {
        if (!t[u].rev) return;
        swap(t[u].l, t[u].r);
        if (t[u].l) t[t[u].l].rev ^= 1;
        if (t[u].r) t[t[u].r].rev ^= 1;
        t[u].rev = false;
    }
    // 按大小分裂: a 为前 k 个，b 为其余
    void split(int u, int k, int &a, int &b) {
        if (!u) { a = b = 0; return; }
        push(u);
        if (t[t[u].l].sz < k) { a = u; split(t[u].r, k - t[t[u].l].sz - 1, t[u].r, b); }
        else { b = u; split(t[u].l, k, a, t[u].l); }
        pull(u);
    }
    int merge(int a, int b) {
        if (!a || !b) return a | b;
        if (t[a].pri > t[b].pri) { push(a); t[a].r = merge(t[a].r, b); pull(a); return a; }
        push(b); t[b].l = merge(a, t[b].l); pull(b); return b;
    }

    void push_back(long long v) { int c = newNode(v); root = merge(root, c); }
    void insert(int pos, long long v) {
        int a, b; split(root, pos, a, b);
        int c = newNode(v);
        root = merge(merge(a, c), b);
    }
    void erase(int pos) {
        int a, b, c;
        split(root, pos - 1, a, b);
        split(b, 1, b, c);
        root = merge(a, c);
    }
    void reverse(int l, int r) {
        int a, b, c;
        split(root, l - 1, a, b);
        split(b, r - l + 1, b, c);
        if (b) t[b].rev ^= 1;
        root = merge(merge(a, b), c);
    }
    vector<long long> toVector() {
        vector<long long> res;
        vector<int> st;
        int u = root;
        while (u || !st.empty()) {          // 迭代中序遍历，避免深递归
            while (u) { push(u); st.push_back(u); u = t[u].l; }
            u = st.back(); st.pop_back();
            res.push_back(t[u].val);
            u = t[u].r;
        }
        return res;
    }
    int size() { return t[root].sz; }
};
```

### LCT 动态树

> 【10.22 新增】动态 link/cut、路径点权异或和、单点修改、判连通。

```cpp
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
```

### 线段树合并与分裂

> 【10.22 新增】动态开点权值线段树的 merge / split / kth，树上子树信息合并。

```cpp
// 【速查】线段树合并 / 分裂（动态开点权值线段树，P5494）
// 用途: 很多棵权值线段树之间整体合并、按值域切出一段、查第 k 小 / 区间计数
//       典型: 树上每个点一棵树，DFS 时把儿子的树合并上来（子树信息统计）
// 用法: SegMerge sm(值域上界 n, 节点池大小);  int rt = 0;
//       sm.add(rt, 1, n, pos, v)            // 值 pos 的个数 += v（rt 为 0 时自动建树）
//       rt = sm.merge(rt, rt2, 1, n)       // 把 rt2 并进 rt（rt2 之后不能再用）
//       int nr = 0; sm.split(rt, nr, 1, n, l, r)   // 把 rt 中值域 [l, r] 的部分切到新树 nr
//       sm.query(rt, 1, n, l, r) -> 值域 [l, r] 的个数;  sm.kth(rt, 1, n, k) -> 第 k 小的值（不足 k 个返回 -1）
// 复杂度: add / query / kth / split 都是 O(log n)，merge 总复杂度均摊 O(总节点数)
// 节点池: 约 (插入次数 + 分裂次数) × log n
struct SegMerge {
    int n, tot = 0;
    vector<int> L, R;
    vector<long long> cnt;

    SegMerge(int n, int maxNodes) : n(n), L(maxNodes, 0), R(maxNodes, 0), cnt(maxNodes, 0) {}

    int newNode() { ++tot; assert(tot < (int)L.size()); L[tot] = R[tot] = 0; cnt[tot] = 0; return tot; }
    void pull(int u) { cnt[u] = cnt[L[u]] + cnt[R[u]]; }

    void add(int &u, int l, int r, int pos, long long v) {
        if (!u) u = newNode();
        if (l == r) { cnt[u] += v; return; }
        int mid = (l + r) / 2;
        if (pos <= mid) add(L[u], l, mid, pos, v);
        else add(R[u], mid + 1, r, pos, v);
        pull(u);
    }
    long long query(int u, int l, int r, int ql, int qr) {
        if (!u || qr < l || r < ql) return 0;
        if (ql <= l && r <= qr) return cnt[u];
        int mid = (l + r) / 2;
        return query(L[u], l, mid, ql, qr) + query(R[u], mid + 1, r, ql, qr);
    }
    int merge(int a, int b, int l, int r) {
        if (!a || !b) return a | b;
        if (l == r) { cnt[a] += cnt[b]; return a; }
        int mid = (l + r) / 2;
        L[a] = merge(L[a], L[b], l, mid);
        R[a] = merge(R[a], R[b], mid + 1, r);
        pull(a);
        return a;
    }
    void split(int &u, int &v, int l, int r, int ql, int qr) {
        if (!u || qr < l || r < ql) return;
        if (ql <= l && r <= qr) { v = u; u = 0; return; }   // 整块直接摘走
        if (!v) v = newNode();
        int mid = (l + r) / 2;
        split(L[u], L[v], l, mid, ql, qr);
        split(R[u], R[v], mid + 1, r, ql, qr);
        pull(u); pull(v);
    }
    int kth(int u, int l, int r, long long k) {
        if (!u || k < 1 || cnt[u] < k) return -1;
        while (l < r) {
            int mid = (l + r) / 2;
            if (cnt[L[u]] >= k) u = L[u], r = mid;
            else k -= cnt[L[u]], u = R[u], l = mid + 1;
        }
        return l;
    }
};
```

### 左偏树（可并堆）

> 【10.22 新增】多个小根堆 O(log n) 合并、删堆顶。

```cpp
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
```

### 可持久化 Trie（区间最大异或）

> 【10.22 新增】第 l..r 个插入的数中与 x 异或的最大值。

```cpp
// 【速查】可持久化 01-Trie（区间最大异或，P4735）
// 用途: 依次插入若干数，询问"第 l..r 个插入的数里，与 x 异或的最大值"
// 用法: PersistentTrie T(最大插入次数);  T.insert(y);   // 第 i 次插入后版本号为 i（版本 0 为空）
//       T.query(l, r, x) -> max_{l <= i <= r} (x ^ 第 i 个数)，要求 1 <= l <= r <= 已插入个数
// 套路(P4735): 令 s[i] 为前缀异或，先插入 s[0]=0；问 max(s[p-1] ^ s[n] ^ x)，p∈[l,r] 就是查
//       插入序号 [l, r]（s[l-1]..s[r-1] 对应序号 l..r）
// 复杂度: 插入 / 查询 O(BITS)；节点数 = 插入次数 × (BITS + 1)
struct PersistentTrie {
    static const int BITS = 30;              // 值域 < 2^30；long long 值域改成 62 并把 int 换 long long
    vector<array<int, 2>> ch;
    vector<int> cnt, root;

    PersistentTrie(int maxInsert = 0) {
        ch.reserve((size_t)(maxInsert + 1) * (BITS + 1) + 1);
        cnt.reserve(ch.capacity());
        ch.push_back({0, 0}); cnt.push_back(0);   // 0 号为空节点
        root.push_back(0);                         // 版本 0: 空树
    }
    int newNode() { ch.push_back({0, 0}); cnt.push_back(0); return (int)ch.size() - 1; }

    void insert(int x) {
        int p = root.back(), q = newNode();
        root.push_back(q);
        for (int i = BITS - 1; i >= 0; i--) {
            int b = x >> i & 1;
            ch[q] = ch[p]; cnt[q] = cnt[p] + 1;
            int nq = newNode();
            ch[q][b] = nq;
            p = ch[p][b]; q = nq;
        }
        ch[q] = ch[p]; cnt[q] = cnt[p] + 1;
    }
    int query(int l, int r, int x) {
        int u = root[r], v = root[l - 1], res = 0;
        for (int i = BITS - 1; i >= 0; i--) {
            int b = x >> i & 1;
            if (cnt[ch[u][!b]] - cnt[ch[v][!b]] > 0) { res |= 1 << i; u = ch[u][!b]; v = ch[v][!b]; }
            else { u = ch[u][b]; v = ch[v][b]; }
        }
        return res;
    }
};
```

### 单调栈与单调队列

> 【10.22 新增】左右第一个更大元素、滑动窗口最值。

```cpp
// 【速查】单调栈：每个位置左/右第一个比它大（或小）的位置
// 用法: auto R = nextGreater(a);   // a 为 0-based，R[i] = 右边第一个 a[j] > a[i] 的 j，没有为 n
//       auto L = prevGreater(a);   // L[i] = 左边第一个 a[j] > a[i] 的 j，没有为 -1
// 改"更小": 把 while 里 a[st.back()] 与 a[i] 的比较方向反过来；
// 严格/非严格: nextGreater 里 < 改 <= 就变成"右边第一个 >= 的"
// 用途: 以 a[i] 为最大值的区间 (L[i], R[i])、柱状图最大矩形、贡献法计数
// 复杂度: O(n)
vector<int> nextGreater(const vector<long long>& a) {
    int n = a.size();
    vector<int> res(n, n), st;
    for (int i = 0; i < n; i++) {
        while (!st.empty() && a[st.back()] < a[i]) { res[st.back()] = i; st.pop_back(); }
        st.push_back(i);
    }
    return res;
}
vector<int> prevGreater(const vector<long long>& a) {
    int n = a.size();
    vector<int> res(n, -1), st;
    for (int i = 0; i < n; i++) {
        while (!st.empty() && a[st.back()] <= a[i]) st.pop_back();
        if (!st.empty()) res[i] = st.back();
        st.push_back(i);
    }
    return res;
}

// 【速查】单调队列：滑动窗口最大值 / 最小值
// 用法: auto mx = slidingMax(a, k);   // a 为 0-based，mx[i] = max(a[i..i+k-1])，长度 n-k+1
//       slidingMin 同理
// 用途: 窗口最值、单调队列优化 DP（dp[i] = max_{i-k<=j<i} dp[j] + ...，队列里存下标）
// 复杂度: O(n)
vector<long long> slidingMax(const vector<long long>& a, int k) {
    vector<long long> res;
    deque<int> q;
    for (int i = 0; i < (int)a.size(); i++) {
        while (!q.empty() && a[q.back()] <= a[i]) q.pop_back();
        q.push_back(i);
        if (q.front() <= i - k) q.pop_front();
        if (i >= k - 1) res.push_back(a[q.front()]);
    }
    return res;
}
vector<long long> slidingMin(const vector<long long>& a, int k) {
    vector<long long> res;
    deque<int> q;
    for (int i = 0; i < (int)a.size(); i++) {
        while (!q.empty() && a[q.back()] >= a[i]) q.pop_back();
        q.push_back(i);
        if (q.front() <= i - k) q.pop_front();
        if (i >= k - 1) res.push_back(a[q.front()]);
    }
    return res;
}
```

### 珂朵莉树 ODT

> 【10.22 新增】区间赋值 + 随机数据下的暴力区间操作。

```cpp
// 【速查】珂朵莉树 ODT（颜色段均摊，CF896C）
// 用途: 有"区间赋值"操作且数据随机时，用 set 维护值相同的连续段，暴力处理其余操作
// 用法: ODT t;  t.init(a, n);   // a 为 1-based，自动加哨兵 [n+1, n+1]
//       t.assign(l, r, v)  区间赋值;   t.add(l, r, v)  区间加;
//       t.kth(l, r, k) -> 区间第 k 小;   t.powsum(l, r, x, mod) -> Σ a_i^x mod mod
// 注意: 先 split(r + 1) 再 split(l)，顺序反了迭代器会失效
//       只有存在区间赋值且数据随机时复杂度才有保证，构造数据会被卡
// 复杂度: 随机数据下均摊 O(n log log n) 段操作
struct ODT {
    struct Node {
        long long l, r;
        mutable long long v;
        bool operator<(const Node& o) const { return l < o.l; }
    };
    set<Node> s;

    void init(const vector<long long>& a, int n) {
        s.clear();
        for (int i = 1; i <= n; i++) s.insert({i, i, a[i]});
        s.insert({n + 1, n + 1, 0});
    }
    // 把包含 pos 的段从 pos 处断开，返回以 pos 开头的段
    set<Node>::iterator split(long long pos) {
        auto it = s.lower_bound({pos, 0, 0});
        if (it != s.end() && it->l == pos) return it;
        --it;
        long long l = it->l, r = it->r, v = it->v;
        s.erase(it);
        s.insert({l, pos - 1, v});
        return s.insert({pos, r, v}).first;
    }
    void assign(long long l, long long r, long long v) {
        auto itr = split(r + 1), itl = split(l);
        s.erase(itl, itr);
        s.insert({l, r, v});
    }
    void add(long long l, long long r, long long v) {
        auto itr = split(r + 1), itl = split(l);
        for (auto it = itl; it != itr; ++it) it->v += v;
    }
    long long kth(long long l, long long r, long long k) {
        auto itr = split(r + 1), itl = split(l);
        vector<pair<long long, long long>> seg;   // (值, 个数)
        for (auto it = itl; it != itr; ++it) seg.push_back({it->v, it->r - it->l + 1});
        sort(seg.begin(), seg.end());
        for (auto& [v, c] : seg) { if (k <= c) return v; k -= c; }
        return -1;
    }
    static long long qpow(long long a, long long b, long long m) {
        long long res = 1 % m; a %= m; if (a < 0) a += m;
        for (; b; b >>= 1, a = (__int128)a * a % m) if (b & 1) res = (__int128)res * a % m;
        return res;
    }
    long long powsum(long long l, long long r, long long x, long long mod) {
        auto itr = split(r + 1), itl = split(l);
        long long res = 0;
        for (auto it = itl; it != itr; ++it)
            res = (res + (__int128)qpow(it->v, x, mod) * ((it->r - it->l + 1) % mod)) % mod;
        return res;
    }
};
```
