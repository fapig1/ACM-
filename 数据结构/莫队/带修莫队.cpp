#include <bits/stdc++.h>
using namespace std;
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

