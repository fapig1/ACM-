#include<bits/stdc++.h>
using namespace std;
#define int long long

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

main()
{
    ios::sync_with_stdio(false);
    cin.tie(0);

    int t=1;
    cin>>t;
    while(t--)solve();

    return 0;
}
