#include<bits/stdc++.h>
using namespace std;
#define int long long

// 【速查】整数三分（单峰函数求极值）
// 用途: cal(x) 在 [l, r] 上先增后减（单峰），求最大值；求最小值把 v1>=v2 的比较反过来、max 改 min
// 用法: 改 cal 和初始 l、r；区间缩到 ≤3 后暴力枚举
// 注意: 有平台（一段相等）时三分会失效
// 复杂度: O(log r · 单次 cal)
int cal(int num){return 0;}
void solve(){
    int l=0,r=2e18;
    while(r-l>2){
        int m1=l+(r-l)/3;
        int m2=r-(r-l)/3;

        int v1=cal(m1);
        int v2=cal(m2);

        //求极大值，改为极小值修改此处符号
        if(v1>=v2){
            r=m2;
        }else{
            l=m1;
        }
    }
    int ans=0;
    for(int j=l;j<=r;j++){
        int cur=cal(j);
        ans=max(ans,cur);
    }
    cout<<ans<<"\n";
}

signed main()
{
    ios::sync_with_stdio(false);
    cin.tie(0);

    int t=1;
    cin>>t;
    while(t--)solve();

    return 0;
}
