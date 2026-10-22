#include <bits/stdc++.h>
using namespace std;
using ll = long long;
const ll INF = 4e18;

//指定流量K的最小费用最大流
// "限额 k 的最小费用流"(SPFA 版 SSP)
// 【速查】限额 k 的最小费用流（SPFA 版）
// 用法: FlowK F(n);  F.addEdge(u, v, cap, cost);  auto [flow, cost] = F.minCostFlow(s, t, k);   // 点 0..n-1
// 返回: {实际流量, 最小费用}；flow < k 说明达不到 k
// 复杂度: O(k × SPFA)；无负费用环
// 10.22: 仅把原来的全局图 n / g 与函数放进结构体（注释里的 addEdge 改为成员函数），算法代码未改
struct FlowK {
    struct Edge { int to, rev; ll cap, cost; };
    int n;                      // 节点数(编号 0..n-1)
    vector<vector<Edge>> g;     // 加边时成对存放(正向 cap/反向 0, 反向 cost 取负)

    FlowK(int n) : n(n), g(n) {}

    // 加边辅助
    void addEdge(int u, int v, ll cap, ll cost) {
        g[u].push_back({v, (int)g[v].size(), cap, cost});
        g[v].push_back({u, (int)g[u].size() - 1, 0, -cost});
    }

    // 返回 {实际流量, 最小费用}
    // 如果 flow < k，说明无法达到指定流量 k
    pair<ll, ll> minCostFlow(int s, int t, ll k) {
        ll flow = 0;
        ll cost = 0;

        vector<ll> dist(n);
        vector<int> inq(n);
        vector<int> preV(n), preE(n);

        while (flow < k) {
            fill(dist.begin(), dist.end(), INF);
            fill(inq.begin(), inq.end(), 0);

            queue<int> q;

            dist[s] = 0;
            q.push(s);
            inq[s] = 1;

            while (!q.empty()) {
                int u = q.front();
                q.pop();
                inq[u] = 0;

                for (int i = 0; i < (int)g[u].size(); i++) {
                    Edge &e = g[u][i];

                    if (e.cap > 0 &&
                        dist[u] != INF &&
                        dist[e.to] > dist[u] + e.cost) {

                        dist[e.to] = dist[u] + e.cost;
                        preV[e.to] = u;
                        preE[e.to] = i;

                        if (!inq[e.to]) {
                            inq[e.to] = 1;
                            q.push(e.to);
                        }
                        }
                }
            }

            // 已经无法继续增广
            if (dist[t] == INF)
                break;

            // 本次最多只能补到 k
            ll f = k - flow;

            for (int v = t; v != s; v = preV[v]) {
                int u = preV[v];
                int id = preE[v];

                f = min(f, g[u][id].cap);
            }

            for (int v = t; v != s; v = preV[v]) {
                int u = preV[v];
                int id = preE[v];

                Edge &e = g[u][id];

                e.cap -= f;
                g[v][e.rev].cap += f;
            }

            flow += f;
            cost += f * dist[t];
        }

        return {flow, cost};
    }
};
