/*
适用：
1. 只考虑容量，不考虑费用的最大流。
2. 二分图匹配、最小割，以及转化为最大流的问题。
3. 容量必须非负，源点和汇点必须不同。

复杂度：
一般图最坏 O(V^2 * E)，空间 O(V + E)。
标准二分图匹配网络可达到 O(E * sqrt(V))。

注意：
1. addEdge 添加有向边，并自动添加容量为 0 的反向边。
2. 无向容量边可通过两次 addEdge 分别添加两个方向。
3. DFS 使用递归，路径特别深时需要注意栈空间。
4. 总流量必须能用 long long 表示。
*/

// Dinic批量增广最大流
struct Dinic {
    using ll = long long;
    static constexpr ll INF = (1LL << 62);

    struct Edge {
        int to, rev;
        ll cap;  // 当前剩余容量
    };

    int n;
    vector<vector<Edge>> g;
    vector<int> level, cur;

    Dinic(int n) : n(n), g(n), level(n), cur(n) {}

    void addEdge(int u, int v, ll cap) {
        int iu = (int)g[u].size();
        int iv = (int)g[v].size();

        // 保证自环的反向边编号也正确
        if (u == v) ++iv;

        g[u].push_back({v, iv, cap});
        g[v].push_back({u, iu, 0});
    }

    // 在残量网络上 BFS，建立层次图
    bool bfs(int s, int t) {
        fill(level.begin(), level.end(), -1);

        queue<int> q;
        level[s] = 0;
        q.push(s);

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (const auto &e : g[u]) {
                if (e.cap > 0 && level[e.to] == -1) {
                    level[e.to] = level[u] + 1;
                    q.push(e.to);
                }
            }
        }

        return level[t] != -1;
    }

    // 在层次图上批量推流，返回实际推送量
    ll dfs(int u, int t, ll limit) {
        if (u == t) return limit;

        ll sent = 0;

        // 当前弧优化：已经处理完的边不再重复扫描
        for (int &i = cur[u]; i < (int)g[u].size(); ++i) {
            Edge &e = g[u][i];

            if (e.cap <= 0 ||
                level[e.to] != level[u] + 1) {
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

    ll maxFlow(int s, int t) {
        assert(s != t);

        ll flow = 0;

        while (bfs(s, t)) {
            fill(cur.begin(), cur.end(), 0);

            ll pushed;
            while ((pushed = dfs(s, t, INF)) > 0) {
                flow += pushed;
            }
        }

        return flow;
    }
};