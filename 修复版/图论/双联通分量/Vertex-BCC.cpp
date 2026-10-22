#include <bits/stdc++.h>
using namespace std;

// 【速查】点双连通分量 + 割点（无向图）
// 用途: 求割点；点双缩点 / 建圆方树
// 用法: VertexBCC v(n); v.addEdge(a, b); v.build();
// 结果: v.isCut[u]、v.bcc[i]（点集，割点会出现在多个点双里）、v.bccEdges[i]（边编号）
// 复杂度: O(n + m)；孤立点单独成一个点双；点 1-based
/*
 * Vertex Biconnected Components (VBCC)
 *
 * 点双连通分量
 *
 * 用法：
 * VertexBCC vbcc(n);
 * vbcc.addEdge(u, v);
 * vbcc.build();
 *
 * 结果：
 * vbcc.bcc          所有点双，bcc[i] 是顶点集合
 * vbcc.isCut[u]     u 是否是割点
 * vbcc.dfn / low
 *
 * 约定：
 * 1. 顶点编号 1 ~ n
 * 2. 桥 (u,v) 会形成一个 {u,v} 点双
 * 3. 孤立点会形成一个 {u} 点双
 */
struct VertexBCC {
    struct Edge {
        int u, v;
    };

    int n;
    int timer;

    vector<Edge> edges;
    vector<vector<int>> g;

    vector<int> dfn, low;
    vector<bool> isCut;

    // 边栈
    vector<int> stk;

    // 所有点双
    vector<vector<int>> bcc;

    // 每个点双包含哪些边
    vector<vector<int>> bccEdges;

    // 用来给弹出的点双去重顶点
    vector<int> vis;
    int visTag;

    VertexBCC(int n)
        : n(n),
          timer(0),
          g(n + 1),
          dfn(n + 1),
          low(n + 1),
          isCut(n + 1),
          vis(n + 1),
          visTag(0) {}

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

    // 从边栈中弹出一个点双
    // 一直弹到 stopEdge 为止
    void popBCC(int stopEdge) {
        ++visTag;

        vector<int> comp;       // 当前点双的点
        vector<int> compEdges;  // 当前点双的边

        while (true) {
            int id = stk.back();
            stk.pop_back();

            compEdges.push_back(id);

            int x = edges[id].u;
            int y = edges[id].v;

            if (vis[x] != visTag) {
                vis[x] = visTag;
                comp.push_back(x);
            }

            if (vis[y] != visTag) {
                vis[y] = visTag;
                comp.push_back(y);
            }

            if (id == stopEdge) {
                break;
            }
        }

        bcc.push_back(move(comp));
        bccEdges.push_back(move(compEdges));
    }

    void dfs(int u, int parentEdge) {
        dfn[u] = low[u] = ++timer;

        int child = 0;

        for (int id : g[u]) {
            // 无向图只跳过真正的父边
            if (id == parentEdge) {
                continue;
            }

            int v = other(id, u);

            if (!dfn[v]) {
                ++child;

                // 树边压栈
                stk.push_back(id);

                dfs(v, id);

                low[u] = min(low[u], low[v]);

                /*
                 * v 子树无法绕过 u 到达 u 的祖先
                 *
                 * 当前边栈直到 (u,v)
                 * 构成一个点双
                 */
                if (low[v] >= dfn[u]) {

                    // 判断割点
                    if (parentEdge != -1 || child > 1) {
                        isCut[u] = true;
                    }

                    popBCC(id);
                }
            }
            else if (dfn[v] < dfn[u]) {
                /*
                 * 后代 -> 祖先 的返祖边
                 *
                 * 只在这个方向压栈一次
                 */
                stk.push_back(id);

                low[u] = min(low[u], dfn[v]);
            }
        }
    }

    void build() {
        timer = 0;
        visTag = 0;

        fill(dfn.begin(), dfn.end(), 0);
        fill(low.begin(), low.end(), 0);
        fill(isCut.begin(), isCut.end(), false);

        stk.clear();
        bcc.clear();

        for (int i = 1; i <= n; ++i) {
            if (dfn[i]) continue;

            if (g[i].empty()) {
                // 孤立点
                dfn[i] = low[i] = ++timer;
                bcc.push_back({i});
            }
            else {
                dfs(i, -1);
            }
        }
    }
};



int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    cin >> n >> m;

    VertexBCC vbcc(n);

    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        vbcc.addEdge(u, v);
    }

    vbcc.build();

    cout << "VBCC count = " << vbcc.bcc.size() << '\n';

    for (int i = 0; i < (int)vbcc.bcc.size(); ++i) {
        cout << "BCC " << i + 1 << ": ";

        for (int u : vbcc.bcc[i]) {
            cout << u << ' ';
        }

        cout << '\n';
    }

    cout << "Cut vertices: ";

    for (int u = 1; u <= n; ++u) {
        if (vbcc.isCut[u]) {
            cout << u << ' ';
        }
    }

    cout << '\n';
}
