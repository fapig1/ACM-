#include <bits/stdc++.h>
using namespace std;

using ll = long long;
const ll INF = 4e18;

struct Dinic {
    struct Edge {
        int to, rev;
        ll cap;
    };

    int n;
    vector<vector<Edge>> g;
    vector<int> level, cur;

    Dinic(int n) : n(n), g(n), level(n), cur(n) {}

    // 有向边 u -> v，容量 cap
    void addEdge(int u, int v, ll cap) {
        Edge a{v, (int)g[v].size(), cap};
        Edge b{u, (int)g[u].size(), 0};
        g[u].push_back(a);
        g[v].push_back(b);
    }

    bool bfs(int s, int t) {
        fill(level.begin(), level.end(), -1);

        queue<int> q;
        level[s] = 0;
        q.push(s);

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
        if (u == t) return flow;

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
};

void solve() {
    int n, m, S, T;
    cin >> n >> m >> S >> T;

    Dinic mf(n + 1);

    while (m--) {
        int u, v;
        ll w;
        cin >> u >> v >> w;

        mf.addEdge(u, v, w);
    }

    cout << mf.maxflow(S, T) << '\n';
}
