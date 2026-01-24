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