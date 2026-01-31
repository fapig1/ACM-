#include <bits/stdc++.h>
using namespace std;
#define int long long

//用于查询若干线段或者直线在某点处的最值
const double eps = 1e-9, INF = 1e18;


int sgn(double x){
    if(fabs(x) < eps) return 0;
    return x < 0 ? -1 : 1;
}

struct Line{
    double k, b;
    bool exists;

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
        if(tr[p].cal(mid) > val.cal(mid)) swap(tr[p], val);
        if(l == r) return;

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
    // 查询 x 处的最大 y 值
    double ask(int p, int l, int r, int x){
        if(x < l || x > r || !tr[p].exists) return INF;
        double res = tr[p].cal(x);
        if(l == r) return res;
        int mid = l + r >> 1;
        if(x <= mid) return min(res, ask(p << 1, l, mid, x));
        return min(res, ask(p << 1 | 1, mid + 1, r, x));
    }
};
