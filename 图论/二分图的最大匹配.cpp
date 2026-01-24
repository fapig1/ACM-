#include <bits/stdc++.h>
#define int long long
#define endl '\n'
using namespace std;
const int INF=1e13;
const double eps=1e-6;
const double PI=acos(-1);
const int N=510;
const int M=110;
const int MOD=998244353;
int n1,n2,m;
int st[N];
int match[N];
vector<int> arr[N];
//匈牙利算法：二分图的最大匹配
bool find(int x)
{
    for(auto y:arr[x])
    {
        if(!st[y])
        {
            st[y]=1;
            if(match[y]==0||find(match[y]))
            {
                match[y]=x;
                return true;
            }
        }
    }
    return false;
}

void solve() 
{
    cin>>n1>>n2>>m;
    for(int i=1;i<=m;i++)
    {
        int u,v;
        cin>>u>>v;
        //因为只从一边找另一边,所以只用存一个方向
        arr[u].push_back(v);
    }
    int res=0;
    for(int i=1;i<=n1;i++)
    {
        memset(st,0,sizeof st);
        if(find(i)) res++;
    }
    cout<<res<<endl;
}

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
