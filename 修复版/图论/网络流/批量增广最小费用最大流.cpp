#include <bits/stdc++.h>
using namespace std;
using ll = long long;
const ll INF = 4e18;
//批量增广最小费用最大流

/*
适用：
1. 在最大化流量的前提下，使总费用最小。
2. 允许负费用边，但初始残量网络不能含负费用环。
   例如：初始分层 DAG 可以包含负费用边。
3. 容量必须非负，源点和汇点必须不同。
4. 同费用最短增广路较多时，批量增广通常更有优势。

实现：
SPFA 求最短距离；
然后只使用满足 dist[v] == dist[u] + cost 的边，
用 Dinic 批量增广，直到当前最短路无法继续增广。

复杂度与限制：
1. 单次 SPFA 最坏 O(VE)。
2. 固定最短距离后的 Dinic 部分，一般图最坏 O(V^2 * E)。
3. 批量增广减少最短路计算次数，但不消除 SPFA 的退化风险。
4. 若每轮只能增广一条路径，额外的 BFS、DFS 可能没有收益。
5. 本模板不检测负费用环，不适合直接处理含负费用环的网络。
6. 距离、总流量、总费用及中间乘加结果均需在 long long 安全范围内。
7. DFS 使用递归，路径特别深时需要注意栈空间。
// 【速查】最小费用最大流（SPFA + 最短路图上 Dinic 多路增广）
// 用法: MinCostMaxFlowBatch mf(点数); mf.addEdge(u, v, cap, cost); auto [flow, cost] = mf.minCostMaxFlow(s, t);
// 适用: 同费用的增广路很多时（如费用种类少）比普通版快；前提同样是无负费用环
// 复杂度: 每轮 SPFA + Dinic，轮数 ≤ 不同最短路长度个数
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
    vector<ll> dist;
    vector<int> inq, level, cur;

    MinCostMaxFlowBatch(int n)
        : n(n), g(n), dist(n), inq(n), level(n), cur(n) {}

    void addEdge(int u, int v, ll cap, ll cost) {
        int iu = (int)g[u].size();
        int iv = (int)g[v].size();

        if (u == v) ++iv;

        g[u].push_back({v, iv, cap, cost});

        // 反向边用于撤销之前的流量，费用取相反数
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
    ll dfs(int u, int t, ll limit) {
        if (u == t) return limit;

        ll sent = 0;

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

    // 返回 {最大流量, 对应的最小总费用}
    pair<ll, ll> minCostMaxFlow(int s, int t) {
        assert(s != t);

        ll flow = 0, cost = 0;

        while (spfa(s, t)) {
            // 固定最短距离，将当前最短路尽量增广完
            while (bfs(s, t)) {
                fill(cur.begin(), cur.end(), 0);

                ll pushed;
                while ((pushed = dfs(s, t, INF)) > 0) {
                    flow += pushed;

                    // 本轮每单位流量的路径费用都是 dist[t]
                    cost += pushed * dist[t];
                }
            }
        }

        return {flow, cost};
    }
};