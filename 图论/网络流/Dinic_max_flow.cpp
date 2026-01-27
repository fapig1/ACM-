#include<bits/stdc++.h>
using namespace std;
#define endl '\n'
#define int long long
const int INF=1e18;
const double eps=1e-6;
const double PI=acos(-1);
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

signed main()
{
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    int _t=1;
    //cin>>_t;
    while (_t--) solve();

    return 0;
}
