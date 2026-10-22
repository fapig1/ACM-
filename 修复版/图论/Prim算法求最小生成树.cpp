#include <bits/stdc++.h>
#define int long long   // 恢复: 边权和可超 32 位 int
#define endl '\n'
using namespace std;

const int N=510;
const int INF=0x3f3f3f3f3f3f3f3fLL;   // 与 memset 0x3f 在 long long 下的值一致
// 【速查】朴素 Prim 最小生成树（稠密图）
// 用法: Prim P;（放全局）  P.n = n;  P.g 先 memset 0x3f，P.g[a][b] = P.g[b][a] = min(P.g[a][b], w)；P.prim()
// 返回: MST 权值和；图不连通返回 INF
// 复杂度: O(n²)，n ≤ 几千的稠密图
// 10.22: 仅把原来的全局变量 / 函数放进结构体，算法代码未改
struct Prim{
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
      if(dist[t]>=INF)   // 10.22 修复: 原版和 0x3f3f3f3f 比较，long long 下永远不等，不连通时输出乱码
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
};

Prim P;

signed main()
{
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    cin>>P.n>>P.m;
    memset(P.g,0x3f,sizeof P.g);
    for(int i=1;i<=P.m;i++)
    {
      int a,b,w;
      cin>>a>>b>>w;
      P.g[a][b]=P.g[b][a]=min(P.g[a][b],w);
    }
    int t=P.prim();
    if(t==INF)
    {
      cout<<"impossible"<<endl;
    }
    else
    cout<<t<<endl;

    return 0;
}
