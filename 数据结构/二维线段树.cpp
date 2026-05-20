#include <bits/stdc++.h>
using namespace std;

// 二维线段树
struct Seg2D {
    int n, m;
    vector<vector<int>> tr, lazy;
    
    Seg2D(int n, int m) : n(n), m(m) {
        tr.assign(n * 4 + 5, vector<int>(m * 4 + 5, 0));
        lazy.assign(n * 4 + 5, vector<int>(m * 4 + 5, 0));
    }
    
    void build(int p, int l, int r, const vector<vector<int>>& a) {
        build_y(p, 1, 1, m, l, r, a);
    }
    
    // 构建Y轴
    void build_y(int px, int py, int ly, int ry, int lx, int rx, const vector<vector<int>>& a) {
        if(ly == ry) {
            if(lx == rx) {
                tr[px][py] = a[lx][ly];
            } else {
                tr[px][py] = tr[px << 1][py] + tr[px << 1 | 1][py];
            }
            return;
        }
        int mid = ly + ry >> 1;
        build_y(px, py << 1, ly, mid, lx, rx, a);
        build_y(px, py << 1 | 1, mid + 1, ry, lx, rx, a);
        pushup_y(px, py);
    }
    
    void build(int px, int lx, int rx, const vector<vector<int>>& a) {
        if(lx == rx) {
            build_y(px, 1, 1, m, lx, rx, a);
            return;
        }
        int mid = lx + rx >> 1;
        build(px << 1, lx, mid, a);
        build(px << 1 | 1, mid + 1, rx, a);
        build_y(px, 1, 1, m, lx, rx, a);
    }
    
    void clear() {
        for(auto& row : tr) fill(row.begin(), row.end(), 0);
        for(auto& row : lazy) fill(row.begin(), row.end(), 0);
    }
    
    void pushup_y(int px, int py) {
        tr[px][py] = tr[px][py << 1] + tr[px][py << 1 | 1];
    }
    
    void pushdown_y(int px, int py, int ly, int ry) {
        if(lazy[px][py]) {
            int mid = ly + ry >> 1;
            tr[px][py << 1] += (mid - ly + 1) * lazy[px][py];
            tr[px][py << 1 | 1] += (ry - mid) * lazy[px][py];
            lazy[px][py << 1] += lazy[px][py];
            lazy[px][py << 1 | 1] += lazy[px][py];
            lazy[px][py] = 0;
        }
    }
    
    // 内层Y轴更新
    void update_y(int px, int py, int ly, int ry, int qly, int qry, int val) {
        if(qly <= ly && ry <= qry) {
            tr[px][py] += (ry - ly + 1) * val;
            lazy[px][py] += val;
            return;
        }
        pushdown_y(px, py, ly, ry);
        int mid = ly + ry >> 1;
        if(qly <= mid) update_y(px, py << 1, ly, mid, qly, qry, val);
        if(mid < qry) update_y(px, py << 1 | 1, mid + 1, ry, qly, qry, val);
        pushup_y(px, py);
    }
    
    // 外层X轴更新
    void update(int p, int l, int r, int ql, int qr, int qly, int qry, int val) {
        if(ql <= l && r <= qr) {
            update_y(p, 1, 1, m, qly, qry, val);
            return;
        }
        int mid = l + r >> 1;
        if(ql <= mid) update(p << 1, l, mid, ql, qr, qly, qry, val);
        if(mid < qr) update(p << 1 | 1, mid + 1, r, ql, qr, qly, qry, val);
        // 更新当前节点
        for(int py = 1; py <= m * 4; py++) {
            tr[p][py] = tr[p << 1][py] + tr[p << 1 | 1][py];
        }
    }
    
    // 内层Y轴查询
    int ask_y(int px, int py, int ly, int ry, int qly, int qry) {
        if(qly <= ly && ry <= qry) {
            return tr[px][py];
        }
        pushdown_y(px, py, ly, ry);
        int mid = ly + ry >> 1;
        int res = 0;
        if(qly <= mid) res += ask_y(px, py << 1, ly, mid, qly, qry);
        if(mid < qry) res += ask_y(px, py << 1 | 1, mid + 1, ry, qly, qry);
        return res;
    }
    
    // 外层X轴查询
    int ask(int p, int l, int r, int ql, int qr, int qly, int qry) {
        if(ql <= l && r <= qr) {
            return ask_y(p, 1, 1, m, qly, qry);
        }
        int mid = l + r >> 1;
        int res = 0;
        if(ql <= mid) res += ask(p << 1, l, mid, ql, qr, qly, qry);
        if(mid < qr) res += ask(p << 1 | 1, mid + 1, r, ql, qr, qly, qry);
        return res;
    }
};

// 使用示例
// int main() {
//     int n = 5, m = 5;
//     vector<vector<int>> a(n + 1, vector<int>(m + 1));
//     
//     Seg2D seg(n, m);
//     seg.build(1, 1, n, a);
//     
//     // 矩形区域 (x1,y1) 到 (x2,y2) 加 val
//     seg.update(1, 1, n, x1, x2, y1, y2, val);
//     
//     // 查询矩形区域 (x1,y1) 到 (x2,y2) 的和
//     int sum = seg.ask(1, 1, n, x1, x2, y1, y2);
//     
//     return 0;
// }