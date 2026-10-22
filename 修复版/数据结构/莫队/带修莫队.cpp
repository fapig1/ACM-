#include <bits/stdc++.h>
using namespace std;
#define int long long
#define double long double

// 【速查】带修莫队（离线，支持单点修改）
// 用途: 普通莫队 + 时间维；本例: 区间不同数个数 + 单点改值（P1903）
// 用法: MoUpd mo;（放全局）  mo.solve()；询问记 (l, r, 之前的修改数 t)，修改记 (pos, x)；改 add/del 即可套其他题
// 复杂度: O(n^{5/3})，B = n^{2/3}；下标 0-based
// 10.22: 仅把原来的全局变量 / 函数放进结构体（B 改为静态成员供 Q 排序使用），算法代码未改
struct MoUpd{
    int n, m;
    inline static int B;
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
    vector<int> cnt = vector<int>(1000006);
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
};

MoUpd mo;
void solve(){ mo.solve(); }
