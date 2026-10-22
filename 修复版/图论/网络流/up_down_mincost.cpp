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

    addEdge(u, v, lower, upper, cost)   要求 0 <= lower <= upper，cost 可正可负

    solve():
        返回 {是否存在可行流, 最小费用}；每个对象只调用一次

    负费用（含负费用环）的处理（10.22 修复）:
        原版把负费用边原样放进残量网络，遇到负费用环时 SPFA 死循环，
        且 SSP 只沿 SS->TT 增广，找不到"环流本身"带来的收益。
        现在负费用边先强制流满 upper（费用、盈亏立即结算），残量网络只放一条
        反向"退流"边 v->u（容量 upper-lower，费用 -cost > 0）。
        这样初始残量网络费用全部非负、没有负环，SSP 求出的就是全局最小费用循环流。
==================================================*/

// 【速查】上下界最小费用可行循环流
// 用途: 每条边流量必须在 [lower, upper] 内、每个点流入=流出，求最小总费用（无源汇的循环流）
// 用法: LowerBoundMinCostFlow L(n);  int id = L.addEdge(u, v, lower, upper, cost);  auto [ok, cost] = L.solve();
//       L.getFlow(id) -> 该边最终流量；点编号 0..n-1；solve() 只能调一次
// 有源汇: 加一条 t->s、[0, INF]、费用 0 的边，就转成循环流
// 复杂度: 一次最小费用流；支持负费用和负费用环
struct LowerBoundMinCostFlow {

    struct OriginalEdge {
        int u;      // 残量边的起点：非负费用边为 u，负费用边为 v
        int id;     // 残量边在 mf.g[u] 中的下标
        ll lower;
        ll upper;
        bool neg;   // 是否按"先流满再退流"处理
    };

    int n;
    int SS, TT;

    MinCostMaxFlow mf;

    vector<ll> balance;   // 节点净流入，>0 时由 SS 补入，<0 时流向 TT

    ll baseCost = 0;

    vector<OriginalEdge> edges;

    LowerBoundMinCostFlow(int n)
        : n(n),
          SS(n),
          TT(n + 1),
          mf(n + 2),
          balance(n, 0) {}

    int addEdge(
        int u,
        int v,
        ll lower,
        ll upper,
        ll cost
    ) {
        if (cost >= 0) {
            // 先流 lower，剩余 upper-lower 按原方向可选
            baseCost += lower * cost;
            balance[u] -= lower;
            balance[v] += lower;
            int id = mf.addEdge(u, v, upper - lower, cost);
            edges.push_back({u, id, lower, upper, false});
        } else {
            // 先流满 upper，剩余 upper-lower 作为 v->u 的退流边，费用 -cost > 0
            baseCost += upper * cost;
            balance[u] -= upper;
            balance[v] += upper;
            int id = mf.addEdge(v, u, upper - lower, -cost);
            edges.push_back({v, id, lower, upper, true});
        }
        return (int)edges.size() - 1;
    }

    pair<bool, ll> solve() {

        ll need = 0;

        for (int i = 0; i < n; i++) {

            if (balance[i] > 0) {
                mf.addEdge(
                    SS,
                    i,
                    balance[i],
                    0
                );

                need += balance[i];
            }

            else if (balance[i] < 0) {
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

        // 残量边上实际走过的流量
        ll used =
            (x.upper - x.lower) - mf.g[x.u][x.id].cap;

        return x.neg ? x.upper - used : x.lower + used;
    }
};
