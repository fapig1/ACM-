#include <bits/stdc++.h>
using namespace std;

const int N = 200005;

int n;
vector<int> g[N];
int dist[N];

void dfs(int u, int fa) {
    for (int v : g[u]) {
        if (v == fa) continue;
        dist[v] = dist[u] + 1;
        dfs(v, u);
    }
}

int get_far(int start) {
    memset(dist, 0, sizeof dist);
    dfs(start, 0);

    int far = start;
    for (int i = 1; i <= n; i++) {
        if (dist[i] > dist[far]) {
            far = i;
        }
    }
    return far;
}

int main() {
    cin >> n;

    for (int i = 1; i < n; i++) {
        int u, v;
        cin >> u >> v;
        g[u].push_back(v);
        g[v].push_back(u);
    }

    int s = get_far(1);
    int t = get_far(s);

    cout << dist[t] << '\n';


    return 0;
}
