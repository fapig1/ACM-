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