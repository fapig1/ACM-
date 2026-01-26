#include <bits/stdc++.h>
using namespace std;

//线段树
struct Seg{
    int n;
    vector<int> tr, lazy;

    Seg(int n) : n(n), tr(n * 4 + 5), lazy(n * 4 + 5) {}

    void build(int p, int l, int r, const vector<int>& a){
        if(l == r){
            tr[p] = a[l];
            return;
        }
        int mid = l + r >> 1;
        build(p << 1, l, mid, a);
        build(p << 1 | 1, mid + 1, r, a);
        pushup(p);
    }

    void clear(){
        fill(tr.begin(), tr.end(), 0);
        fill(lazy.begin(), lazy.end(), 0);
    }

    void pushup(int p){
        tr[p] = tr[p << 1] + tr[p << 1 | 1];
    }

    void pushdown(int p, int l, int r){
        if(lazy[p]){
            int mid = l + r >> 1;
            tr[p << 1] += (mid - l + 1) * lazy[p];
            tr[p << 1 | 1] += (r - mid) * lazy[p];
            lazy[p << 1] += lazy[p];
            lazy[p << 1 | 1] += lazy[p];
            lazy[p] = 0;
        }
    }

    void update(int p, int l, int r, int ql, int qr, int val){
        if(ql <= l && r <= qr){
            tr[p] += (r - l + 1) * val;
            lazy[p] += val;
            return;
        }
        pushdown(p, l, r);
        int mid = l + r >> 1;
        if(ql <= mid) update(p << 1, l, mid, ql, qr, val);
        if(mid < qr) update(p << 1 | 1, mid + 1, r, ql, qr, val);
        pushup(p);
    }

    int ask(int p, int l, int r, int ql, int qr){
        if(ql <= l && r <= qr){
            return tr[p];
        }
        pushdown(p, l, r);
        int mid = l + r >> 1;
        int res = 0;
        if(ql <= mid) res += ask(p << 1, l, mid, ql, qr);
        if(mid < qr) res += ask(p << 1 | 1, mid + 1, r, ql, qr);
        return res;
    }

};