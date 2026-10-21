#include <bits/stdc++.h>
using namespace std;
using ll = long long;
const ll INF = 4e18;
// 用dij()代替spfa
struct MinCostMaxFlow {
    struct Edge {
        int to, rev;
        ll cap, cost;
    };

    int n;
    vector<vector<Edge>> g;

    MinCostMaxFlow(int n) : n(n), g(n) {}

    void addEdge(int u, int v, ll cap, ll cost) {
        Edge a{v, (int)g[v].size(), cap, cost};
        Edge b{u, (int)g[u].size(), 0, -cost};

        g[u].push_back(a);
        g[v].push_back(b);
    }

    pair<ll, ll> minCostFlow(int s, int t, ll k) {
        const ll INF = (1LL << 62);

        ll flow = 0;
        ll cost = 0;

        // 势能
        vector<ll> h(n, INF);

        /*
         * 因为初始存在负费用边，所以先 SPFA 一次，
         * 得到从 s 出发的最短距离作为初始势能。
         */
        {
            vector<int> inq(n, 0);
            queue<int> q;

            h[s] = 0;
            q.push(s);
            inq[s] = 1;

            while (!q.empty()) {
                int u = q.front();
                q.pop();
                inq[u] = 0;

                for (auto &e : g[u]) {
                    if (e.cap <= 0) continue;

                    if (h[u] != INF &&
                        h[e.to] > h[u] + e.cost) {

                        h[e.to] = h[u] + e.cost;

                        if (!inq[e.to]) {
                            inq[e.to] = 1;
                            q.push(e.to);
                        }
                    }
                }
            }

            // 不可达点势能设 0
            for (int i = 0; i < n; i++) {
                if (h[i] == INF)
                    h[i] = 0;
            }
        }

        vector<ll> dist(n);
        vector<int> preV(n), preE(n);

        while (flow < k) {
            fill(dist.begin(), dist.end(), INF);

            priority_queue<
                pair<ll, int>,
                vector<pair<ll, int>>,
                greater<pair<ll, int>>
            > pq;

            dist[s] = 0;
            pq.push({0, s});

            while (!pq.empty()) {
                auto [d, u] = pq.top();
                pq.pop();

                if (d != dist[u])
                    continue;

                for (int i = 0; i < (int)g[u].size(); i++) {
                    Edge &e = g[u][i];

                    if (e.cap <= 0)
                        continue;

                    // reduced cost
                    ll nd = d + e.cost + h[u] - h[e.to];

                    if (dist[e.to] > nd) {
                        dist[e.to] = nd;

                        preV[e.to] = u;
                        preE[e.to] = i;

                        pq.push({nd, e.to});
                    }
                }
            }

            if (dist[t] == INF)
                break;

            // 更新势能
            for (int i = 0; i < n; i++) {
                if (dist[i] != INF)
                    h[i] += dist[i];
            }

            ll f = k - flow;

            for (int v = t; v != s; v = preV[v]) {
                int u = preV[v];
                int id = preE[v];

                f = min(f, g[u][id].cap);
            }

            // 求这一条增广路的真实费用
            ll pathCost = 0;

            for (int v = t; v != s; v = preV[v]) {
                int u = preV[v];
                int id = preE[v];

                Edge &e = g[u][id];

                pathCost += e.cost;

                e.cap -= f;
                g[v][e.rev].cap += f;
            }

            flow += f;
            cost += f * pathCost;
        }

        return {flow, cost};
    }
};
