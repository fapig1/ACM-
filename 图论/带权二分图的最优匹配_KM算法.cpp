#include<bits/stdc++.h>
using namespace std;
#define int long long

const int inf=0x7fffffff;
const int MAXN=505;

int cnta,cntb;  //左部数量，右部数量
int w[MAXN][MAXN];//边权，从左到右

//以下套用板子不用更改
int visa[MAXN],visb[MAXN];//访问标记
int la[MAXN],lb[MAXN];//顶标
int match[MAXN],upd[MAXN];//匹配点、更新
int visnum=0;
bool check(int x) {//匈牙利
    visa[x]=visnum;
    for(int y=1; y<=cnta; y++){
        if(w[x][y]!=-inf){
            if(visb[y]<visnum)
            if(la[x]+lb[y]==w[x][y]) {
                visb[y]=visnum;
                if(!match[y]||check(match[y])) {
                    match[y]=x;
                    return true;
                }
            } else upd[y]=min(upd[y],la[x]+lb[y]-w[x][y]);
        }
    }

    return false;
}

int KM() {//KM板子
    memset(match,0,sizeof(match));
    for(int i=1; i<=cnta; i++) {
        la[i]=-inf;
        lb[i]=0;
        for(int j=1; j<=cntb; j++){
            la[i]=max(la[i],w[i][j]);
        }
    }
    for(int i=1; i<=cnta; i++){
        while(true) {
            visnum++;
            for(int j=1; j<=cntb; j++){
                upd[j]=inf;
            }
            if(check(i))break;
            int delta=inf;
            for(int j=1; j<=cntb; j++){
                if(visb[j]<visnum)delta=min(delta,upd[j]);
            }
            for(int j=1; j<=cntb; j++) {
                if(visa[j]==visnum)la[j]-=delta;
                if(visb[j]==visnum)lb[j]+=delta;
            }
        }
    }
    int ans=0;
    for(int i=1; i<=cntb; i++){
        ans+=w[match[i]][i];
    }
    return ans;
}

void solve(){
    cin>>cnta;
    cntb=cnta;
    int sum=0;
    for(int i=1;i<=cnta;i++){
        for(int j=1;j<=cntb;j++)
        {
            int in;
            cin>>in;
            w[i][j]=in;
            sum+=in;
        }
    }
    int det=KM();
    cout<<sum-det<<"\n";
}

main()
{
    ios::sync_with_stdio(false);
    cin.tie(0);

    int t=1;
    //cin>>t;
    while(t--)solve();

    return 0;
}
