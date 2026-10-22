#include <bits/stdc++.h>
using namespace std;

// 【速查】差分约束（P5960）
// 用途: 一组不等式 x_a - x_b <= c，求一组整数解或判无解
// 用法: DiffConstraints d(n);  d.add(a, b, c);   // 约束 x_a - x_b <= c，变量 1..n
//       常用翻译:  x_a - x_b >= c -> add(b, a, -c);   x_a == x_b + c -> add(a, b, c) 且 add(b, a, -c)
//       auto x = d.solve();   // 无解（有负环）返回空 vector；否则 x[1..n] 是一组解（都 <= 0）
// 求"最大解/最小解": 以某点为源求最短路得到各变量相对它的最大值；求最小值就把不等式反过来求最长路
// 复杂度: SPFA 最坏 O(nm)
struct DiffConstraints {
    int n;
    vector<vector<pair<int, long long>>> g;
    DiffConstraints(int n) : n(n), g(n + 1) {}
    void add(int a, int b, long long c) { g[b].push_back({a, c}); }   // x_a <= x_b + c：边 b -> a，权 c
    vector<long long> solve() {
        vector<long long> dist(n + 1, 0);       // 相当于超级源点 0 向每个点连 0 边
        vector<int> cnt(n + 1, 0);
        vector<char> inq(n + 1, 1);
        deque<int> q;
        for (int i = 1; i <= n; i++) q.push_back(i);
        while (!q.empty()) {
            int u = q.front(); q.pop_front(); inq[u] = 0;
            for (auto [v, w] : g[u]) {
                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    if (!inq[v]) {
                        if (++cnt[v] > n) return {};   // 入队超过 n 次 => 负环 => 无解
                        inq[v] = 1; q.push_back(v);
                    }
                }
            }
        }
        return dist;
    }
};
