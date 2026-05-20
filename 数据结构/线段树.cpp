#include <bits/stdc++.h>
using namespace std;
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