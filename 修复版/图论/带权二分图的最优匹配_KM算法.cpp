#include<bits/stdc++.h>
using namespace std;
#define int long long

const int inf=0x7fffffff;
const int MAXN=505;

int cnta,cntb;  //左部数量，右部数量
int w[MAXN][MAXN];//边权，从左到右

// 【速查】KM 算法（二分图最大权完美匹配，BFS 版）
// 用法: cnta = cntb = n；w[i][j] = 左 i 连右 j 的权（无边填 -inf）；ans = KM()
// 返回: 最大权和；match[j] = 与右点 j 匹配的左点；返回值 < -5e14 表示不存在完美匹配
// 最小权: 权取负，答案取 -KM()
// 复杂度: O(n³)，n ≤ 500 没问题
//以下套用板子不用更改
// BFS 版 KM，O(n^3)。10.22 修复: 原版是 DFS 版，每次调整顶标后从头重新 DFS，
//   最坏 O(n^4)：n=500、w[i][j]=i*j 实测 10s（P6577 这类卡 DFS-KM 的数据必 TLE）。
// 约定: 1-based；左右部规模相等(cnta==cntb)；求最大权完美匹配。
//   w[x][y] == -inf 表示无边，内部按极小权 NEG 处理；
//   返回值 < NEG/2 说明不存在完美匹配（被迫用了"无边"）。
// 原理: 维护顶标 la[x]+lb[y] >= w[x][y]，只走相等边。每轮从一个左部点出发，
//   像 Dijkstra 一样用 slack 记录未访问右部点的最小差值，每步把整棵交错树的顶标
//   平移 d（树内相等边不变，树外 slack 减 d），O(n) 步找到增广路，总 O(n^3)。
int la[MAXN],lb[MAXN];//顶标
int match[MAXN];//match[y]: 与右部点 y 匹配的左部点（0 表示未匹配）
int slk[MAXN],pre[MAXN];//slack 与交错树上的前驱右部点
bool visb[MAXN];

int KM() {//KM板子
    const int NEG=-(int)1e15, BIG=(int)4e18;
    int n=cnta;
    auto W=[&](int x,int y){ return w[x][y]==-inf ? NEG : w[x][y]; };
    for(int i=1; i<=n; i++){
        la[i]=NEG;
        for(int j=1; j<=n; j++) la[i]=max(la[i],W(i,j));
    }
    for(int j=0; j<=n; j++){ lb[j]=0; match[j]=0; }
    for(int s=1; s<=n; s++){
        for(int j=0; j<=n; j++){ visb[j]=false; slk[j]=BIG; pre[j]=0; }
        int y=0;
        match[0]=s;                      // 右部虚点 0 挂在起点 s 上
        do{
            visb[y]=true;
            int x=match[y], ny=0, d=BIG;
            for(int j=1; j<=n; j++){
                if(visb[j]) continue;
                int t=la[x]+lb[j]-W(x,j);
                if(t<slk[j]){ slk[j]=t; pre[j]=y; }
                if(slk[j]<d){ d=slk[j]; ny=j; }
            }
            for(int j=0; j<=n; j++){
                if(visb[j]){ la[match[j]]-=d; lb[j]+=d; }
                else slk[j]-=d;
            }
            y=ny;
        }while(match[y]);                // 走到未匹配的右部点即找到增广路
        while(y){ match[y]=match[pre[y]]; y=pre[y]; }   // 沿前驱翻转
    }
    int ans=0;
    for(int i=1; i<=n; i++){
        ans+=W(match[i],i);
    }
    return ans;
}

void solve(){
    cin>>cnta;
    cntb=cnta;
    for(int i=1;i<=cnta;i++){
        for(int j=1;j<=cntb;j++)
        {
            int in;
            cin>>in;
            w[i][j]=in;
        }
    }
    int det=KM();
    /*
        如果要求完美匹配的最小权，将边权取负数w[i][j]=-w[i][j]
        最后答案输出-KM()即可
    */
    cout<<det<<"\n";
}

signed main()
{
    ios::sync_with_stdio(false);
    cin.tie(0);

    int t=1;
    //cin>>t;
    while(t--)solve();

    return 0;
}
