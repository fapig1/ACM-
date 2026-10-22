#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// 【速查】Kruskal 最小生成树
// 用法: vector<array<ll,3>> edges;  edges.push_back({w, u, v});   // 注意边权放第一个，方便直接 sort
//       ll res; bool ok = kruskal(n, edges, res);   // ok = false 表示图不连通；点 1-based
// 复杂度: O(m log m)，稀疏图首选；最大生成树把 sort 改成降序
bool kruskal(int n, vector<array<ll, 3>> edges, ll& res) {
    sort(edges.begin(), edges.end());
    vector<int> fa(n + 1);
    iota(fa.begin(), fa.end(), 0);
    auto find = [&](int x) { while (fa[x] != x) x = fa[x] = fa[fa[x]]; return x; };
    res = 0;
    int cnt = 0;
    for (auto& [w, u, v] : edges) {
        int a = find(u), b = find(v);
        if (a != b) { fa[a] = b; res += w; cnt++; }
    }
    return cnt == n - 1;
}

// 示例: 读 n m 和 m 条边 (a b w)，输出 MST 权值和，不连通输出 impossible
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n, m;
    cin >> n >> m;
    vector<array<ll, 3>> edges(m);
    for (auto& [w, u, v] : edges) cin >> u >> v >> w;
    ll res;
    if (!kruskal(n, edges, res)) cout << "impossible" << '\n';
    else cout << res << '\n';
    return 0;
}
