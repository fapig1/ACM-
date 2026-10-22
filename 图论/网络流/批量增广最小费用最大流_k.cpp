// 批量增广最小费用最大流，支持流量上限 K
/*
适用：
1. 在流量不超过 K 的条件下，优先最大化流量，再最小化费用。
2. 不传 K 时，求最小费用最大流。
3. 允许负费用边，但初始残量网络不能含负费用环。
   例如：初始分层 DAG 可以包含负费用边。
4. 容量必须非负，源点和汇点必须不同。
5. 同费用最短增广路较多时，批量增广通常更有优势。

实现：
SPFA 求最短距离；
然后只使用满足 dist[v] == dist[u] + cost 的边，
用 Dinic 批量增广，直到当前最短路无法继续增广或达到 K。

复杂度与限制：
1. 单次 SPFA 最坏 O(VE)。
2. 固定最短距离后的 Dinic 部分，一般图最坏 O(V^2 * E)。
3. 批量增广减少最短路计算次数，但不消除 SPFA 的退化风险。
4. 若每轮只能增广一条路径，额外的 BFS、DFS 可能没有收益。
5. 不检测负费用环，不适合直接处理含负费用环的网络。
6. 最短距离的绝对值应小于 INF；
   总流量、总费用及中间乘加结果需在 long long 安全范围内。
7. DFS 使用递归，路径特别深时需要注意栈空间。
8. 点编号为 0 ~ n-1，支持重边。
9. 运行后会修改残量网络；K 表示本次调用新增流量的上限。
*/
struct MinCostMaxFlowBatch {
    static constexpr ll INF = (1LL << 62);

    struct Edge {
        int to, rev;
        ll cap, cost;
    };

    int n;
    vector<vector<Edge>> g;
    vector<ll> dist;
    vector<int> inq, level, cur;

    MinCostMaxFlowBatch(int n)
        : n(n), g(n), dist(n), inq(n), level(n), cur(n) {}

    // 添加有向边，以及对应的反向残量边
    void addEdge(int u, int v, ll cap, ll cost) {
        int iu = (int)g[u].size();
        int iv = (int)g[v].size();

        // 处理自环的反向边编号
        if (u == v) ++iv;

        g[u].push_back({v, iv, cap, cost});
        g[v].push_back({u, iu, 0, -cost});
    }

    // 在当前残量网络上求费用最短路
    bool spfa(int s, int t) {
        fill(dist.begin(), dist.end(), INF);
        fill(inq.begin(), inq.end(), 0);

        queue<int> q;
        dist[s] = 0;
        inq[s] = 1;
        q.push(s);

        while (!q.empty()) {
            int u = q.front();
            q.pop();
            inq[u] = 0;

            for (const auto &e : g[u]) {
                if (e.cap > 0 &&
                    dist[e.to] > dist[u] + e.cost) {

                    dist[e.to] = dist[u] + e.cost;

                    if (!inq[e.to]) {
                        inq[e.to] = 1;
                        q.push(e.to);
                    }
                }
            }
        }

        return dist[t] != INF;
    }

    // 仅在满足最短路等式的残量边上建立层次图
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
                    dist[e.to] == dist[u] + e.cost) {

                    level[e.to] = level[u] + 1;
                    q.push(e.to);
                }
            }
        }

        return level[t] != -1;
    }

    // 沿层次递增的最短路边批量推流
    // 实际推送量不会超过 limit
    ll dfs(int u, int t, ll limit) {
        if (u == t || limit == 0) return limit;

        ll sent = 0;

        // 当前弧优化
        for (int &i = cur[u]; i < (int)g[u].size(); ++i) {
            Edge &e = g[u][i];

            if (e.cap <= 0 ||
                level[e.to] != level[u] + 1 ||
                dist[e.to] != dist[u] + e.cost) {
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
    // 如果 flow < k，说明无法达到指定流量 k
    // 不传 k 时，以 long long 最大值作为上限
    pair<ll, ll> minCostMaxFlow(
        int s, int t, ll k = LLONG_MAX
    ) {
        assert(s != t && k >= 0);

        ll flow = 0;
        ll cost = 0;

        // 第一层：达到流量上限后，不再运行 SPFA
        while (flow < k && spfa(s, t)) {

            // 第二层：在当前最短路子图上批量增广
            while (flow < k && bfs(s, t)) {
                fill(cur.begin(), cur.end(), 0);

                // 第三层：每次最多补到 k
                while (flow < k) {
                    ll pushed = dfs(s, t, k - flow);

                    if (pushed == 0) break;

                    flow += pushed;

                    // 本轮每单位流量的路径费用均为 dist[t]
                    cost += pushed * dist[t];
                }
            }
        }

        return {flow, cost};
    }
};