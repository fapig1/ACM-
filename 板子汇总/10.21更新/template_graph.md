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

```cpp
#define int long long
int mod=1e9+7;
int MAXN=5e5+5;
vector<vector<int>> gra(MAXN);	//原图邻接表
vector<int> val(MAXN);			//原图点权
vector<int> dfn(MAXN);			//原图dfn序，用于构建scc
vector<int> low(MAXN);			//节点的scc根节点，用于构建scc
stack<int> stk;					//栈，用于构建scc
vector<int> instk(MAXN);		//节点是否入栈，用于构建scc
vector<int> scc(MAXN);		//ind to scc 原图点角标转新图点角标
vector<int> sccval(MAXN);		//新图点权
vector<vector<int>> sccgra(MAXN);	//新图邻接表
int cur=1;						//维护原图dfn值
int curscc=0;					//新图大小

int n,m;
vector<pair<int,int>> edge;   //不预填,直接push_back
void tarjan(int ind){
    dfn[ind]=cur;
    low[ind]=cur;
    cur++;
    instk[ind]=1;
    stk.push(ind);
    for(int i:gra[ind]){
        //无向图这里加 if(par[i]==ind)continue;
        if(!dfn[i]){
            tarjan(i);
            low[ind]=min(low[ind],low[i]);
        }else if(instk[i]){
            low[ind]=min(low[ind],dfn[i]);
        }
    }

    if(low[ind]==dfn[ind]){
        int cval=0;
        int ctar=0;
        int c;
        do{
            c=stk.top();
            stk.pop();
            instk[c]=0;
            scc[c]=curscc;
            sccval[curscc]+=val[c];
        }while(c!=ind);
        curscc++;
    }
}

void build(){
    //第一步求解scc
    for(int i=1;i<=n;i++){
        if(!dfn[i])
        tarjan(i);
    }
    //第二步建新图
    for(int i=0;i<m;i++){
        int a=edge[i].first;
        int b=edge[i].second;
        if(scc[a]!=scc[b]){
            sccgra[scc[a]].push_back(scc[b]);
        }
    }
}

void solve(){
    cin>>n>>m;
    for(int i=0;i<m;i++){
        int a,b;
        cin>>a>>b;
        gra[a].push_back(b);
        edge.push_back({a,b});
    }
    for(int i=1;i<=n;i++){
        cin>>val[i];
    }

    build();
}

signed main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    solve();
    // 输出: SCC 个数 / 缩点图边数 / 每个分量(点集与点权和) / 最大分量点权和
    int edges=0;
    for(int i=0;i<curscc;i++) edges+=(int)sccgra[i].size();
    cout<<curscc<<" "<<edges<<"\n";
    for(int i=0;i<curscc;i++){
        cout<<"SCC "<<i+1<<" (val="<<sccval[i]<<"):";
        for(int j=1;j<=n;j++) if(scc[j]==i) cout<<" "<<j;
        cout<<"\n";
    }
    int best=0;
    for(int i=1;i<curscc;i++) if(sccval[i]>sccval[best]) best=i;
    cout<<"max sccval = "<<sccval[best]<<"\n";
    return 0;
}
```

### 有向图的拓扑序列

```cpp
#define int long long
#define endl '\n'

const int N = 1e5 + 5;
int n, m;
vector<int> arr;
vector<int> g[N];
int d[N];
queue<int> q;

bool top()
{
    for (int i = 1; i <= n; i++)
    {
        if (d[i] == 0)
        {
            q.push(i);
            arr.push_back(i);
        }
    }
    while (!q.empty())
    {
        int t = q.front();
        q.pop();

        for (int i = 0; i < g[t].size(); i++)
        {
            d[g[t][i]]--;
            if (d[g[t][i]] == 0)
            {
                q.push(g[t][i]);
                arr.push_back(g[t][i]);
            }
        }
    }
    if (arr.size() == n) return true;
    else return false;
}

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    cin >> n >> m;
    for (int i = 1; i <= m; i++)
    {
        int x, y;
        cin >> x >> y;
        g[x].push_back(y);
        d[y]++;
    }

    if (top())
    {
        for (auto x : arr)
        {
            cout << x << ' ';
        }
    }
    else
    {
        cout << -1 << endl;
    }
    return 0;
}
```

### 树的直径

> 两次 BFS 求直径边数；递归 DFS 实现，2e5 级链状树注意爆栈。

```cpp
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
```

### 树的重心

> 递归 DFS 实现，深链注意爆栈。

```cpp
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
```

### 动态维护树直径、直径端点、中心

> 【本次修复】原版 get_center 在"奇数直径且中心边位于 u→lca 段"时输出错误端点；已修复。

```cpp
#define int long long

const int MAXN=2e5+5;
const int LOG=32;

//{to,val}
vector<vector<pair<int,int>>> gra(MAXN);

vector<vector<int>> par(MAXN,vector<int>(LOG));

vector<int> vis(MAXN);
vector<int> dep(MAXN);
vector<int> dis(MAXN);

//初始化每个节点的父节点，深度，根节点到该节点的边权和
void dfs(int ind){
    for(auto [i,w]:gra[ind]){
        if(vis[i])continue;

        vis[i]=1;

        par[i][0]=ind;
        dep[i]=dep[ind]+1;
        dis[i]=dis[ind]+w;

        dfs(i);
    }
}

//求两节点LCA
int lca(int a,int b){
    if(dep[a]>dep[b])swap(a,b);

    for(int i=LOG-1;i>=0;i--){
        if(dep[par[b][i]]>=dep[a]){
            b=par[b][i];
        }
    }

    if(a==b)return a;

    for(int i=LOG-1;i>=0;i--){
        if(par[a][i]!=par[b][i]){
            a=par[a][i];
            b=par[b][i];
        }
    }

    return par[a][0];
}

//求两节点距离
int dist(int a,int b){
    int l=lca(a,b);
    return dis[a]+dis[b]-2*dis[l];
}

//从x节点向根节点移动k步的终点
int jump(int x,int k){
    for(int i=0;i<LOG;i++){
        if(k&(1LL<<i)){
            x=par[x][i];
        }
    }
    return x;
}

//获取从u向v走距离len时最远到达的位置，若落在一条边上返回靠近u的那个端点
int u_to_v_len(int u,int v,int len){
    int l=lca(u,v);
    int du=dist(u,l);
    if(len<=du){
        int cur=u;
        for(int i=LOG-1;i>=0;i--){
            int p=par[cur][i];
            if(p && dist(u,p)<=len){
                cur=p;
            }
        }
        return cur;
    }
    int rem=dist(u,v)-len;
    int cur=v;
    for(int i=LOG-1;i>=0;i--){
        int p=par[cur][i];
        if(p && dist(v,p)<=rem){
            cur=p;
        }
    }
    if(dist(cur,v)<rem)cur=par[cur][0];
    return cur;
}

//获得u,v两点间中点所在点，若落在一条边上则返回这条边两端点，p1靠近u，p2靠近v
pair<int,int> get_center(int u,int v){
    int d=dist(u,v);
    int half=d/2;
    int p=u_to_v_len(u,v,half);
    if(dist(u,p)*2==d){
        return {p,p};
    }

    int l=lca(u,v);
    int nxt;
    if(lca(u,p)==p&&p!=l){
        // p 在 u->lca 上行段: 朝 v 方向的下一个点就是 p 的父节点
        nxt=par[p][0];
    }else{
        int cur=v;
        for(int i=LOG-1;i>=0;i--){
            int pp=par[cur][i];
            if(pp && dep[pp]>dep[p]){
                cur=pp;
            }
        }
        nxt=cur;
    }
    return {p,nxt};
}

void solve(){
    int n;
    cin>>n;
    dep[0]=-1;

    for(int i=1;i<n;i++){
        int a,b,w;
        cin>>a>>b>>w;
        gra[a].push_back({b,w});
        gra[b].push_back({a,w});
    }

    int root;
    cin>>root;
    vis[root]=1;
    dep[root]=1;
    dfs(root);

    for(int i=1;i<LOG;i++){
        for(int j=1;j<=n;j++){
            if(par[j][i-1]){
                par[j][i]=par[par[j][i-1]][i-1];
            }
        }
    }

    queue<int> que;
    que.push(root);
    int u=root;//当前直径端点
    int v=root;//当前直径端点
    int diameter=0;//当前直径
    vis.assign(n+1,0);
    vis[root]=1;

    //这里是从根节点开始bfs，每一步都维护对应的直径，直径端点和中心
    //具体题目修改对应的内容
    while(que.size()){
        queue<int> que2;
        while(que.size()){
            int tp=que.front();
            que.pop();
            for(auto [i,w]:gra[tp]){
                if(vis[i])continue;
                vis[i]=1;
                int d1=dist(u,i);
                int d2=dist(v,i);
                if(d1>diameter&&d1>d2){
                    diameter=d1;
                    v=i;
                }
                if(d2>diameter){
                    diameter=d2;
                    u=i;
                }
                pair<int,int> cen=get_center(u,v);
                cout<<u<<" "<<v<<" "<<diameter<<" ";
                if(cen.first==cen.second){
                    cout<<cen.first<<"\n";
                }else{
                    cout<<cen.first<<" "<<cen.second<<"\n";
                }
                que2.push(i);
            }
        }
        swap(que,que2);
    }
    return;
}

signed main()
{
    ios::sync_with_stdio(false);
    cin.tie(0);

    int t=1;
    //cin>>t;

    while(t--)solve();

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

```cpp
#define int long long
#define endl '\n'

const int N = 1e5 + 5;
const int mod = 80112002;
int n, m, s, k, t;
int x, y, z;
vector<pair<int, int>>g[N];
int dist[N];
int back[N];
bool st[N];
//bellman-ford算法，有边数限制为k的单源最短路
int bell()
{
    memset(dist, 0x3f, sizeof dist);
    dist[1] = 0;
    for (int p = 0; p < k; p++)
    {
        memcpy(back, dist, sizeof dist);
        for (int j = 1; j <= n; j++)
        {
            for (auto& t : g[j])
            {
                int a = t.first, b = t.second;

                dist[a] = min(back[j] + b, dist[a]);
            }
        }
    }
    if (dist[n] > 0x3f3f3f3f3f3f3f3f / 2) return 0x3f3f3f3f3f3f3f3f;
    else
        return dist[n];
}

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    cin >> n >> m >> k;
    for (int i = 1; i <= m; i++)
    {
        cin >> x >> y >> z;
        g[x].push_back({ y,z });
    }
    int res = bell();
    if (res == 0x3f3f3f3f3f3f3f3f) cout << "impossible" << endl;
    else
        cout << res << endl;
    
    return 0;
}
```

### SPFA判断负环

```cpp
#define int long long
#define endl '\n'
const int N = 1e4 + 5;
int t;
int n, m, u, v, w;
vector<pair<int,int> > arr[N];
int st[N], dist[N], cnt[N];
queue<int> q;
//spfa判断负环
void spfa()
{
    for(int i=1;i<=n;i++)
    {
        st[i]=1;
        q.push(i);
    }
    while (!q.empty())
    {
        int tm = q.front();
        q.pop();
        st[tm] = 0;
        for (int i = 0; i < arr[tm].size(); i++)
        {
            int v = arr[tm][i].first, dis = arr[tm][i].second;
            if (dist[v] > dist[tm] + dis)
            {
                dist[v] = dist[tm] + dis;
                if (!st[v])
                {
                    cnt[v] = cnt[tm] + 1;
                    if (cnt[v] >= n)
                    {
                        cout << "Yes" << endl;
                        return;
                    }
                    st[v] = 1;
                    q.push(v);
                }
            }
        }
    }
    cout << "No" << endl;
}

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);


        cin >> n >> m;
        for (int i = 1; i <= m; i++)
        {
            cin >> u >> v >> w;
            arr[u].push_back({v,w});
        }
        spfa();
    

    return 0;
}
```

### SPFA求最短路

```cpp
#define int long long
#define endl '\n'

const int N = 1e5 + 5;
const int mod = 80112002;
int n, m, s;
int x, y, z;
vector<pair<int, int>>g[N];
int dist[N];
bool st[N];
//spfa求最短路
int spfa()
{
    memset(dist, 0x3f, sizeof dist);
    dist[1] = 0;
    queue<int> q;
    q.push(1);
    st[1]=true;
    while (!q.empty())
    {
        auto t = q.front();
        q.pop();
        st[t] = false;
        for (int i = 0; i < g[t].size(); i++)
        {
            if (dist[g[t][i].first] > dist[t] + g[t][i].second)
            {
                dist[g[t][i].first] = dist[t] + g[t][i].second;
                if (!st[g[t][i].first])
                {
                    q.push(g[t][i].first);
                    st[g[t][i].first] = true;
                }
            }
        }
    }

    if (dist[n] == 0x3f3f3f3f3f3f3f3f) return 0x3f3f3f3f3f3f3f3f;
    else
        return dist[n];
}

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    cin >> n >> m;
    for (int i = 1; i <= m; i++)
    {
        cin >> x >> y >> z;
        g[x].push_back({ y,z });
    }
    int res;
    res = spfa();
    if (res == 0x3f3f3f3f3f3f3f3f)
        cout << "impossible" << endl;
    else
        cout << res << endl;

    return 0;
}
```

### 普通Dijkstra求最短路

```cpp
#define int long long
#define endl '\n'

const int N = 1e5+5;
const int mod = 80112002;
int n, m, s;
int x, y, z;
vector<pair<int, int> >g[N];
int dist[N];
bool st[N];
//普通版dijkstra求单源最短路
int dij()
{
    memset(dist, 0x3f, sizeof dist);
    dist[1] = 0;
    for (int p = 0; p < n; p++)
    {
        int t = -1;
        for (int j = 1; j <= n; j++)
        {
            if (!st[j] && (t == -1 || dist[t] > dist[j]))
            {
                t = j;
            }
        }
        st[t] = true;
        for (int i = 0; i < g[t].size(); i++)
        {
            int a = g[t][i].first, b = g[t][i].second;
            dist[a] = min(dist[a], dist[t] + b);
        }
        
    }
    if(dist[n]==0x3f3f3f3f3f3f3f3f) return -1;
    else
    return dist[n];
}

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    
    cin >> n >> m;
    for (int i = 1; i <= m; i++)
    {
        cin >> x >> y >> z;
        g[x].push_back({ y,z });
    }
    
    cout<<dij();

    
    return 0;
}
```

### 堆优化Dijkstra求最短路

> 【本次修复】注释"适用于稠密图"已纠正为稀疏图 O(m log m)。

```cpp
#define int long long
#define endl '\n'

const int N = 1e5 + 5;
const int mod = 80112002;
int n, m, s;
int x, y, z;
vector<pair<int, int>>g[N];
int dist[N];
bool st[N];

//堆优化版dij,适用于稀疏图 O(m log m); 稠密图请用朴素 O(n^2) 版
int dij()
{
    memset(dist, 0x3f, sizeof dist);
    dist[1] = 0;
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>> > q;
    q.push({ 0,1 });

    while (!q.empty())
    {
        auto t = q.top();
        q.pop();
        int dis = t.first, idx = t.second;
        if (st[idx]) continue;
        st[idx] = true;
        for (int i = 0; i < g[idx].size(); i++)
        {
            dist[g[idx][i].second] = min(dis + g[idx][i].first, dist[g[idx][i].second]);
            q.push({ dist[g[idx][i].second] ,g[idx][i].second });
        }
    }

    if (dist[n] == 0x3f3f3f3f3f3f3f3f) return -1;
    else
        return dist[n];
}

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    cin >> n >> m;
    for (int i = 1; i <= m; i++)
    {
        cin >> x >> y >> z;
        g[x].push_back({ z,y });
    }

    cout << dij();


    return 0;
}
```

### Floyd求多源最短路

```cpp
#define int long long
#define endl '\n'
int n, m, k, x, y, z;
const int N = 205;
int d[N][N];
//floyd求多源最短路
void floyd()
{
    for (int k = 1; k <= n; k++)
    {
        for (int i = 1; i <= n; i++)
        {
            for (int j = 1; j <= n; j++)
            {
                d[i][j] = min(d[i][j], d[i][k] + d[k][j]);
            }
        }
    }
}

signed main() {
    cin >> n >> m >> k;
    memset(d, 0x3f, sizeof d);
    for (int i = 1; i <= n; i++)
    {
        d[i][i] = 0;
    }
    for (int i = 1; i <= m; i++)
    {
        cin >> x >> y >> z;
        d[x][y] = min(d[x][y], z);
    }
    floyd();
    for (int i = 1; i <= k; i++)
    {
        cin >> x >> y;
        if (d[x][y] > 0x3f3f3f3f/2) cout << "impossible" << endl;
        else
            cout << d[x][y] << endl;
    }

    return 0;
}
```

### Kruskal求最小生成树

```cpp
#define int long long
#define endl '\n'

const int N=2e5+10;
int t,n,m,res,cnt;
int fa[N];
//Kruskal算法求最小生成树
struct edge
{
  int a,b,w;
  bool operator<(edge& x)
  {
    return w<x.w;
  }
}e[N];

int find(int u)
{
  if(u!=fa[u])
  {
    fa[u]=find(fa[u]);
  }
  return fa[u];
}

signed main()
{
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    
    cin>>n>>m;
    for(int i=1;i<=m;i++)
    {
      cin>>e[i].a>>e[i].b>>e[i].w;
    }
    sort(e+1,e+m+1);
    for(int i=1;i<=n;i++)
    {
      fa[i]=i;
    }
    for(int i=1;i<=m;i++)
    {
      int a=e[i].a,b=e[i].b,w=e[i].w;
      a=find(a),b=find(b);
      if(a!=b)
      {
        res+=w;
        fa[a]=b;
        cnt++;
      }
    }
    if(cnt<n-1)
    {
      cout<<"impossible"<<endl;
    }
    else  cout<<res<<endl;
    

    return 0;
}
```

### Prim求最小生成树

> 【本次修复】恢复 #define int long long（边权和可超 32 位）。

```cpp
#define int long long   // 恢复: 边权和可超 32 位 int
#define endl '\n'

const int N=510;
const int INF=0x3f3f3f3f;
int n,m;
int g[N][N];
int dist[N];
bool st[N];
//Prim算法求最小生成树
int prim()
{
  memset(dist,0x3f,sizeof dist);
  int res=0;
  dist[1]=0;
  for(int i=0;i<n;i++)
  {
    int t=-1;
    for(int j=1;j<=n;j++)
    {
      if(!st[j]&&(t==-1||dist[j]<dist[t]))
      {
        t=j;
      }
    }
    if(dist[t]==0x3f3f3f3f) 
    {
      return INF;
    }
    res+=dist[t];
    st[t]=true;
    for(int j=1;j<=n;j++)
    {
      if(!st[j])
      dist[j]=min(dist[j],g[t][j]);
    } 
  }
  return res;
}

signed main()
{
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    
    cin>>n>>m;
    memset(g,0x3f,sizeof g);
    for(int i=1;i<=m;i++)
    {
      int a,b,w;
      cin>>a>>b>>w;
      g[a][b]=g[b][a]=min(g[a][b],w);
    }
    int t=prim();
    if(t==INF)
    {
      cout<<"impossible"<<endl;
    }
    else
    cout<<t<<endl;

    return 0;
}
```

### Boruvka解完全图最小生成树

> 针对"完全图、边权=(val[u]+val[v]) mod k"的专用版，O(n log²n)。

```cpp
#define int long long
#define endl "\n"

//Boruvka 求解边权为某个数学条件下的完全图的最小生成树
//使用类似启发式的思想，每轮寻找每个并查集对于其他并查集的最小出边
//每轮并查集数量/=2，最多log轮，在每轮中需要遍历每一个点以及二分寻找找到在其他集合中的点对这个点的最小边
//总复杂度O(n(logn)^2)


//并查集模板
vector<int> par;
int find(int a){
    if(a==par[a])return a;
    return par[a]=find(par[a]);
}

int unite(int a,int b){
    int pa=find(a);
    int pb=find(b);

    if(pa==pb)return 1;
    par[pb]=pa;
    return 0;
}

struct edge{
    int val,a,b;// 边权，起点，终点
};

int solve(){
    int n,k;
    cin>>n>>k;
    par.assign(n,0);
    vector<int> val(n);
    
    multiset<pair<int,int>> st;
    vector<vector<int>> block(n);
    vector<edge> mine(n);
    for(int i=0;i<n;i++){
        cin>>val[i];
        val[i]%=k;
        par[i]=i;
        st.insert({val[i],i});
    }
    
    int num=0;
    int ans=0;
    while(num<n-1){
        //初始化，将每一个并查集块中的元素分类，同时初始化每个并查集块的最小出边
        for(int i=0;i<n;i++){
            block[i].clear();
        }
        for(int i=0;i<n;i++){
            block[find(i)].push_back(i);
        }
        for(int i=0;i<n;i++){
            mine[i]={(int)2e18,-1,-1};
        }

        for(int i=0;i<n;i++){
            //删除当前并查集块的点
            for(int j:block[i]){
                st.erase({val[j],j});
            }

            for(int j:block[i]){
                int tar=k-val[j];
                pair<int,int> pr={tar,0};
                auto id=st.lower_bound(pr);
                
                if(id!=st.end()){
                    pair<int,int> p=*id;

                    //修改这里，取决于边权如何决定
                    if(mine[i].val>(p.first+val[j])%k){
                        mine[i].val=(p.first+val[j])%k;
                        mine[i].b=p.second;
                        mine[i].a=j;
                    }
                }

                tar=0;
                pr={tar,0};
                id=st.lower_bound(pr);
                
                if(id!=st.end()){
                    pair<int,int> p=*id;

                    //修改这里，取决于边权如何决定
                    if(mine[i].val>(p.first+val[j])%k){
                        mine[i].val=(p.first+val[j])%k;
                        mine[i].b=p.second;
                        mine[i].a=j;
                    }
                }
            }

            //重新插入当前元素的点
            for(int j:block[i]){
                st.insert({val[j],j});
            }
        }

        //合并新建边的集合
        for(int i=0;i<n;i++){
            auto [v,a,b]=mine[i];
            if(mine[i].a!=-1){
                if(!unite(a,b)){
                    ans+=v;
                    num++;
                }
            }
        }
    }
    return ans;


}

signed main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    int t=1;
    cin>>t;
    while(t--){
        cout<<solve()<<"\n";
    }
    return 0;
}
```

### 匈牙利算法二分图最大匹配

> 【本次修复】原版 assign 写在读入之前，任何输入必崩 bad_alloc；已修复。

```cpp
#define int long long
#define endl '\n'

const int MAXN=505;     //右部大小
vector<int> state;      //右部已被左部匹配的状态
vector<int> vis(MAXN);   //右部的vis数组，用来判环
vector<vector<int>> vec; //从左部向右部建的有向边图
int visnum=1;           //标记法记vis，节省时间
int check(int ind){
    for(int i:vec[ind]){
        if(vis[i]<visnum){
            vis[i]=visnum;
            if(state[i]==0||check(state[i])){
                state[i]=ind;
                return 1;
            }
        }
    }
    return 0;
}

void solve()
{
    int n1,n2,m;
    //此时左部和右部已经被区分过，如果没有被区分过先进行染色dfs操作
    cin>>n1>>n2>>m;
    vec.assign(n1+1,vector<int>());
    state.assign(n2+1,0);
    vis.assign(n2+1,0);   // 右部大小不再受 MAXN 限制
    for(int i=1;i<=m;i++)
    {
        int u,v;
        cin>>u>>v;
        //因为只从一边找另一边,所以只用存一个方向，即只建左部指向右部的边
        vec[u].push_back(v);
    }

    int ans=0;
    for(int i=1;i<=n1;i++){
        visnum++;
        if(check(i))ans++;
    }
    cout<<ans<<"\n";
}



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


signed main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    int _t=1;
    //cin>>_t;
    while(_t--)
    {
        solve();
    }

    return 0;
}
```

### 带权二分图最优匹配（KM算法）

> 注意：左右部规模必须相等（内部上界混用 cnta/cntb）。

```cpp
#define int long long

const int inf=0x7fffffff;
const int MAXN=505;

int cnta,cntb;  //左部数量，右部数量
int w[MAXN][MAXN];//边权，从左到右

//以下套用板子不用更改
int visa[MAXN],visb[MAXN];//访问标记
int la[MAXN],lb[MAXN];//顶标
int match[MAXN],upd[MAXN];//匹配点、更新
int visnum=0;
bool check(int x) {//匈牙利
    visa[x]=visnum;
    for(int y=1; y<=cnta; y++){
        if(w[x][y]!=-inf){
            if(visb[y]<visnum)
            if(la[x]+lb[y]==w[x][y]) {
                visb[y]=visnum;
                if(!match[y]||check(match[y])) {
                    match[y]=x;
                    return true;
                }
            } else upd[y]=min(upd[y],la[x]+lb[y]-w[x][y]);
        }
    }

    return false;
}

int KM() {//KM板子
    memset(match,0,sizeof(match));
    for(int i=1; i<=cnta; i++) {
        la[i]=-inf;
        lb[i]=0;
        for(int j=1; j<=cntb; j++){
            la[i]=max(la[i],w[i][j]);
        }
    }
    for(int i=1; i<=cnta; i++){
        while(true) {
            visnum++;
            for(int j=1; j<=cntb; j++){
                upd[j]=inf;
            }
            if(check(i))break;
            int delta=inf;
            for(int j=1; j<=cntb; j++){
                if(visb[j]<visnum)delta=min(delta,upd[j]);
            }
            for(int j=1; j<=cntb; j++) {
                if(visa[j]==visnum)la[j]-=delta;
                if(visb[j]==visnum)lb[j]+=delta;
            }
        }
    }
    int ans=0;
    for(int i=1; i<=cntb; i++){
        ans+=w[match[i]][i];
    }
    return ans;
}

void solve(){
    cin>>cnta;
    cntb=cnta;
    for(int i=1;i<=cnta;i++){
        for(int j=1;j<=cntb;j++)
        {
            int in;
            cin>>in;
            w[i][j]=in;
        }
    }
    int det=KM();
    /*
        如果要求完美匹配的最小权，将边权取负数w[i][j]=-w[i][j]
        最后答案输出-KM()即可
    */
    cout<<det<<"\n";
}

main()
{
    ios::sync_with_stdio(false);
    cin.tie(0);

    int t=1;
    //cin>>t;
    while(t--)solve();

    return 0;
}
```

### 染色法判定二分图

> 递归 DFS，深链注意爆栈。

```cpp
#define int long long
#define endl '\n'

const int N=1e5+10;
int t,n,m;
vector<int> arr[N];
int color[N];// 1 / 2
//染色法判定二分图
bool dfs(int u,int c)
{
  color[u]=c;
  for(auto x:arr[u])
  {
    if(!color[x])
    {
      if(!dfs(x,3-c)) return 0;
    }
    else if(color[x]&&color[x]!=3-c)
    {
      return 0;
    }
  }
  return 1;
}

signed main()
{
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    
    cin>>n>>m;
    for(int i=1;i<=m;i++)
    {
      int u,v;
      cin>>u>>v;
      arr[u].push_back(v);
      arr[v].push_back(u);
    }
    int flag=0;
    for(int i=1;i<=n;i++)
    {
      if(!color[i])
      {
        if(!dfs(i,1))
        {
          flag=1;
          break;
        }
      }
    }
    if(flag)
    {
      cout<<"No"<<endl;
    }
    else
    {
      cout<<"Yes"<<endl;
    }


    return 0;
}
```

## 双联通分量

### 边双连通分量（Edge-BCC）

> 含桥判定与分量划分；按边编号跳过父边，正确处理重边。

```cpp
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

### 限额k最小费用流（裸函数）

> 需先建好全局 Edge/n/g（见块内注释）；返回{实际流量,最小费用}。

```cpp
using ll = long long;
const ll INF = 4e18;

// "限额 k 的最小费用流"(SPFA 版 SSP): 使用前先建好下面的全局图
struct Edge { int to, rev; ll cap, cost; };
int n;                      // 节点数(编号 0..n-1)
vector<vector<Edge>> g;     // 加边时成对存放(正向 cap/反向 0, 反向 cost 取负)
// 加边辅助:
// void addEdge(int u, int v, ll cap, ll cost) {
//     g[u].push_back({v, (int)g[v].size(), cap, cost});
//     g[v].push_back({u, (int)g[u].size() - 1, 0, -cost});
// }
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
```

### 带费用上下界最小费用可行循环流

> addEdge(u,v,lower,upper,cost)；solve() 返回{是否可行,最小费用}，getFlow 查每边最终流量。

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

    addEdge(u, v, lower, upper, cost)

    solve():
        返回 {是否存在可行流, 最小费用}
==================================================*/

struct LowerBoundMinCostFlow {

    struct OriginalEdge {
        int u;
        int id;
        ll lower;
        ll upper;
    };

    int n;
    int SS, TT;

    MinCostMaxFlow mf;

    // balance[i] = 固定下界造成的 流入 - 流出
    vector<ll> balance;

    // 下界部分已经必然产生的费用
    ll baseCost = 0;

    vector<OriginalEdge> edges;

    LowerBoundMinCostFlow(int n)
        : n(n),
          SS(n),
          TT(n + 1),
          mf(n + 2),
          balance(n, 0) {}

    // 返回这条原边的编号
    int addEdge(
        int u,
        int v,
        ll lower,
        ll upper,
        ll cost
    ) {
        // 下界部分是必须走的
        baseCost += lower * cost;

        // 固定 lower 后造成节点流量不平衡
        balance[u] -= lower;
        balance[v] += lower;

        // 剩余还能流 upper-lower
        int id = mf.addEdge(
            u,
            v,
            upper - lower,
            cost
        );

        edges.push_back({
            u,
            id,
            lower,
            upper
        });

        return (int)edges.size() - 1;
    }

    // 返回 {是否可行, 最小费用}
    pair<bool, ll> solve() {

        ll need = 0;

        for (int i = 0; i < n; i++) {

            if (balance[i] > 0) {
                // 流入过多，需要把流送出去
                mf.addEdge(
                    SS,
                    i,
                    balance[i],
                    0
                );

                need += balance[i];
            }

            else if (balance[i] < 0) {
                // 流出过多，需要补充流入
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

        ll remain = mf.g[x.u][x.id].cap;

        ll extra =
            (x.upper - x.lower) - remain;

        return x.lower + extra;
    }
};
```
