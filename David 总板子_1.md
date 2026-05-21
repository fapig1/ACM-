# David 总板子_1

*5.20更新*

主要包含**图论**和**数据结构**的主要模板

---------------

[TOC]

## 图论

### Tarjan缩点

```c++
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

```



### 倍增LCA

```c++
const int N=5e5+5;
vector<vector<int>> gra(N);
vector<vector<int>> par(N,vector<int>(19));//父节点们
vector<int> dep(N);//深度
int n,q,root;

//dfs求深度和第一个父亲
void dfs(int ind){
    for(int i:gra[ind]){
        if(i==par[ind][0])continue;
        par[i][0]=ind;
        dep[i]=dep[ind]+1;
        dfs(i);
    }
}

void solve(){
    cin>>n>>q>>root;

    for(int i = 1;i < n;i ++){
        int a,b;
        cin>>a>>b;
        gra[a].push_back(b);
        gra[b].push_back(a);
    }

    dfs(root);

    //关键步骤：求每一个节点的第2^n个父亲是谁，可以nlogn递推求出
    for(int i=1;i<=19;i++){
        for(int j=1;j<=n;j++){
            if(par[j][i-1]==0)continue;
            par[j][i]=par[par[j][i-1]][i-1];
        }
    }

    //先把两个点深度变为一样的，再同时往上走，使用倍增进行优化
    for(int i=0;i<q;i++){
        int a,b;
        cin>>a>>b;
        if(dep[a]>dep[b]){swap(a,b);}

        for(int i=19;i>=0;i--){
            if(par[b][i]==0)continue;
            if(dep[par[b][i]]>=dep[a]){
                b=par[b][i];
            }
        }
        if(b==a){
            int lca=a;
            cout<<lca<<endl;
            continue;
        }

        for(int i=19;i>=0;i--){
            if(par[b][i]==0)continue;

            if(par[b][i]!=par[a][i]){
                b=par[b][i];
                a=par[a][i];
            }
        }
        int lca=par[a][0];
        cout<<lca<<endl;
    }

}
```



### K边最短路 bellman-ford

```c++
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



### 普通树链剖分

```c++
//树链剖分求lca
//O(nlog⁡n + q(logn)^2)
const int N=5e5+5;
vector<vector<int>> gra(N);
vector<int> par(N,0);//父节点
vector<int> dep(N);//深度
vector<int> siz(N);//子树大小
vector<int> mxchd(N);//最大子节点

vector<int> top(N);//当前节点重链的最顶端节点
vector<int> dfn(N);//重链优先的dfs序
vector<int> rdfn(N);//dfs序的逆，即rdfn[dfn[x]]=x

int n,q,root;
int dfnind = 1;
//第一次dfs找齐每一个节点的父节点，深度，子树大小和最大子节点
void dfs1(int ind){

    top[ind] = ind;

    int csize = 1;
    int mxcd = 0;
    int mxsiz = 0;
    for(int i : gra[ind]){
        if(i == par[ind]) continue;
        par[i] = ind;

        dep[i] = dep[ind] + 1;

        dfs1(i);

        csize += siz[i];
        
        if(siz[i] > mxsiz){
            mxsiz = siz[i];
            mxcd = i;
        }
    }
    siz[ind] = csize;
    mxchd[ind] = mxcd;
    return;

}

//第二次dfs进行重链优先的dfs，找到每一个节点的top，并且标好dfn序
void dfs2(int ind,int tp){

    dfn[ind] = dfnind;
    rdfn[dfnind] = ind;
    dfnind ++;

    top[ind] = tp;

    if(mxchd[ind] != 0)
    dfs2(mxchd[ind],tp);

    for(int i : gra[ind]){
        if(i == par[ind])continue;
        if(i == mxchd[ind])continue;
        dfs2(i, i);
    }

    return;
}

void solve(){
    cin>>n>>q>>root;

    for(int i = 1;i < n;i ++){
        int a,b;
        cin>>a>>b;
        gra[a].push_back(b);
        gra[b].push_back(a);
    }

    dfs1(root);
    dfs2(root,root);

    //求lca的原理：先找到两个点dfn序最大的那个点
    //如果他的top比另一个点还要大，那么往上走一条链，直到top小于等于另外一个节点
    //另外一个节点也同理，直到两个节点在同一条链上，dfn序最小的节点即为lca
    for(int i=0;i<q;i++){
        int a,b;
        cin>>a>>b;

        if(dfn[a] > dfn[b])swap(a, b);
        while(dfn[top[a]] < dfn[top[b]]){
            b = par[top[b]];
            while(dfn[top[b]] < dfn[top[a]]){
                a = par[top[a]];
            }
        }

        int lca = rdfn[min(dfn[a],dfn[b])];
        cout<<lca<<endl;
    }

}
```





### 线段树树链剖分

```c++
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

### 动态维护带权树直径/直径端点/中心

```c++
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
```



### 简单树直径

```c++
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



### 简单树重心

```c++
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





### Spfa判负环

```c++
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



### 二分图相关

#### 匈牙利二分图最大匹配

```c++
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
    vec.assign(n1+1,vector<int>());
    state.assign(n2+1,0);
    //此时左部和右部已经被区分过，如果没有被区分过先进行染色dfs操作
    cin>>n1>>n2>>m;
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

```

**补充定义和定理：**

- 最大匹配数：最大匹配的匹配边的数目

- 最小点覆盖数：选取最少的点，使任意一条边至少有一个端点被选择

- 最大独立集：选取最多的点，使任意所选两点均不相连

- 最小路径覆盖数：对于一个 DAG（有向无环图），选取最少条路径，使得每个顶点属于且仅属于一条路径。路径长可以为 0（即单个点）。

- 数量关系：

	- 定理1：最大匹配数 = 最小点覆盖数（这是 Konig 定理）

	- 定理2：最大独立集 = 顶点数 - 最大匹配数

	- 定理3：最小路径覆盖数 = 顶点数 - 最大匹配数

		

#### 带权二分图最优匹配 KM算法

```c++
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
```



### 网络流相关

#### Dinic最大流

```c++
const int N=210;
const int M=10010;
const int MOD=998244353;

int idx=1;//从2 3开始配对
int h[N];
int d[N],cur[N];
int S,T;
int n,m;

struct edge {
    int v,c,ne;
}e[M];

void add(int a,int b,int c) {
    e[++idx]={b,c,h[a]};
    h[a]=idx;
}
bool bfs() {
    memset(d,0,sizeof d);
    queue<int> q;
    q.push(S);
    d[S]=1;
    while (!q.empty()) {
        int u=q.front();
        q.pop();
        for (int i=h[u];i!=-1;i=e[i].ne) {
            int v=e[i].v;
            if (d[v]==0&&e[i].c) {
                d[v]=d[u]+1;
                q.push(v);
                if (v==T) return true;
            }
        }
    }
    return false;
}
int dfs(int u,int mf) { //多路增广
    if (u==T) return mf;
    int sum=0;
    for (int i=cur[u];i!=-1;i=e[i].ne) {
        cur[u]=i;//当前弧优化
        int v=e[i].v;
        if (d[v]==d[u]+1&&e[i].c) {
            int f=dfs(v,min(mf,e[i].c));
            e[i].c-=f;
            e[i^1].c+=f;//更新残留网
            sum+=f;//累加u的流出流量
            mf-=f;//减少u的剩余容量
            if (mf==0) break;//余量优化
        }
    }
    if (sum==0) d[u]=0;//残枝优化
    return sum;
}

// O ( n * n * m )
int dinic() {
    int flow=0;
    while (bfs()) {
        memcpy(cur,h,sizeof h);
        flow+=dfs(S,INF);
    }
    return flow;
}

void solve()
{
    memset(h,-1,sizeof h);
    cin>>n>>m>>S>>T;
    for (int i=1;i<=m;i++) {
        int u,v,w;
        cin>>u>>v>>w;
        add(u,v,w);
        add(v,u,0);
    }
    cout<<dinic()<<endl;
}
```



#### EK最大流

```c++
const int N=210;
const int M=10010;
const int MOD=998244353;

int idx=1;//从2 3开始配对
int h[N];
int mf[N],pre[N];
int S,T;
int n,m;

struct edge {
    int v,c,ne;
}e[M]; //注意边集要开两倍大小

void add(int a,int b,int c) {
    e[++idx]={b,c,h[a]};
    h[a]=idx;
}
bool bfs() {
    memset(mf,0,sizeof mf);
    queue<int> q;
    q.push(S);
    mf[S]=INF;
    while (!q.empty()) {
        int u=q.front();
        q.pop();
        for (int i=h[u];i!=-1;i=e[i].ne) {
            int v=e[i].v;
            if (mf[v]==0&&e[i].c) {
                mf[v]=min(mf[u],e[i].c);
                pre[v]=i;
                q.push(v);
                if (v==T) return true;
            }
        }
    }
    return false;
}
// O ( n * m * m )
int EK() {
    int flow=0;
    while (bfs()) {
        int v=T;
        while (v!=S) {
            int i=pre[v];
            e[i].c-=mf[T];
            e[i^1].c+=mf[T];
            v=e[i^1].v;
        }
        flow+=mf[T];
    }
    return flow;
}

void solve()
{
    memset(h,-1,sizeof h);
    cin>>n>>m>>S>>T;
    for (int i=1;i<=m;i++) {
        int u,v,w;
        cin>>u>>v>>w;
        add(u,v,w);
        add(v,u,0);
    }
    cout<<EK()<<endl;
}
```



#### Dinic最小割

```c++
const int N=210;
const int M=10010;
const int MOD=998244353;

int idx=1;//从2 3开始配对
int h[N];
int d[N],cur[N];
int S,T;
int n,m;
bool vis[N];

struct edge {
    int v,c,ne;
}e[M];

void add(int a,int b,int c) {
    e[++idx]={b,c,h[a]};
    h[a]=idx;
}
bool bfs() {
    memset(d,0,sizeof d);
    queue<int> q;
    q.push(S);
    d[S]=1;
    while (!q.empty()) {
        int u=q.front();
        q.pop();
        for (int i=h[u];i!=-1;i=e[i].ne) {
            int v=e[i].v;
            if (d[v]==0&&e[i].c) {
                d[v]=d[u]+1;
                q.push(v);
                if (v==T) return true;
            }
        }
    }
    return false;
}
int dfs(int u,int mf) { //多路增广
    if (u==T) return mf;
    int sum=0;
    for (int i=cur[u];i!=-1;i=e[i].ne) {
        cur[u]=i;//当前弧优化
        int v=e[i].v;
        if (d[v]==d[u]+1&&e[i].c) {
            int f=dfs(v,min(mf,e[i].c));
            e[i].c-=f;
            e[i^1].c+=f;//更新残留网
            sum+=f;//累加u的流出流量
            mf-=f;//减少u的剩余容量
            if (mf==0) break;//余量优化
        }
    }
    if (sum==0) d[u]=0;//残枝优化
    return sum;
}

// O ( n * n * m )
int dinic() {
    int flow=0;
    while (bfs()) {
        memcpy(cur,h,sizeof h);
        flow+=dfs(S,INF);
    }
    return flow;
}

void mincut(int u) {
    vis[u]=1;
    for (int i=h[u];i!=-1;i=e[i].ne) {
        int v=e[i].v;
        if (!vis[v]&&e[i].c) mincut(v);
    }
}

void solve()
{
    memset(h,-1,sizeof h);
    cin>>n>>m>>S>>T;
    vector<int> a(n+5),b(n+5);
    int BASE=m+1; // BASE > m
    for (int i=1;i<=m;i++) {
        int w;
        cin>>a[i]>>b[i]>>w;
        add(a[i],b[i],w*BASE+1);
        add(b[i],a[i],0);
    }
    //最小割最大流定理
    int ans=dinic();
    cout<<ans/BASE<<' '<<ans%BASE<<endl; // 最小割(最大流）大小 最小割的边数
    //最小割的划分
    mincut(S);
    for (int i=1;i<=n;i++) {
        if (vis[i]) cout<<i<<' '; // S
        cout<<endl;
    }
    for (int i=1;i<=n;i++) {
        if (!vis[i]) cout<<i<<' '; // T
        cout<<endl;
    }
}
```



## 数据结构

### 莫队

#### 普通莫队

```c++

const int maxn = 5e4;
int B;
int cur = 0;

struct Q{
    int l, r, id;

    bool operator<(const Q& b)const{
        int pos1 = l / B, pos2 = b.l / B;
        if(pos1 != pos2) return pos1 < pos2;
        if(pos1 & 1) return r < b.r;
        return r > b.r;
    }
};

vector<int> cnt(maxn + 5);
vector<int> a(maxn + 5);
vector<Q> ask(maxn + 5);
vector<pair<int,int>> ans(maxn + 5);

int l = 1, r = 0;

void add(int pos){
    int val = a[pos];
    cur += cnt[val];
    cnt[val]++; 
}

void del(int pos){
    int val = a[pos];
    cnt[val]--;
    cur -= cnt[val];
}

void solve(){
    int n, m;
    cin >> n >> m;
    B = max(1.0, n / sqrt(m * 2.0 / 3.0));
    for(int i = 1; i <= n; i++){
        cin >> a[i];
    }
    for(int i = 0; i < m; i++){
        cin >> ask[i].l >> ask[i].r;
        ask[i].id = i;
    }
    sort(ask.begin(), ask.begin() + m);
    for(int i = 0; i < m; i++){
        auto[ql, qr, id] = ask[i];
        while(l > ql) add(--l);
        while(r < qr) add(++r);
        while(l < ql) del(l++);
        while(r > qr) del(r--);
        int len = r - l + 1;
        int fm = cur == 0 ? 1 : len * (len - 1) / 2;
        int g = cur == 0 ? 1 : __gcd(cur, fm);
        ans[id] = {cur / g, fm / g};
    }
    for(int i = 0; i < m; i++){
        cout << ans[i].first << '/' << ans[i].second << '\n';
    }


}


```



#### 带修莫队

```c++

int n, m, B;
struct Q{
    int l, r, t, id;

    bool operator<(const Q& b)const{
        if(l / B != b.l / B) return l / B < b.l / B;
        if(r / B != b.r / B) return r / B < b.r / B;
        return t < b.t;
    }

    Q(int l, int r, int t, int id) : l(l), r(r), t(t), id(id) {}
};

struct OP{
    int p, x;

    OP(int p, int x): p(p), x(x) {}
};

vector<int> a;
vector<Q> ask;
vector<OP> op; 
vector<int> cnt(1000006);
int res = 0;

void add(int val){
    if(cnt[val] == 0) res++;
    cnt[val]++; 
}

void del(int val){
    cnt[val]--;
    if(cnt[val] == 0) res--;
}

void solve(){
    cin >> n >> m;
    a.resize(n);
    for(int i = 0; i < n; i++){
        cin >> a[i];
    }

    for(int i = 0; i < m; i++){
        char c;
        int x, y;
        cin >> c >> x >> y;
        if(c == 'Q'){
            ask.push_back(Q(x - 1, y - 1, op.size(), ask.size()));
        } else {
            op.push_back(OP(x - 1, y));
        }
    }
    B = pow(n, 2.0 / 3.0);
    sort(ask.begin(), ask.end());
    vector<int> ans(ask.size());
    int curT = 0, l = 0, r = -1;
    for(int i = 0; i < ask.size(); i++){
        auto[ql, qr, t, id] = ask[i];
        while(l > ql) add(a[--l]);
        while(r < qr) add(a[++r]);
        while(l < ql) del(a[l++]);
        while(r > qr) del(a[r--]);
        while(curT < t){
            int pos = op[curT].p;
            if(l <= pos && pos <= r){
                del(a[pos]), add(op[curT].x);
            }
            swap(a[pos], op[curT].x);
            curT++;
        }
        while(curT > t){
            curT--;
            int pos = op[curT].p;
            if(l <= pos && pos <= r){
                del(a[pos]), add(op[curT].x);
            }     
            swap(a[pos], op[curT].x);
        }
        ans[id] = res;
    }
    for(int i : ans) cout << i << '\n';

}
```



#### 回滚莫队

```c++

int n, q, B;
struct Q{
    int l, r, id;

    bool operator<(const Q& b)const{
        if(l / B != b.l / B) return l / B < b.l / B;
        return r < b.r;
    }
    Q(int l, int r, int id) : l(l), r(r), id(id) {}
};

vector<int> a;
vector<Q> ask;
vector<int> fst, lst;
vector<int> cnt(200005);

void solve(){
    cin >> n;
    a.resize(n);
    fst.resize(n + 5, - 1);
    lst.resize(n + 5, -1);
    for(int i = 0; i < n; i++){
        cin >> a[i];
    }    
    cin >> q;
    B = max(1.0, n / sqrt(q * 2.0 / 3.0));
    auto b = a;
    sort(b.begin(), b.end());
    b.erase(unique(b.begin(), b.end()), b.end());
    for(int i = 0; i < n; i++){
        a[i] = lower_bound(b.begin(), b.end(), a[i]) - b.begin();
    }
    for(int i = 0; i < q; i++){
        int x, y;
        cin >> x >> y;
        ask.push_back(Q(x - 1, y - 1, i));
    }
    sort(ask.begin(), ask.end());
    vector<int> ans(q);
    int lb = -1, r = -1, res = 0;
    for(int i = 0; i < q; i++){
        auto[ql, qr, id] = ask[i];
        int cur = ql / B;

        if(cur != lb){
            fill(fst.begin(), fst.end(), -1);
            fill(lst.begin(), lst.end(), -1);
            res = 0;
            lb = cur;
            r = (cur + 1) * B - 1;
        }
        
        if(cur == qr / B){
            int temp = 0;
            vector<int> wait;
            for(int j = ql; j <= qr; j++){
                int val = a[j];
                if(fst[val] == -1){
                    fst[val] = j;
                    wait.push_back(val);
                }
                lst[val] = j;
                temp = max(temp, j - fst[val]);
            }
            ans[id] = temp;
            for(int val : wait){
                fst[val] = -1;
                lst[val] = -1;
            }
            continue;
        }


        while(r < qr){
            r++;
            int val = a[r];
            if(fst[val] == -1) fst[val] = r;
            lst[val] = r;
            res = max(res, lst[val] - fst[val]);
        }

        int l = min(n, (cur + 1) * B);
        int tmp = res;
        stack<array<int,3>> wait;// val fst lst 
        while(l > ql){
            l--;
            int val = a[l];
            wait.push({val, fst[val], lst[val]});
            if(lst[val] == -1) lst[val] = l;
            fst[val] = l;
            res = max(res, lst[val] - fst[val]);
        }

        ans[id] = res;
        res = tmp;

        while(wait.size()){
            auto[val, l, r] = wait.top();
            wait.pop();
            fst[val] = l;
            lst[val] = r; 
        }

    }

    for(int i : ans) cout << i << '\n';

}
```



### 区间加/乘线段树

```c++
//线段树
struct Seg{
    int n, mod;
    vector<int> tr, lazyAdd, lazyMul;

    Seg(int n, int mod) : n(n), mod(mod),
    tr(n * 4 + 5),
    lazyAdd(n * 4 + 5),
    lazyMul(n * 4 + 5, 1) {}

    void build(int p, int l, int r, const vector<int>& a){
        if(l == r){
            tr[p] = a[l] % mod;
            return;
        }
        int mid = l + r >> 1;
        build(p << 1, l, mid, a);
        build(p << 1 | 1, mid + 1, r, a);
        pushup(p);
    }

    void clear(){
        fill(tr.begin(), tr.end(), 0);
        fill(lazyAdd.begin(), lazyAdd.end(), 0);
        fill(lazyMul.begin(), lazyMul.end(), 1);
    }

    void pushup(int p){
        tr[p] = (tr[p << 1] + tr[p << 1 | 1]) % mod;
    }

    void apply(int p, int l, int r, int mul, int add){
        tr[p] = (1LL * tr[p] * mul % mod + 1LL * (r - l + 1) * add % mod) % mod;
        lazyMul[p] = 1LL * lazyMul[p] * mul % mod;
        lazyAdd[p] = (1LL * lazyAdd[p] * mul % mod + add) % mod;
    }

    void pushdown(int p, int l, int r){
        if(lazyMul[p] != 1 || lazyAdd[p] != 0){
            int mid = l + r >> 1;
            apply(p << 1, l, mid, lazyMul[p], lazyAdd[p]);
            apply(p << 1 | 1, mid + 1, r, lazyMul[p], lazyAdd[p]);
            lazyMul[p] = 1;
            lazyAdd[p] = 0;
        }
    }

    void updateMul(int p, int l, int r, int ql, int qr, int val){
        if(ql <= l && r <= qr){
            apply(p, l, r, val, 0);
            return;
        }
        pushdown(p, l, r);
        int mid = l + r >> 1;
        if(ql <= mid) updateMul(p << 1, l, mid, ql, qr, val);
        if(mid < qr) updateMul(p << 1 | 1, mid + 1, r, ql, qr, val);
        pushup(p);
    }

    void updateAdd(int p, int l, int r, int ql, int qr, int val){
        if(ql <= l && r <= qr){
            apply(p, l, r, 1, val);
            return;
        }
        pushdown(p, l, r);
        int mid = l + r >> 1;
        if(ql <= mid) updateAdd(p << 1, l, mid, ql, qr, val);
        if(mid < qr) updateAdd(p << 1 | 1, mid + 1, r, ql, qr, val);
        pushup(p);
    }

    int ask(int p, int l, int r, int ql, int qr){
        if(ql <= l && r <= qr){
            return tr[p];
        }
        pushdown(p, l, r);
        int mid = l + r >> 1;
        int res = 0;
        if(ql <= mid) res = (res + ask(p << 1, l, mid, ql, qr)) % mod;
        if(mid < qr) res = (res + ask(p << 1 | 1, mid + 1, r, ql, qr)) % mod;
        return res;
    }
};
```



### 二维线段树

```c++
// 二维线段树
struct Seg2D {
    int n, m;
    vector<vector<int>> tr, lazy;
    
    Seg2D(int n, int m) : n(n), m(m) {
        tr.assign(n * 4 + 5, vector<int>(m * 4 + 5, 0));
        lazy.assign(n * 4 + 5, vector<int>(m * 4 + 5, 0));
    }
    
    void build(int p, int l, int r, const vector<vector<int>>& a) {
        build_y(p, 1, 1, m, l, r, a);
    }
    
    // 构建Y轴
    void build_y(int px, int py, int ly, int ry, int lx, int rx, const vector<vector<int>>& a) {
        if(ly == ry) {
            if(lx == rx) {
                tr[px][py] = a[lx][ly];
            } else {
                tr[px][py] = tr[px << 1][py] + tr[px << 1 | 1][py];
            }
            return;
        }
        int mid = ly + ry >> 1;
        build_y(px, py << 1, ly, mid, lx, rx, a);
        build_y(px, py << 1 | 1, mid + 1, ry, lx, rx, a);
        pushup_y(px, py);
    }
    
    void build(int px, int lx, int rx, const vector<vector<int>>& a) {
        if(lx == rx) {
            build_y(px, 1, 1, m, lx, rx, a);
            return;
        }
        int mid = lx + rx >> 1;
        build(px << 1, lx, mid, a);
        build(px << 1 | 1, mid + 1, rx, a);
        build_y(px, 1, 1, m, lx, rx, a);
    }
    
    void clear() {
        for(auto& row : tr) fill(row.begin(), row.end(), 0);
        for(auto& row : lazy) fill(row.begin(), row.end(), 0);
    }
    
    void pushup_y(int px, int py) {
        tr[px][py] = tr[px][py << 1] + tr[px][py << 1 | 1];
    }
    
    void pushdown_y(int px, int py, int ly, int ry) {
        if(lazy[px][py]) {
            int mid = ly + ry >> 1;
            tr[px][py << 1] += (mid - ly + 1) * lazy[px][py];
            tr[px][py << 1 | 1] += (ry - mid) * lazy[px][py];
            lazy[px][py << 1] += lazy[px][py];
            lazy[px][py << 1 | 1] += lazy[px][py];
            lazy[px][py] = 0;
        }
    }
    
    // 内层Y轴更新
    void update_y(int px, int py, int ly, int ry, int qly, int qry, int val) {
        if(qly <= ly && ry <= qry) {
            tr[px][py] += (ry - ly + 1) * val;
            lazy[px][py] += val;
            return;
        }
        pushdown_y(px, py, ly, ry);
        int mid = ly + ry >> 1;
        if(qly <= mid) update_y(px, py << 1, ly, mid, qly, qry, val);
        if(mid < qry) update_y(px, py << 1 | 1, mid + 1, ry, qly, qry, val);
        pushup_y(px, py);
    }
    
    // 外层X轴更新
    void update(int p, int l, int r, int ql, int qr, int qly, int qry, int val) {
        if(ql <= l && r <= qr) {
            update_y(p, 1, 1, m, qly, qry, val);
            return;
        }
        int mid = l + r >> 1;
        if(ql <= mid) update(p << 1, l, mid, ql, qr, qly, qry, val);
        if(mid < qr) update(p << 1 | 1, mid + 1, r, ql, qr, qly, qry, val);
        // 更新当前节点
        for(int py = 1; py <= m * 4; py++) {
            tr[p][py] = tr[p << 1][py] + tr[p << 1 | 1][py];
        }
    }
    
    // 内层Y轴查询
    int ask_y(int px, int py, int ly, int ry, int qly, int qry) {
        if(qly <= ly && ry <= qry) {
            return tr[px][py];
        }
        pushdown_y(px, py, ly, ry);
        int mid = ly + ry >> 1;
        int res = 0;
        if(qly <= mid) res += ask_y(px, py << 1, ly, mid, qly, qry);
        if(mid < qry) res += ask_y(px, py << 1 | 1, mid + 1, ry, qly, qry);
        return res;
    }
    
    // 外层X轴查询
    int ask(int p, int l, int r, int ql, int qr, int qly, int qry) {
        if(ql <= l && r <= qr) {
            return ask_y(p, 1, 1, m, qly, qry);
        }
        int mid = l + r >> 1;
        int res = 0;
        if(ql <= mid) res += ask(p << 1, l, mid, ql, qr, qly, qry);
        if(mid < qr) res += ask(p << 1 | 1, mid + 1, r, ql, qr, qly, qry);
        return res;
    }
};

// 使用示例
// int main() {
//     int n = 5, m = 5;
//     vector<vector<int>> a(n + 1, vector<int>(m + 1));
//     
//     Seg2D seg(n, m);
//     seg.build(1, 1, n, a);
//     
//     // 矩形区域 (x1,y1) 到 (x2,y2) 加 val
//     seg.update(1, 1, n, x1, x2, y1, y2, val);
//     
//     // 查询矩形区域 (x1,y1) 到 (x2,y2) 的和
//     int sum = seg.ask(1, 1, n, x1, x2, y1, y2);
//     
//     return 0;
// }
```



### 李超线段树

```c++
//普通李超线段树
//用于查询若干线段或者直线在某点处的最值
const double eps = 1e-9, INF = 1e18;


int sgn(double x){
    if(fabs(x) < eps) return 0;
    return x < 0 ? -1 : 1;
}

struct Line{
    double k, b;
    bool exists;
                                //更改处（-INF）
    Line(double k = 0, double b = INF) : k(k), b(b), exists(false) {}

    double cal(int x)const{
        return k * x + b;
    }
};
//插入线段复杂度为 log * log 其余为 log (当前为维护最小值版本)
struct LCSeg{
    int n;
    vector<Line> tr;
    LCSeg(int n) : n(n), tr(n * 4) {}

    void add(int p, int l, int r, Line val){
        if(!tr[p].exists){
            tr[p] = val;
            tr[p].exists = true;
            return;
        }
        int mid = l + r >> 1;
                        //更改处 <
        if(tr[p].cal(mid) > val.cal(mid)) swap(tr[p], val);
        if(l == r) return;
                        //更改处 >
        if(val.cal(l) < tr[p].cal(l)){
            add(p << 1, l, mid, val);
        } else {
            add(p << 1 | 1, mid + 1, r, val);
        }
    }

    // 插入一条全局直线
    void addLine(double k, double b){
        Line l(k, b);
        l.exists = true;
        add(1, 1, n, l);
    }

    // 插入一条在 [L, R] 范围内的线段
    void addSeg(int p, int l, int r, int ql, int qr, Line val){
        if(ql <= l && r <= qr){
            add(p, l, r, val);
            return;
        }
        int mid = l + r >> 1;
        if(ql <= mid) addSeg(p << 1, l, mid, ql, qr, val);
        if(mid < qr) addSeg(p << 1 | 1, mid + 1, r, ql, qr, val);
    }   
    // 查询 x 处的最大 y 值
    double ask(int p, int l, int r, int x){
        if(x < l || x > r || !tr[p].exists) return INF;//更改处-INF
        double res = tr[p].cal(x);
        if(l == r) return res;
        int mid = l + r >> 1;
                        //更改处 max
        if(x <= mid) return min(res, ask(p << 1, l, mid, x));
        return min(res, ask(p << 1 | 1, mid + 1, r, x));
    }
};

```

#### 离散化李超线段树

```c++
//离散化李超线段树（对所有可能的x值进行离散化处理）（最大值版本）
const int INF = 2e18; // 维护最大值，初始b设为-INF

struct Line {
    int k, b;
    bool exists;
    Line(int k = 0, int b = -INF) : k(k), b(b), exists(false) {}
    int cal(int x) const {
        return k * x + b;
    }
};

struct LCSeg {
    int n;
    vector<Line> tr;
    vector<int>& coords; //离散化后的数组，sort+erase（unique）

    LCSeg(int n, vector<int>& c) : n(n), tr(n * 4), coords(c) {}

    void add(int p, int l, int r, Line val) {
        if (!tr[p].exists) {
            tr[p] = val; tr[p].exists = true;
            return;
        }
        
        int mid = l + r >> 1;
        
        int mid_x = coords[mid - 1]; 
        int l_x = coords[l - 1];
        int r_x = coords[r - 1];

        if (tr[p].cal(mid_x) < val.cal(mid_x)) swap(tr[p], val);
        if (l == r) return;

        if (val.cal(l_x) > tr[p].cal(l_x)) add(p << 1, l, mid, val);
        else if (val.cal(r_x) > tr[p].cal(r_x)) add(p << 1 | 1, mid + 1, r, val);
    }

    void addLine(int k, int b) {
        Line l(k, b);
        l.exists = true;
        add(1, 1, n, l);
    }

    int ask(int p, int l, int r, int idx) {
        if (!tr[p].exists) return -INF;
        int cur_x = coords[idx - 1];
        int res = tr[p].cal(cur_x);
        if (l == r) return res;
        
        int mid = l + r >> 1;
        if (idx <= mid) return max(res, ask(p << 1, l, mid, idx));
        return max(res, ask(p << 1 | 1, mid + 1, r, idx));
    }
};

// auto get_id = [&](int x) {
//     return lower_bound(coords.begin(), coords.end(), x) - coords.begin() + 1;
// };

// for (int i = 1; i <= n; i++) {
//     int x_idx = get_id(pre[i]);  ！！！要引用离散化后的角标（1-based）
//     dp[i] = seg.ask(1, 1, m, x_idx) + a * pre[i] * pre[i] + b * pre[i];
//     int new_k = -2 * a * pre[i];
//     int new_b = dp[i] + a * pre[i] * pre[i] - b * pre[i] + c;
//     seg.addLine(new_k, new_b);
// }
```



### 主席树

```c++
//主席树单点修改单点查询

const int N=1e6+5;

struct node{
    int l,r;
    ll val;
}tree[N*40];
//模版上写20，但是我交了re，写大一点过了，有些玄学

vector<ll> vec(N);
vector<int> root(N);//root表示第i个版本的根节点的index

int top=0;
//在更新操作时进行动态开点的函数
int clone(int ind){
    top++;
    tree[top]=tree[ind];
    return top;
}

//建树
int build(int ind,int l,int r){
    top++;
    ind=top;

    if(l==r){
        tree[ind].val=vec[l];
        return ind;
    }

    int mid=(l+r)/2;
    //实际上更新点可以有很多种方式，我使用的方式是为了统一更新建树和查询函数参数的格式
    //build里面的第一个参数意义不大，为了统一参数那么写的，写一样的就行
    tree[ind].l = build(tree[ind].l,l,mid);
    tree[ind].r = build(tree[ind].r,mid+1,r);
    return ind;
}

//更新
int update(int ind,int l,int r,int t,ll v){
    ind=clone(ind);

    if(l==r){
        tree[ind].val=v;
        return ind;
    }
    int mid=(l+r)/2;
    if(t<=mid){
        tree[ind].l=update(tree[ind].l,l,mid,t,v);
    }else{
        tree[ind].r=update(tree[ind].r,mid+1,r,t,v);
    }
    return ind;
}

//查询
ll query(int ind,int l,int r,int t){
    if(l==r){
        return tree[ind].val; 
    }

    int mid=(l+r)/2;
    if(t<=mid){
        return query(tree[ind].l,l,mid,t);
    }else{
        return query(tree[ind].r,mid+1,r,t);
    }
}

int n,q;
void solve(){
    cin>>n>>q;

    for(int i = 1;i <= n;i ++){
        cin>>vec[i];
    }

    root[0]=build(1,1,n);
    for(int i=1;i<=q;i++){
        int version;
        int command;
        cin>>version>>command;
        if(command==1){
            int t;
            ll v;
            cin>>t>>v;
            root[i]=update(root[version],1,n,t,v);
        }else{
            int t;
            cin>>t;
            ll ans=query(root[version],1,n,t);
            cout<<ans<<endl;
            root[i]=root[version];
        }

    }

}
```





### 带权并查集

```c++
//带权并查集，维护每个连通块根节点到每个节点中某个权值关系
struct DSU{
    int n;
    vector<int> fa, sz, dis;

    DSU(int n):n(n),fa(n+1),sz(n+1,1),dis(n+1,0){
        for(int i=1;i<=n;i++){
            fa[i]=i;
        }
    }

    int find(int x){
        if(fa[x]==x) return x;

        int f=fa[x];
        fa[x]=find(fa[x]);

        dis[x]+=dis[f];

        return fa[x];
    }

    bool merge(int x,int y,int d){
        int fx=find(x);
        int fy=find(y);

        if(fx==fy){
            return dis[y]-dis[x]==d;
        }

        if(sz[fx]>sz[fy]){
            swap(x,y);
            swap(fx,fy);
            d=-d;
        }

        fa[fx]=fy;

        dis[fx]=dis[y]-dis[x]-d;

        sz[fy]+=sz[fx];

        return true;
    }
};
```


