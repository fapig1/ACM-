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