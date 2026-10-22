#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// 【速查】带权倍增 LCA 工具箱 + 逐点加入时动态维护直径 / 端点 / 中心
// 用法: TreeLCA t(n);  t.addEdge(u, v, w);  t.build(root);    // 点 1-based，边权非负
//   t.lca(a, b)    t.dist(a, b)（带权）    t.jump(x, k) -> x 向上 k 步（不存在为 0）
//   t.uToV(u, v, len) -> 从 u 往 v 走不超过 len 能到的最远点（落在边中间时取靠近 u 的端点）
//   t.center(u, v) -> 路径"中点"：落在点上返回 {p, p}，落在边上返回该边两端 {靠 u, 靠 v}
// 动态直径: 新点 x 加入时，新直径只可能是 (u, x) 或 (v, x)，比较 dist 即可（见 main，按 BFS 序加点）
// 复杂度: 预处理 O(n log n)，lca / dist / jump O(log n)，uToV / center O(log² n)
struct TreeLCA {
    int n, LOG;
    vector<vector<pair<int, ll>>> g;
    vector<vector<int>> par;          // par[k][v] = v 的第 2^k 个祖先
    vector<int> dep;
    vector<ll> dis;                   // 根到 v 的边权和

    TreeLCA(int n) : n(n), g(n + 1), dep(n + 1, 0), dis(n + 1, 0) {
        LOG = 1; while ((1 << LOG) <= n) LOG++;
        par.assign(LOG, vector<int>(n + 1, 0));
    }
    void addEdge(int u, int v, ll w) { g[u].push_back({v, w}); g[v].push_back({u, w}); }
    void build(int root) {
        vector<int> q = {root};
        vector<char> vis(n + 1, 0);
        vis[root] = 1;
        for (size_t i = 0; i < q.size(); i++) {
            int u = q[i];
            for (auto [v, w] : g[u]) if (!vis[v]) {
                vis[v] = 1; par[0][v] = u; dep[v] = dep[u] + 1; dis[v] = dis[u] + w;
                q.push_back(v);
            }
        }
        for (int k = 1; k < LOG; k++)
            for (int v = 1; v <= n; v++) par[k][v] = par[k - 1][par[k - 1][v]];
    }
    int jump(int x, int k) {
        if (k > dep[x]) return 0;
        for (int i = 0; i < LOG; i++) if (k >> i & 1) x = par[i][x];
        return x;
    }
    int lca(int a, int b) {
        if (dep[a] < dep[b]) swap(a, b);
        a = jump(a, dep[a] - dep[b]);
        if (a == b) return a;
        for (int i = LOG - 1; i >= 0; i--) if (par[i][a] != par[i][b]) a = par[i][a], b = par[i][b];
        return par[0][a];
    }
    ll dist(int a, int b) { return dis[a] + dis[b] - 2 * dis[lca(a, b)]; }
    int uToV(int u, int v, ll len) {
        int l = lca(u, v);
        ll du = dist(u, l);
        if (len <= du) {                       // 终点在 u -> lca 的上行段
            int cur = u;
            for (int i = LOG - 1; i >= 0; i--) {
                int p = par[i][cur];
                if (p && dist(u, p) <= len) cur = p;
            }
            return cur;
        }
        ll rem = dist(u, v) - len;             // 终点在 lca -> v 的下行段：从 v 往上走 rem
        int cur = v;
        for (int i = LOG - 1; i >= 0; i--) {
            int p = par[i][cur];
            if (p && dist(v, p) <= rem) cur = p;
        }
        if (dist(cur, v) < rem) cur = par[0][cur];
        return cur;
    }
    pair<int, int> center(int u, int v) {
        ll d = dist(u, v);
        int p = uToV(u, v, d / 2);
        if (dist(u, p) * 2 == d) return {p, p};
        int l = lca(u, v), nxt;
        if (lca(u, p) == p && p != l) nxt = par[0][p];          // p 在 u -> lca 上行段：下一个点是父亲
        else {                                                   // 否则是 p 往 v 方向的儿子
            int cur = v;
            for (int i = LOG - 1; i >= 0; i--) {
                int pp = par[i][cur];
                if (pp && dep[pp] > dep[p]) cur = pp;
            }
            nxt = cur;
        }
        return {p, nxt};
    }
};

// 示例: 读 n、n-1 条带权边和根；从根开始 BFS 逐点加入，每加一个点输出当前直径端点、直径长度、中心
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n;
    cin >> n;
    TreeLCA t(n);
    for (int i = 1; i < n; i++) { int a, b; ll w; cin >> a >> b >> w; t.addEdge(a, b, w); }
    int root;
    cin >> root;
    t.build(root);
    int u = root, v = root;
    ll diameter = 0;
    vector<char> vis(n + 1, 0);
    vis[root] = 1;
    queue<int> que;
    que.push(root);
    while (!que.empty()) {
        int tp = que.front(); que.pop();
        for (auto [i, w] : t.g[tp]) {
            if (vis[i]) continue;
            vis[i] = 1;
            ll d1 = t.dist(u, i), d2 = t.dist(v, i);
            if (d1 > diameter && d1 > d2) { diameter = d1; v = i; }
            if (d2 > diameter) { diameter = d2; u = i; }
            auto cen = t.center(u, v);
            cout << u << " " << v << " " << diameter << " ";
            if (cen.first == cen.second) cout << cen.first << "\n";
            else cout << cen.first << " " << cen.second << "\n";
            que.push(i);
        }
    }
    return 0;
}
