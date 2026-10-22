#include <bits/stdc++.h>
using namespace std;
using ll = long long;
const ll INF = 4e18;
// 【速查】EK 最大流（BFS 找增广路）
// 用法: EdmondsKarp mf(点数); mf.addEdge(u, v, cap); ll f = mf.maxflow(s, t);
// 复杂度: O(n·m²)，好写但慢，比赛优先用 Dinic
struct EdmondsKarp {
    struct Edge {
        int to, rev;
        ll cap;
    };

    int n;
    vector<vector<Edge>> g;

    EdmondsKarp(int n) : n(n), g(n) {}

    void addEdge(int u, int v, ll cap) {
        Edge a{v, (int)g[v].size(), cap};
        Edge b{u, (int)g[u].size(), 0};

        g[u].push_back(a);
        g[v].push_back(b);
    }

    ll maxflow(int s, int t) {
        ll flow = 0;

        while (true) {
            vector<int> preV(n, -1);
            vector<int> preE(n, -1);
            vector<ll> mf(n, 0);

            queue<int> q;
            q.push(s);
            mf[s] = INF;

            while (!q.empty() && mf[t] == 0) {
                int u = q.front();
                q.pop();

                for (int i = 0; i < (int)g[u].size(); i++) {
                    Edge &e = g[u][i];

                    if (e.cap > 0 && mf[e.to] == 0) {
                        mf[e.to] = min(mf[u], e.cap);
                        preV[e.to] = u;
                        preE[e.to] = i;
                        q.push(e.to);

                        if (e.to == t)
                            break;
                    }
                }
            }

            if (mf[t] == 0)
                break;

            ll f = mf[t];
            flow += f;

            int v = t;

            while (v != s) {
                int u = preV[v];
                int id = preE[v];

                Edge &e = g[u][id];

                e.cap -= f;
                g[v][e.rev].cap += f;

                v = u;
            }
        }

        return flow;
    }
};
