#include <bits/stdc++.h>
#define int long long
using namespace std;
int mod=1e9+7;

//https://www.luogu.com.cn/problem/P6047 丝之割
//由于dp的形式千变万化，这里仅供思路参考
struct pt{
    int x,y;
    bool operator<(const pt&o){
        if(x==o.x){
            return y>o.y;
        }else{
            return x<o.x;
        }
    }
};

//点p1,p2的斜率是否大于k
bool check(pt p1,pt p2,int k){
    return (p2.y-p1.y)>=k*(p2.x-p1.x);
}

//点p1,p2的斜率是否大于p2,p3
bool check(pt p1,pt p2,pt p3){
    return (p2.y-p1.y)*(p3.x-p2.x)>=(p3.y-p2.y)*(p2.x-p1.x);
}

void solve(){
    int n,m;
    cin>>n>>m;
    vector<int> veca(n+1);
    vector<int> vecb(n+1);
    vector<int> mina(n+1,INT_MAX);
    vector<int> minb(n+2,INT_MAX);
    for(int i=1;i<=n;i++){
        cin>>veca[i];
        mina[i]=min(mina[i-1],veca[i]);
    }
    for(int i=1;i<=n;i++){
        cin>>vecb[i];
    }
    for(int i=n;i>=0;i--){
        minb[i]=min(minb[i+1],vecb[i]);
    }
    vector<pt> p(m); 
    for(int i=0;i<m;i++){
        cin>>p[i].x>>p[i].y;
    }
    sort(p.begin(),p.end());
    vector<pt> tar;
    for(int i=0;i<m;i++){
        if(tar.empty()||(tar.back().y<p[i].y&&tar.back().x<p[i].x)){
            tar.push_back(p[i]);
        }
    }
    //预处理出每个点左侧最小值，右侧最小值，以及去除交叉线段

    //dp[i]=dp[j]+minb(i+1)*mina(j)
    //因为minb(i+1)已知且单调不减，使用斜率优化DP
    //设b=y-k*x   b=dp[i]    y=dp[j]    k=minb(i+1)  x=-mina(j)
    //得到的新点{b,-mina(i)}插入单调队列中，保证单调队列中两点间斜率单调递增
    //使用一个cur维护当前符合要求的点（要么是终点，要么cur与cur+1两点间斜率刚好是单调队列中比k稍大的点）
    //时间复杂度O(n)
    //以下部分可以作为模板参考，替换其中的x,y,k,b和check的计算方式，只要保证k单调就可以应用
    int len=tar.size();
    vector<pt> dp;
    int cur=0;
    dp.push_back({-mina[tar[0].x-1],0});
    for(int i=0;i<tar.size();i++){
        int k=minb[tar[i].y+1];
        while(cur+1<dp.size()&&!check(dp[cur],dp[cur+1],k)){
            cur++;      
        }

        int x=dp[cur].x;
        int y=dp[cur].y;
        int b=y-x*k;

        int nx=0;
        if(i<tar.size()-1)nx=-mina[tar[i+1].x-1];   //防止末尾越界

        while(dp.size()>=2&&check(dp[dp.size()-2],dp[dp.size()-1],{nx,b})){
            dp.pop_back();
        }
        while(cur>=dp.size())cur--;

        dp.push_back({nx,b});
    }
    cout<<dp.back().y<<"\n";
    
}

main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    int t=1;
    while(t--)solve();
    return 0;
}