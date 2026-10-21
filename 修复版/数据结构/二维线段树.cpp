#include <bits/stdc++.h>
using namespace std;

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
