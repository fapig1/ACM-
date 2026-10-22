#include <bits/stdc++.h>
using namespace std;
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
