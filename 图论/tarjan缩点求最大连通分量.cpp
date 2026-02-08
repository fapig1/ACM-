#include <bits/stdc++.h>
#define int long long
using namespace std;
int mod=1e9+7;
int MAXN=5e5+5;
vector<vector<int>> gra(MAXN);	//原图邻接表
vector<int> val(MAXN);			//原图点权
vector<int> dfn(MAXN);			//原图dfn序，用于构建scc
vector<int> low(MAXN);			//节点的scc根节点，用于构建scc
stack<int> stk;					//栈，用于构建scc
vector<int> instk(MAXN);		//节点是否入栈，用于构建scc
vector<int> itscc(MAXN);		//ind to scc 原图点角标转新图点角标
vector<int> sccval(MAXN);		//新图点权
vector<vector<int>> sccgra(MAXN);	//新图邻接表
int cur=1;						//维护原图dfn值
int curscc=0;					//新图大小

int n,m;
vector<pair<int,int>> edge(MAXN);
void tarjan(int ind){
    dfn[ind]=cur;
    low[ind]=cur;
    cur++;
    instk[ind]=1;
    stk.push(ind);
    for(int i:gra[ind]){
        //无向图这里加 if(par[i]==ind)continue;
        if(!dfn[i]){
            tarjan(i);
            low[ind]=min(low[ind],low[i]);
        }else if(instk[i]){
            low[ind]=min(low[ind],dfn[i]);
        }
    }

    if(low[ind]==dfn[ind]){
        int cval=0;
        int ctar=0;
        int c;
        do{
            c=stk.top();
            stk.pop();
            instk[c]=0;
            itscc[c]=curscc;
            sccval[curscc]+=val[c];
        }while(c!=ind);
        curscc++;
    }
}

void build(){
    //第一步求解scc
    for(int i=1;i<=n;i++){
        if(!dfn[i])
        tarjan(i);
    }
    //第二步建新图
    for(int i=0;i<m;i++){
        int a=edge[i].first;
        int b=edge[i].second;
        if(itscc[a]!=itscc[b]){
            sccgra[itscc[a]].push_back(itscc[b]);
        }
    }
}

void solve(){
    cin>>n>>m;
    for(int i=0;i<m;i++){
        int a,b;
        cin>>a>>b;
        gra[a].push_back(b);
        edge.push_back({a,b});
    }
    for(int i=1;i<=n;i++){
        cin>>val[i];
    }
    
    build();
}
