struct Dinic {
    struct Edge {
        int to, rev;
        ll cap;

        // 如果需要输出原图边，可以记录原始容量
        ll originalCap;
    };

    int n;
    vector<vector<Edge>> g;
    vector<int> level, cur;

    Dinic(int n) : n(n), g(n), level(n), cur(n) {}

    void addEdge(int u, int v, ll cap) {
        Edge a{v, (int)g[v].size(), cap, cap};
        Edge b{u, (int)g[u].size(), 0, 0};

        g[u].push_back(a);
        g[v].push_back(b);
    }

    bool bfs(int s, int t) {
        fill(level.begin(), level.end(), -1);

        queue<int> q;
        q.push(s);
        level[s] = 0;

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (auto &e : g[u]) {
                if (e.cap > 0 && level[e.to] == -1) {
                    level[e.to] = level[u] + 1;
                    q.push(e.to);
                }
            }
        }

        return level[t] != -1;
    }

    ll dfs(int u, int t, ll flow) {
        if (u == t)
            return flow;

        for (int &i = cur[u]; i < (int)g[u].size(); i++) {
            Edge &e = g[u][i];

            if (e.cap > 0 && level[e.to] == level[u] + 1) {
                ll f = dfs(e.to, t, min(flow, e.cap));

                if (f > 0) {
                    e.cap -= f;
                    g[e.to][e.rev].cap += f;
                    return f;
                }
            }
        }

        return 0;
    }

    ll maxflow(int s, int t) {
        ll flow = 0;

        while (bfs(s, t)) {
            fill(cur.begin(), cur.end(), 0);

            while (ll f = dfs(s, t, INF)) {
                flow += f;
            }
        }

        return flow;
    }

    // 最大流跑完后调用
    // vis[i] = 1：最小割 S 侧
    // vis[i] = 0：最小割 T 侧
    vector<int> mincut(int s) {
        vector<int> vis(n, 0);
        queue<int> q;

        vis[s] = 1;
        q.push(s);

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (auto &e : g[u]) {
                if (e.cap > 0 && !vis[e.to]) {
                    vis[e.to] = 1;
                    q.push(e.to);
                }
            }
        }

        return vis;
    }
};


void solve() {
    int n, m, S, T;
    cin >> n >> m >> S >> T;

    Dinic mf(n + 1);

    for (int i = 0; i < m; i++) {
        int u, v;
        ll w;
        cin >> u >> v >> w;

        mf.addEdge(u, v, w);
    }

    ll flow = mf.maxflow(S, T);

    cout << "mincut = " << flow << '\n';

    auto side = mf.mincut(S);

    cout << "S side: ";
    for (int i = 1; i <= n; i++) {
        if (side[i])
            cout << i << ' ';
    }
    cout << '\n';

    cout << "T side: ";
    for (int i = 1; i <= n; i++) {
        if (!side[i])
            cout << i << ' ';
    }
    cout << '\n';
}
