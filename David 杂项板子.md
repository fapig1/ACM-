# David 杂项板子



[TOC]



## 图论

### 树链剖分_lca

```c++
#include<bits/stdc++.h>
#define ll long long
using namespace std;

//https://www.luogu.com.cn/problem/P3379
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



### 树链剖分_线段树

```c++
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

signed main()
{
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    
    cin>>n>>m>>root>>mod;
    for(int i=1;i<=n;i++)
    {
      cin>>w[i];
    }
    for(int i=1;i<n;i++)
    {
      cin>>x>>y;
      arr[x].push_back(y);
      arr[y].push_back(x);
    }
    dfs1(root,0);
    dfs2(root,root);

    build(1,1,tot);
    while(m--)
    {
      cin>>op;
      if(op==1)
      {
        cin>>x>>y>>z;
        update_path(x,y,z);
      }
      else if(op==2)
      {
        cin>>x>>y;
        cout<<query_path(x,y)%mod<<endl;
      }
      else if(op==3)
      {
        cin>>x>>z;
        update_tree(x,z);
      }
      else
      {
        cin>>x;
        cout<<query_tree(x)%mod<<endl;
      }
    }

    return 0;
}

```



### bellman-ford 边数限制单源最短路

```c++
#include <bits/stdc++.h>
#define int long long
#define endl '\n'
using namespace std;

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



### 有向图的拓扑排序

```c++
#include <bits/stdc++.h>
#define int long long
#define endl '\n'
using namespace std;

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





### 倍增_lca



```c++
#include<bits/stdc++.h>
#define ll long long
#define endl '\n'
using namespace std;

//https://www.luogu.com.cn/problem/P3379
//倍增法求lca

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



### Kruskal最小生成树

```c++
#include <bits/stdc++.h>
#define int long long
#define endl '\n'
using namespace std;

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



### Prim最小生成树

```c++
#include <bits/stdc++.h>
//#define int long long
#define endl '\n'
using namespace std;

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

int main()
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



### spfa判负环

```c++
#include <bits/stdc++.h>
#define int long long
#define endl '\n'
using namespace std;
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



### spfa最短路

```c++
#include <bits/stdc++.h>
#define int long long
#define endl '\n'
using namespace std;

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



### tarjan求强连通分量

```c++
void tarjan(int u)
{
  dfn[u]=low[u]=++timestm;
  stk[++top]=u,in_stk[u]=true;
  for(int v:arr[u])
  {
    if(!dfn[v])
    {
      tarjan(v);
      low[u]=min(low[u],low[v]);
    }
    else if(in_stk[v])
    {
      low[u]=min(low[u],dfn[v]);
    }
  }
  if(dfn[u]==low[u])
  {
    int y;
    ++scc_cnt;
    do
    {
      y=stk[top--];
      in_stk[y]=false;
      id[y]=scc_cnt;
      siz[scc_cnt]++;
    } while (y!=u);
  }
}
```



### 堆优化Dijkstra求最短路

```c++
#include <bits/stdc++.h>
#define int long long
#define endl '\n'
using namespace std;

const int N = 1e5 + 5;
const int mod = 80112002;
int n, m, s;
int x, y, z;
vector<pair<int, int>>g[N];
int dist[N];
bool st[N];

//堆优化版dij,适用于稠密图
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



### floyd多源最短路

```c++
#include <bits/stdc++.h>
#define int long long
#define endl '\n'
using namespace std;
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



### 二分图最大匹配

```c++
#include <bits/stdc++.h>
#define int long long
#define endl '\n'
using namespace std;
const int INF=1e13;
const double eps=1e-6;
const double PI=acos(-1);
const int N=510;
const int M=110;
const int MOD=998244353;
int n1,n2,m;
int st[N];
int match[N];
vector<int> arr[N];
//匈牙利算法：二分图的最大匹配
bool find(int x)
{
    for(auto y:arr[x])
    {
        if(!st[y])
        {
            st[y]=1;
            if(match[y]==0||find(match[y]))
            {
                match[y]=x;
                return true;
            }
        }
    }
    return false;
}

void solve() 
{
    cin>>n1>>n2>>m;
    for(int i=1;i<=m;i++)
    {
        int u,v;
        cin>>u>>v;
        //因为只从一边找另一边,所以只用存一个方向
        arr[u].push_back(v);
    }
    int res=0;
    for(int i=1;i<=n1;i++)
    {
        memset(st,0,sizeof st);
        if(find(i)) res++;
    }
    cout<<res<<endl;
}


```



### 染色法判二分图

```c++
#include <bits/stdc++.h>
#define int long long
#define endl '\n'
using namespace std;

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



## 数据结构

### 主席树

```c+
#include<bits/stdc++.h>
#define ll long long
#define endl '\n'
using namespace std;

//https://www.luogu.com.cn/problem/P3919
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



### 带修莫队

```c++
#define endl '\n'
#define int long long
#define double long double
const double PI = acos(-1);
const int N = 1e6 + 10;
const int M = 1010;
const int MOD = 1e9 + 7;

int _t, n, x, y, B, m;
int arr[N], cnt[N];
int sum;  
int ans[N];  

struct update  
{
    int pos, old_val, new_val;
} u[N];

struct Q
{
    int l, r, id, tim;
} q[N];

bool cmp(Q& a, Q& b)
{
    if (a.l / B != b.l / B) return a.l < b.l;
    if (a.r / B != b.r / B)
    {
        return (a.l / B % 2 == 0) ? (a.r < b.r) : (a.r > b.r);
    }
    return a.tim < b.tim;
}

void add(int x)
{
    if (cnt[x] == 0) sum++;
    cnt[x]++;
}

void del(int x)
{
    cnt[x]--;
    if (cnt[x] == 0) sum--;
}
void apply(int t, int cur_l, int cur_r)
{
    if (u[t].pos >= cur_l && u[t].pos <= cur_r)
    {
        del(arr[u[t].pos]);
        add(u[t].new_val);
    }
    swap(arr[u[t].pos], u[t].new_val);
}

char op;
int q_cnt, u_cnt;
int cur_l = 1, cur_r = 0, cur_t = 0; 

signed main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    
    cin >> n >> m;
    B = pow(n, 2.0 / 3) + 1;
    
    for (int i = 1; i <= n; i++)
    {
        cin >> arr[i];
    }
    for (int i = 1; i <= m; i++)
    {
        cin >> op;
        if (op == 'Q')
        {
            q_cnt++;
            cin >> q[q_cnt].l >> q[q_cnt].r;
            q[q_cnt].id = q_cnt;
            q[q_cnt].tim = u_cnt;  
        }
        else
        {
            u_cnt++;
            cin >> u[u_cnt].pos >> u[u_cnt].new_val;
            u[u_cnt].old_val = arr[u[u_cnt].pos];
            arr[u[u_cnt].pos] = u[u_cnt].new_val;
        }
    }
    for (int i = u_cnt; i >= 1; i--)
    {
        arr[u[i].pos] = u[i].old_val;
    }
    sort(q + 1, q + q_cnt + 1, cmp);
  
    for (int i = 1; i <= q_cnt; i++)
    {
        int L = q[i].l, R = q[i].r, T = q[i].tim;
        while (cur_t < T) apply(++cur_t, cur_l, cur_r);
        while (cur_t > T) apply(cur_t--, cur_l, cur_r);

        while (cur_l > L) add(arr[--cur_l]);
        while (cur_l < L) del(arr[cur_l++]);
        while (cur_r < R) add(arr[++cur_r]);
        while (cur_r > R) del(arr[cur_r--]);
        ans[q[i].id] = sum;
    }
    
    for (int i = 1; i <= q_cnt; i++)
    {
        cout << ans[i] << endl;
    }

    return 0;
}

```





## 计算几何

### 二维

```c++
#include <bits/stdc++.h>
using namespace std;

const double PI = acos(-1), eps = 1e-9;

int sgn(double x) {
    if (fabs(x) < eps) return 0;
    return x < 0 ? -1 : 1;
}

struct Point{
    double x, y;
    Point(double x = 0.0, double y = 0.0) : x(x), y(y) {}
    
    Point operator- (const Point& b) const {return Point(x - b.x, y - b.y);}

    Point operator+ (const Point& b) const {return Point(x + b.x, y + b.y);}

    double operator% (const Point& b) const {return x * b.y - y * b.x;}

    Point operator* (const double b) const {return Point(x * b, y * b);}

    double operator* (const Point& b) const {return x * b.x + y * b.y;}

    Point operator/ (const double b) const {return Point(x / b, y / b);}

    bool operator< (const Point& b) const {return x < b.x || (x == b.x && y < b.y);}

    bool operator== (const Point& b) const {return sgn(x - b.x) == 0 && sgn(y - b.y) == 0;}

    bool operator!= (const Point& b) const {return sgn(x - b.x) != 0 || sgn(y - b.y) != 0;}
};

double dis(Point a){
    return sqrt(a * a);
}

double ddis(Point a){
    return a * a;
}

double Angle(Point a, Point b){
    return acos(a * b / dis(a) / dis(b));
}
//在直线的哪边
// 点在直线上, 返回 0 (三点共线)
// 点在直线的逆时针方向, 返回 1
// 点在直线的顺时针方向, 返回 -1
//直线ab, 点c
int Cross(Point a, Point b, Point c){
    return sgn((b - a) % (c - a));
}


double cross(Point a, Point b, Point c){
    return (b - a) % (c - a);
}



struct Line{
    Point s, e;
    Line(){}
    Line(Point x, Point y): s(x), e(y) {}
};

int Cross(Point c, Line l){
    Point a = l.s, b = l.e;
    return sgn((b - a) % (c - a));
}
//三点共线
bool In_one_line(Point a, Point b, Point c){
    return !sgn((b - a) % (c - a));
}
//点到直线距离
double dist_ptl(Point p, Point a, Point b){
    Point v1 = p - a, v2 = b - a;
    return fabs((v1 % v2) / dis(v2));
}
//点到线段距离
double dist_pts(Point p, Point a, Point b){
    if(a == b) return dis(p - a);
    Point v1 = b - a, v2 = p - a, v3 = p - b;
    if(sgn(v1 * v2) < 0) return dis(p - a);
    if(sgn(v1 * v3) > 0) return dis(p - b);
    return fabs((v1 % v2) / dis(v1));
}
//点在线段上
bool OnSegment(Point p, Point a, Point b){
    Point pa = a - p, pb = b - p;
    return sgn(pa % pb) == 0 && sgn(pa * pb) <= 0;
}
//直线与线段相交(线段ab,直线cd)
bool Intersect_line_seg(Point a, Point b, Point c, Point d){
    return Cross(a, b, c) * Cross(a, b, d) <= 0;
}
//线段与线段相交
bool Intersect_seg(Point a, Point b, Point c, Point d){
    if (OnSegment(a, c, d) || OnSegment(b, c, d) || OnSegment(c, a, b) || OnSegment(d, a, b)) return 1;
    if (Cross(a, b, c) * Cross(a, b, d) >= 0) return 0;
    if (Cross(c, d, a) * Cross(c, d, b) >= 0) return 0;
    return 1;
}
//线段到线段距离
double dist_sts(Point a, Point b, Point c, Point d){
    if(Intersect_seg(a, b, c, d)){
        return 0.0;
    }
    return min({
        dist_pts(a, c, d),
        dist_pts(b, c, d),
        dist_pts(c, a, b),
        dist_pts(d, a, b),
    });
}
//直线平行
bool Line_parallel(Line A, Line B){
    return sgn((A.s - A.e) % (B.s - B.e)) == 0;
}
//直线交点
Point Inter_Line_Point(Point a, Point b, Point c, Point d){
    Point u = b - a, v = d - c;
    double t = ((a - c) % v) / (v % u);
    return a + u * t;
}

//  ------ 多边形 -------
// 三角形面积
double Triangle_area(Point A, Point B, Point C){
    return fabs((B - A) % (C - A)) / 2;
}
//凸多边形面积
// 因为叉积求得的三角形面积是有向的, 在外面的面积可以正负抵消掉
// 所以能够求任意多边形面积(凸, !凸)
// p[]下标从 0 开始, 长度为 n
double area(vector<Point>& p, int n){
    double S = 0;
    for(int i = 1; i <= n - 2; i++){
        S += (p[i] - p[0]) % (p[i + 1] - p[0]);
    }
    return fabs(S / 2);// 无向面积
    //return S / 2;//有向面积
}

//点在多边形内(扫描线)
// 适用于任意多边形, 不用考虑精度误差和多边形的给出顺序
// 点在多边形边上, 返回 -1
// 点在多边形内, 返回 1
// 点在多边形外, 返回 0

// p[] 的下标从 0 开始, 长度为 n
int InPolygon(Point P, vector<Point>& p, int n) {
    bool flag = false;		
    for (int i = 0, j = n - 1; i < n; j = i++) {
        Point p1 = p[i], p2 = p[j];
        if (OnSegment(P, p1, p2)) return -1;
        if (sgn(P.y - p1.y) > 0 == sgn(P.y - p2.y) > 0) continue;
        if (sgn((P.y - p1.y) * (p1.x - p2.x) / (p1.y - p2.y) + p1.x - P.x) > 0) 
            flag = !flag;
    }
    return flag;
}
//是否为凸多边形
bool Is_contex(vector<Point>& p, int n){
    bool s[3] = {0, 0, 0};
    for (int i = 0, j = n - 1, k = n - 2; i < n; k = j, j = i++) {
        int cnt = sgn((p[i] - p[j]) % (p[k] - p[j])) + 1;
        s[cnt] = true;
        if (s[0] && s[2]) return false;
    }
    return true;
}
// ------ 圆 -------
struct Circle{
    Point o;
    double r;
    Circle(Point o = Point(), double r = 0) : o(o), r(r) {}

    double S(){return PI * r * r;}
    double C(){return PI * 2 * r;}
};
//扇形面积
double SectorArea(Point A, Point B, double R){
    double angle = Angle(A, B);
    if(sgn(A % B) < 0) angle = -angle;
    return R * R * angle / 2;
}
//点和圆的位置关系
// 点在圆上, 返回 0
// 点在圆外, 返回 -1
// 点在圆内, 返回 1
int PWC(Point p, Circle c){
    double d = dis(p - c.o);
    if(sgn(d - c.r) == 0) return 0;
    if(sgn(d - c.r) > 0) return -1;
    return 1;
}
//直线和圆的位置关系
// 相切, 返回 0
// 相交, 返回 1
// 相离, 返回 -1
int LWC(Point A, Point B, Circle c) {
    double d = dist_ptl(c.o, A, B);
    if (sgn(d - c.r) == 0) return 0;
    if (sgn(d - c.r) > 0) return -1;
    return 1;
}

//直线和圆的交点
pair<Point, Point> Intersection_line_circle(Point A, Point B, Circle c) {
    Point AB = B - A;
    Point pr = A + AB * ((c.o - A) * AB / (AB * AB));
    double base = sqrt(c.r * c.r - ((pr - c.o) * (pr - c.o)));
    
    if (sgn(base) == 0) return make_pair(pr, pr);

    Point e = AB / sqrt(AB * AB);
    return make_pair(pr + e * base, pr - e * base);
}
//圆与圆的位置关系
// 相离, 返回 -1
// 外切, 返回 0
// 内切(A 包含 B), 返回 1
// 内切(B 包含 A), 返回 2
// 内含(A 包含 B), 返回 3
// 内含(B 包含 A), 返回 4
// 相交, 返回 5

int Circle_with_circle(Circle A, Circle B) {
    double len1 = dis(A.o - B.o);
    double len2 = A.r + B.r;
    if (sgn(len1 - len2) > 0) return -1;
    if (sgn(len1 - len2) == 0) return 0;
    if (sgn(len1 + len2 - 2 * A.r) == 0) return 1;
    if (sgn(len1 + len2 - 2 * B.r) == 0) return 2;
    if (sgn(len1 + len2 - 2 * A.r) < 0) return 3;
    if (sgn(len1 + len2 - 2 * B.r) < 0) return 4;
    return 5;
}

//圆与圆的交点
// 相交, 返回两点坐标
// 相切, 返回两个一样的相切点

// 要先判断是否相交或相切再调用
pair<Point, Point> Intersection_circle_circle(Circle A, Circle B) {
    Point AB = B.o - A.o;
    double d = dis(AB);
    double a = acos((A.r * A.r + d * d - B.r * B.r) / (2.0 * A.r * d));
    double t = atan2(AB.y, AB.x);
    Point x(A.r * cos(t + a), A.r * sin(t + a));
    Point y(A.r * cos(t - a), A.r * sin(t - a));
    return make_pair(A.o + x, A.o + y);
}
//求圆外一点对圆的两个切点
pair<Point, Point> TangentPoint_point_circle(Point p, Circle c) {
    double d = dis(p - c.o);
    double l = sqrt(d * d - c.r * c.r);
    Point e = (c.o - p) / d;
    double angle = asin(c.r / d);

    Point t1(sin(angle), cos(angle));
    Point t2(sin(-angle), cos(-angle));
    Point e1(e % t1, e * t1);
    Point e2(e % t2, e * t2);
    e1 = e1 * l + p;
    e2 = e2 * l + p;
    return make_pair(e1, e2);
}

//求三角形外接圆
Circle get_circumcircle(Point A, Point B, Point C) {
    double Bx = B.x - A.x, By = B.y - A.y;
    double Cx = C.x - A.x, Cy = C.y - A.y;
    double D = 2 * (Bx * Cy - By * Cx);

    double x = (Cy * (Bx * Bx + By * By) - By * (Cx * Cx + Cy * Cy)) / D + A.x;
    double y = (Bx * (Cx * Cx + Cy * Cy) - Cx * (Bx * Bx + By * By)) / D + A.y;
    Point P(x, y);
    return Circle(P, dis(A - P));
}
//三角形内切圆
Circle get_incircle(Point A, Point B, Point C) {
    double a = dis(B - C);
    double b = dis(A - C);
    double c = dis(A - B);
    Point p = (A * a + B * b + C * c) / (a + b + c);
    return Circle(p, dist_ptl(p, A, B));
}
// 要保证传入的点是整点
//线段上的整点个数
int IntegerPoint_on_seg(Point A, Point B) {
    int x = abs(A.x - B.x);
    int y = abs(A.y - B.y);
    if (x == 0 || y == 0) return 1;
    return __gcd(x, y) + 1;	// 包含端点
    return __gcd(x, y) - 1;	// 不包含端点
}
// 返回多边形边**上**整点的个数
// 点需要是顺时针(逆时针)给出

// p[] 下标从 0 开始, 长度为 n
int IntegerPoint_on_polygon(vector<Point>& p, int n) {
    int res = 0;
    for (int i = 0, j = n - 1; i < n; j = i++) {
        int x = abs(p[i].x - p[j].x);
        int y = abs(p[i].y - p[j].y);
        res += __gcd(x, y);
    }
    return res;
}
// 返回不包括边界的, 多边形**内**整点个数
int IntegerPoint_in_polygon(vector<Point>& p, int n) {
    double A = area(p, n);
    double B = IntegerPoint_on_polygon(p, n);
    return A - B / 2 + 1;
}
//级角排序
void polarSort(vector<Point>& points, const Point& center) {
    sort(points.begin(), points.end(), [&center](const Point& a, const Point& b) {
        double angleA = atan2(a.y - center.y, a.x - center.x);
        double angleB = atan2(b.y - center.y, b.x - center.x);
        if (fabs(angleA - angleB) < 1e-9) { // 极角非常接近时，按距离排序
            double distA = hypot(a.x - center.x, a.y - center.y);
            double distB = hypot(b.x - center.x, b.y - center.y);
            return distA < distB;
        }
        return angleA < angleB;
    });
}

//凸包算法
vector<Point> Andrew(vector<Point>& p, int n){
    sort(p.begin(), p.end());
    vector<Point> ans;
    for(int i = 0; i < n; i++){
        while(ans.size() >= 2 && Cross(ans[ans.size() - 2], ans.back(), p[i]) <= 0){
            ans.pop_back();
        }
        ans.push_back(p[i]);
    }

    int t = ans.size();
    for(int i = n - 2; i >= 0; i--){
        while(ans.size() > t && Cross(ans[ans.size() - 2], ans.back(), p[i]) <= 0){
            ans.pop_back();
        }
        ans.push_back(p[i]);
    }

    if(ans.size() > 1) ans.pop_back();
    return ans;

}




int main(){
    ios::sync_with_stdio(false);
    cin.tie(0), cout.tie(0);

    int n, q;
    cin >> n;
    vector<Point> p(n);
    for(int i = 0; i < n; i++){
        cin >> p[i].x >> p[i].y;
    }

    cin >> q;
    while(q--){
        Line l;
        cin >> l.s.x >> l.s.y >> l.e.x >> l.e.y;
        vector<Point> P;
        for(int i = 0; i < n; i++){
            if(Cross(p[i], l) > 0) P.push_back(p[i]);
        }

        for(int i = 0; i < n; i++){
            if(Intersect_line_seg(p[i], p[(i + 1) % n], l.s, l.e)){
                P.push_back(Inter_Line_Point(p[i], p[(i + 1) % n], l.s, l.e));
            }
        }


        if(P.size() < 3){
            cout << fixed << setprecision(15) << 0.0 << '\n';
            continue;
        }
        double ans = area(P, P.size());
        cout << fixed << setprecision(15) << ans << '\n';
    }
    
    return 0;
}
```



### 三维

```c++
#include <bits/stdc++.h>
using namespace std;

const double PI = acos(-1);

int sgn(double x) {
    if (fabs(x) < 1e-8) return 0;
    return x < 0 ? -1 : 1;
}

struct Point{
    double x, y, z;
    Point(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}

    Point operator+ (const Point& B) const {return Point(x + B.x, y + B.y, z + B.z);}

    Point operator- (const Point& B) const {return Point(x - B.x, y - B.y, z - B.z);}
    //点乘
    double operator* (const Point& B) const {return x * B.x + y * B.y + z * B.z;}
    //叉乘
    Point operator% (const Point& B) const {return Point(y * B.z - z * B.y, z * B.x - x * B.z, x * B.y - y * B.x);}
    //数乘
    Point operator* (const double& B) const {return Point(x * B, y * B, z * B);}
    //数除（
    Point operator/ (const double& B) const {return Point(x / B, y / B, z / B);}
    
    bool operator== (const Point& B) const {return sgn(x - B.x) == 0 && sgn(y - B.y) == 0 && sgn(z - B.z) == 0;}
    
    bool operator!= (const Point& B) const {return sgn(x - B.x) != 0 || sgn(y - B.y) != 0 || sgn(z - B.z) != 0;}


};

double dis(Point& a){
	return sqrt(a * a);
}

double ddis(Point& a){
	return a * a;
}

// 两点间距离
double dis(Point& a, Point& b){
    return sqrt((a - b) * (a - b));
}
//两点间距离平方
double ddis(Point& a, Point& b){
    return (a - b) * (a - b);
}
//向量夹角
double Angle(Point& a, Point& b){
    return acos(a * b / dis(a) / dis(b));
}




int main(){

    return 0;
}

```



## 字符串

### 字典树（trie树）

```c++
#define int long long
using namespace std;
const int N=3e6+10;
const int base=13331;
const int mod=1e9+7;
int n,_T,q;
string s;
int ch[N][70]={0};
int idx;
int cnt[N];

int hashn(char x)
{
  if(x>='A'&&x<='Z')
  {
    return x-'A';
  }
  else if(x>='a'&&x<='z')
  {
    return x-'a'+26;
  }
  else 
  return x-'0'+52;
}

void insert(string s)
{
  int p=0;
  int len=s.length();
  for(int i=0;i<len;i++)
  {
    int j=hashn(s[i]);
    if(!ch[p][j]) ch[p][j]=++idx;
    p=ch[p][j];
    cnt[p]++;
  }
  
}

int query(string s)
{
  int p=0;
  int len=s.length();
  for(int i=0;i<len;i++)
  {
    int j=hashn(s[i]);
    if(!ch[p][j]) return 0;
    p=ch[p][j];
  }
  return cnt[p];
}

void solve()
{
  idx=0;
  cin>>n>>q;
  for(int i=1;i<=n;i++)
  {
    cin>>s;
    insert(s);
  }
  while(q--)
  {
    cin>>s;
    cout<<query(s)<<endl;
  }
  for(int i=0;i<=idx;i++)
  {
    for(int j=0;j<=65;j++)
    {
      ch[i][j]=0;
    }
  }
  for(int i=0;i<=idx;i++)
  {
    cnt[i]=0;
  }
}

signed main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    
    cin>>_T;
    while(_T--)
    {
      solve();
    }

    return 0;
}

```



### AC自动机

```c++
using namespace std;
const int N=(1e4+10)*55;
const int base=13331;
const int M=1e6+10;
int n,k,_n,T,idx;
char s[M];
int cnt[N];
int ne[N];
int ch[N][26];

void insert()
{
    int p=0;
    for(int i=0;s[i];i++)
    {
        int j=s[i]-'a';
        if(!ch[p][j]) ch[p][j]=++idx;
        p=ch[p][j];
    }
    cnt[p]++;
}

void build()
{
    queue<int> q;
    for(int i=0;i<26;i++)
    {
        if(ch[0][i]) q.push(ch[0][i]);
    }
    while(!q.empty())
    {
        int u=q.front();
        q.pop();
        for(int i=0;i<26;i++)
        {
            int v=ch[u][i];
            if(v) ne[v]=ch[ne[u]][i],q.push(v);
            else ch[u][i]=ch[ne[u]][i];
        }
    }
}

int query()
{
    int ans=0;
    for(int k=0,i=0;s[k];k++)
    {
        i=ch[i][s[k]-'a'];
        for(int j=i;j&&~cnt[j];j=ne[j])
        {
            ans+=cnt[j];
            cnt[j]=-1;
        }
    }
    return ans;
}


void solve()
{
    idx=0;
  memset(cnt,0,sizeof cnt);
  memset(ne,0,sizeof ne);
  memset(ch,0,sizeof ch);
  cin>>n;
  for(int i=1;i<=n;i++)
  {
      cin>>s;
      insert();
  }
  build();
  cin>>s;
  cout<<query()<<endl;
}

```



### 马拉车求最长回文串

```c++
void getd()
{
  d[1]=1;
  for(int i=2,l=0,r=1;i<=n;i++)
  {
    if(i<=r) d[i]=min(r-i+1,d[r-i+l]);
    while(s[i-d[i]]==s[i+d[i]]) d[i]++;
    if(i+d[i]-1>r) l=i-d[i]+1,r=i+d[i]-1;
  }
}

```



### KMP

```c++
using namespace std;
const int N=1e6+10;
const int base=13331;
const int mod=1e9+7;
int n,m;
char s1[N],s2[N];
string s,p;
int ne[N];

signed main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
  
    cin>>s>>p;
    int m=s.length(),n=p.length();
    ne[1]=0;
    for(int i=1;i<=m;i++)
    {
      s1[i]=s[i-1];
    }
    for(int i=1;i<=n;i++)
    {
      s2[i]=p[i-1];
    }
    for(int i=2,j=0;i<=n;i++)
    {
      while(j&&s2[i]!=s2[j+1]) j=ne[j];
      if(s2[i]==s2[j+1]) j++;
      ne[i]=j;
    }
    for(int i=1,j=0;i<=m;i++)
    {
      while(j&&s1[i]!=s2[j+1]) j=ne[j];
      if(s1[i]==s2[j+1]) j++;
      if(j==n) 
      {
        cout<<i-n+1<<endl;
        j=ne[j];
      }
    }

    for(int i=1;i<=n;i++)
    {
      cout<<ne[i]<<' ';
    }

    return 0;
}
```



### 字符串常用库函数

```c++
#include <cstring>

char str1[20] = "hello";
char str2[20] = "world";

// 字符串长度
size_t len = strlen(str1);           // 5

// 字符串拷贝
strcpy(str1, "new");                 // str1 = "new"
strncpy(str1, "hello", 3);           // 安全拷贝

// 字符串连接
strcat(str1, " world");              // str1 = "hello world"
strncat(str1, "!!!", 2);             // 安全连接

// 字符串比较
int cmp = strcmp(str1, str2);        // 返回0相等，<0小于，>0大于
int cmp_n = strncmp(str1, str2, 3);  // 比较前n个字符

// 字符串查找
char* pos = strchr(str1, 'l');       // 查找字符第一次出现
char* pos2 = strrchr(str1, 'l');     // 查找字符最后一次出现
char* substr = strstr(str1, "world"); // 查找子串

// 内存操作（常用于字符串）
memset(str1, 0, sizeof(str1));       // 清空
memcpy(str2, str1, len);             // 内存拷贝
memmove(str2, str1, len);            // 安全内存移动


#include <cctype>

char ch = 'A';

// 字符分类
isalpha(ch);    // 是否是字母
isdigit(ch);    // 是否是数字
isalnum(ch);    // 是否是字母或数字
isspace(ch);    // 是否是空白字符
isupper(ch);    // 是否是大写字母
islower(ch);    // 是否是小写字母
ispunct(ch);    // 是否是标点符号

// 字符转换
tolower(ch);    // 转小写 → 'a'
toupper(ch);    // 转大写 → 'A'

// 实际应用：字符串处理
string s = "Hello World 123!";
for (char& c : s) {
    if (isupper(c)) c = tolower(c);  // 转小写
}


#include <string>
using namespace std;

string s = "hello world";

// 查找操作
size_t pos = s.find("world");        // 查找子串，返回位置
pos = s.find('o');                   // 查找字符
pos = s.rfind('o');                  // 从后往前查找
pos = s.find_first_of("aeiou");      // 查找任何匹配字符
pos = s.find_last_of("aeiou");       // 从后往前查找任何匹配字符
pos = s.find_first_not_of("helo ");  // 查找第一个不匹配字符

// 子串操作
string sub = s.substr(6, 5);         // 从位置6开始取5个字符 → "world"
s.substr(6);                         // 从位置6到结尾 → "world"

// 修改操作
s.insert(5, " beautiful");           // 插入 → "hello beautiful world"
s.erase(5, 10);                      // 删除 → "hello world"
s.replace(6, 5, "everyone");         // 替换 → "hello everyone"

// 追加和连接
s.append("!!!");                     // 追加 → "hello world!!!"
s += "!!!";                          // 同样效果
s.push_back('!');                    // 追加单个字符

// 比较
int result = s.compare("hello");     // 比较字符串
bool equal = (s == "hello world");   // 直接比较

// 数值转换
string num_str = to_string(123);     // 数字转字符串 → "123"
int num = stoi("456");               // 字符串转int
double d = stod("3.14");             // 字符串转double
long l = stol("1000000");            // 字符串转long



#include <sstream>
#include <string>
using namespace std;

// 字符串分割
string s = "apple,banana,orange";
stringstream ss(s);
string token;
while (getline(ss, token, ',')) {
    cout << token << endl;  // 输出: apple banana orange
}

// 数字和字符串转换
stringstream ss2;
ss2 << "年龄: " << 25 << " 分数: " << 95.5;
string result = ss2.str();  // "年龄: 25 分数: 95.5"

// 从字符串提取数字
string data = "100 200 300";
stringstream ss3(data);
int a, b, c;
ss3 >> a >> b >> c;  // a=100, b=200, c=300



#include <bits/stdc++.h>
using namespace std;

// 快速字符串处理函数
void string_demo() {
    string s = "Hello World";
    
    // 1. 删除所有空格
    s.erase(remove(s.begin(), s.end(), ' '), s.end());
    cout << s << endl;  // "HelloWorld"
    
    // 2. 字符串转小写
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    cout << s << endl;  // "helloworld"
    
    // 3. 字符串分割（简单版）
    string data = "apple,banana,orange";
    replace(data.begin(), data.end(), ',', ' ');
    stringstream ss(data);
    string token;
    while (ss >> token) {
        cout << token << endl;
    }
    
    // 4. 检查回文
    string palindrome = "racecar";
    bool is_pal = equal(palindrome.begin(), palindrome.end(), palindrome.rbegin());
    cout << (is_pal ? "是回文" : "不是回文") << endl;
    
    // 5. 字符串反转
    reverse(s.begin(), s.end());
    cout << s << endl;  // "dlrowolleh"
}


#include <string>
#include <algorithm>

// 自定义便捷函数
inline string to_lower(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

inline string to_upper(string s) {
    transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}

inline string trim(const string& s) {
    auto start = s.find_first_not_of(" \t\n\r");
    auto end = s.find_last_not_of(" \t\n\r");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

inline vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        if (!token.empty()) tokens.push_back(token);
    }
    return tokens;
}
```





## 数学

### 线性基

```c++
#include <bits/stdc++.h>
using namespace std;
#define int long long

//线性基LinearBasis
vector<int> p(64);
void insert(int x){
    for(int i = 63; i >= 0; i--){
        if(!(x >> i)) continue;
        if(!p[i]){
            p[i] = x;
            break;
        }
        x ^= p[i];
    }
}

//矩阵
struct mat{
    int n, m;
    vector<vector<int>> a;
    mat(int x = 0, int y = 0){
        n = x;
        m = y;
        a.resize(x, vector<int> (y));
    }

    mat operator+ (const mat& b){
        mat c(n, m);
        for(int i = 0; i < n; i++){
            for(int j = 0; j < m; j++){
                c.a[i][j] = a[i][j] + b.a[i][j];
            }
        }
        return c;
    } 

    mat operator- (const mat& b){
        mat c(n, m);
        for(int i = 0; i < n; i++){
            for(int j = 0; j < m; j++){
                c.a[i][j] = a[i][j] - b.a[i][j];
            }
        }
        return c;
    } 

    mat operator* (const mat& b){
        mat c(n, b.m);
        for(int i = 0; i < n; i++){
            for(int j = 0; j < b.m; j++){
                for(int k = 0; k < m; k++){
                    c.a[i][j] += a[i][k] * b.a[k][j];
                }
            }
        }
        return c;
    } 

    mat transpose(){
        mat res(m, n);
        for(int i = 0; i < n; i++){
            for(int j = 0; j < m; j++){
                res.a[j][i] = a[i][j];
            }
        }
        return res;
    }

    void print(){
        for(int i = 0; i < n; i++){
            for(int j = 0; j < m; j++){
                cout << a[i][j] << " ";
            }
            cout << '\n';
        }
    }

};
```



### 数论杂项

```c++
#include <bits/stdc++.h>
using namespace std;
#define int long long
const int MOD = 1e9 + 7;

int ksm(int x, int a, int m){
    int ans = 1;
    while(a){
        if(a & 1) ans = ans * x % m;
        x = x * x % m;
        a >>= 1;
    }
    return ans;
}


//扩展欧几里得算法
int exgcd(int a, int b, int &x, int &y){
    if(!b){
        x = 1;
        y = 0;
    }
    int d = exgcd(b, a % b, x, y);
    int t = x;
    x = y;
    y = t - (a / b) * y;
    return d;//最大公约数
}

//中国剩余定理(x === a (mod r) )
int CRT(int k, vector<int> a, vector<int> r){
    int n = 1, ans = 0;
    for(int i = 0; i < k; i++){
        n = n * r[i];
    }
    for(int i = 0; i < k; i++){
        int m = n / r[i], b, y;
        exgcd(m, r[i], b, y);
        ans = (ans + a[i] * m * b % n) % n;
    }
    return (ans % n + n) % n;
}

//扩展中国剩余定理
//求解方程组 x ≡ a[i] (mod m[i])
int excrt(vector<int> m, vector<int> a, int n){
    int M = m[0];
    int ans =  a[0];
    for(int i = 1; i < n; i++){
        int c = (a[i] - ans % m[i] + m[i]) % m[i];
        int x, y;
        int d = exgcd(M, m[i], x, y);
        if(c % d != 1) return -1;
        int k = m[i] % d;
        x = ksm(x, c / d, k);
        ans += x * M;
        M *= k;
        ans = (ans % M + M) % M;
    }
    return ans;
}

//一个数的欧拉函数
int oula_phi(int n){
    int ans = n;
    for(int i = 2; i * i <= n; i++){
        if(n % i == 0){
            ans = ans / i * (i - 1);
            while(n % i == 0) n /= i;
        }
        if(n > 1) ans = ans / n * (n - 1);
        return ans;
    }
}

//筛法求欧拉函数
vector<int> sieve_phi(int n) {
    vector<int> phi(n + 1);
    vector<bool> st(n + 1, false);
    vector<int> primes;
    
    phi[1] = 1; // 特殊情况：1的欧拉函数值为1[1,3,5](@ref)
    
    for (int i = 2; i <= n; i++) {
        if (!st[i]) { // i是质数
            primes.push_back(i);
            phi[i] = i - 1; // 质数的欧拉函数值为i-1[1,2](@ref)
        }
        
        for (int j = 0; primes[j] <= n / i; j++) {
            st[primes[j] * i] = true;
            
            if (i % primes[j] == 0) {
                // primes[j]是i的最小质因子
                phi[primes[j] * i] = phi[i] * primes[j]; // 情况1[2,5](@ref)
                break;
            } else {
                // primes[j]与i互质
                phi[primes[j] * i] = phi[i] * (primes[j] - 1); // 情况2[1,3](@ref)
            }
        }
    }
    
    return phi;
}

// 狄利克雷卷积快速幂 - 计算f的k次卷积幂
void dirichlet_power(const vector<int>& f, vector<int>& res, int n, int k) {
    // 初始化结果为狄利克雷卷积的单位元ε
    res.assign(n + 1, 0);
    res[1] = 1;  // ε(1)=1, ε(n)=0(n>1)
    
    vector<int> temp(f), base(f);
    
    while (k) {
        if (k & 1) {
            vector<int> new_res(n + 1, 0);
            // 计算res = res * base
            for (int i = 1; i <= n; i++) {
                for (int j = 1; i * j <= n; j++) {
                    new_res[i * j] = (new_res[i * j] + res[i] * base[j]) % MOD;
                }
            }
            res = new_res;
        }
        
        // 计算base = base * base
        vector<int> new_base(n + 1, 0);
        for (int i = 1; i <= n; i++) {
            for (int j = 1; i * j <= n; j++) {
                new_base[i * j] = (new_base[i * j] + base[i] * base[j]) % MOD;
            }
        }
        base = new_base;
        
        k >>= 1;
    }
}
//SOSDP
int main() {
    int n = 50;
    int total_states = (1 << n);
    vector<int> dp(total_states, 0); // dp数组初始化，根据题意可能初始为A[i]

    // SOS DP 计算超集和
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < total_states; ++j) {
            if ((j >> i) & 1) { // 如果状态j的第i位是1
                dp[j] += dp[j ^ (1 << i)]; // 累加不含第i位的状态的值
            }
        }
    }

    // 此时dp[mask]存储了所有满足 (i & mask) == mask 的A[i]之和
    return 0;
}
```



```c++
#include <bits/stdc++.h>
using namespace std;
#define int long long

//欧拉筛
const int mod = 1e9 + 7;
const int maxn = 2e5;
vector<int> primes;

void init(){
    vector<bool> nop(maxn + 5);
    nop[0] = nop[1] = true;
    for(int i = 2; i <= maxn; i++){
        if(!nop[i]){
            primes.push_back(i);
            if(i > sqrt(maxn)) continue;
            for(int j = i * i; j <= maxn; j++){
                nop[j] = true;
            }
        }
    }
}

//模逆元 排列数
vector<int> fac(maxn + 5), inv(maxn + 5);
int ksm(int x, int a, int mod){
    int ans = 1;
    while(a){
        if(a & 1) ans = ans * x % mod;
        x = x * x % mod;
        a >>= 1;
    }
    return ans;
}

void init(){
    fac[0] = 1;
    for(int i = 1; i <= maxn; i++){
        fac[i] = fac[i - 1] * i % mod;
    }
    inv[maxn] = ksm(inv[maxn], mod - 2, mod);
    for(int i = maxn - 1; i >= 0; i--){
        inv[i] = inv[i + 1] * (i + 1);
    }
}

int A(int n, int m){
    return fac[n] * inv[m] % mod;
}

int C(int n, int m){
    return fac[n] * inv[m] % mod * inv[n - m] % mod;
}

//错位排列Derangement
int Derangement(int n){
    vector<int> dp(n + 1);
    dp[1] = 0;
    dp[2] = 1;
    for(int i = 3; i <= n; i++){
        dp[i] = (n - 1) * (dp[i - 1] + dp[i - 2]) % mod;
    }
}

//卡特兰数CatalanNumber
int CatalanNumber(int n){
    int ans = C(2 * n, n) - C(2 * n, n + 1);
    return (ans % mod + mod) % mod;
}
```



## 其他杂项

### 结构体内置比较器

```c++
struct node{
    int a,b,c,d;

    bool operator<(const node& other) const {
        if (a != other.a) return a < other.a;
        if (b != other.b) return b < other.b;
        if (c != other.c) return c < other.c;
        return d < other.d;
    }
};
```



### STL库函数

```c++
#include <numeric>
#include <algorithm>
#include <cmath>

// 最大公约数
__gcd(a, b);        // GNU扩展，最快
gcd(a, b);          // C++17标准

// 最小公倍数
lcm(a, b);          // C++17

// 累积计算
accumulate(v.begin(), v.end(), 0);                          // 求和
accumulate(v.begin(), v.end(), 1, multiplies<int>());       // 求积
partial_sum(v.begin(), v.end(), result.begin());            // 前缀和
adjacent_difference(v.begin(), v.end(), result.begin());    // 差分

// 其他数学
__builtin_popcount(x);      // 二进制中1的个数
abs(x); pow(x, y); sqrt(x); // 绝对值、幂、平方根


#include <algorithm>

// 二分查找（要求有序）
lower_bound(v.begin(), v.end(), x);     // 第一个≥x的位置
upper_bound(v.begin(), v.end(), x);     // 第一个>x的位置
binary_search(v.begin(), v.end(), x);   // 是否存在x
equal_range(v.begin(), v.end(), x);     // 返回[x,x]的区间

// 统计
count(v.begin(), v.end(), x);           // 统计x出现次数
count_if(v.begin(), v.end(), pred);     // 统计满足条件的元素

// 最值
max_element(v.begin(), v.end());        // 最大元素位置
min_element(v.begin(), v.end());        // 最小元素位置


// 填充
fill(v.begin(), v.end(), value);        // 填充值
iota(v.begin(), v.end(), start);        // 填充递增序列

// 变换
transform(v.begin(), v.end(), result.begin(), func); // 对每个元素应用函数

// 交换
swap(a, b);                             // 交换两个值
iter_swap(it1, it2);                    // 交换迭代器指向的值

// 反转
reverse(v.begin(), v.end());            // 反转序列



// 下一个排列（字典序）
next_permutation(v.begin(), v.end());   // 变为下一个排列
prev_permutation(v.begin(), v.end());   // 变为上一个排列

// 使用示例：生成所有排列
sort(v.begin(), v.end());
do {
    // 处理当前排列
} while (next_permutation(v.begin(), v.end()));



// 集合运算（要求有序）
set_union(a.begin(), a.end(), b.begin(), b.end(), result.begin());      // 并集
set_intersection(a.begin(), a.end(), b.begin(), b.end(), result.begin()); // 交集
set_difference(a.begin(), a.end(), b.begin(), b.end(), result.begin()); // 差集
includes(a.begin(), a.end(), b.begin(), b.end());                       // 包含关系



#include <bitset>
#include <bit>  // C++20

bitset<32> bs(x);           // 位集操作
__builtin_clz(x);           // 前导0个数
__builtin_ctz(x);           // 后缀0个数
__builtin_ffs(x);           // 最低位1的位置
popcount(x);                // C++20, 1的个数



ios::sync_with_stdio(false);
cin.tie(0);
cout.tie(0);
```



### 迭代器操作

```c++
#include <iterator>

// 前进n步（不改变原迭代器）
auto it2 = next(it, n);      // 返回it后面第n个位置的迭代器
auto it2 = next(it);         // 相当于next(it, 1)

// 后退n步（不改变原迭代器）
auto it2 = prev(it, n);      // 返回it前面第n个位置的迭代器
auto it2 = prev(it);         // 相当于prev(it, 1)

// 前进n步（改变原迭代器）
advance(it, n);              // 将it前进n步


#include <iterator>

// 计算两个迭代器之间的距离
int dist = distance(it1, it2);

// 获取迭代器在容器中的位置索引
int pos = distance(container.begin(), it);



#include <vector>
#include <iostream>
using namespace std;

int main() {
    vector<int> v = {1, 2, 3, 4, 5};
    
    // 正向迭代器转反向迭代器
    auto it = v.begin() + 2;  // 指向3
    auto rit = reverse_iterator<vector<int>::iterator>(it);
    cout << *rit << endl;     // 输出: 2（指向前一个元素）
    
    // 反向迭代器转正向迭代器
    auto rit2 = v.rbegin() + 1;  // 指向4
    auto it2 = rit2.base();      // 指向5
    cout << *it2 << endl;        // 输出: 5
    
    return 0;
}


```

