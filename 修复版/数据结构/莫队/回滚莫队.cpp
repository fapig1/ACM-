#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// 【速查】回滚莫队（只加不删）
// 用途: 信息只能方便地"加入"、难"删除"时（区间最值类）。本例: 区间内相同数的最远距离（P5906）
// 用法: RollbackMo mo;  mo.a = 数组（0-based，值已离散化到 0..K-1）;  mo.init(K);
//       auto res = mo.solve(qs);   // qs[i] = {l, r}（0-based 闭区间）
// 做法: 左端点同块的询问按 r 升序一起处理：右指针只右移；左边每次从块的右边界往左加入，
//       记下改动，答完后回滚；l、r 在同一块的小询问直接暴力
// 改题: 改 init / addRight / addLeft / 回滚记录的内容 / brute
// 复杂度: O(n√q)
struct RollbackMo {
    vector<int> a, fst, lst;                   // fst / lst: 当前区间内每个值第一次 / 最后一次出现的位置
    void init(int K) { fst.assign(K, -1); lst.assign(K, -1); }
    ll brute(int l, int r) {                   // 小区间暴力（用完复原）
        ll best = 0;
        vector<int> touched;
        for (int j = l; j <= r; j++) {
            int v = a[j];
            if (fst[v] == -1) { fst[v] = j; touched.push_back(v); }
            best = max(best, (ll)(j - fst[v]));
        }
        for (int v : touched) fst[v] = -1;
        return best;
    }
    vector<ll> solve(const vector<pair<int, int>>& qs) {
        int n = a.size(), q = qs.size();
        int B = max(1, (int)(n / sqrt(max(1.0, q * 2.0 / 3.0))));
        vector<int> ord(q);
        iota(ord.begin(), ord.end(), 0);
        sort(ord.begin(), ord.end(), [&](int x, int y) {
            if (qs[x].first / B != qs[y].first / B) return qs[x].first / B < qs[y].first / B;
            return qs[x].second < qs[y].second;
        });
        vector<ll> ans(q);
        int curBlock = -1, r = -1;
        ll res = 0;
        for (int id : ord) {
            auto [ql, qr] = qs[id];
            int b = ql / B;
            if (b != curBlock) {                   // 换块：清空，右指针放到块的右边界
                fill(fst.begin(), fst.end(), -1); fill(lst.begin(), lst.end(), -1);
                res = 0; curBlock = b; r = (b + 1) * B - 1;
            }
            if (b == qr / B) { ans[id] = brute(ql, qr); continue; }
            while (r < qr) {                       // 右边只加
                int v = a[++r];
                if (fst[v] == -1) fst[v] = r;
                lst[v] = r;
                res = max(res, (ll)(lst[v] - fst[v]));
            }
            ll saved = res;
            vector<array<int, 3>> stk;             // (值, 旧 fst, 旧 lst)，用于回滚
            for (int l = min(n, (b + 1) * B) - 1; l >= ql; l--) {
                int v = a[l];
                stk.push_back({v, fst[v], lst[v]});
                if (lst[v] == -1) lst[v] = l;
                fst[v] = l;
                res = max(res, (ll)(lst[v] - fst[v]));
            }
            ans[id] = res;
            res = saved;
            while (!stk.empty()) { auto [v, f, l2] = stk.back(); stk.pop_back(); fst[v] = f; lst[v] = l2; }
        }
        return ans;
    }
};

// 示例（P5906）: 读 n、n 个数、q、q 个询问（1-based），输出区间内相同数的最远距离
void solve() {
    int n;
    cin >> n;
    RollbackMo mo;
    mo.a.resize(n);
    for (auto& x : mo.a) cin >> x;
    vector<int> b = mo.a;
    sort(b.begin(), b.end());
    b.erase(unique(b.begin(), b.end()), b.end());
    for (auto& x : mo.a) x = lower_bound(b.begin(), b.end(), x) - b.begin();
    mo.init(b.size());
    int q;
    cin >> q;
    vector<pair<int, int>> qs(q);
    for (auto& [l, r] : qs) { cin >> l >> r; l--; r--; }
    for (ll v : mo.solve(qs)) cout << v << '\n';
}
