#include <bits/stdc++.h>
#define int long long
#define endl '\n'
using namespace std;

const int MAXN=505;     //右部大小
// 【速查】匈牙利算法（二分图最大匹配）
// 用法: Hungarian H;（放全局）  左部 1..n1，右部 1..n2；H.vec.assign(n1+1, {}); H.state.assign(n2+1, 0); H.vis.assign(n2+1, 0);
//       H.vec[u].push_back(v) 只建左→右；每个左点 H.visnum++ 后调 H.check(i)，成功 ans++
// 结果: ans = 最大匹配数；H.state[v] = 与右点 v 匹配的左点
// 常用: 最小点覆盖 = 最大匹配；最大独立集 = 点数 - 最大匹配；DAG 最小路径覆盖 = n - 最大匹配
// 复杂度: O(n·m)；规模大（1e5 级）改用 Dinic
// 10.22: 仅把原来的全局变量 / 函数放进结构体，算法代码未改
struct Hungarian{
    vector<int> state;      //右部已被左部匹配的状态
    vector<int> vis=vector<int>(MAXN);   //右部的vis数组，用来判环
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
};

Hungarian H;

void solve()
{
    int n1,n2,m;
    //此时左部和右部已经被区分过，如果没有被区分过先进行染色dfs操作
    cin>>n1>>n2>>m;
    H.vec.assign(n1+1,vector<int>());
    H.state.assign(n2+1,0);
    H.vis.assign(n2+1,0);   // 右部大小不再受 MAXN 限制
    for(int i=1;i<=m;i++)
    {
        int u,v;
        cin>>u>>v;
        //因为只从一边找另一边,所以只用存一个方向，即只建左部指向右部的边
        H.vec[u].push_back(v);
    }

    int ans=0;
    for(int i=1;i<=n1;i++){
        H.visnum++;
        if(H.check(i))ans++;
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
