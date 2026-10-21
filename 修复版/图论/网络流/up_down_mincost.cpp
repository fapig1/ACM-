#include <bits/stdc++.h>
using namespace std;

using ll = long long;
const ll INF = (1LL << 60);

/*==================================================
                普通最小费用最大流
==================================================*/

struct MinCostMaxFlow {
    struct Edge {
        int to, rev;
        ll cap, cost;
    };

    int n;
    vector<vector<Edge>> g;

    MinCostMaxFlow(int n) : n(n), g(n) {}

    // 返回正向边在 g[u] 中的下标
    int addEdge(int u, int v, ll cap, ll cost) {
        int id = g[u].size();

        Edge a{v, (int)g[v].size(), cap, cost};
        Edge b{u, (int)g[u].size(), 0, -cost};

        g[u].push_back(a);
        g[v].push_back(b);

        return id;
    }

    // 最多发送 limit 单位流
    // 返回 {实际流量, 最小费用}
    pair<ll, ll> minCostMaxFlow(
        int s,
        int t,
        ll limit = INF
    ) {
        ll flow = 0;
        ll cost = 0;

        vector<ll> dist(n);
        vector<int> inq(n);
        vector<int> preV(n), preE(n);

        while (flow < limit) {
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

            if (dist[t] == INF)
                break;

            ll f = limit - flow;

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


/*==================================================
          带费用上下界最小费用可行循环流

    addEdge(u, v, lower, upper, cost)

    solve():
        返回 {是否存在可行流, 最小费用}
==================================================*/

struct LowerBoundMinCostFlow {

    struct OriginalEdge {
        int u;
        int id;
        ll lower;
        ll upper;
    };

    int n;
    int SS, TT;

    MinCostMaxFlow mf;

    // balance[i] = 固定下界造成的 流入 - 流出
    vector<ll> balance;

    // 下界部分已经必然产生的费用
    ll baseCost = 0;

    vector<OriginalEdge> edges;

    LowerBoundMinCostFlow(int n)
        : n(n),
          SS(n),
          TT(n + 1),
          mf(n + 2),
          balance(n, 0) {}

    // 返回这条原边的编号
    int addEdge(
        int u,
        int v,
        ll lower,
        ll upper,
        ll cost
    ) {
        // 下界部分是必须走的
        baseCost += lower * cost;

        // 固定 lower 后造成节点流量不平衡
        balance[u] -= lower;
        balance[v] += lower;

        // 剩余还能流 upper-lower
        int id = mf.addEdge(
            u,
            v,
            upper - lower,
            cost
        );

        edges.push_back({
            u,
            id,
            lower,
            upper
        });

        return (int)edges.size() - 1;
    }

    // 返回 {是否可行, 最小费用}
    pair<bool, ll> solve() {

        ll need = 0;

        for (int i = 0; i < n; i++) {

            if (balance[i] > 0) {
                // 流入过多，需要把流送出去
                mf.addEdge(
                    SS,
                    i,
                    balance[i],
                    0
                );

                need += balance[i];
            }

            else if (balance[i] < 0) {
                // 流出过多，需要补充流入
                mf.addEdge(
                    i,
                    TT,
                    -balance[i],
                    0
                );
            }
        }

        auto [flow, cost]
            = mf.minCostMaxFlow(SS, TT, need);

        if (flow != need) {
            return {false, 0};
        }

        return {
            true,
            baseCost + cost
        };
    }

    // solve() 后可以查询某条原边最终流量
    ll getFlow(int edgeId) {
        auto &x = edges[edgeId];

        ll remain = mf.g[x.u][x.id].cap;

        ll extra =
            (x.upper - x.lower) - remain;

        return x.lower + extra;
    }
};
