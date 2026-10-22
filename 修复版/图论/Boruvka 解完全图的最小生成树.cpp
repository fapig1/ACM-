#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// 【速查】Boruvka 完全图最小生成树
// 用途: n 个点两两有边、边权由公式给出，边太多不能全建。本例 w(i, j) = (a_i + a_j) mod k
// 用法: ll ans = boruvkaModSum(a, k);   // a 为 0-based 点权
// 套路: 每轮给每个连通块找"连到别的块的最小边"，全部合并；块数每轮至少减半，最多 log n 轮
//       换题只需改 bestEdge 里"在别的块中找使 w(j, ?) 最小的点"的查询方式（这里用 multiset 二分）
// 复杂度: O(n log² n)
ll boruvkaModSum(vector<ll> a, ll k) {
    int n = a.size();
    for (auto& x : a) x %= k;
    vector<int> fa(n);
    iota(fa.begin(), fa.end(), 0);
    auto find = [&](int x) { while (fa[x] != x) x = fa[x] = fa[fa[x]]; return x; };
    multiset<pair<ll, int>> st;                 // 所有点 (值, 编号)
    for (int i = 0; i < n; i++) st.insert({a[i], i});
    ll ans = 0;
    int merged = 0;
    vector<vector<int>> block(n);
    while (merged < n - 1) {
        for (auto& b : block) b.clear();
        for (int i = 0; i < n; i++) block[find(i)].push_back(i);
        vector<array<ll, 3>> best(n, {LLONG_MAX, -1, -1});   // 每个块的最小出边 (w, u, v)
        for (int r = 0; r < n; r++) {
            if (block[r].empty()) continue;
            for (int j : block[r]) st.erase({a[j], j});   // 暂时删掉本块的点，set 里只剩别的块
            for (int j : block[r]) {
                // (a_j + x) mod k 最小: 要么取 x >= k - a_j 中最小的（和 >= k），要么取最小的 x
                for (ll tar : {k - a[j], 0LL}) {
                    auto it = st.lower_bound({tar, -1});
                    if (it == st.end()) continue;
                    ll w = (it->first + a[j]) % k;
                    if (w < best[r][0]) best[r] = {w, j, it->second};
                }
            }
            for (int j : block[r]) st.insert({a[j], j});
        }
        for (int r = 0; r < n; r++) {
            if (best[r][1] == -1) continue;
            int x = find(best[r][1]), y = find(best[r][2]);
            if (x != y) { fa[x] = y; ans += best[r][0]; merged++; }
        }
    }
    return ans;
}

// 示例: 读 t 组，每组 n k 和 n 个数，输出 MST 权值和
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int t;
    cin >> t;
    while (t--) {
        int n; ll k;
        cin >> n >> k;
        vector<ll> a(n);
        for (auto& x : a) cin >> x;
        cout << boruvkaModSum(a, k) << "\n";
    }
    return 0;
}
