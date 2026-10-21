#include <bits/stdc++.h>
using namespace std;

/*
 * Edge Biconnected Components (EBCC)
 *
 * 边双连通分量
 *
 * 用法：
 * EdgeBCC ebcc(n);
 * ebcc.addEdge(u, v);
 * ebcc.build();
 *
 * 结果：
 * ebcc.isBridge[id]   第 id 条边是否是桥
 * ebcc.belong[u]      u 属于哪个边双
 * ebcc.components     所有边双的顶点集合
 *
 * 顶点编号：
 * 1 ~ n
 *
 * 边编号：
 * 0 ~ m-1
 */
struct EdgeBCC {
    struct Edge {
        int u, v;
    };

    int n;
    int timer;
    int bccCnt;

    vector<Edge> edges;
    vector<vector<int>> g;

    vector<int> dfn, low;

    vector<bool> isBridge;

    // belong[u] = u 所属的边双编号
    vector<int> belong;

    // 每个边双包含哪些顶点
    vector<vector<int>> components;

    EdgeBCC(int n)
        : n(n),
          timer(0),
          bccCnt(0),
          g(n + 1),
          dfn(n + 1),
          low(n + 1),
          belong(n + 1) {}

    int addEdge(int u, int v) {
        int id = edges.size();

        edges.push_back({u, v});

        g[u].push_back(id);
        g[v].push_back(id);

        return id;
    }

    int other(int id, int u) {
        return edges[id].u ^ edges[id].v ^ u;
    }

    /*
     * 第一遍 Tarjan：
     * 找桥
     */
    void dfsBridge(int u, int parentEdge) {
        dfn[u] = low[u] = ++timer;

        for (int id : g[u]) {
            if (id == parentEdge) {
                continue;
            }

            int v = other(id, u);

            if (!dfn[v]) {
                dfsBridge(v, id);

                low[u] = min(low[u], low[v]);

                /*
                 * 注意这里是严格 >
                 */
                if (low[v] > dfn[u]) {
                    isBridge[id] = true;
                }
            }
            else {
                low[u] = min(low[u], dfn[v]);
            }
        }
    }

    /*
     * 第二遍：
     * 忽略所有桥进行 DFS 染色
     */
    void dfsColor(int u, int id) {
        belong[u] = id;
        components[id - 1].push_back(u);

        for (int edgeId : g[u]) {
            if (isBridge[edgeId]) {
                continue;
            }

            int v = other(edgeId, u);

            if (!belong[v]) {
                dfsColor(v, id);
            }
        }
    }

    void build() {
        int m = edges.size();

        timer = 0;
        bccCnt = 0;

        fill(dfn.begin(), dfn.end(), 0);
        fill(low.begin(), low.end(), 0);
        fill(belong.begin(), belong.end(), 0);

        isBridge.assign(m, false);
        components.clear();

        // 第一遍：找桥
        for (int i = 1; i <= n; ++i) {
            if (!dfn[i]) {
                dfsBridge(i, -1);
            }
        }

        // 第二遍：删除桥以后染色
        for (int i = 1; i <= n; ++i) {
            if (!belong[i]) {
                ++bccCnt;

                components.push_back({});

                dfsColor(i, bccCnt);
            }
        }
    }
};



int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    cin >> n >> m;

    EdgeBCC ebcc(n);

    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;

        ebcc.addEdge(u, v);
    }

    ebcc.build();

    cout << "Edge-BCC count = " << ebcc.bccCnt << '\n';

    for (int i = 0; i < ebcc.bccCnt; ++i) {
        cout << "BCC " << i + 1 << ": ";

        for (int u : ebcc.components[i]) {
            cout << u << ' ';
        }

        cout << '\n';
    }

    cout << "Bridges:\n";

    for (int id = 0; id < m; ++id) {
        if (ebcc.isBridge[id]) {
            auto [u, v] = ebcc.edges[id];

            cout << u << ' ' << v << '\n';
        }
    }
}
