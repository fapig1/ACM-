#include <bits/stdc++.h>
using namespace std;

const int N = 200005;

int n;
vector<int> g[N];

int sz[N];
int best = INT_MAX;
vector<int> centroid;

void dfs(int u, int fa) {
    sz[u] = 1;

    int max_part = 0;

    for (int v : g[u]) {
        if (v == fa) continue;

        dfs(v, u);

        sz[u] += sz[v];
        max_part = max(max_part, sz[v]);
    }

    max_part = max(max_part, n - sz[u]);

    if (max_part < best) {
        best = max_part;
        centroid.clear();
        centroid.push_back(u);
    } else if (max_part == best) {
        centroid.push_back(u);
    }
}

int main() {
    cin >> n;

    for (int i = 1; i < n; i++) {
        int u, v;
        cin >> u >> v;
        g[u].push_back(v);
        g[v].push_back(u);
    }

    dfs(1, 0);

    cout << "Centroid:";
    for (int x : centroid) {
        cout << ' ' << x;
    }
    cout << '\n';

    cout << "Min max component size = " << best << '\n';

    return 0;
}
