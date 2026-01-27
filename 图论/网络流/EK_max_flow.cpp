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
