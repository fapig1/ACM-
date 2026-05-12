#include <bits/stdc++.h>
#define int long long
#define endl '\n'
using namespace std;

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


signed main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    int _t=1;
    //cin>>_t;
    while(_t--)
    {
        solve();
    }

    return 0;
}
