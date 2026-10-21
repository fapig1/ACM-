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

> 【本次修复】节点池 N*40(640MB) 改为 N*25(400MB)。P3919 版本语义：每个操作（含查询）都产生新版本。

```cpp
#define ll long long
#define endl '\n'

//https://www.luogu.com.cn/problem/P3919
//主席树单点修改单点查询

const int N=1e6+5;

struct node{
    int l,r;
    ll val;
}tree[N*25];
//节点数按 (n + q·log n) 估计: n=1e6,q=1e6 时 ≈ 1e6+2e7 = 2.1e7, N*25=2.5e7 留有余量
//原版 N*40 = 4e7 节点 × 16B = 640MB, 512MB 内存限制的评测机会 MLE

vector<ll> vec(N);
vector<int> root(N);//root表示第i个版本的根节点的index

int top=0;
//在更新操作时进行动态开点的函数
int clone(int ind){
    top++;
    tree[top]=tree[ind];
    return top;
}

//建树
int build(int ind,int l,int r){
    top++;
    ind=top;

    if(l==r){
        tree[ind].val=vec[l];
        return ind;
    }

    int mid=(l+r)/2;
    //实际上更新点可以有很多种方式，我使用的方式是为了统一更新建树和查询函数参数的格式
    //build里面的第一个参数意义不大，为了统一参数那么写的，写一样的就行
    tree[ind].l = build(tree[ind].l,l,mid);
    tree[ind].r = build(tree[ind].r,mid+1,r);
    return ind;
}

//更新
int update(int ind,int l,int r,int t,ll v){
    ind=clone(ind);

    if(l==r){
        tree[ind].val=v;
        return ind;
    }
    int mid=(l+r)/2;
    if(t<=mid){
        tree[ind].l=update(tree[ind].l,l,mid,t,v);
    }else{
        tree[ind].r=update(tree[ind].r,mid+1,r,t,v);
    }
    return ind;
}

//查询
ll query(int ind,int l,int r,int t){
    if(l==r){
        return tree[ind].val; 
    }

    int mid=(l+r)/2;
    if(t<=mid){
        return query(tree[ind].l,l,mid,t);
    }else{
        return query(tree[ind].r,mid+1,r,t);
    }
}

int n,q;
void solve(){
    cin>>n>>q;

    for(int i = 1;i <= n;i ++){
        cin>>vec[i];
    }

    root[0]=build(1,1,n);
    for(int i=1;i<=q;i++){
        int version;
        int command;
        cin>>version>>command;
        if(command==1){
            int t;
            ll v;
            cin>>t>>v;
            root[i]=update(root[version],1,n,t,v);
        }else{
            int t;
            cin>>t;
            ll ans=query(root[version],1,n,t);
            cout<<ans<<endl;
            root[i]=root[version];
        }

    }

}


namespace Pseg{
    const int N = 1e6 + 5;      // 版本数上限
    const int M = N * 25;       // 节点数上限，按需调整

    struct PSeg{
        struct node{
            int l, r;
            ll val;             // 按题型可改成 cnt
        }tree[M];

        int root[N];            // root[i] 表示第 i 个版本的根
        int top;                // 当前节点编号

        PSeg(){
            top = 0;
        }

        // 动态开点：复制旧节点，返回新节点编号
        int clone(int t){
            top++;
            tree[top] = tree[t];
            return top;
        }

        // 建树：带初值，建版本 0
        // vec 是初始数组，下标从 1 开始
        int build(int t, int l, int r, const vector<ll>& vec){
            t = ++top;
            if(l == r){
                tree[t].val = vec[l];
                return t;
            }
            int mid = (l + r) / 2;
            tree[t].l = build(tree[t].l, l, mid, vec);
            tree[t].r = build(tree[t].r, mid + 1, r, vec);
            return t;
        }

        // 建空树：所有权值为 0
        int buildEmpty(int t, int l, int r){
            t = ++top;
            tree[t].val = 0;
            if(l == r) return t;
            int mid = (l + r) / 2;
            tree[t].l = buildEmpty(tree[t].l, l, mid);
            tree[t].r = buildEmpty(tree[t].r, mid + 1, r);
            return t;
        }

        // 单点修改：在 t 版本基础上，把位置 pos 改成 v
        int update(int t, int l, int r, int pos, ll v){
            t = clone(t);
            if(l == r){
                tree[t].val = v;
                return t;
            }
            int mid = (l + r) / 2;
            if(pos <= mid)
                tree[t].l = update(tree[t].l, l, mid, pos, v);
            else
                tree[t].r = update(tree[t].r, mid + 1, r, pos, v);
            return t;
        }

        // 单点查询：查询 t 版本中位置 pos 的值
        ll query(int t, int l, int r, int pos){
            if(l == r) return tree[t].val;
            int mid = (l + r) / 2;
            if(pos <= mid)
                return query(tree[t].l, l, mid, pos);
            else
                return query(tree[t].r, mid + 1, r, pos);
        }

        // 针对于区间第K小问题的插入
        // int insert(int t, int l, int r, int pos){
        //     t = clone(t);
        //     tree[t].val++;
        //     if(l == r) return t;
        //     int mid = (l + r) / 2;
        //     if(pos <= mid)
        //         tree[t].l = insert(tree[t].l, l, mid, pos);
        //     else
        //         tree[t].r = insert(tree[t].r, mid + 1, r, pos);
        //     return t;
        // }

        // 针对于区间第K小问题的查询，返回ind
        // int queryKth(int u, int v, int l, int r, int k){
        //     if(l == r) return l;
        //     int mid = (l + r) / 2;
        //     int leftCnt = tree[tree[v].l].val - tree[tree[u].l].val;
        //     if(k <= leftCnt)
        //         return queryKth(tree[u].l, tree[v].l, l, mid, k);
        //     else
        //         return queryKth(tree[u].r, tree[v].r, mid + 1, r, k - leftCnt);
        // }
    };

    // 第K小使用示例
    // int n, q;
    // vector<ll> a, vec;

    // 放全局不会爆栈
    // PSeg pst;

    // void solve(){
    //     // 快捷清空
    //     pst.top=0;
    
    //     cin >> n >> q;
    //     a.assign(n + 1, 0);
    //     for(int i = 1; i <= n; i++){
    //         cin >> a[i];
    //         vec.push_back(a[i]);
    //     }

    //     sort(vec.begin(), vec.end());
    //     vec.erase(unique(vec.begin(), vec.end()), vec.end());
    //     int m = vec.size();

    //     //这句话很关键！！！！！
    //     pst.root[0] = pst.buildEmpty(0, 1, m);

    //     for(int i = 1; i <= n; i++){
    //         int pos = lower_bound(vec.begin(), vec.end(), a[i]) - vec.begin() + 1;
    //         pst.root[i] = pst.insert(pst.root[i-1], 1, m, pos);
    //     }

    //     while(q--){
    //         int l, r, k;
    //         cin >> l >> r >> k;
    //         int idx = pst.queryKth(pst.root[l-1], pst.root[r], 1, m, k);
    //         cout << vec[idx - 1] << endl;
    //     }
    // }
}
```

### ST表

> 静态区间最大值。

```cpp
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

```cpp
#define int long long
#define double long double

const int maxn = 5e4;
int B;
int cur = 0;

struct Q{
    int l, r, id;

    bool operator<(const Q& b)const{
        int pos1 = l / B, pos2 = b.l / B;
        if(pos1 != pos2) return pos1 < pos2;
        if(pos1 & 1) return r < b.r;
        return r > b.r;
    }
};

vector<int> cnt(maxn + 5);
vector<int> a(maxn + 5);
vector<Q> ask(maxn + 5);
vector<pair<int,int>> ans(maxn + 5);

int l = 1, r = 0;

void add(int pos){
    int val = a[pos];
    cur += cnt[val];
    cnt[val]++; 
}

void del(int pos){
    int val = a[pos];
    cnt[val]--;
    cur -= cnt[val];
}

void solve(){
    int n, m;
    cin >> n >> m;
    B = max(1.0, n / sqrt(m * 2.0 / 3.0));
    for(int i = 1; i <= n; i++){
        cin >> a[i];
    }
    // 离散化: a_i 值域可能很大(如 1e9), cnt 数组只按种类数开
    {
        vector<int> all(a.begin() + 1, a.begin() + n + 1);
        sort(all.begin(), all.end());
        all.erase(unique(all.begin(), all.end()), all.end());
        for(int i = 1; i <= n; i++)
            a[i] = lower_bound(all.begin(), all.end(), a[i]) - all.begin() + 1;
    }
    for(int i = 0; i < m; i++){
        cin >> ask[i].l >> ask[i].r;
        ask[i].id = i;
    }
    sort(ask.begin(), ask.begin() + m);
    for(int i = 0; i < m; i++){
        auto[ql, qr, id] = ask[i];
        while(l > ql) add(--l);
        while(r < qr) add(++r);
        while(l < ql) del(l++);
        while(r > qr) del(r--);
        int len = r - l + 1;
        int fm = cur == 0 ? 1 : len * (len - 1) / 2;
        int g = cur == 0 ? 1 : __gcd(cur, fm);
        ans[id] = {cur / g, fm / g};
    }
    for(int i = 0; i < m; i++){
        cout << ans[i].first << '/' << ans[i].second << '\n';
    }


}
```

### 带修莫队

> 三维排序 (l块,r块,t)；值域≤1e6，更大需自行离散化。

```cpp
#define int long long
#define double long double

int n, m, B;
struct Q{
    int l, r, t, id;

    bool operator<(const Q& b)const{
        if(l / B != b.l / B) return l / B < b.l / B;
        if(r / B != b.r / B) return r / B < b.r / B;
        return t < b.t;
    }

    Q(int l, int r, int t, int id) : l(l), r(r), t(t), id(id) {}
};

struct OP{
    int p, x;

    OP(int p, int x): p(p), x(x) {}
};

vector<int> a;
vector<Q> ask;
vector<OP> op; 
vector<int> cnt(1000006);
int res = 0;

void add(int val){
    if(cnt[val] == 0) res++;
    cnt[val]++; 
}

void del(int val){
    cnt[val]--;
    if(cnt[val] == 0) res--;
}

void solve(){
    cin >> n >> m;
    a.resize(n);
    for(int i = 0; i < n; i++){
        cin >> a[i];
    }

    for(int i = 0; i < m; i++){
        char c;
        int x, y;
        cin >> c >> x >> y;
        if(c == 'Q'){
            ask.push_back(Q(x - 1, y - 1, op.size(), ask.size()));
        } else {
            op.push_back(OP(x - 1, y));
        }
    }
    B = pow(n, 2.0 / 3.0);
    sort(ask.begin(), ask.end());
    vector<int> ans(ask.size());
    int curT = 0, l = 0, r = -1;
    for(int i = 0; i < ask.size(); i++){
        auto[ql, qr, t, id] = ask[i];
        while(l > ql) add(a[--l]);
        while(r < qr) add(a[++r]);
        while(l < ql) del(a[l++]);
        while(r > qr) del(a[r--]);
        while(curT < t){
            int pos = op[curT].p;
            if(l <= pos && pos <= r){
                del(a[pos]), add(op[curT].x);
            }
            swap(a[pos], op[curT].x);
            curT++;
        }
        while(curT > t){
            curT--;
            int pos = op[curT].p;
            if(l <= pos && pos <= r){
                del(a[pos]), add(op[curT].x);
            }     
            swap(a[pos], op[curT].x);
        }
        ans[id] = res;
    }
    for(int i : ans) cout << i << '\n';

}
```

### 回滚莫队

> 不删除莫队；P5906 区间最远重复位置。

```cpp
#define int long long
#define double long double

int n, q, B;
struct Q{
    int l, r, id;

    bool operator<(const Q& b)const{
        if(l / B != b.l / B) return l / B < b.l / B;
        return r < b.r;
    }
    Q(int l, int r, int id) : l(l), r(r), id(id) {}
};

vector<int> a;
vector<Q> ask;
vector<int> fst, lst;
vector<int> cnt(200005);

void solve(){
    cin >> n;
    a.resize(n);
    fst.resize(n + 5, - 1);
    lst.resize(n + 5, -1);
    for(int i = 0; i < n; i++){
        cin >> a[i];
    }    
    cin >> q;
    B = max(1.0, n / sqrt(q * 2.0 / 3.0));
    auto b = a;
    sort(b.begin(), b.end());
    b.erase(unique(b.begin(), b.end()), b.end());
    for(int i = 0; i < n; i++){
        a[i] = lower_bound(b.begin(), b.end(), a[i]) - b.begin();
    }
    for(int i = 0; i < q; i++){
        int x, y;
        cin >> x >> y;
        ask.push_back(Q(x - 1, y - 1, i));
    }
    sort(ask.begin(), ask.end());
    vector<int> ans(q);
    int lb = -1, r = -1, res = 0;
    for(int i = 0; i < q; i++){
        auto[ql, qr, id] = ask[i];
        int cur = ql / B;

        if(cur != lb){
            fill(fst.begin(), fst.end(), -1);
            fill(lst.begin(), lst.end(), -1);
            res = 0;
            lb = cur;
            r = (cur + 1) * B - 1;
        }
        
        if(cur == qr / B){
            int temp = 0;
            vector<int> wait;
            for(int j = ql; j <= qr; j++){
                int val = a[j];
                if(fst[val] == -1){
                    fst[val] = j;
                    wait.push_back(val);
                }
                lst[val] = j;
                temp = max(temp, j - fst[val]);
            }
            ans[id] = temp;
            for(int val : wait){
                fst[val] = -1;
                lst[val] = -1;
            }
            continue;
        }


        while(r < qr){
            r++;
            int val = a[r];
            if(fst[val] == -1) fst[val] = r;
            lst[val] = r;
            res = max(res, lst[val] - fst[val]);
        }

        int l = min(n, (cur + 1) * B);
        int tmp = res;
        stack<array<int,3>> wait;// val fst lst 
        while(l > ql){
            l--;
            int val = a[l];
            wait.push({val, fst[val], lst[val]});
            if(lst[val] == -1) lst[val] = l;
            fst[val] = l;
            res = max(res, lst[val] - fst[val]);
        }

        ans[id] = res;
        res = tmp;

        while(wait.size()){
            auto[val, l, r] = wait.top();
            wait.pop();
            fst[val] = l;
            lst[val] = r; 
        }

    }

    for(int i : ans) cout << i << '\n';

}
```
