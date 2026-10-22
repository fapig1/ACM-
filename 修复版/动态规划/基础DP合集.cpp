#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// 【速查】背包合集（容量 W，物品 (重量 w, 价值 v)，求最大价值）
// knap01(W, items)            01 背包，每个物品最多 1 个。倒序枚举容量。O(nW)
// knapComplete(W, items)      完全背包，每个物品无限个。正序枚举容量。O(nW)
// knapMultiBinary(W, items3)  多重背包 (w, v, 个数 c)，二进制拆分成 01 背包。O(W Σlog c)
// knapMultiQueue(W, items3)   多重背包单调队列优化。O(nW)，c 很大时用它
// knapGroup(W, groups)        分组背包，每组最多选 1 个。O(W × 总物品数)
// 返回: dp[0..W]，dp[j] = 容量不超过 j 的最大价值（要"恰好装满"就把 dp 初值改为 -INF、dp[0]=0）
vector<ll> knap01(int W, const vector<pair<int, ll>>& items) {
    vector<ll> dp(W + 1, 0);
    for (auto [w, v] : items) for (int j = W; j >= w; j--) dp[j] = max(dp[j], dp[j - w] + v);
    return dp;
}
vector<ll> knapComplete(int W, const vector<pair<int, ll>>& items) {
    vector<ll> dp(W + 1, 0);
    for (auto [w, v] : items) for (int j = w; j <= W; j++) dp[j] = max(dp[j], dp[j - w] + v);
    return dp;
}
vector<ll> knapMultiBinary(int W, const vector<array<ll, 3>>& items) {
    vector<pair<int, ll>> split;
    for (auto [w, v, c] : items)
        for (ll k = 1; c > 0; k <<= 1) { ll t = min(k, c); split.push_back({(int)(w * t), v * t}); c -= t; }
    vector<pair<int, ll>> ok;
    for (auto& p : split) if (p.first <= W) ok.push_back(p);
    return knap01(W, ok);
}
vector<ll> knapMultiQueue(int W, const vector<array<ll, 3>>& items) {
    vector<ll> dp(W + 1, 0), pre;
    for (auto [w, v, c] : items) {
        if (w == 0) { for (auto& x : dp) x += v * c; continue; }
        pre = dp;
        for (int r = 0; r < w; r++) {               // 按 j mod w 分组，组内是滑动窗口最大值
            deque<pair<ll, ll>> q;                   // (k, pre[r + k w] - k v)
            for (ll k = 0; r + k * w <= W; k++) {
                ll val = pre[r + k * w] - k * v;
                while (!q.empty() && q.back().second <= val) q.pop_back();
                q.push_back({k, val});
                while (q.front().first < k - c) q.pop_front();
                dp[r + k * w] = q.front().second + k * v;
            }
        }
    }
    return dp;
}
vector<ll> knapGroup(int W, const vector<vector<pair<int, ll>>>& groups) {
    vector<ll> dp(W + 1, 0), pre;
    for (auto& g : groups) {
        pre = dp;                                    // 只从"上一组"的状态转移，保证每组只选一个（重量为 0 的物品也安全）
        for (int j = 0; j <= W; j++)
            for (auto [w, v] : g) if (w <= j) dp[j] = max(dp[j], pre[j - w] + v);
    }
    return dp;
}

// 【速查】区间 DP：石子合并（相邻两堆合并，代价为两堆之和，求最小总代价，P1775）
// stoneMerge(a)       朴素 O(n³)：dp[l][r] = min_k dp[l][k] + dp[k+1][r] + sum(l, r)
// stoneMergeKnuth(a)  四边形不等式优化 O(n²)：最优断点 opt[l][r-1] <= opt[l][r] <= opt[l+1][r]
// 环形: 把数组复制一份接在后面，取所有长度为 n 的区间的最小值
// 适用 Knuth 的条件: 代价 w 满足四边形不等式且区间包含单调（区间和、区间平方和等）
ll stoneMerge(const vector<ll>& a) {
    int n = a.size();
    if (n <= 1) return 0;
    vector<ll> s(n + 1, 0);
    for (int i = 0; i < n; i++) s[i + 1] = s[i] + a[i];
    vector<vector<ll>> dp(n, vector<ll>(n, 0));
    for (int len = 2; len <= n; len++)
        for (int l = 0; l + len - 1 < n; l++) {
            int r = l + len - 1; dp[l][r] = LLONG_MAX;
            for (int k = l; k < r; k++) dp[l][r] = min(dp[l][r], dp[l][k] + dp[k + 1][r]);
            dp[l][r] += s[r + 1] - s[l];
        }
    return dp[0][n - 1];
}
ll stoneMergeKnuth(const vector<ll>& a) {
    int n = a.size();
    if (n <= 1) return 0;
    vector<ll> s(n + 1, 0);
    for (int i = 0; i < n; i++) s[i + 1] = s[i] + a[i];
    vector<vector<ll>> dp(n, vector<ll>(n, 0));
    vector<vector<int>> opt(n, vector<int>(n, 0));
    for (int i = 0; i < n; i++) opt[i][i] = i;
    for (int len = 2; len <= n; len++)
        for (int l = 0; l + len - 1 < n; l++) {
            int r = l + len - 1; dp[l][r] = LLONG_MAX;
            for (int k = opt[l][r - 1]; k <= min(r - 1, opt[l + 1][r]); k++) {
                ll v = dp[l][k] + dp[k + 1][r];
                if (v < dp[l][r]) dp[l][r] = v, opt[l][r] = k;
            }
            dp[l][r] += s[r + 1] - s[l];
        }
    return dp[0][n - 1];
}

// 【速查】决策单调性 · 分治优化（把序列分成 k 段，最小化每段代价之和，CF321E / P4072 类）
// 用法: ll ans = divideConquerDP(n, k, cost);   // cost(j, i) = 第 j+1..i 个元素组成一段的代价（1-based 前缀意义）
// 条件: 最优断点随 i 单调不减（cost 满足四边形不等式即可，如 (段和)²、区间逆序对等）
// 复杂度: O(k n log n) 次 cost 调用（cost 要 O(1)，一般用前缀和）
ll divideConquerDP(int n, int k, const function<ll(int, int)>& cost) {
    const ll INF = LLONG_MAX / 4;
    vector<ll> prv(n + 1, INF), cur(n + 1, INF);
    prv[0] = 0;
    function<void(int, int, int, int)> solve = [&](int l, int r, int optl, int optr) {
        if (l > r) return;
        int m = (l + r) / 2, best = optl;
        ll bv = INF;
        for (int j = optl; j <= min(m - 1, optr); j++) {
            if (prv[j] >= INF) continue;
            ll v = prv[j] + cost(j, m);
            if (v < bv) bv = v, best = j;
        }
        cur[m] = bv;
        solve(l, m - 1, optl, best);
        solve(m + 1, r, best, optr);
    };
    for (int t = 1; t <= k; t++) {
        fill(cur.begin(), cur.end(), INF);
        solve(1, n, 0, n - 1);
        swap(prv, cur);
    }
    return prv[n];
}

// 【速查】树形 DP（迭代 DFS 序，不爆栈）
// treeMIS(n, edges, w)        带权最大独立集（没有上司的舞会 P1352）：选的点两两不相邻，点权和最大
// treeKnapsack(n, fa, val, m) 树上背包（选课 P2014）：选一个点必须先选它的父亲（fa[i]=0 表示可直接选），
//                              恰好选 m 个点的最大价值；O(n·m)（按子树大小合并）
// 点 1-based
ll treeMIS(int n, const vector<pair<int, int>>& edges, const vector<ll>& w) {
    vector<vector<int>> g(n + 1);
    for (auto [u, v] : edges) g[u].push_back(v), g[v].push_back(u);
    vector<int> order, fa(n + 1, 0);
    vector<char> vis(n + 1, 0);
    vector<array<ll, 2>> dp(n + 1, {0, 0});            // dp[u][0] 不选 u，dp[u][1] 选 u
    ll total = 0;
    for (int s = 1; s <= n; s++) {
        if (vis[s]) continue;
        order.clear(); order.push_back(s); vis[s] = 1;
        for (size_t i = 0; i < order.size(); i++)
            for (int v : g[order[i]]) if (!vis[v]) vis[v] = 1, fa[v] = order[i], order.push_back(v);
        for (int i = order.size() - 1; i >= 0; i--) {   // BFS 序倒过来 = 先儿子后父亲
            int u = order[i];
            dp[u][1] += w[u];
            if (fa[u]) { dp[fa[u]][0] += max(dp[u][0], dp[u][1]); dp[fa[u]][1] += dp[u][0]; }
        }
        total += max(dp[s][0], dp[s][1]);
    }
    return total;
}
ll treeKnapsack(int n, const vector<int>& fa, const vector<ll>& val, int m) {
    // 0 号点是虚根（必选，价值 0），答案 = dp[0][m + 1]
    const ll NEG = LLONG_MIN / 4;
    vector<vector<int>> ch(n + 1);
    for (int i = 1; i <= n; i++) ch[fa[i]].push_back(i);
    vector<int> order = {0};
    for (size_t i = 0; i < order.size(); i++) for (int c : ch[order[i]]) order.push_back(c);
    vector<vector<ll>> dp(n + 1);
    vector<int> sz(n + 1, 1);
    for (int i = order.size() - 1; i >= 0; i--) {
        int u = order[i];
        dp[u] = {NEG, u ? val[u] : 0};                // dp[u][j]: u 子树里选 j 个（含 u）的最大价值
        for (int c : ch[u]) {
            int lim = min(sz[u] + sz[c], m + 1);
            vector<ll> nd(lim + 1, NEG);
            for (int a = 1; a <= min(sz[u], m + 1); a++) if (dp[u][a] > NEG)
                for (int b = 0; b <= sz[c] && a + b <= lim; b++) {
                    ll vb = b == 0 ? 0 : dp[c][b];
                    if (vb > NEG) nd[a + b] = max(nd[a + b], dp[u][a] + vb);
                }
            sz[u] += sz[c];
            dp[u] = nd;
            dp[c].clear(); dp[c].shrink_to_fit();
        }
        if ((int)dp[u].size() > m + 2) dp[u].resize(m + 2);
    }
    return (int)dp[0].size() > m + 1 ? dp[0][m + 1] : NEG;
}

// 【速查】状压 DP：旅行商 TSP（从 0 出发经过所有点恰好一次再回到 0 的最短回路）
// 用法: ll ans = tsp(d);   // d 为 n×n 距离矩阵（可不对称），n ≤ 20；不可达用很大的数
// 改"路径"（不回起点）: 答案改成 min_i dp[full][i]
// 复杂度: O(2^n n²)，内存 2^n × n 个 long long
ll tsp(const vector<vector<ll>>& d) {
    int n = d.size();
    if (n == 1) return 0;
    const ll INF = LLONG_MAX / 4;
    vector<vector<ll>> dp(1 << n, vector<ll>(n, INF));
    dp[1][0] = 0;
    for (int mask = 1; mask < (1 << n); mask++) {
        if (!(mask & 1)) continue;
        for (int u = 0; u < n; u++) {
            if (dp[mask][u] >= INF || !(mask >> u & 1)) continue;
            for (int v = 0; v < n; v++) if (!(mask >> v & 1))
                dp[mask | 1 << v][v] = min(dp[mask | 1 << v][v], dp[mask][u] + d[u][v]);
        }
    }
    ll ans = INF;
    for (int u = 1; u < n; u++) ans = min(ans, dp[(1 << n) - 1][u] + d[u][0]);
    return ans;
}
