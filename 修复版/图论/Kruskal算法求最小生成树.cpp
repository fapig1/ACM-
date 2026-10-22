#include <bits/stdc++.h>
#define int long long
#define endl '\n'
using namespace std;

const int N=2e5+10;
// 【速查】Kruskal 最小生成树
// 用法: Kruskal K;（放全局）  K.n = n; K.m = m;  读 K.e[1..m] = {a, b, w};  K.run()
// 结果: K.res = MST 权值和；K.cnt < n-1 说明图不连通（输出 impossible）
// 复杂度: O(m log m)，稀疏图首选
// 10.22: 仅把原来的全局变量 / 函数放进结构体（main 里的建树循环挪成 run），算法代码未改
//Kruskal算法求最小生成树
struct edge
{
  int a,b,w;
  bool operator<(edge& x)
  {
    return w<x.w;
  }
};

struct Kruskal{
  int n,m,res,cnt;
  int fa[N];
  edge e[N];

  int find(int u)
  {
    if(u!=fa[u])
    {
      fa[u]=find(fa[u]);
    }
    return fa[u];
  }

  void run()
  {
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
  }
};

Kruskal K;

signed main()
{
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    cin>>K.n>>K.m;
    for(int i=1;i<=K.m;i++)
    {
      cin>>K.e[i].a>>K.e[i].b>>K.e[i].w;
    }
    K.run();
    if(K.cnt<K.n-1)
    {
      cout<<"impossible"<<endl;
    }
    else  cout<<K.res<<endl;


    return 0;
}
