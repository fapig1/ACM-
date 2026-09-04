# David_图论



[TOC]

## 普通图论

### Tarjan缩点求最大连通分量

```cpp
#define int long long
using namespace std;
int mod=1e9+7;
int MAXN=5e5+5;
vector<vector<int>> gra(MAXN);	//原图邻接表
vector<int> val(MAXN);			//原图点权
vector<int> dfn(MAXN);			//原图dfn序，用于构建scc
vector<int> low(MAXN);			//节点的scc根节点，用于构建scc
stack<int> stk;					//栈，用于构建scc
vector<int> instk(MAXN);		//节点是否入栈，用于构建scc
vector<int> itscc(MAXN);		//ind to scc 原图点角标转新图点角标
vector<int> sccval(MAXN);		//新图点权
vector<vector<int>> sccgra(MAXN);	//新图邻接表
int cur=1;						//维护原图dfn值
int curscc=0;					//新图大小

int n,m;
vector<pair<int,int>> edge(MAXN);
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
            itscc[c]=curscc;
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
        if(itscc[a]!=itscc[b]){
            sccgra[itscc[a]].push_back(itscc[b]);
        }
    }
}
```

### 倍增LCA

```cpp
//#define int long long
const int N=4e4+10;
int n,a,b,m,x,y;
vector<int> arr[N];
int depth[N],fa[N][16];

void bfs(int root)
{
  memset(depth,0x3f,sizeof depth);
  depth[0]=0,depth[root]=1;
  queue<int> q;
  q.push(root);
  while(!q.empty())
  {
    int t=q.front();
    q.pop();
    for(auto j:arr[t])
    {
      if(depth[j]>depth[t]+1)
      {
        depth[j]=depth[t]+1;
        q.push(j);
        fa[j][0]=t;//j往上跳一步是t
        for(int k=1;k<=15;k++)
        {
          fa[j][k]=fa[fa[j][k-1]][k-1];
        }
      }
    }
  }
}

int lca(int a,int b)
{
  if(depth[a]<depth[b])
  {
    swap(a,b);
  }
  for(int k=15;k>=0;k--)
  {
    if(depth[fa[a][k]]>=depth[b])
    {
      a=fa[a][k];
    }
  }
  if(a==b)
  return a;
  for(int k=15;k>=0;k--)
  {
    if(fa[a][k]!=fa[b][k])
    {
      a=fa[a][k];
      b=fa[b][k];
    }
  }
  return fa[a][0];
}
```

### 树链剖分（独立版）

```cpp
#define int long long
const int N=1e5+10;
int n,m,root,mod,x,y,z,op;
vector<int> arr[N];
int w[N],fa[N],sz[N],son[N],dep[N],top[N],id[N],nw[N];
int tot;

void dfs1(int u,int father)
{
  fa[u]=father,sz[u]=1,dep[u]=dep[father]+1;
  for(auto v:arr[u])
  {
    if(v==father) continue;
    dfs1(v,u);
    sz[u]+=sz[v];
    if(sz[son[u]]<sz[v])
    {
      son[u]=v;
    }
  }
}

void dfs2(int u,int t)
{
  top[u]=t,id[u]=++tot,nw[tot]=w[u]%mod;
  if(!son[u]) return;
  dfs2(son[u],t);
  for(auto v:arr[u])
  {
    if(v==fa[u]||v==son[u])
    {
      continue;
    }
    dfs2(v,v);
  }
}

struct tree
{
  int l,r,sum,lazy;
}tr[4*N];

void pushup(int u)
{
  tr[u].sum=(tr[u*2].sum+tr[u*2+1].sum)%mod;
}

void pushdown(int u)
{
  if(tr[u].lazy)
  {
    tr[u*2].lazy=(tr[u*2].lazy+tr[u].lazy)%mod;
    tr[u*2+1].lazy=(tr[u*2+1].lazy+tr[u].lazy)%mod;
    tr[u*2].sum=(tr[u*2].sum+tr[u].lazy*(tr[u*2].r-tr[u*2].l+1))%mod;
    tr[u*2+1].sum=(tr[u*2+1].sum+tr[u].lazy*(tr[u*2+1].r-tr[u*2+1].l+1))%mod;
    tr[u].lazy=0;
  }
}

void build(int u,int l,int r)
{
  tr[u].l=l;
  tr[u].r=r;
  tr[u].lazy=0;
  if(l==r)
  {
    tr[u].sum=nw[l]%mod;
    return;
  }
  int mid=l+r>>1;
  build(u*2,l,mid);
  build(u*2+1,mid+1,r);
  pushup(u);
}

int query(int u,int l,int r)
{
  if(l<=tr[u].l&&tr[u].r<=r)
  {
    return tr[u].sum%mod;
  }
  int mid=tr[u].l+tr[u].r>>1,res=0;
  pushdown(u);
  if(l<=mid) res=(res+query(u*2,l,r))%mod;
  if(mid<r) res=(res+query(u*2+1,l,r))%mod;
  
  return res%mod;
}

int query_path(int u,int v)
{
  int res=0;
  while(top[u]!=top[v])
  {
    if(dep[top[u]]<dep[top[v]]) swap(u,v);
    res=(res+query(1,id[top[u]],id[u]))%mod;
    u=fa[top[u]];
  }
  if(dep[u]<dep[v]) swap(u,v);
  res=(res+query(1,id[v],id[u]))%mod;
  return res%mod;
}

void update(int u,int l,int r,int k)
{
  k%=mod;
  if(l<=tr[u].l&&tr[u].r<=r)
  {
    tr[u].lazy=(tr[u].lazy+k)%mod;
    tr[u].sum=(tr[u].sum+k*(tr[u].r-tr[u].l+1))%mod;
    return;
  }
  pushdown(u);
  int mid=tr[u].l+tr[u].r>>1;
  if(l<=mid) update(u*2,l,r,k);
  if(mid<r) update(u*2+1,l,r,k);
  pushup(u);
}

void update_path(int u,int v,int k)
{
  while(top[u]!=top[v])
  {
    if(dep[top[u]]<dep[top[v]]) swap(u,v);
    update(1,id[top[u]],id[u],k);
    u=fa[top[u]];
  }
  if(dep[u]<dep[v]) swap(u,v);
  update(1,id[v],id[u],k);
}

void update_tree(int u,int k)
{
  update(1,id[u],id[u]+sz[u]-1,k);
}

int query_tree(int u)
{
  return query(1,id[u],id[u]+sz[u]-1)%mod;
}
```

### 树链剖分（结构体版）

```cpp
// #include "../数据结构/线段树.cpp"

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
```

### 动态维护树直径、端点、中心

```cpp
#define int long long

const int MAXN=2e5+5;
const int LOG=32;

vector<vector<pair<int,int>>> gra(MAXN);
vector<vector<int>> par(MAXN,vector<int>(LOG));
vector<int> vis(MAXN);
vector<int> dep(MAXN);
vector<int> dis(MAXN);

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

int dist(int a,int b){
    int l=lca(a,b);
    return dis[a]+dis[b]-2*dis[l];
}

int jump(int x,int k){
    for(int i=0;i<LOG;i++){
        if(k&(1LL<<i)){
            x=par[x][i];
        }
    }
    return x;
}

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
        int cur=u;
        for(int i=LOG-1;i>=0;i--){
            int pp=par[cur][i];
            if(pp && dep[pp]>dep[p]){
                cur=pp;
            }
        }
        nxt=par[cur][0];
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
```

### 树的直径

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
```

### 树的重心

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
```

### Bellman-Ford（有边数限制最短路）

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
```

### 堆优化Dijkstra求最短路

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
```

### Floyd求多源最短路

```cpp
#define int long long
#define endl '\n'
int n, m, k, x, y, z;
const int N = 205;
int d[N][N];
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
```

### Prim算法求最小生成树

```cpp
//#define int long long
#define endl '\n'
const int N=510;
const int INF=0x3f3f3f3f;
int n,m;
int g[N][N];
int dist[N];
bool st[N];
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
```

### Kruskal算法求最小生成树

```cpp
#define int long long
#define endl '\n'
const int N=2e5+10;
int t,n,m,res,cnt;
int fa[N];
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
```

### 拓扑排序

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
```

### 匈牙利二分图最大匹配

```cpp
#define int long long
#define endl '\n'
const int MAXN=505;
vector<int> state;
vector<int> vis(MAXN);
vector<vector<int>> vec;
int visnum=1;
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
```

### 带权二分图最优匹配（KM算法）

```cpp
#define int long long

const int inf=0x7fffffff;
const int MAXN=505;

int cnta,cntb;
int w[MAXN][MAXN];

int visa[MAXN],visb[MAXN];
int la[MAXN],lb[MAXN];
int match[MAXN],upd[MAXN];
int visnum=0;
bool check(int x) {
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

int KM() {
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

/*
    如果要求完美匹配的最小权，将边权取负数w[i][j]=-w[i][j]
    最后答案输出-KM()即可
*/
```

### 染色法判定二分图

```cpp
#define int long long
#define endl '\n'
const int N=1e5+10;
int t,n,m;
vector<int> arr[N];
int color[N];// 1 / 2
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
```

## 网络流

### Dinic最大流

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

### Dinic最小割

```cpp
using ll = long long;
const ll INF = 4e18;

struct Dinic {
    struct Edge {
        int to, rev;
        ll cap;
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
```

### 最小费用最大流

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
```

### 带费用上下界最小费用可行循环流

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

    int addEdge(int u, int v, ll cap, ll cost) {
        int id = g[u].size();
        Edge a{v, (int)g[v].size(), cap, cost};
        Edge b{u, (int)g[u].size(), 0, -cost};
        g[u].push_back(a);
        g[v].push_back(b);
        return id;
    }

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
    vector<ll> balance;
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
        baseCost += lower * cost;
        balance[u] -= lower;
        balance[v] += lower;
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

    ll getFlow(int edgeId) {
        auto &x = edges[edgeId];
        ll remain = mf.g[x.u][x.id].cap;
        ll extra =
            (x.upper - x.lower) - remain;
        return x.lower + extra;
    }
};
```