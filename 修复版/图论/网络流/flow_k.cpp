#include <bits/stdc++.h>
using namespace std;
using ll = long long;
const ll INF = 4e18;

// 【速查】限额 k 的最小费用流（SPFA 版 SSP，结构体封装）
// 用法: MinCostFlowK mf(点数);  mf.addEdge(u, v, cap, cost);  auto [flow, cost] = mf.minCostFlow(s, t, k);
// 返回: {实际流量, 最小费用}；flow < k 说明达不到 k；k 传 INF 就是最小费用最大流
// 复杂度: O(k × SPFA)；可有负费用边，不能有负费用环；点 0-based
// 10.22: 原版是全局 n / g + 裸函数，已封装为结构体（接口与 dij_k_flow_cost 一致）
struct MinCostFlowK {
    struct Edge { int to, rev; ll cap, cost; };
    int n;
    vector<vector<Edge>> g;

    MinCostFlowK(int n) : n(n), g(n) {}
    void addEdge(int u, int v, ll cap, ll cost) {
        g[u].push_back({v, (int)g[v].size(), cap, cost});
        g[v].push_back({u, (int)g[u].size() - 1, 0, -cost});
    }
    pair<ll, ll> minCostFlow(int s, int t, ll k) {
        ll flow = 0, cost = 0;
        vector<ll> dist(n);
        vector<int> inq(n), preV(n), preE(n);
        while (flow < k) {
            fill(dist.begin(), dist.end(), INF);
            fill(inq.begin(), inq.end(), 0);
            queue<int> q;
            dist[s] = 0; q.push(s); inq[s] = 1;
            while (!q.empty()) {
                int u = q.front(); q.pop(); inq[u] = 0;
                for (int i = 0; i < (int)g[u].size(); i++) {
                    Edge& e = g[u][i];
                    if (e.cap > 0 && dist[e.to] > dist[u] + e.cost) {
                        dist[e.to] = dist[u] + e.cost;
                        preV[e.to] = u; preE[e.to] = i;
                        if (!inq[e.to]) { inq[e.to] = 1; q.push(e.to); }
                    }
                }
            }
            if (dist[t] == INF) break;             // 已经无法继续增广
            ll f = k - flow;                        // 本次最多只能补到 k
            for (int v = t; v != s; v = preV[v]) f = min(f, g[preV[v]][preE[v]].cap);
            for (int v = t; v != s; v = preV[v]) {
                Edge& e = g[preV[v]][preE[v]];
                e.cap -= f;
                g[v][e.rev].cap += f;
            }
            flow += f;
            cost += f * dist[t];
        }
        return {flow, cost};
    }
};
