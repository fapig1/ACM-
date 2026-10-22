#include <bits/stdc++.h>
using namespace std;
// 【速查】批量增广最小费用最大流（势能 + Dijkstra + Dinic，可限流量 K）
// 用法: MinCostMaxFlowBatch mf(点数);  mf.addEdge(u, v, cap, cost);  auto [flow, cost] = mf.minCostMaxFlow(s, t, K);
// 返回: 流量不超过 K 时的 {最大流量, 最小费用}；不传 K 就是最小费用最大流
// 前提: 可有负费用边（第一次用 Bellman-Ford 求势能），源点可达负费用环会抛异常；点 0..n-1
// 复杂度: 每轮 Dijkstra O(m log n) + Dinic；比 SPFA 版稳定
// 10.22: 来自 1dfe9f8（czc560）新增，修复版内容未改

// 势能 + Dijkstra + Dinic 批量增广最小费用最大流
/*
适用：
1. 在流量不超过 K 的条件下，优先最大化流量，再最小化费用。
2. 不传 K 时，求最小费用最大流。
3. 允许负费用边，但初始残量网络不能含负费用环。
4. 同费用最短增广路较多时，批量增广通常更有优势。

实现：
1. 初始可用边费用均非负：势能初始化为 0。
2. 否则，用一次 Bellman-Ford 初始化势能。
3. 每轮使用 Dijkstra 求约化费用最短路，并更新势能。
4. 在约化费用为 0 的残量边上，用 Dinic 批量增广。

限制：
1. 负费用边情况下，势能初始化最坏 O(VE)。
2. 本模板检测到从源点可达的负费用环时会抛出异常。
3. 容量必须非负，源点与汇点必须不同。
4. 距离、势能、总费用及中间运算不能溢出 long long；
   最短路距离需小于 INF。
5. DFS 使用递归，路径特别深时需注意栈空间。
6. 点编号为 0 ~ n-1，支持重边。
7. 运行后修改残量网络，K 是本次调用新增流量的上限。
*/
struct MinCostMaxFlowBatch {
    using ll = long long;
    static constexpr ll INF = (1LL << 62);

    struct Edge {
        int to, rev;
        ll cap, cost;
    };

    int n;
    vector<vector<Edge>> g;
    vector<ll> h, dist;       // 势能、约化费用最短距离
    vector<int> level, cur;

    MinCostMaxFlowBatch(int n)
        : n(n), g(n), h(n), dist(n), level(n), cur(n) {}

    void addEdge(int u, int v, ll cap, ll cost) {
        int iu = (int)g[u].size();
        int iv = (int)g[v].size();

        if (u == v) ++iv;

        g[u].push_back({v, iv, cap, cost});
        g[v].push_back({u, iu, 0, -cost});
    }

    // 初始化势能：保证从源点可达的残量边约化费用非负
    void initPotential(int s) {
        fill(h.begin(), h.end(), 0);

        bool hasNegative = false;
        for (int u = 0; u < n; ++u) {
            for (const auto &e : g[u]) {
                if (e.cap > 0 && e.cost < 0) {
                    hasNegative = true;
                }
            }
        }

        // 当前所有可用边费用非负，直接使用零势能
        if (!hasNegative) return;

        // 有负费用边，用 Bellman-Ford 求初始最短距离
        fill(h.begin(), h.end(), INF);
        h[s] = 0;

        for (int round = 1; round < n; ++round) {
            bool changed = false;

            for (int u = 0; u < n; ++u) {
                if (h[u] == INF) continue;

                for (const auto &e : g[u]) {
                    if (e.cap > 0 &&
                        h[e.to] > h[u] + e.cost) {

                        h[e.to] = h[u] + e.cost;
                        changed = true;
                    }
                }
            }

            if (!changed) break;
        }

        // 检测从源点可达的负费用环
        for (int u = 0; u < n; ++u) {
            if (h[u] == INF) continue;

            for (const auto &e : g[u]) {
                if (e.cap > 0 &&
                    h[e.to] > h[u] + e.cost) {

                    throw runtime_error(
                        "Reachable negative-cost cycle"
                    );
                }
            }
        }

        // 初始不可达点不会因后续增广变成源点可达
        for (auto &x : h) {
            if (x == INF) x = 0;
        }
    }

    // 约化费用 w(u,v) = cost(u,v) + h[u] - h[v]
    bool dijkstra(int s, int t) {
        fill(dist.begin(), dist.end(), INF);

        priority_queue<
            pair<ll, int>,
            vector<pair<ll, int>>,
            greater<pair<ll, int>>
        > pq;

        dist[s] = 0;
        pq.push({0, s});

        // 跑完所有可达点，再统一更新势能
        // 此处不要在第一次弹出 t 时直接退出
        while (!pq.empty()) {
            auto [du, u] = pq.top();
            pq.pop();

            if (du != dist[u]) continue;

            for (const auto &e : g[u]) {
                if (e.cap <= 0) continue;

                ll w = e.cost + h[u] - h[e.to];

                if (dist[e.to] > du + w) {
                    dist[e.to] = du + w;
                    pq.push({dist[e.to], e.to});
                }
            }
        }

        if (dist[t] == INF) return false;

        // 仅更新本轮可达点的势能
        for (int i = 0; i < n; ++i) {
            if (dist[i] != INF) {
                h[i] += dist[i];
            }
        }

        return true;
    }

    // 在约化费用为 0 的残量边上建立层次图
    bool bfs(int s, int t) {
        fill(level.begin(), level.end(), -1);

        queue<int> q;
        level[s] = 0;
        q.push(s);

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (const auto &e : g[u]) {
                if (e.cap > 0 &&
                    level[e.to] == -1 &&
                    e.cost + h[u] - h[e.to] == 0) {

                    level[e.to] = level[u] + 1;
                    q.push(e.to);
                }
            }
        }

        return level[t] != -1;
    }

    // 沿层次递增、约化费用为 0 的边批量推流
    ll dfs(int u, int t, ll limit) {
        if (u == t || limit == 0) return limit;

        ll sent = 0;

        for (int &i = cur[u]; i < (int)g[u].size(); ++i) {
            Edge &e = g[u][i];

            if (e.cap <= 0 ||
                level[e.to] != level[u] + 1 ||
                e.cost + h[u] - h[e.to] != 0) {
                continue;
            }

            ll pushed = dfs(
                e.to, t, min(limit - sent, e.cap)
            );

            if (pushed == 0) continue;

            e.cap -= pushed;
            g[e.to][e.rev].cap += pushed;
            sent += pushed;

            if (sent == limit) return sent;
        }

        return sent;
    }

    // 返回 {实际流量, 对应的最小费用}
    // 如果 flow < k，说明无法送满 k
    pair<ll, ll> minCostMaxFlow(
        int s, int t, ll k = LLONG_MAX
    ) {
        assert(s != t && k >= 0);

        if (k == 0) return {0, 0};

        initPotential(s);

        ll flow = 0;
        ll cost = 0;

        while (flow < k && dijkstra(s, t)) {
            // 更新势能后，本轮最短路的原费用
            ll pathCost = h[t] - h[s];

            // 将当前最短路尽量增广完
            while (flow < k && bfs(s, t)) {
                fill(cur.begin(), cur.end(), 0);

                while (flow < k) {
                    ll pushed = dfs(s, t, k - flow);

                    if (pushed == 0) break;

                    flow += pushed;
                    cost += pushed * pathCost;
                }
            }
        }

        return {flow, cost};
    }
};