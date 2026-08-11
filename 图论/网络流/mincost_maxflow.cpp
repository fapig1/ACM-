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

    // 返回 {最大流, 最小费用}
    pair<ll, ll> minCostMaxFlow(int s, int t) {
        ll maxFlow = 0;
        ll minCost = 0;

        vector<ll> dist(n);
        vector<int> inq(n);
        vector<int> preV(n), preE(n);

        while (true) {
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

            ll f = INF;

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

            maxFlow += f;
            minCost += f * dist[t];
        }

        return {maxFlow, minCost};
    }
};

void solve() {
    int n, m;
    cin >> n >> m;

    int S = 0;
    int T = n + m + 1;

    MinCostMaxFlow mf(T + 1);

    for (int i = 1; i <= n; i++) {
        mf.addEdge(S, i, 1, 0);

        int x;
        cin >> x;

        while (x--) {
            int w;
            cin >> w;

            mf.addEdge(i, n + w, 1, 0);
        }
    }

    for (int i = 1; i <= m; i++) {
        int y;
        cin >> y;

        while (y--) {
            ll w;
            cin >> w;

            mf.addEdge(n + i, T, 1, w);
        }
    }

    auto [flow, cost] = mf.minCostMaxFlow(S, T);

    cout << cost << '\n';
}
