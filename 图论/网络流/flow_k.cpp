//指定流量K的最小费用最大流
// 返回 {实际流量, 最小费用}
// 如果 flow < k，说明无法达到指定流量 k
pair<ll, ll> minCostFlow(int s, int t, ll k) {
    ll flow = 0;
    ll cost = 0;

    vector<ll> dist(n);
    vector<int> inq(n);
    vector<int> preV(n), preE(n);

    while (flow < k) {
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
                    dist[u] != INF &&
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

        // 已经无法继续增广
        if (dist[t] == INF)
            break;

        // 本次最多只能补到 k
        ll f = k - flow;

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

        flow += f;
        cost += f * dist[t];
    }

    return {flow, cost};
}
