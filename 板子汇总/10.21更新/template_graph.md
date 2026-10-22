# David_图论（10.21 修复版）

> **通用前导**：本文档所有代码块为片段，需配合以下前导使用：

```cpp
#include <bits/stdc++.h>
using namespace std;
```

标注 **【本次修复】** 的模板经过 2026-10-21 全量审查修复，并全部通过随机对拍/压测回归验证；各修复点在代码注释中亦有说明。

[TOC]

## 普通图论

### Tarjan缩点求最大连通分量

> 【本次修复】原版 edge 向量 MAXN 预填后 push_back 导致缩点图恒为空，且无 main；已修复并补输出。
> 【10.22 结构体封装】原全局变量 + 全局函数改为结构体 / 传参函数，用法见块内速查卡。

```cpp
using ll = long long;

// 【速查】Tarjan 强连通分量 + 缩点（有向图 → DAG）
// 用途: 有向图里"互相可达"的点缩成一个点，之后在 DAG 上做拓扑 / DP（如最大点权路径，P3387）
// 用法: SCC s(n);  s.addEdge(u, v);  s.build();            // 点 1-based
// 结果: s.comp[u] = 点 u 所属分量 (0..s.cnt-1)；分量编号是"逆拓扑序"（编号大的指向编号小的）
//       auto dag = s.dag();  缩点后的 DAG 邻接表（已去重），dag[c] 是分量 c 的出边
// 复杂度: O(n + m)；迭代实现，1e6 深链也不会爆栈
struct SCC {
    int n, cnt = 0;
    vector<vector<int>> g;
    vector<int> comp;

    SCC(int n) : n(n), g(n + 1), comp(n + 1, -1) {}
    void addEdge(int u, int v) { g[u].push_back(v); }

    void build() {
        int timer = 0;
        vector<int> dfn(n + 1, -1), low(n + 1), it(n + 1, 0), st, call;
        vector<char> inStack(n + 1, 0);
        for (int s = 1; s <= n; s++) {
            if (dfn[s] != -1) continue;
            dfn[s] = low[s] = timer++; st.push_back(s); inStack[s] = 1; call.push_back(s);
            while (!call.empty()) {
                int u = call.back();
                if (it[u] < (int)g[u].size()) {
                    int v = g[u][it[u]++];
                    if (dfn[v] == -1) { dfn[v] = low[v] = timer++; st.push_back(v); inStack[v] = 1; call.push_back(v); }
                    else if (inStack[v]) low[u] = min(low[u], dfn[v]);
                } else {
                    call.pop_back();
                    if (!call.empty()) low[call.back()] = min(low[call.back()], low[u]);
                    if (low[u] == dfn[u]) {          // u 是一个分量的根，弹栈
                        while (true) {
                            int w = st.back(); st.pop_back(); inStack[w] = 0;
                            comp[w] = cnt;
                            if (w == u) break;
                        }
                        cnt++;
                    }
                }
            }
        }
    }
    vector<vector<int>> dag() {
        vector<vector<int>> d(cnt);
        for (int u = 1; u <= n; u++)
            for (int v : g[u]) if (comp[u] != comp[v]) d[comp[u]].push_back(comp[v]);
        for (auto& e : d) { sort(e.begin(), e.end()); e.erase(unique(e.begin(), e.end()), e.end()); }
        return d;
    }
};

// 示例: 读入 n m、m 条有向边、n 个点权；输出分量数与缩点图边数（含重边）、每个分量的点集与点权和、最大分量点权和
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n, m;
    cin >> n >> m;
    SCC s(n);
    vector<pair<int, int>> edges(m);
    for (auto& [a, b] : edges) { cin >> a >> b; s.addEdge(a, b); }
    vector<ll> val(n + 1);
    for (int i = 1; i <= n; i++) cin >> val[i];
    s.build();
    vector<ll> sccval(s.cnt, 0);
    for (int i = 1; i <= n; i++) sccval[s.comp[i]] += val[i];
    int cross = 0;
    for (auto [a, b] : edges) if (s.comp[a] != s.comp[b]) cross++;
    cout << s.cnt << " " << cross << "\n";
    for (int c = 0; c < s.cnt; c++) {
        cout << "SCC " << c + 1 << " (val=" << sccval[c] << "):";
        for (int j = 1; j <= n; j++) if (s.comp[j] == c) cout << " " << j;
        cout << "\n";
    }
    int best = 0;
    for (int c = 1; c < s.cnt; c++) if (sccval[c] > sccval[best]) best = c;
    cout << "max sccval = " << (s.cnt ? sccval[best] : 0) << "\n";
    return 0;
}
```

### 有向图的拓扑序列

> 【10.22 结构体封装】原全局变量 + 全局函数改为结构体 / 传参函数，用法见块内速查卡。

```cpp
// 【速查】拓扑排序（Kahn / BFS 入度法）
// 用法: vector<vector<int>> g(n + 1);  g[x].push_back(y);   // 点 1-based，有向边 x -> y
//       vector<int> order;  bool ok = topoSort(n, g, order);
// 返回: true 时 order 是一个拓扑序；false 说明有环（order.size() < n）
// 字典序最小的拓扑序: 把 queue 换成 priority_queue<int, vector<int>, greater<int>>
// 复杂度: O(n + m)
bool topoSort(int n, const vector<vector<int>>& g, vector<int>& order) {
    vector<int> deg(n + 1, 0);
    for (int u = 1; u <= n; u++) for (int v : g[u]) deg[v]++;
    queue<int> q;
    order.clear();
    for (int i = 1; i <= n; i++) if (deg[i] == 0) q.push(i);
    while (!q.empty()) {
        int u = q.front(); q.pop();
        order.push_back(u);
        for (int v : g[u]) if (--deg[v] == 0) q.push(v);
    }
    return (int)order.size() == n;
}

// 示例: 读 n m 和 m 条边，输出拓扑序，有环输出 -1
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n, m;
    cin >> n >> m;
    vector<vector<int>> g(n + 1);
    for (int i = 0; i < m; i++) { int x, y; cin >> x >> y; g[x].push_back(y); }
    vector<int> order;
    if (topoSort(n, g, order)) { for (int x : order) cout << x << ' '; }
    else cout << -1 << '\n';
    return 0;
}
```

### 树的直径

> 两次 BFS（非递归），支持非负边权。
> 【10.22 结构体封装】原全局变量 + 全局函数改为结构体 / 传参函数，用法见块内速查卡。

```cpp
using ll = long long;

// 【速查】树的直径（两次 BFS，非递归）
// 用法: vector<vector<pair<int,ll>>> g(n + 1);  g[u].push_back({v, w}); g[v].push_back({u, w});   // 点 1-based
//       auto [len, s, t] = treeDiameter(n, g);   // 直径长度与两个端点
// 注意: 边权必须非负（有负边权用树形 DP：每个点维护向下最长、次长链）；无边权就全部填 1
// 复杂度: O(n)
// 从 s 出发求到所有点的距离（树上 BFS 即可，不需要 Dijkstra）
vector<ll> treeDist(int n, const vector<vector<pair<int, ll>>>& g, int s) {
    vector<ll> d(n + 1, -1);
    vector<int> q = {s};
    d[s] = 0;
    for (size_t i = 0; i < q.size(); i++) {
        int u = q[i];
        for (auto [v, w] : g[u]) if (d[v] < 0) { d[v] = d[u] + w; q.push_back(v); }
    }
    return d;
}
tuple<ll, int, int> treeDiameter(int n, const vector<vector<pair<int, ll>>>& g) {
    auto d1 = treeDist(n, g, 1);
    int s = max_element(d1.begin() + 1, d1.end()) - d1.begin();
    auto d2 = treeDist(n, g, s);
    int t = max_element(d2.begin() + 1, d2.end()) - d2.begin();
    return {d2[t], s, t};
}

// 示例: 读 n 和 n-1 条无权边，输出直径长度
int main() {
    int n;
    cin >> n;
    vector<vector<pair<int, ll>>> g(n + 1);
    for (int i = 1; i < n; i++) { int u, v; cin >> u >> v; g[u].push_back({v, 1}); g[v].push_back({u, 1}); }
    auto [len, s, t] = treeDiameter(n, g);
    cout << len << '\n';
    return 0;
}
```

### 树的重心

> BFS 序非递归实现。
> 【10.22 结构体封装】原全局变量 + 全局函数改为结构体 / 传参函数，用法见块内速查卡。

```cpp
// 【速查】树的重心（非递归）
// 用途: 找删掉后"最大剩余连通块"最小的点（点分治的分治中心）
// 用法: vector<vector<int>> g(n + 1);  建双向边;   int best;  auto c = findCentroids(n, g, best);
// 返回: 重心（1 个或 2 个，升序）；best = 删去重心后最大连通块的大小
// 复杂度: O(n)；点 1-based
vector<int> findCentroids(int n, const vector<vector<int>>& g, int& best) {
    vector<int> order = {1}, fa(n + 1, 0), sz(n + 1, 1);
    for (size_t i = 0; i < order.size(); i++) {
        int u = order[i];
        for (int v : g[u]) if (v != fa[u]) { fa[v] = u; order.push_back(v); }
    }
    for (int i = n - 1; i > 0; i--) sz[fa[order[i]]] += sz[order[i]];   // BFS 序倒过来累加子树大小
    best = INT_MAX;
    vector<int> res;
    for (int u = 1; u <= n; u++) {
        int mx = n - sz[u];
        for (int v : g[u]) if (v != fa[u]) mx = max(mx, sz[v]);
        if (mx < best) best = mx, res = {u};
        else if (mx == best) res.push_back(u);
    }
    return res;
}

// 示例: 读 n 和 n-1 条边，输出重心与最大连通块大小
int main() {
    int n;
    cin >> n;
    vector<vector<int>> g(n + 1);
    for (int i = 1; i < n; i++) { int u, v; cin >> u >> v; g[u].push_back(v); g[v].push_back(u); }
    int best;
    auto c = findCentroids(n, g, best);
    cout << "Centroid:";
    for (int x : c) cout << ' ' << x;
    cout << '\n';
    cout << "Min max component size = " << best << '\n';
    return 0;
}
```

### 动态维护树直径、直径端点、中心

> 【本次修复】原版 get_center 在"奇数直径且中心边位于 u→lca 段"时输出错误端点；已修复。
> 【10.22 结构体封装】原全局变量 + 全局函数改为结构体 / 传参函数，用法见块内速查卡。

```cpp
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
```

### 树链剖分（结构体版）

> 【本次修复】原版第 2 行错误 include 中文路径的线段树文件导致无法编译；已删除。用法：HLD hld(n); addEdge; build(a,root); updatePath/askPath/updateTree/askTree/getLCA。

```cpp
//线段树(辅助树链剖分)
struct Seg{
    int n;
    vector<int> tr, lazy;

    Seg(int n) : n(n), tr(n * 4 + 5), lazy(n * 4 + 5) {}

    void build(int p, int l, int r, const vector<int>& a){
        if(l == r){
            tr[p] = a[l];
            return;
        }
        int mid = l + r >> 1;
        build(p << 1, l, mid, a);
        build(p << 1 | 1, mid + 1, r, a);
        pushup(p);
    }

    void clear(){
        fill(tr.begin(), tr.end(), 0);
        fill(lazy.begin(), lazy.end(), 0);
    }

    void pushup(int p){
        tr[p] = tr[p << 1] + tr[p << 1 | 1];
    }

    void pushdown(int p, int l, int r){
        if(lazy[p]){
            int mid = l + r >> 1;
            tr[p << 1] += (mid - l + 1) * lazy[p];
            tr[p << 1 | 1] += (r - mid) * lazy[p];
            lazy[p << 1] += lazy[p];
            lazy[p << 1 | 1] += lazy[p];
            lazy[p] = 0;
        }
    }

    void update(int p, int l, int r, int ql, int qr, int val){
        if(ql <= l && r <= qr){
            tr[p] += (r - l + 1) * val;
            lazy[p] += val;
            return;
        }
        pushdown(p, l, r);
        int mid = l + r >> 1;
        if(ql <= mid) update(p << 1, l, mid, ql, qr, val);
        if(mid < qr) update(p << 1 | 1, mid + 1, r, ql, qr, val);
        pushup(p);
    }

    int ask(int p, int l, int r, int ql, int qr){
        if(ql <= l && r <= qr){
            return tr[p];
        }
        pushdown(p, l, r);
        int mid = l + r >> 1;
        int res = 0;
        if(ql <= mid) res += ask(p << 1, l, mid, ql, qr);
        if(mid < qr) res += ask(p << 1 | 1, mid + 1, r, ql, qr);
        return res;
    }

};

// 【速查】树链剖分（重链剖分 + 线段树）
// 用途: 树上路径加/路径和、子树加/子树和、LCA
// 用法: HLD h(n); h.addEdge(u, v); h.build(a, root);       // a 为 1-based 点权
//       h.updatePath(u, v, x)  h.askPath(u, v)  h.updateTree(u, x)  h.askTree(u)  h.getLCA(u, v)  h.getDist(u, v)
// 复杂度: 预处理 O(n)，路径操作 O(log² n)，子树操作 O(log n)
//树链剖分
struct HLD{
    int n, timer;
    vector<int> sz, dep, fa, son, top, dfn, rnk;
    vector<vector<int>> adj;
    Seg seg;

    HLD(int n) : n(n), timer(0), sz(n + 1), dep(n + 1), fa(n + 1), son(n + 1), top(n + 1), dfn(n + 1), rnk(n + 1), adj(n + 1), seg(n) {}

    void addEdge(int u, int v){
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    //处理 sz, dep, fa, son
    void dfs1(int u, int p, int d){
        sz[u] = 1; fa[u] = p; dep[u] = d; son[u] = 0;
        for(int v : adj[u]){
            if(v == p) continue;
            dfs1(v, u, d + 1);
            sz[u] += sz[v];
            if(sz[v] > sz[son[u]]) son[u] = v;
        }
    }

    //处理 top, dfn
    void dfs2(int u, int tp){
        top[u] = tp; dfn[u] = ++timer; rnk[timer] = u;
        if(!son[u]) return;
        dfs2(son[u], tp);
        for(int v : adj[u]){
            if(v == fa[u] || v == son[u]) continue;
            dfs2(v, v);
        }
    }

    //建树
    void build(const vector<int>& a, int root = 1){
        dfs1(root, 0, 1);
        dfs2(root, root);
        vector<int> v(n + 1);
        for(int i = 1; i <= n; i++){
            v[i] = a[rnk[i]];
        }
        seg.build(1, 1, n, v);
    }

    // loglog复杂度太大 可以用树上差分优化
    void updatePath(int u, int v, int val){
        while(top[u] != top[v]){
            if(dep[top[u]] < dep[top[v]]) swap(u, v);
            seg.update(1, 1, n, dfn[top[u]], dfn[u], val);
            u = fa[top[u]];
        }
        if(dep[u] > dep[v]) swap(u, v);
        seg.update(1, 1, n, dfn[u], dfn[v], val);
    }

    //询问路径
    int askPath(int u, int v){
        int res = 0;
        while(top[u] != top[v]){
            if(dep[top[u]] < dep[top[v]]) swap(u, v);
            res += seg.ask(1, 1, n, dfn[top[u]], dfn[u]);
            u = fa[top[u]];
        }
        if(dep[u] > dep[v]) swap(u, v);
        res += seg.ask(1, 1, n, dfn[u], dfn[v]);
        return res;
    }

    //更新树
    void updateTree(int u, int val){
        seg.update(1, 1, n, dfn[u], dfn[u] + sz[u] - 1, val);
    }

    //询问子树
    int askTree(int u){
        return seg.ask(1, 1, n, dfn[u], dfn[u] + sz[u] - 1);
    }

    //获得两点LCA
    int getLCA(int u, int v){
        while(top[u] != top[v]){
            if(dep[top[u]] < dep[top[v]]) swap(u, v);
            u = fa[top[u]];
        }
        return dep[u] < dep[v] ? u : v;
    }

    //获取两点间距离（无边权）
    int getDist(int u, int v){
        return dep[u] + dep[v] - 2 * dep[getLCA(u, v)];
    }
};



signed main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr), cout.tie(nullptr);
    
    return 0;
}
```

### 有边数限制的最短路（Bellman-Ford）

> 【10.22 结构体封装】原全局变量 + 全局函数改为结构体 / 传参函数，用法见块内速查卡。

```cpp
using ll = long long;

// 【速查】Bellman-Ford（最多经过 k 条边的最短路）
// 用法: vector<array<ll,3>> edges;  edges.push_back({u, v, w});     // 有向边，可有负边
//       auto d = bellmanFordK(n, edges, s, k);   // d[v] = s 到 v 最多 k 条边的最短距离，到不了为 BF_INF
// 复杂度: O(k · m)；每轮必须用上一轮的 back 数组，否则会"串联"超过 k 条边
const ll BF_INF = LLONG_MAX / 4;
vector<ll> bellmanFordK(int n, const vector<array<ll, 3>>& edges, int s, int k) {
    vector<ll> d(n + 1, BF_INF), back;
    d[s] = 0;
    for (int it = 0; it < k; it++) {
        back = d;
        for (auto& [u, v, w] : edges)
            if (back[u] < BF_INF && back[u] + w < d[v]) d[v] = back[u] + w;
    }
    return d;
}

// 示例: 读 n m k 和 m 条边，输出 1 到 n 最多 k 条边的最短路，到不了输出 impossible
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n, m, k;
    cin >> n >> m >> k;
    vector<array<ll, 3>> edges(m);
    for (auto& [u, v, w] : edges) cin >> u >> v >> w;
    auto d = bellmanFordK(n, edges, 1, k);
    if (d[n] >= BF_INF) cout << "impossible" << '\n';
    else cout << d[n] << '\n';
    return 0;
}
```

### SPFA判断负环

> 【10.22 结构体封装】原全局变量 + 全局函数改为结构体 / 传参函数，用法见块内速查卡。

```cpp
using ll = long long;

// 【速查】SPFA 判负环
// 用法: vector<vector<pair<int,ll>>> g(n + 1);  g[u].push_back({v, w});   // 点 1-based
//       bool neg = hasNegativeCycle(n, g);
// 说明: 所有点一开始都入队（相当于超级源点），能判整张图任意位置的负环；
//       只想判"从 s 出发能到的负环"就只把 s 入队、dist 初始化为 INF、dist[s] = 0
// 复杂度: 最坏 O(nm)
bool hasNegativeCycle(int n, const vector<vector<pair<int, ll>>>& g) {
    vector<ll> dist(n + 1, 0);
    vector<int> cnt(n + 1, 0);                 // cnt[v] = 当前最短路的边数
    vector<char> inq(n + 1, 1);
    queue<int> q;
    for (int i = 1; i <= n; i++) q.push(i);
    while (!q.empty()) {
        int u = q.front(); q.pop(); inq[u] = 0;
        for (auto [v, w] : g[u]) {
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                cnt[v] = cnt[u] + 1;
                if (cnt[v] >= n) return true;  // 最短路边数 >= n 说明有环
                if (!inq[v]) { inq[v] = 1; q.push(v); }
            }
        }
    }
    return false;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n, m;
    cin >> n >> m;
    vector<vector<pair<int, ll>>> g(n + 1);
    for (int i = 0; i < m; i++) { int u, v; ll w; cin >> u >> v >> w; g[u].push_back({v, w}); }
    cout << (hasNegativeCycle(n, g) ? "Yes" : "No") << '\n';
    return 0;
}
```

### SPFA求最短路

> 【10.22 结构体封装】原全局变量 + 全局函数改为结构体 / 传参函数，用法见块内速查卡。

```cpp
using ll = long long;

// 【速查】SPFA 单源最短路（可有负边，无负环）
// 用法: vector<vector<pair<int,ll>>> g(n + 1);  g[u].push_back({v, w});   // 点 1-based
//       auto d = spfa(n, g, s);   // d[v] = s 到 v 的最短路，到不了为 SPFA_INF
// 复杂度: 一般很快，最坏 O(nm)（会被卡）；没有负边请用 Dijkstra
const ll SPFA_INF = LLONG_MAX / 4;
vector<ll> spfa(int n, const vector<vector<pair<int, ll>>>& g, int s) {
    vector<ll> d(n + 1, SPFA_INF);
    vector<char> inq(n + 1, 0);
    queue<int> q;
    d[s] = 0; q.push(s); inq[s] = 1;
    while (!q.empty()) {
        int u = q.front(); q.pop(); inq[u] = 0;
        for (auto [v, w] : g[u]) {
            if (d[u] + w < d[v]) {
                d[v] = d[u] + w;
                if (!inq[v]) { inq[v] = 1; q.push(v); }
            }
        }
    }
    return d;
}

// 示例: 读 n m 和 m 条边，输出 1 到 n 的最短路，到不了输出 impossible
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n, m;
    cin >> n >> m;
    vector<vector<pair<int, ll>>> g(n + 1);
    for (int i = 0; i < m; i++) { int u, v; ll w; cin >> u >> v >> w; g[u].push_back({v, w}); }
    auto d = spfa(n, g, 1);
    if (d[n] >= SPFA_INF) cout << "impossible" << '\n';
    else cout << d[n] << '\n';
    return 0;
}
```

### 普通Dijkstra求最短路

> 【10.22 结构体封装】原全局变量 + 全局函数改为结构体 / 传参函数，用法见块内速查卡。

```cpp
using ll = long long;

// 【速查】朴素 Dijkstra（稠密图）
// 用法: vector<vector<pair<int,ll>>> g(n + 1);  g[u].push_back({v, w});   // 点 1-based，边权非负
//       auto d = dijkstraDense(n, g, s);   // 到不了为 DIJ_INF
// 复杂度: O(n² + m)，n ≤ 几千、m 接近 n² 时用；稀疏图用堆优化版
const ll DIJ_INF = LLONG_MAX / 4;
vector<ll> dijkstraDense(int n, const vector<vector<pair<int, ll>>>& g, int s) {
    vector<ll> d(n + 1, DIJ_INF);
    vector<char> done(n + 1, 0);
    d[s] = 0;
    for (int it = 0; it < n; it++) {
        int t = -1;
        for (int j = 1; j <= n; j++) if (!done[j] && (t == -1 || d[j] < d[t])) t = j;
        if (d[t] >= DIJ_INF) break;
        done[t] = 1;
        for (auto [v, w] : g[t]) d[v] = min(d[v], d[t] + w);
    }
    return d;
}

// 示例: 读 n m 和 m 条边，输出 1 到 n 的最短路，到不了输出 -1
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n, m;
    cin >> n >> m;
    vector<vector<pair<int, ll>>> g(n + 1);
    for (int i = 0; i < m; i++) { int u, v; ll w; cin >> u >> v >> w; g[u].push_back({v, w}); }
    auto d = dijkstraDense(n, g, 1);
    cout << (d[n] >= DIJ_INF ? -1 : d[n]);
    return 0;
}
```

### 堆优化Dijkstra求最短路

> 【本次修复】注释"适用于稠密图"已纠正为稀疏图 O(m log m)。
> 【10.22 结构体封装】原全局变量 + 全局函数改为结构体 / 传参函数，用法见块内速查卡。

```cpp
using ll = long long;

// 【速查】堆优化 Dijkstra（稀疏图）
// 用法: vector<vector<pair<int,ll>>> g(n + 1);  g[u].push_back({v, w});   // {终点, 边权}，与其他最短路模板一致
//       auto d = dijkstra(n, g, s);   // d[v] = s 到 v 的最短路，到不了为 DIJ_INF；边权必须非负
// 复杂度: O(m log m)
// 10.22 改动: 原版存边是 {边权, 终点}，与其他模板相反，已统一为 {终点, 边权}
const ll DIJ_INF = LLONG_MAX / 4;
vector<ll> dijkstra(int n, const vector<vector<pair<int, ll>>>& g, int s) {
    vector<ll> d(n + 1, DIJ_INF);
    priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<>> pq;
    d[s] = 0; pq.push({0, s});
    while (!pq.empty()) {
        auto [du, u] = pq.top(); pq.pop();
        if (du != d[u]) continue;              // 过期的堆元素
        for (auto [v, w] : g[u])
            if (du + w < d[v]) { d[v] = du + w; pq.push({d[v], v}); }
    }
    return d;
}

// 示例: 读 n m 和 m 条边，输出 1 到 n 的最短路，到不了输出 -1
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n, m;
    cin >> n >> m;
    vector<vector<pair<int, ll>>> g(n + 1);
    for (int i = 0; i < m; i++) { int u, v; ll w; cin >> u >> v >> w; g[u].push_back({v, w}); }
    auto d = dijkstra(n, g, 1);
    cout << (d[n] >= DIJ_INF ? -1 : d[n]);
    return 0;
}
```

### Floyd求多源最短路

> 【10.22 结构体封装】原全局变量 + 全局函数改为结构体 / 传参函数，用法见块内速查卡。

```cpp
using ll = long long;

// 【速查】Floyd 全源最短路
// 用法: Floyd f(n);  f.addEdge(x, y, w);（有向，重边自动取最小）;  f.run();
//       f.d[i][j] 即 i 到 j 的最短路，f.reachable(i, j) 判可达；点 1-based
// 可有负边，不能有负环（跑完后 f.d[i][i] < 0 说明有负环）
// 复杂度: O(n³)，n ≤ 500 左右
struct Floyd {
    static constexpr ll INF = LLONG_MAX / 4;
    int n;
    vector<vector<ll>> d;
    Floyd(int n) : n(n), d(n + 1, vector<ll>(n + 1, INF)) { for (int i = 1; i <= n; i++) d[i][i] = 0; }
    void addEdge(int x, int y, ll w) { d[x][y] = min(d[x][y], w); }
    void run() {
        for (int k = 1; k <= n; k++)
            for (int i = 1; i <= n; i++) {
                if (d[i][k] >= INF) continue;
                for (int j = 1; j <= n; j++)
                    if (d[k][j] < INF) d[i][j] = min(d[i][j], d[i][k] + d[k][j]);
            }
    }
    bool reachable(int i, int j) { return d[i][j] < INF / 2; }
};

// 示例: 读 n m k、m 条边、k 个询问，输出最短路或 impossible
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n, m, k;
    cin >> n >> m >> k;
    Floyd f(n);
    for (int i = 0; i < m; i++) { int x, y; ll z; cin >> x >> y >> z; f.addEdge(x, y, z); }
    f.run();
    while (k--) {
        int x, y; cin >> x >> y;
        if (!f.reachable(x, y)) cout << "impossible" << '\n';
        else cout << f.d[x][y] << '\n';
    }
    return 0;
}
```

### Kruskal求最小生成树

> 【10.22 结构体封装】原全局变量 + 全局函数改为结构体 / 传参函数，用法见块内速查卡。

```cpp
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
```

### Prim求最小生成树

> 边权与答案用 long long（10.22 修复了不连通判断）。
> 【10.22 结构体封装】原全局变量 + 全局函数改为结构体 / 传参函数，用法见块内速查卡。

```cpp
using ll = long long;

// 【速查】朴素 Prim 最小生成树（稠密图）
// 用法: Prim p(n);  p.addEdge(a, b, w);（无向，重边取最小）;  ll res; bool ok = p.run(res);
// 返回: ok = false 表示图不连通；res = MST 权值和；点 1-based
// 复杂度: O(n²)，n ≤ 几千的稠密图（稀疏图用 Kruskal）
struct Prim {
    static constexpr ll INF = LLONG_MAX / 4;
    int n;
    vector<vector<ll>> g;
    Prim(int n) : n(n), g(n + 1, vector<ll>(n + 1, INF)) {}
    void addEdge(int a, int b, ll w) { g[a][b] = g[b][a] = min(g[a][b], w); }
    bool run(ll& res) {
        vector<ll> dist(n + 1, INF);
        vector<char> in(n + 1, 0);
        res = 0;
        dist[1] = 0;
        for (int it = 0; it < n; it++) {
            int t = -1;
            for (int j = 1; j <= n; j++) if (!in[j] && (t == -1 || dist[j] < dist[t])) t = j;
            if (dist[t] >= INF) return false;     // 剩下的点都连不上
            res += dist[t];
            in[t] = 1;
            for (int j = 1; j <= n; j++) if (!in[j]) dist[j] = min(dist[j], g[t][j]);
        }
        return true;
    }
};

// 示例: 读 n m 和 m 条边，输出 MST 权值和，不连通输出 impossible
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n, m;
    cin >> n >> m;
    Prim p(n);
    for (int i = 0; i < m; i++) { int a, b; ll w; cin >> a >> b >> w; p.addEdge(a, b, w); }
    ll res;
    if (!p.run(res)) cout << "impossible" << '\n';
    else cout << res << '\n';
    return 0;
}
```

### Boruvka解完全图最小生成树

> 针对"完全图、边权=(val[u]+val[v]) mod k"的专用版，O(n log²n)。
> 【10.22 结构体封装】原全局变量 + 全局函数改为结构体 / 传参函数，用法见块内速查卡。

```cpp
using ll = long long;

// 【速查】Boruvka 完全图最小生成树
// 用途: n 个点两两有边、边权由公式给出，边太多不能全建。本例 w(i, j) = (a_i + a_j) mod k
// 用法: ll ans = boruvkaModSum(a, k);   // a 为 0-based 点权
// 套路: 每轮给每个连通块找"连到别的块的最小边"，全部合并；块数每轮至少减半，最多 log n 轮
//       换题只需改 bestEdge 里"在别的块中找使 w(j, ?) 最小的点"的查询方式（这里用 multiset 二分）
// 复杂度: O(n log² n)
ll boruvkaModSum(vector<ll> a, ll k) {
    int n = a.size();
    for (auto& x : a) x %= k;
    vector<int> fa(n);
    iota(fa.begin(), fa.end(), 0);
    auto find = [&](int x) { while (fa[x] != x) x = fa[x] = fa[fa[x]]; return x; };
    multiset<pair<ll, int>> st;                 // 所有点 (值, 编号)
    for (int i = 0; i < n; i++) st.insert({a[i], i});
    ll ans = 0;
    int merged = 0;
    vector<vector<int>> block(n);
    while (merged < n - 1) {
        for (auto& b : block) b.clear();
        for (int i = 0; i < n; i++) block[find(i)].push_back(i);
        vector<array<ll, 3>> best(n, {LLONG_MAX, -1, -1});   // 每个块的最小出边 (w, u, v)
        for (int r = 0; r < n; r++) {
            if (block[r].empty()) continue;
            for (int j : block[r]) st.erase({a[j], j});   // 暂时删掉本块的点，set 里只剩别的块
            for (int j : block[r]) {
                // (a_j + x) mod k 最小: 要么取 x >= k - a_j 中最小的（和 >= k），要么取最小的 x
                for (ll tar : {k - a[j], 0LL}) {
                    auto it = st.lower_bound({tar, -1});
                    if (it == st.end()) continue;
                    ll w = (it->first + a[j]) % k;
                    if (w < best[r][0]) best[r] = {w, j, it->second};
                }
            }
            for (int j : block[r]) st.insert({a[j], j});
        }
        for (int r = 0; r < n; r++) {
            if (best[r][1] == -1) continue;
            int x = find(best[r][1]), y = find(best[r][2]);
            if (x != y) { fa[x] = y; ans += best[r][0]; merged++; }
        }
    }
    return ans;
}

// 示例: 读 t 组，每组 n k 和 n 个数，输出 MST 权值和
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int t;
    cin >> t;
    while (t--) {
        int n; ll k;
        cin >> n >> k;
        vector<ll> a(n);
        for (auto& x : a) cin >> x;
        cout << boruvkaModSum(a, k) << "\n";
    }
    return 0;
}
```

### 匈牙利算法二分图最大匹配

> 【本次修复】原版 assign 写在读入之前，任何输入必崩 bad_alloc；已修复。
> 【10.22 结构体封装】原全局变量 + 全局函数改为结构体 / 传参函数，用法见块内速查卡。

```cpp
// 【速查】匈牙利算法（二分图最大匹配）
// 用法: Hungarian h(n1, n2);  h.addEdge(u, v);   // 左部 1..n1，右部 1..n2，只建 左 -> 右
//       int ans = h.solve();   h.matchR[v] = 与右点 v 匹配的左点（0 为未匹配），h.matchL[u] 同理
// 常用: 最小点覆盖 = 最大匹配；最大独立集 = 点数 - 最大匹配；DAG 最小路径覆盖 = n - 最大匹配（拆点）
// 复杂度: O(n1 · m)；规模到 1e5 级改用 Dinic；递归深度 ≤ n1
struct Hungarian {
    int n1, n2, stamp = 0;
    vector<vector<int>> g;
    vector<int> matchL, matchR, vis;

    Hungarian(int n1, int n2) : n1(n1), n2(n2), g(n1 + 1), matchL(n1 + 1, 0), matchR(n2 + 1, 0), vis(n2 + 1, 0) {}
    void addEdge(int u, int v) { g[u].push_back(v); }
    bool dfs(int u) {
        for (int v : g[u]) {
            if (vis[v] == stamp) continue;
            vis[v] = stamp;
            if (!matchR[v] || dfs(matchR[v])) { matchR[v] = u; matchL[u] = v; return true; }
        }
        return false;
    }
    int solve() {
        int res = 0;
        for (int u = 1; u <= n1; u++) { ++stamp; if (dfs(u)) res++; }   // 时间戳代替每轮清空 vis
        return res;
    }
};

/*
补充定义和定理：
最大匹配数：最大匹配的匹配边的数目
最小点覆盖数：选取最少的点，使任意一条边至少有一个端点被选择
最大独立集：选取最多的点，使任意所选两点均不相连
最小路径覆盖数：对于一个 DAG（有向无环图），选取最少条路径，使得每个顶点属于且仅属于一条路径。路径长可以为 0（即单个点）。
定理1：最大匹配数 = 最小点覆盖数（这是 Konig 定理）
定理2：最大独立集 = 顶点数 - 最大匹配数
定理3：最小路径覆盖数 = 顶点数 - 最大匹配数
*/

// 示例: 读 n1 n2 m 和 m 条 左->右 边，输出最大匹配数
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n1, n2, m;
    cin >> n1 >> n2 >> m;
    Hungarian h(n1, n2);
    for (int i = 0; i < m; i++) { int u, v; cin >> u >> v; h.addEdge(u, v); }
    cout << h.solve() << "\n";
    return 0;
}
```

### 带权二分图最优匹配（KM算法）

> 【10.22 修复】DFS 版（最坏 O(n⁴)，n=500 构造数据 10s）改为 BFS 版 O(n³)。注意：左右部规模必须相等；w 默认 KM::NO_EDGE 表示无边，返回值 < KM::NO_EDGE/2 表示无完美匹配。
> 【10.22 结构体封装】原全局变量 + 全局函数改为结构体 / 传参函数，用法见块内速查卡。

```cpp
using ll = long long;

// 【速查】KM 算法（二分图最大权完美匹配，BFS 版）
// 用法: KM km(n);  km.w[i][j] = 左 i 连右 j 的权（1-based，默认 KM::NO_EDGE 表示无边）;  ll ans = km.solve();
// 返回: 最大权和；km.match[j] = 与右点 j 匹配的左点；返回值 < KM::NO_EDGE / 2 表示不存在完美匹配
// 最小权: 权取负，答案取 -solve()；左右部规模不等时补虚点（权 0）补成方阵
// 复杂度: O(n³)，n ≤ 500 没问题
// 原理: 维护顶标 la[x] + lb[y] >= w[x][y]，只走相等边。每轮从一个左部点出发，
//   像 Dijkstra 一样用 slack 记录未访问右部点的最小差值，每步把整棵交错树的顶标
//   平移 d（树内相等边不变，树外 slack 减 d），O(n) 步找到增广路，总 O(n³)。
// 10.22: 原 DFS 版最坏 O(n⁴)（n=500、w=i·j 实测 10s），已换成 BFS 版；本次再封装成结构体
struct KM {
    static constexpr ll NO_EDGE = -(ll)1e15;
    int n;
    vector<vector<ll>> w;
    vector<int> match;                         // match[y]: 与右部点 y 匹配的左部点

    KM(int n) : n(n), w(n + 1, vector<ll>(n + 1, NO_EDGE)), match(n + 1, 0) {}

    ll solve() {
        const ll BIG = LLONG_MAX / 4;
        vector<ll> la(n + 1, NO_EDGE), lb(n + 1, 0), slk(n + 1);
        vector<int> pre(n + 1);
        vector<char> visb(n + 1);
        for (int i = 1; i <= n; i++) for (int j = 1; j <= n; j++) la[i] = max(la[i], w[i][j]);
        fill(match.begin(), match.end(), 0);
        for (int s = 1; s <= n; s++) {
            fill(visb.begin(), visb.end(), 0); fill(slk.begin(), slk.end(), BIG); fill(pre.begin(), pre.end(), 0);
            int y = 0;
            match[0] = s;                      // 右部虚点 0 挂在起点 s 上
            do {
                visb[y] = 1;
                int x = match[y], ny = 0;
                ll d = BIG;
                for (int j = 1; j <= n; j++) {
                    if (visb[j]) continue;
                    ll t = la[x] + lb[j] - w[x][j];
                    if (t < slk[j]) { slk[j] = t; pre[j] = y; }
                    if (slk[j] < d) { d = slk[j]; ny = j; }
                }
                for (int j = 0; j <= n; j++) {
                    if (visb[j]) { la[match[j]] -= d; lb[j] += d; }
                    else slk[j] -= d;
                }
                y = ny;
            } while (match[y]);                // 走到未匹配的右部点即找到增广路
            while (y) { match[y] = match[pre[y]]; y = pre[y]; }   // 沿前驱翻转
        }
        ll ans = 0;
        for (int j = 1; j <= n; j++) ans += w[match[j]][j];
        return ans;
    }
};

// 示例: 读 n 和 n×n 权值矩阵，输出最大权完美匹配
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n;
    cin >> n;
    KM km(n);
    for (int i = 1; i <= n; i++) for (int j = 1; j <= n; j++) cin >> km.w[i][j];
    cout << km.solve() << "\n";
    return 0;
}
```

### 染色法判定二分图

> BFS 非递归实现。
> 【10.22 结构体封装】原全局变量 + 全局函数改为结构体 / 传参函数，用法见块内速查卡。

```cpp
// 【速查】染色法判二分图（BFS，非递归）
// 用法: vector<vector<int>> g(n + 1);  建双向边;   vector<int> color;  bool ok = isBipartite(n, g, color);
// 结果: ok 时 color[i] ∈ {1, 2} 是两侧的划分（每个连通块独立染色）
// 复杂度: O(n + m)；点 1-based
bool isBipartite(int n, const vector<vector<int>>& g, vector<int>& color) {
    color.assign(n + 1, 0);
    for (int s = 1; s <= n; s++) {
        if (color[s]) continue;
        color[s] = 1;
        vector<int> q = {s};
        for (size_t i = 0; i < q.size(); i++) {
            int u = q[i];
            for (int v : g[u]) {
                if (!color[v]) { color[v] = 3 - color[u]; q.push_back(v); }
                else if (color[v] == color[u]) return false;
            }
        }
    }
    return true;
}

// 示例: 读 n m 和 m 条无向边，输出 Yes / No
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n, m;
    cin >> n >> m;
    vector<vector<int>> g(n + 1);
    for (int i = 0; i < m; i++) { int u, v; cin >> u >> v; g[u].push_back(v); g[v].push_back(u); }
    vector<int> color;
    cout << (isBipartite(n, g, color) ? "Yes" : "No") << '\n';
    return 0;
}
```

## 双联通分量

### 边双连通分量（Edge-BCC）

> 含桥判定与分量划分；按边编号跳过父边，正确处理重边。

```cpp
// 【速查】边双连通分量 + 桥（无向图）
// 用途: 求桥；把边双缩成点后得到一棵树（"桥树"）
// 用法: EdgeBCC e(n); e.addEdge(u, v)（返回边编号 0..m-1）; e.build();
// 结果: e.isBridge[id]、e.belong[u]（所属边双 0..bccCnt-1）、e.components[c]（点集）
// 复杂度: O(n + m)；支持重边和自环；点 1-based
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
```

### 点双连通分量（Vertex-BCC）

> 含割点判定与边栈实现；约定：桥形成 {u,v} 点双，孤立点自成一点双。

```cpp
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
```

## 网络流

### Dinic最大流（单路增广）

```cpp
using ll = long long;
const ll INF = 4e18;

// 【速查】Dinic 最大流（单路增广 + 当前弧优化）
// 用法: Dinic mf(点数); mf.addEdge(u, v, cap);  ll f = mf.maxflow(s, t);   // 点编号 0..点数-1，1-based 就开 n+1
// 复杂度: 一般 O(n²m)，实际很快；二分图上 O(m√n)
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
```

### Dinic最大流（批量增广）

> 多路增广+当前弧，含自环 rev 处理；一般图 O(V²E)，二分图 O(E√V)。

```cpp
using ll = long long;
const ll INF = 4e18;
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

// 【速查】Dinic 最大流（多路增广版，常数更小）
// 用法: Dinic mf(点数); mf.addEdge(u, v, cap);  ll f = mf.maxFlow(s, t);   // 注意是 maxFlow（大写 F）
// 复杂度: 同 Dinic；卡常时用这个
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
```

### Dinic最小割

> 最大流后残量网络 BFS 分类 S/T 侧。

```cpp
using ll = long long;
const ll INF = 4e18;
// 【速查】最小割（Dinic + 找割集）
// 用法: Dinic mf(点数); mf.addEdge(u, v, cap); ll cut = mf.maxflow(s, t);  auto side = mf.mincut(s);
// 结果: 最小割 = 最大流；side[u] = 1 表示 u 在 S 侧；割边 = 从 S 侧指向 T 侧的原边
// 复杂度: 最大流 + O(n + m)
struct Dinic {
    struct Edge {
        int to, rev;
        ll cap;

        // 如果需要输出原图边，可以记录原始容量
        ll originalCap;
    };

    int n;
    vector<vector<Edge>> g;
    vector<int> level, cur;

    Dinic(int n) : n(n), g(n), level(n), cur(n) {}

    void addEdge(int u, int v, ll cap) {
        Edge a{v, (int)g[v].size(), cap, cap};
        Edge b{u, (int)g[u].size(), 0, 0};

        g[u].push_back(a);
        g[v].push_back(b);
    }

    bool bfs(int s, int t) {
        fill(level.begin(), level.end(), -1);

        queue<int> q;
        q.push(s);
        level[s] = 0;

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
        if (u == t)
            return flow;

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

    // 最大流跑完后调用
    // vis[i] = 1：最小割 S 侧
    // vis[i] = 0：最小割 T 侧
    vector<int> mincut(int s) {
        vector<int> vis(n, 0);
        queue<int> q;

        vis[s] = 1;
        q.push(s);

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (auto &e : g[u]) {
                if (e.cap > 0 && !vis[e.to]) {
                    vis[e.to] = 1;
                    q.push(e.to);
                }
            }
        }

        return vis;
    }
};


void solve() {
    int n, m, S, T;
    cin >> n >> m >> S >> T;

    Dinic mf(n + 1);

    for (int i = 0; i < m; i++) {
        int u, v;
        ll w;
        cin >> u >> v >> w;

        mf.addEdge(u, v, w);
    }

    ll flow = mf.maxflow(S, T);

    cout << "mincut = " << flow << '\n';

    auto side = mf.mincut(S);

    cout << "S side: ";
    for (int i = 1; i <= n; i++) {
        if (side[i])
            cout << i << ' ';
    }
    cout << '\n';

    cout << "T side: ";
    for (int i = 1; i <= n; i++) {
        if (!side[i])
            cout << i << ' ';
    }
    cout << '\n';
}
```

### EK最大流

```cpp
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
```

### 最小费用最大流（SPFA-SSP）

> 要求初始残量网络无负费用环。

```cpp
using ll = long long;
const ll INF = 4e18;
// 【速查】最小费用最大流（SPFA 版 SSP）
// 用法: MinCostMaxFlow mf(点数); mf.addEdge(u, v, cap, cost); auto [flow, cost] = mf.minCostMaxFlow(s, t);
// 返回: {最大流, 在此流量下的最小费用}；最大费用就把 cost 取负、答案取负
// 前提: 可以有负费用边，但不能有负费用环（否则死循环）
// 复杂度: O(流量 × SPFA)，一般够用
struct MinCostMaxFlow {
    struct Edge {
        int to, rev;
        ll cap, cost;
    };

    int n;
    vector<vector<Edge>> g;

    MinCostMaxFlow(int n) : n(n), g(n) {}

    void addEdge(int u, int v, ll cap, ll cost) {
        Edge a{v, (int)g[v].size(), cap, cost};
        Edge b{u, (int)g[u].size(), 0, -cost};

        g[u].push_back(a);
        g[v].push_back(b);
    }

    // 返回 {最大流, 最小费用}
    pair<ll, ll> minCostMaxFlow(int s, int t) {
        ll maxFlow = 0;
        ll minCost = 0;

        vector<ll> dist(n);
        vector<int> inq(n);
        vector<int> preV(n), preE(n);

        while (true) {
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

            if (dist[t] == INF)
                break;

            ll f = INF;

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

            maxFlow += f;
            minCost += f * dist[t];
        }

        return {maxFlow, minCost};
    }
};

void solve() {
    int n, m;
    cin >> n >> m;

    int S = 0;
    int T = n + m + 1;

    MinCostMaxFlow mf(T + 1);

    for (int i = 1; i <= n; i++) {
        mf.addEdge(S, i, 1, 0);

        int x;
        cin >> x;

        while (x--) {
            int w;
            cin >> w;

            mf.addEdge(i, n + w, 1, 0);
        }
    }

    for (int i = 1; i <= m; i++) {
        int y;
        cin >> y;

        while (y--) {
            ll w;
            cin >> w;

            mf.addEdge(n + i, T, 1, w);
        }
    }

    auto [flow, cost] = mf.minCostMaxFlow(S, T);

    cout << cost << '\n';
}
```

### 最小费用最大流（批量增广）

> SPFA 定距离 + 最短路 DAG 上 Dinic 批量推流；不检测负费用环。

```cpp
using ll = long long;
const ll INF = 4e18;
//批量增广最小费用最大流

/*
适用：
1. 在最大化流量的前提下，使总费用最小。
2. 允许负费用边，但初始残量网络不能含负费用环。
   例如：初始分层 DAG 可以包含负费用边。
3. 容量必须非负，源点和汇点必须不同。
4. 同费用最短增广路较多时，批量增广通常更有优势。

实现：
SPFA 求最短距离；
然后只使用满足 dist[v] == dist[u] + cost 的边，
用 Dinic 批量增广，直到当前最短路无法继续增广。

复杂度与限制：
1. 单次 SPFA 最坏 O(VE)。
2. 固定最短距离后的 Dinic 部分，一般图最坏 O(V^2 * E)。
3. 批量增广减少最短路计算次数，但不消除 SPFA 的退化风险。
4. 若每轮只能增广一条路径，额外的 BFS、DFS 可能没有收益。
5. 本模板不检测负费用环，不适合直接处理含负费用环的网络。
6. 距离、总流量、总费用及中间乘加结果均需在 long long 安全范围内。
7. DFS 使用递归，路径特别深时需要注意栈空间。
// 【速查】最小费用最大流（SPFA + 最短路图上 Dinic 多路增广）
// 用法: MinCostMaxFlowBatch mf(点数); mf.addEdge(u, v, cap, cost); auto [flow, cost] = mf.minCostMaxFlow(s, t);
// 适用: 同费用的增广路很多时（如费用种类少）比普通版快；前提同样是无负费用环
// 复杂度: 每轮 SPFA + Dinic，轮数 ≤ 不同最短路长度个数
*/
struct MinCostMaxFlowBatch {
    using ll = long long;
    static constexpr ll INF = (1LL << 62);

    struct Edge {
        int to, rev;
        ll cap, cost;
    };

    int n;
    vector<vector<Edge>> g;
    vector<ll> dist;
    vector<int> inq, level, cur;

    MinCostMaxFlowBatch(int n)
        : n(n), g(n), dist(n), inq(n), level(n), cur(n) {}

    void addEdge(int u, int v, ll cap, ll cost) {
        int iu = (int)g[u].size();
        int iv = (int)g[v].size();

        if (u == v) ++iv;

        g[u].push_back({v, iv, cap, cost});

        // 反向边用于撤销之前的流量，费用取相反数
        g[v].push_back({u, iu, 0, -cost});
    }

    // 在当前残量网络上求费用最短路
    bool spfa(int s, int t) {
        fill(dist.begin(), dist.end(), INF);
        fill(inq.begin(), inq.end(), 0);

        queue<int> q;
        dist[s] = 0;
        inq[s] = 1;
        q.push(s);

        while (!q.empty()) {
            int u = q.front();
            q.pop();
            inq[u] = 0;

            for (const auto &e : g[u]) {
                if (e.cap > 0 &&
                    dist[e.to] > dist[u] + e.cost) {

                    dist[e.to] = dist[u] + e.cost;

                    if (!inq[e.to]) {
                        inq[e.to] = 1;
                        q.push(e.to);
                    }
                }
            }
        }

        return dist[t] != INF;
    }

    // 仅在满足最短路等式的残量边上建立层次图
    bool bfs(int s, int t) {
        fill(level.begin(), level.end(), -1);

        queue<int> q;
        level[s] = 0;
        q.push(s);

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (const auto &e : g[u]) {
                if (e.cap > 0 &&
                    level[e.to] == -1 &&
                    dist[e.to] == dist[u] + e.cost) {

                    level[e.to] = level[u] + 1;
                    q.push(e.to);
                }
            }
        }

        return level[t] != -1;
    }

    // 沿层次递增的最短路边批量推流
    ll dfs(int u, int t, ll limit) {
        if (u == t) return limit;

        ll sent = 0;

        for (int &i = cur[u]; i < (int)g[u].size(); ++i) {
            Edge &e = g[u][i];

            if (e.cap <= 0 ||
                level[e.to] != level[u] + 1 ||
                dist[e.to] != dist[u] + e.cost) {
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

    // 返回 {最大流量, 对应的最小总费用}
    pair<ll, ll> minCostMaxFlow(int s, int t) {
        assert(s != t);

        ll flow = 0, cost = 0;

        while (spfa(s, t)) {
            // 固定最短距离，将当前最短路尽量增广完
            while (bfs(s, t)) {
                fill(cur.begin(), cur.end(), 0);

                ll pushed;
                while ((pushed = dfs(s, t, INF)) > 0) {
                    flow += pushed;

                    // 本轮每单位流量的路径费用都是 dist[t]
                    cost += pushed * dist[t];
                }
            }
        }

        return {flow, cost};
    }
};
```

### Dijkstra+势函数费用流（可限额k）

> 初始负费用边用一次 SPFA 求势能，之后全程 Dijkstra。

```cpp
using ll = long long;
const ll INF = 4e18;
// 【速查】最小费用流（Dijkstra + 势能，可限制流量 k）
// 用法: MinCostMaxFlow mf(点数); mf.addEdge(u, v, cap, cost); auto [flow, cost] = mf.minCostFlow(s, t, k);
// 返回: 最多流 k 单位时的 {流量, 最小费用}；k 传 INF 就是最小费用最大流；flow < k 说明流不到 k
// 前提: 允许负费用边（先 SPFA 一次求初始势能），不能有负费用环
// 复杂度: O(流量 × m log n)，比 SPFA 版稳
// 用dij()代替spfa
struct MinCostMaxFlow {
    struct Edge {
        int to, rev;
        ll cap, cost;
    };

    int n;
    vector<vector<Edge>> g;

    MinCostMaxFlow(int n) : n(n), g(n) {}

    void addEdge(int u, int v, ll cap, ll cost) {
        Edge a{v, (int)g[v].size(), cap, cost};
        Edge b{u, (int)g[u].size(), 0, -cost};

        g[u].push_back(a);
        g[v].push_back(b);
    }

    pair<ll, ll> minCostFlow(int s, int t, ll k) {
        const ll INF = (1LL << 62);

        ll flow = 0;
        ll cost = 0;

        // 势能
        vector<ll> h(n, INF);

        /*
         * 因为初始存在负费用边，所以先 SPFA 一次，
         * 得到从 s 出发的最短距离作为初始势能。
         */
        {
            vector<int> inq(n, 0);
            queue<int> q;

            h[s] = 0;
            q.push(s);
            inq[s] = 1;

            while (!q.empty()) {
                int u = q.front();
                q.pop();
                inq[u] = 0;

                for (auto &e : g[u]) {
                    if (e.cap <= 0) continue;

                    if (h[u] != INF &&
                        h[e.to] > h[u] + e.cost) {

                        h[e.to] = h[u] + e.cost;

                        if (!inq[e.to]) {
                            inq[e.to] = 1;
                            q.push(e.to);
                        }
                    }
                }
            }

            // 不可达点势能设 0
            for (int i = 0; i < n; i++) {
                if (h[i] == INF)
                    h[i] = 0;
            }
        }

        vector<ll> dist(n);
        vector<int> preV(n), preE(n);

        while (flow < k) {
            fill(dist.begin(), dist.end(), INF);

            priority_queue<
                pair<ll, int>,
                vector<pair<ll, int>>,
                greater<pair<ll, int>>
            > pq;

            dist[s] = 0;
            pq.push({0, s});

            while (!pq.empty()) {
                auto [d, u] = pq.top();
                pq.pop();

                if (d != dist[u])
                    continue;

                for (int i = 0; i < (int)g[u].size(); i++) {
                    Edge &e = g[u][i];

                    if (e.cap <= 0)
                        continue;

                    // reduced cost
                    ll nd = d + e.cost + h[u] - h[e.to];

                    if (dist[e.to] > nd) {
                        dist[e.to] = nd;

                        preV[e.to] = u;
                        preE[e.to] = i;

                        pq.push({nd, e.to});
                    }
                }
            }

            if (dist[t] == INF)
                break;

            // 更新势能
            for (int i = 0; i < n; i++) {
                if (dist[i] != INF)
                    h[i] += dist[i];
            }

            ll f = k - flow;

            for (int v = t; v != s; v = preV[v]) {
                int u = preV[v];
                int id = preE[v];

                f = min(f, g[u][id].cap);
            }

            // 求这一条增广路的真实费用
            ll pathCost = 0;

            for (int v = t; v != s; v = preV[v]) {
                int u = preV[v];
                int id = preE[v];

                Edge &e = g[u][id];

                pathCost += e.cost;

                e.cap -= f;
                g[v][e.rev].cap += f;
            }

            flow += f;
            cost += f * pathCost;
        }

        return {flow, cost};
    }
};
```

### 限额k最小费用流（SPFA版）

> 结构体 MinCostFlowK，接口与 Dijkstra+势函数版一致；返回{实际流量,最小费用}。
> 【10.22 结构体封装】原全局变量 + 全局函数改为结构体 / 传参函数，用法见块内速查卡。

```cpp
using ll = long long;
const ll INF = 4e18;

// 【速查】限额 k 的最小费用流（SPFA 版 SSP，结构体封装）
// 用法: MinCostFlowK mf(点数);  mf.addEdge(u, v, cap, cost);  auto [flow, cost] = mf.minCostFlow(s, t, k);
// 返回: {实际流量, 最小费用}；flow < k 说明达不到 k；k 传 INF 就是最小费用最大流
// 复杂度: O(k × SPFA)；可有负费用边，不能有负费用环；点 0-based
// 10.22: 原版是全局 n / g + 裸函数，已封装为结构体（接口与 dij_k_flow_cost 一致）
struct MinCostFlowK {
    struct Edge { int to, rev; ll cap, cost; };
    int n;
    vector<vector<Edge>> g;

    MinCostFlowK(int n) : n(n), g(n) {}
    void addEdge(int u, int v, ll cap, ll cost) {
        g[u].push_back({v, (int)g[v].size(), cap, cost});
        g[v].push_back({u, (int)g[u].size() - 1, 0, -cost});
    }
    pair<ll, ll> minCostFlow(int s, int t, ll k) {
        ll flow = 0, cost = 0;
        vector<ll> dist(n);
        vector<int> inq(n), preV(n), preE(n);
        while (flow < k) {
            fill(dist.begin(), dist.end(), INF);
            fill(inq.begin(), inq.end(), 0);
            queue<int> q;
            dist[s] = 0; q.push(s); inq[s] = 1;
            while (!q.empty()) {
                int u = q.front(); q.pop(); inq[u] = 0;
                for (int i = 0; i < (int)g[u].size(); i++) {
                    Edge& e = g[u][i];
                    if (e.cap > 0 && dist[e.to] > dist[u] + e.cost) {
                        dist[e.to] = dist[u] + e.cost;
                        preV[e.to] = u; preE[e.to] = i;
                        if (!inq[e.to]) { inq[e.to] = 1; q.push(e.to); }
                    }
                }
            }
            if (dist[t] == INF) break;             // 已经无法继续增广
            ll f = k - flow;                        // 本次最多只能补到 k
            for (int v = t; v != s; v = preV[v]) f = min(f, g[preV[v]][preE[v]].cap);
            for (int v = t; v != s; v = preV[v]) {
                Edge& e = g[preV[v]][preE[v]];
                e.cap -= f;
                g[v][e.rev].cap += f;
            }
            flow += f;
            cost += f * dist[t];
        }
        return {flow, cost};
    }
};
```

### 带费用上下界最小费用可行循环流

> addEdge(u,v,lower,upper,cost)；solve() 返回{是否可行,最小费用}，getFlow 查每边最终流量。
> 【10.22 修复】支持负费用与负费用环（负费用边先流满再退流）；原版遇负费用环 SPFA 死循环。

```cpp
using ll = long long;
const ll INF = (1LL << 60);

/*==================================================
                普通最小费用最大流
==================================================*/

struct MinCostMaxFlow {
    struct Edge {
        int to, rev;
        ll cap, cost;
    };

    int n;
    vector<vector<Edge>> g;

    MinCostMaxFlow(int n) : n(n), g(n) {}

    // 返回正向边在 g[u] 中的下标
    int addEdge(int u, int v, ll cap, ll cost) {
        int id = g[u].size();

        Edge a{v, (int)g[v].size(), cap, cost};
        Edge b{u, (int)g[u].size(), 0, -cost};

        g[u].push_back(a);
        g[v].push_back(b);

        return id;
    }

    // 最多发送 limit 单位流
    // 返回 {实际流量, 最小费用}
    pair<ll, ll> minCostMaxFlow(
        int s,
        int t,
        ll limit = INF
    ) {
        ll flow = 0;
        ll cost = 0;

        vector<ll> dist(n);
        vector<int> inq(n);
        vector<int> preV(n), preE(n);

        while (flow < limit) {
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

            if (dist[t] == INF)
                break;

            ll f = limit - flow;

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
};


/*==================================================
          带费用上下界最小费用可行循环流

    addEdge(u, v, lower, upper, cost)   要求 0 <= lower <= upper，cost 可正可负

    solve():
        返回 {是否存在可行流, 最小费用}；每个对象只调用一次

    负费用（含负费用环）的处理（10.22 修复）:
        原版把负费用边原样放进残量网络，遇到负费用环时 SPFA 死循环，
        且 SSP 只沿 SS->TT 增广，找不到"环流本身"带来的收益。
        现在负费用边先强制流满 upper（费用、盈亏立即结算），残量网络只放一条
        反向"退流"边 v->u（容量 upper-lower，费用 -cost > 0）。
        这样初始残量网络费用全部非负、没有负环，SSP 求出的就是全局最小费用循环流。
==================================================*/

// 【速查】上下界最小费用可行循环流
// 用途: 每条边流量必须在 [lower, upper] 内、每个点流入=流出，求最小总费用（无源汇的循环流）
// 用法: LowerBoundMinCostFlow L(n);  int id = L.addEdge(u, v, lower, upper, cost);  auto [ok, cost] = L.solve();
//       L.getFlow(id) -> 该边最终流量；点编号 0..n-1；solve() 只能调一次
// 有源汇: 加一条 t->s、[0, INF]、费用 0 的边，就转成循环流
// 复杂度: 一次最小费用流；支持负费用和负费用环
struct LowerBoundMinCostFlow {

    struct OriginalEdge {
        int u;      // 残量边的起点：非负费用边为 u，负费用边为 v
        int id;     // 残量边在 mf.g[u] 中的下标
        ll lower;
        ll upper;
        bool neg;   // 是否按"先流满再退流"处理
    };

    int n;
    int SS, TT;

    MinCostMaxFlow mf;

    vector<ll> balance;   // 节点净流入，>0 时由 SS 补入，<0 时流向 TT

    ll baseCost = 0;

    vector<OriginalEdge> edges;

    LowerBoundMinCostFlow(int n)
        : n(n),
          SS(n),
          TT(n + 1),
          mf(n + 2),
          balance(n, 0) {}

    int addEdge(
        int u,
        int v,
        ll lower,
        ll upper,
        ll cost
    ) {
        if (cost >= 0) {
            // 先流 lower，剩余 upper-lower 按原方向可选
            baseCost += lower * cost;
            balance[u] -= lower;
            balance[v] += lower;
            int id = mf.addEdge(u, v, upper - lower, cost);
            edges.push_back({u, id, lower, upper, false});
        } else {
            // 先流满 upper，剩余 upper-lower 作为 v->u 的退流边，费用 -cost > 0
            baseCost += upper * cost;
            balance[u] -= upper;
            balance[v] += upper;
            int id = mf.addEdge(v, u, upper - lower, -cost);
            edges.push_back({v, id, lower, upper, true});
        }
        return (int)edges.size() - 1;
    }

    pair<bool, ll> solve() {

        ll need = 0;

        for (int i = 0; i < n; i++) {

            if (balance[i] > 0) {
                mf.addEdge(
                    SS,
                    i,
                    balance[i],
                    0
                );

                need += balance[i];
            }

            else if (balance[i] < 0) {
                mf.addEdge(
                    i,
                    TT,
                    -balance[i],
                    0
                );
            }
        }

        auto [flow, cost]
            = mf.minCostMaxFlow(SS, TT, need);

        if (flow != need) {
            return {false, 0};
        }

        return {
            true,
            baseCost + cost
        };
    }

    // solve() 后可以查询某条原边最终流量
    ll getFlow(int edgeId) {
        auto &x = edges[edgeId];

        // 残量边上实际走过的流量
        ll used =
            (x.upper - x.lower) - mf.g[x.u][x.id].cap;

        return x.neg ? x.upper - used : x.lower + used;
    }
};
```

### LCA（倍增）与虚树

> 【10.22 新增】非递归倍增 LCA、k 级祖先；按 DFS 序建虚树。

```cpp
// 【速查】LCA（倍增，非递归预处理）
// 用法: LCA t(n);  t.addEdge(u, v);  t.build(root);      // 点 1-based，无向树
//       t.lca(u, v);  t.dist(u, v)（边数，带权就另开前缀和 sum[u] + sum[v] - 2 sum[lca]）;
//       t.kth(u, k) -> u 往上第 k 个祖先（不存在返回 0）;  t.dep[u] 深度（根为 0）、t.tin[u] DFS 序
// 复杂度: 预处理 O(n log n)，每次查询 O(log n)
struct LCA {
    int n, LOG;
    vector<vector<int>> g, up;
    vector<int> dep, tin;

    LCA(int n) : n(n), g(n + 1), dep(n + 1, 0), tin(n + 1, 0) {
        LOG = 1;
        while ((1 << LOG) <= n) LOG++;
        up.assign(LOG, vector<int>(n + 1, 0));
    }
    void addEdge(int u, int v) { g[u].push_back(v); g[v].push_back(u); }
    void build(int root) {
        int timer = 0;
        vector<int> st = {root};
        vector<char> vis(n + 1, 0);
        vis[root] = 1; up[0][root] = 0;
        while (!st.empty()) {                    // 迭代 DFS，深链不会爆栈
            int u = st.back(); st.pop_back();
            tin[u] = ++timer;
            for (int v : g[u]) if (!vis[v]) {
                vis[v] = 1; dep[v] = dep[u] + 1; up[0][v] = u;
                st.push_back(v);
            }
        }
        for (int k = 1; k < LOG; k++)
            for (int v = 1; v <= n; v++) up[k][v] = up[k - 1][up[k - 1][v]];
    }
    int kth(int u, int k) {
        if (k > dep[u]) return 0;
        for (int i = 0; i < LOG; i++) if (k >> i & 1) u = up[i][u];
        return u;
    }
    int lca(int u, int v) {
        if (dep[u] < dep[v]) swap(u, v);
        u = kth(u, dep[u] - dep[v]);
        if (u == v) return u;
        for (int k = LOG - 1; k >= 0; k--)
            if (up[k][u] != up[k][v]) u = up[k][u], v = up[k][v];
        return up[0][u];
    }
    int dist(int u, int v) { return dep[u] + dep[v] - 2 * dep[lca(u, v)]; }
};

// 【速查】虚树（只保留关键点及其两两 LCA 的压缩树）
// 用途: 多次询问、每次给 k 个关键点（Σk 不大）做树形 DP（P2495 消耗战、P3320 等）
// 用法: auto [nodes, par] = buildVirtualTree(t, keys);   // t 为建好的 LCA
//       nodes: 虚树所有点（按 DFS 序，nodes[0] 是虚树根）; par[i]: nodes[i] 在虚树上的父亲（par[0] = 0）
//       虚树边 (par[i], nodes[i]) 对应原树上一条祖先链，边权按题意用原树信息算（如 dist、链上最小边）
// 复杂度: O(k log k + k log n)
pair<vector<int>, vector<int>> buildVirtualTree(LCA& t, vector<int> keys) {
    auto byTin = [&](int a, int b) { return t.tin[a] < t.tin[b]; };
    sort(keys.begin(), keys.end(), byTin);
    keys.erase(unique(keys.begin(), keys.end()), keys.end());
    vector<int> nodes = keys;
    for (int i = 0; i + 1 < (int)keys.size(); i++) nodes.push_back(t.lca(keys[i], keys[i + 1]));
    sort(nodes.begin(), nodes.end(), byTin);
    nodes.erase(unique(nodes.begin(), nodes.end()), nodes.end());
    vector<int> par(nodes.size(), 0);
    for (int i = 1; i < (int)nodes.size(); i++) par[i] = t.lca(nodes[i - 1], nodes[i]);   // DFS 序相邻两点的 LCA 就是父亲
    return {nodes, par};
}
```

### 2-SAT

> 【10.22 新增】迭代 Tarjan，给出一组可行解。

```cpp
// 【速查】2-SAT（P4782）
// 用途: n 个布尔变量，若干条"a 或 b"形式的约束，判断是否有解并给出一组解
// 用法: TwoSat s(n);                       // 变量 0..n-1
//       s.addClause(x, a, y, b)            // 约束: (x == a) 或 (y == b)，a、b ∈ {0, 1}
//       常用翻译:  x 必须为 a      -> addClause(x, a, x, a)
//                 x、y 不能同时为 1 -> addClause(x, 0, y, 0)
//                 x 为 1 则 y 为 1  -> addClause(x, 0, y, 1)
//       bool ok = s.solve();  ok 时 s.ans[x] ∈ {0, 1} 是一组解
// 复杂度: O(n + m)；Tarjan 用迭代写法，2e6 个点也不会爆栈
struct TwoSat {
    int n;
    vector<vector<int>> g;
    vector<int> ans;

    TwoSat(int n) : n(n), g(2 * n) {}
    int id(int x, int v) { return 2 * x + v; }          // 点 2x+v 表示 "x == v"
    void addClause(int x, int a, int y, int b) {
        g[id(x, !a)].push_back(id(y, b));             // x != a  =>  y == b
        g[id(y, !b)].push_back(id(x, a));             // y != b  =>  x == a
    }
    bool solve() {
        int N = 2 * n, timer = 0, cc = 0;
        vector<int> dfn(N, -1), low(N), comp(N, -1), it(N, 0), st, call;
        for (int s = 0; s < N; s++) {
            if (dfn[s] != -1) continue;
            dfn[s] = low[s] = timer++; st.push_back(s); call.push_back(s);
            while (!call.empty()) {
                int u = call.back();
                if (it[u] < (int)g[u].size()) {
                    int v = g[u][it[u]++];
                    if (dfn[v] == -1) { dfn[v] = low[v] = timer++; st.push_back(v); call.push_back(v); }
                    else if (comp[v] == -1) low[u] = min(low[u], dfn[v]);
                } else {
                    call.pop_back();
                    if (!call.empty()) low[call.back()] = min(low[call.back()], low[u]);
                    if (low[u] == dfn[u]) {
                        while (true) { int w = st.back(); st.pop_back(); comp[w] = cc; if (w == u) break; }
                        cc++;
                    }
                }
            }
        }
        ans.assign(n, 0);
        for (int x = 0; x < n; x++) {
            if (comp[2 * x] == comp[2 * x + 1]) return false;
            ans[x] = comp[2 * x + 1] < comp[2 * x];     // Tarjan 编号小 = 拓扑序靠后，选它
        }
        return true;
    }
};
```

### 点分治

> 【10.22 新增】树上距离为 k 的点对是否存在（P3806），非递归。

```cpp
// 【速查】点分治（P3806：询问树上是否存在距离恰好为 k 的点对）
// 用途: 统计树上路径（距离 = k / ≤ k 的点对数、带限制的路径计数等），每层以重心为根统计"经过重心的路径"
// 用法: CentroidDecomp cd(n);  cd.addEdge(u, v, w);   // 点 1-based，边权 >= 0
//       vector<char> res = cd.solve(queries);          // res[i] = 是否存在距离为 queries[i] 的点对
// 改题: 换掉 calc() 里"用当前子树距离去配之前子树距离"的部分即可（如计数距离 ≤ k 的点对）
// 复杂度: O(n log n · q)；求重心、算距离都用 BFS，不会爆栈
struct CentroidDecomp {
    int n;
    vector<vector<pair<int, int>>> g;
    vector<char> removed;
    vector<int> sz, par;

    CentroidDecomp(int n) : n(n), g(n + 1), removed(n + 1, 0), sz(n + 1), par(n + 1) {}
    void addEdge(int u, int v, int w) { g[u].push_back({v, w}); g[v].push_back({u, w}); }

    // BFS 收集以 s 为起点（父亲为 p）的连通块，顺序为 BFS 序
    vector<int> collect(int s, int p) {
        vector<int> order = {s};
        par[s] = p;
        for (size_t i = 0; i < order.size(); i++) {
            int u = order[i];
            for (auto [v, w] : g[u]) if (v != par[u] && !removed[v]) { par[v] = u; order.push_back(v); }
        }
        return order;
    }
    int centroid(int s) {
        vector<int> order = collect(s, 0);
        int tot = order.size();
        for (int i = tot - 1; i >= 0; i--) {
            int u = order[i]; sz[u] = 1;
            for (auto [v, w] : g[u]) if (v != par[u] && !removed[v]) sz[u] += sz[v];
        }
        for (int u : order) {
            int mx = tot - sz[u];
            for (auto [v, w] : g[u]) if (v != par[u] && !removed[v]) mx = max(mx, sz[v]);
            if (mx * 2 <= tot) return u;
        }
        return s;
    }

    vector<int> qs;
    vector<char> res;
    int maxK = 0;
    vector<char> exist;                    // exist[d] = 之前的子树里有到重心距离为 d 的点

    void calc(int c) {
        vector<int> used = {0};
        exist[0] = 1;
        for (auto [v0, w0] : g[c]) {
            if (removed[v0]) continue;
            // 当前子树里所有点到重心的距离
            vector<int> ds;
            vector<pair<int, long long>> st = {{v0, w0}};
            vector<int> from = {c};
            for (size_t i = 0; i < st.size(); i++) {
                auto [u, d] = st[i];
                if (d <= maxK) ds.push_back((int)d);
                for (auto [v, w] : g[u]) if (v != from[i] && !removed[v]) { st.push_back({v, d + w}); from.push_back(u); }
            }
            for (int d : ds)
                for (size_t i = 0; i < qs.size(); i++)
                    if (!res[i] && qs[i] >= d && exist[qs[i] - d]) res[i] = 1;
            for (int d : ds) if (!exist[d]) { exist[d] = 1; used.push_back(d); }
        }
        for (int d : used) exist[d] = 0;
    }
    vector<char> solve(const vector<int>& queries) {
        qs = queries;
        res.assign(qs.size(), 0);
        maxK = 0;
        for (int k : qs) maxK = max(maxK, k);
        exist.assign(maxK + 1, 0);
        vector<int> todo = {1};
        while (!todo.empty()) {               // 用栈代替递归分治
            int s = todo.back(); todo.pop_back();
            int c = centroid(s);
            calc(c);
            removed[c] = 1;
            for (auto [v, w] : g[c]) if (!removed[v]) todo.push_back(v);
        }
        return res;
    }
};
```

### 树上启发式合并（dsu on tree）

> 【10.22 新增】离线统计每个子树的信息（CF600E 子树众数之和）。

```cpp
// 【速查】树上启发式合并 dsu on tree（CF600E：每个子树里出现次数最多的颜色之和）
// 用途: 离线回答"每个点的子树里 xxx"（众数、不同颜色数、某深度的点数……），信息只能加不能快速合并时
// 用法: DsuOnTree t(n);  t.addEdge(u, v);  t.col[i] = 颜色（非负整数，大就先离散化）;  auto ans = t.solve(1);   // ans[u]
// 改题: 改 addPoint / clearAll 维护的信息，以及 solve 里记录答案的那一行
// 原理: 先算轻儿子并清空，再算重儿子并保留，最后把轻儿子子树逐点加回来；每个点被加 O(log n) 次
// 复杂度: O(n log n)；预处理是迭代的，主过程递归深度 = 树高（链状 1e6 可能爆栈）
struct DsuOnTree {
    int n;
    vector<vector<int>> g;
    vector<int> col, sz, heavy, tin, tout, ver, fa;
    vector<long long> ans;

    DsuOnTree(int n) : n(n), g(n + 1), col(n + 1), sz(n + 1, 1), heavy(n + 1, 0),
                       tin(n + 1), tout(n + 1), ver(n + 1), fa(n + 1, 0), ans(n + 1) {}
    void addEdge(int u, int v) { g[u].push_back(v); g[v].push_back(u); }

    // ---- 需要按题意修改的部分 ----
    vector<int> cnt;
    int mx = 0;
    long long sum = 0;
    void addPoint(int u) {
        int c = col[u];
        cnt[c]++;
        if (cnt[c] > mx) mx = cnt[c], sum = c;
        else if (cnt[c] == mx) sum += c;
    }
    void clearAll(int u) {                     // 清空 u 子树的贡献
        for (int i = tin[u]; i <= tout[u]; i++) cnt[col[ver[i]]]--;
        mx = 0; sum = 0;
    }
    // ------------------------------

    void dfs(int u, bool keep) {
        for (int v : g[u]) if (v != fa[u] && v != heavy[u]) dfs(v, false);
        if (heavy[u]) dfs(heavy[u], true);
        for (int v : g[u]) if (v != fa[u] && v != heavy[u])
            for (int i = tin[v]; i <= tout[v]; i++) addPoint(ver[i]);
        addPoint(u);
        ans[u] = sum;                          // 记录答案
        if (!keep) clearAll(u);
    }
    vector<long long> solve(int root) {
        // 迭代求 fa、DFS 序、子树大小、重儿子
        vector<int> order, st = {root};
        fa[root] = 0;
        vector<int> it(n + 1, 0);
        int timer = 0;
        tin[root] = ++timer; ver[timer] = root;
        while (!st.empty()) {
            int u = st.back();
            if (it[u] < (int)g[u].size()) {
                int v = g[u][it[u]++];
                if (v == fa[u]) continue;
                fa[v] = u; tin[v] = ++timer; ver[timer] = v;
                st.push_back(v);
            } else {
                st.pop_back();
                tout[u] = timer;
                if (fa[u]) { sz[fa[u]] += sz[u]; if (!heavy[fa[u]] || sz[u] > sz[heavy[fa[u]]]) heavy[fa[u]] = u; }
            }
        }
        cnt.assign(max(n, *max_element(col.begin(), col.end())) + 1, 0);   // 颜色可以超过 n（但需 >= 0）
        mx = 0; sum = 0;
        dfs(root, true);
        return ans;
    }
};
```

### 欧拉路径 / 欧拉回路

> 【10.22 新增】有向（字典序最小）与无向版本，Hierholzer 迭代实现。

```cpp
// 【速查】欧拉路径 / 欧拉回路（Hierholzer，迭代版）
// 用途: 一笔画：每条边恰好走一次（P7771 有向字典序最小、P2731 无向）
// 用法: EulerDirected e(n);  e.addEdge(u, v);  auto path = e.solve();
//       EulerUndirected e(n); e.addEdge(u, v);  auto path = e.solve();
//       返回经过的点序列（长度 m+1）；不存在欧拉路径返回空 vector；点 1-based
// 字典序最小: 有向版已按出边终点排序（从最小可行起点出发，每次走最小的边）
// 判定: 有向——所有点 出度-入度 ∈ {0}，或恰好一个 +1（起点）一个 -1（终点）；无向——奇度点 0 或 2 个；且边连通
// 复杂度: O(m log m)（排序），不排序 O(n + m)
struct EulerDirected {
    int n, m = 0;
    vector<vector<int>> g;
    EulerDirected(int n) : n(n), g(n + 1) {}
    void addEdge(int u, int v) { g[u].push_back(v); m++; }
    vector<int> solve() {
        vector<int> in(n + 1, 0), out(n + 1, 0);
        for (int u = 1; u <= n; u++) { sort(g[u].begin(), g[u].end()); out[u] = g[u].size(); for (int v : g[u]) in[v]++; }
        int start = 0, plus = 0, minus = 0;
        for (int u = 1; u <= n; u++) {
            int d = out[u] - in[u];
            if (d == 1) plus++, start = u;
            else if (d == -1) minus++;
            else if (d != 0) return {};
        }
        if (!(plus == 0 && minus == 0) && !(plus == 1 && minus == 1)) return {};
        if (!start) for (int u = 1; u <= n; u++) if (out[u]) { start = u; break; }
        if (!start) return {1};                   // 没有边
        vector<int> ptr(n + 1, 0), st = {start}, path;
        while (!st.empty()) {
            int u = st.back();
            if (ptr[u] < (int)g[u].size()) st.push_back(g[u][ptr[u]++]);
            else { path.push_back(u); st.pop_back(); }
        }
        if ((int)path.size() != m + 1) return {};   // 边不连通
        reverse(path.begin(), path.end());
        return path;
    }
};

struct EulerUndirected {
    int n, m = 0;
    vector<vector<pair<int, int>>> g;              // (邻点, 边编号)
    EulerUndirected(int n) : n(n), g(n + 1) {}
    void addEdge(int u, int v) { g[u].push_back({v, m}); g[v].push_back({u, m}); m++; }
    vector<int> solve() {
        int start = 0, odd = 0;
        for (int u = 1; u <= n; u++) if (g[u].size() % 2) { odd++; if (!start) start = u; }
        if (odd != 0 && odd != 2) return {};
        if (!start) for (int u = 1; u <= n; u++) if (!g[u].empty()) { start = u; break; }
        if (!start) return {1};
        for (int u = 1; u <= n; u++) sort(g[u].begin(), g[u].end());
        vector<char> used(m, 0);
        vector<int> ptr(n + 1, 0), st = {start}, path;
        while (!st.empty()) {
            int u = st.back();
            while (ptr[u] < (int)g[u].size() && used[g[u][ptr[u]].second]) ptr[u]++;
            if (ptr[u] < (int)g[u].size()) {
                auto [v, id] = g[u][ptr[u]++];
                used[id] = 1;
                st.push_back(v);
            } else { path.push_back(u); st.pop_back(); }
        }
        if ((int)path.size() != m + 1) return {};
        reverse(path.begin(), path.end());
        return path;
    }
};
```

### 差分约束

> 【10.22 新增】x_a - x_b <= c 的可行解 / 判无解。

```cpp
// 【速查】差分约束（P5960）
// 用途: 一组不等式 x_a - x_b <= c，求一组整数解或判无解
// 用法: DiffConstraints d(n);  d.add(a, b, c);   // 约束 x_a - x_b <= c，变量 1..n
//       常用翻译:  x_a - x_b >= c -> add(b, a, -c);   x_a == x_b + c -> add(a, b, c) 且 add(b, a, -c)
//       auto x = d.solve();   // 无解（有负环）返回空 vector；否则 x[1..n] 是一组解（都 <= 0）
// 求"最大解/最小解": 以某点为源求最短路得到各变量相对它的最大值；求最小值就把不等式反过来求最长路
// 复杂度: SPFA 最坏 O(nm)
struct DiffConstraints {
    int n;
    vector<vector<pair<int, long long>>> g;
    DiffConstraints(int n) : n(n), g(n + 1) {}
    void add(int a, int b, long long c) { g[b].push_back({a, c}); }   // x_a <= x_b + c：边 b -> a，权 c
    vector<long long> solve() {
        vector<long long> dist(n + 1, 0);       // 相当于超级源点 0 向每个点连 0 边
        vector<int> cnt(n + 1, 0);
        vector<char> inq(n + 1, 1);
        deque<int> q;
        for (int i = 1; i <= n; i++) q.push_back(i);
        while (!q.empty()) {
            int u = q.front(); q.pop_front(); inq[u] = 0;
            for (auto [v, w] : g[u]) {
                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    if (!inq[v]) {
                        if (++cnt[v] > n) return {};   // 入队超过 n 次 => 负环 => 无解
                        inq[v] = 1; q.push_back(v);
                    }
                }
            }
        }
        return dist;
    }
};
```

### Kruskal 重构树

> 【10.22 新增】瓶颈路（最小化最大边 / 最大化最小边）、只走 <= x 的边能到的点集。

```cpp
// 【速查】Kruskal 重构树（P1967 货车运输、P4197）
// 用途: "u 到 v 所有路径中，最大边权的最小值"（瓶颈路）= 重构树上 val[lca(u, v)]；
//       "从 u 出发只走边权 <= x 的边能到哪些点" = u 往上跳到 val <= x 的最高祖先，它的子树叶子就是答案
// 用法: KruskalTree t(n);  t.addEdge(u, v, w);  t.build();       // 点 1-based，按边权升序合并（最小瓶颈）
//       t.bottleneck(u, v) -> 最小化的最大边权，不连通返回 -1;
//       t.highest(u, x) -> u 只走 <= x 的边能到的连通块对应的重构树节点（子树叶子 = 能到的点）
//       求"最大化最小边"（货车运输）: build(true) 改为按边权降序合并，bottleneck 返回最小边的最大值
// 复杂度: O(m log m + n log n)；重构树共 2n-1 个点，叶子 1..n，内部点 val = 合并它的那条边
struct KruskalTree {
    int n, tot;
    vector<array<long long, 3>> edges;
    vector<int> fa, dep;
    vector<long long> val;
    vector<vector<int>> up, ch;
    int LOG;

    KruskalTree(int n) : n(n) {}
    void addEdge(int u, int v, long long w) { edges.push_back({w, u, v}); }
    int find(int x) { while (fa[x] != x) x = fa[x] = fa[fa[x]]; return x; }

    void build(bool maxBottleneck = false) {
        sort(edges.begin(), edges.end());
        if (maxBottleneck) reverse(edges.begin(), edges.end());
        int N = 2 * n;
        fa.resize(N); iota(fa.begin(), fa.end(), 0);
        val.assign(N, 0); ch.assign(N, {});
        tot = n;
        for (auto& [w, u, v] : edges) {
            int a = find(u), b = find(v);
            if (a == b) continue;
            ++tot; val[tot] = w;
            fa[a] = fa[b] = tot;
            ch[tot] = {a, b};
        }
        LOG = 1; while ((1 << LOG) <= tot) LOG++;
        up.assign(LOG, vector<int>(tot + 1, 0));
        dep.assign(tot + 1, 0);
        for (int r = tot; r >= 1; r--) {       // 父亲编号一定比儿子大，倒序就是"先父后子"
            for (int c : ch[r]) { up[0][c] = r; dep[c] = dep[r] + 1; }
        }
        for (int k = 1; k < LOG; k++)
            for (int v = 1; v <= tot; v++) up[k][v] = up[k - 1][up[k - 1][v]];
    }
    int lca(int u, int v) {
        if (dep[u] < dep[v]) swap(u, v);
        for (int k = LOG - 1; k >= 0; k--) if (dep[u] - (1 << k) >= dep[v]) u = up[k][u];
        if (u == v) return u;
        for (int k = LOG - 1; k >= 0; k--) if (up[k][u] != up[k][v]) u = up[k][u], v = up[k][v];
        return up[0][u];
    }
    long long bottleneck(int u, int v) {
        if (u == v) return 0;
        if (find(u) != find(v)) return -1;
        return val[lca(u, v)];
    }
    int highest(int u, long long x) {           // 仅最小瓶颈（升序建树）时语义成立
        for (int k = LOG - 1; k >= 0; k--) {
            int p = up[k][u];
            if (p && val[p] <= x) u = p;
        }
        return u;
    }
};
```

### 最小树形图（朱刘）与一般图最大匹配（带花树）

> 【10.22 新增】有向图最小生成树形图；非二分图最大匹配。

```cpp
// 【速查】最小树形图（朱刘算法，P4716）
// 用途: 有向图里以 root 为根、能到达所有点的边权和最小的"有向生成树"
// 用法: long long w = zhuLiu(n, root, edges);   // 点 0..n-1，edges 为 {u, v, w}（u -> v）
//       返回最小权和；有点到不了返回 -1
// 复杂度: O(n·m)
long long zhuLiu(int n, int root, vector<array<long long, 3>> edges) {
    const long long INF = LLONG_MAX / 4;
    long long res = 0;
    vector<long long> in(n);
    vector<int> pre(n), id(n), vis(n);
    while (true) {
        fill(in.begin(), in.end(), INF);
        for (auto& [u, v, w] : edges)               // 每个点选一条最小入边
            if (u != v && w < in[v]) in[v] = w, pre[v] = (int)u;
        for (int i = 0; i < n; i++) if (i != root && in[i] == INF) return -1;
        int cnt = 0;
        fill(id.begin(), id.end(), -1); fill(vis.begin(), vis.end(), -1);
        in[root] = 0;
        for (int i = 0; i < n; i++) {               // 找环
            res += in[i];
            int v = i;
            while (vis[v] != i && id[v] == -1 && v != root) vis[v] = i, v = pre[v];
            if (v != root && id[v] == -1) {
                for (int u = pre[v]; u != v; u = pre[u]) id[u] = cnt;
                id[v] = cnt++;
            }
        }
        if (cnt == 0) break;                        // 无环，完成
        for (int i = 0; i < n; i++) if (id[i] == -1) id[i] = cnt++;
        for (auto& [u, v, w] : edges) {             // 缩环，入环边权减去环内入边
            long long vv = v;
            u = id[u]; v = id[v];
            if (u != v) w -= in[vv];
        }
        n = cnt; root = id[root];
    }
    return res;
}

// 【速查】一般图最大匹配（带花树，UOJ79）
// 用途: 不是二分图的最大匹配（有奇环）
// 用法: Blossom b(n);  b.addEdge(u, v);  int cnt = b.solve();   // 点 1-based
//       b.match[u] = u 的匹配点（0 表示未匹配）
// 复杂度: O(n³)，n ≤ 1000 没问题
struct Blossom {
    int n;
    vector<vector<int>> g;
    vector<int> match, pre, fa, typ, vis;
    int stamp = 0;

    Blossom(int n) : n(n), g(n + 1), match(n + 1), pre(n + 1), fa(n + 1), typ(n + 1), vis(n + 1) {}
    void addEdge(int u, int v) { g[u].push_back(v); g[v].push_back(u); }
    int find(int x) { while (fa[x] != x) x = fa[x] = fa[fa[x]]; return x; }
    int lca(int x, int y) {
        ++stamp;
        while (true) {
            if (x) {
                x = find(x);
                if (vis[x] == stamp) return x;
                vis[x] = stamp;
                x = match[x] ? pre[match[x]] : 0;
            }
            swap(x, y);
        }
    }
    void blossom(int x, int y, int l, queue<int>& q) {   // 把奇环缩成花
        while (find(x) != l) {
            pre[x] = y; y = match[x];
            if (typ[y] == 2) typ[y] = 1, q.push(y);
            if (find(x) == x) fa[x] = l;
            if (find(y) == y) fa[y] = l;
            x = pre[y];
        }
    }
    bool bfs(int s) {
        for (int i = 1; i <= n; i++) fa[i] = i, typ[i] = 0, pre[i] = 0;
        queue<int> q; q.push(s); typ[s] = 1;
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (int v : g[u]) {
                if (find(u) == find(v) || typ[v] == 2) continue;
                if (!typ[v]) {
                    typ[v] = 2; pre[v] = u;
                    if (!match[v]) {                        // 找到增广路，沿 pre 翻转
                        for (int x = v, last; x; x = last) last = match[pre[x]], match[x] = pre[x], match[pre[x]] = x;
                        return true;
                    }
                    typ[match[v]] = 1; q.push(match[v]);
                } else {                                     // 两个偶点相遇：奇环
                    int l = lca(u, v);
                    blossom(u, v, l, q); blossom(v, u, l, q);
                }
            }
        }
        return false;
    }
    int solve() {
        int res = 0;
        for (int i = 1; i <= n; i++) if (!match[i] && bfs(i)) res++;
        return res;
    }
};
```
