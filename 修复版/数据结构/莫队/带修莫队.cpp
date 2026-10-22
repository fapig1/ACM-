#include <bits/stdc++.h>
using namespace std;
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
