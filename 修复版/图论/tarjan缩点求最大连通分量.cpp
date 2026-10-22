#include <bits/stdc++.h>
#define int long long
using namespace std;
int mod=1e9+7;
// 【速查】Tarjan 强连通分量 + 缩点（有向图 → DAG）
// 用途: 有向图里"互相可达"的点缩成一个点，之后在 DAG 上做拓扑 / DP（如最大点权路径）
// 用法: SCC T;（放全局）  T.n = n; T.m = m;  T.gra[a].push_back(b); T.edge.push_back({a, b}); 读 T.val[]; T.build();
// 结果: T.scc[i] = 点 i 所属分量 (0..T.curscc-1)，T.sccval[c] = 分量点权和，T.sccgra = 缩点后的 DAG（可能有重边）
// 复杂度: O(n + m)；点 1-based；递归实现，1e6 深链会爆栈
// 10.22: 仅把原来的全局变量 / 函数放进结构体，算法代码未改
struct SCC{
    int MAXN;
    vector<vector<int>> gra;	//原图邻接表
    vector<int> val;			//原图点权
    vector<int> dfn;			//原图dfn序，用于构建scc
    vector<int> low;			//节点的scc根节点，用于构建scc
    stack<int> stk;					//栈，用于构建scc
    vector<int> instk;		//节点是否入栈，用于构建scc
    vector<int> scc;		//ind to scc 原图点角标转新图点角标
    vector<int> sccval;		//新图点权
    vector<vector<int>> sccgra;	//新图邻接表
    int cur=1;						//维护原图dfn值
    int curscc=0;					//新图大小

    int n,m;
    vector<pair<int,int>> edge;   //不预填,直接push_back

    SCC(int maxn=5e5+5):MAXN(maxn),gra(maxn),val(maxn),dfn(maxn),low(maxn),instk(maxn),scc(maxn),sccval(maxn),sccgra(maxn){}

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
            int c;
            do{
                c=stk.top();
                stk.pop();
                instk[c]=0;
                scc[c]=curscc;
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
            if(scc[a]!=scc[b]){
                sccgra[scc[a]].push_back(scc[b]);
            }
        }
    }
};

SCC T;

void solve(){
    cin>>T.n>>T.m;
    for(int i=0;i<T.m;i++){
        int a,b;
        cin>>a>>b;
        T.gra[a].push_back(b);
        T.edge.push_back({a,b});
    }
    for(int i=1;i<=T.n;i++){
        cin>>T.val[i];
    }

    T.build();
}

signed main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    solve();
    // 输出: SCC 个数 / 缩点图边数 / 每个分量(点集与点权和) / 最大分量点权和
    int edges=0;
    for(int i=0;i<T.curscc;i++) edges+=(int)T.sccgra[i].size();
    cout<<T.curscc<<" "<<edges<<"\n";
    for(int i=0;i<T.curscc;i++){
        cout<<"SCC "<<i+1<<" (val="<<T.sccval[i]<<"):";
        for(int j=1;j<=T.n;j++) if(T.scc[j]==i) cout<<" "<<j;
        cout<<"\n";
    }
    int best=0;
    for(int i=1;i<T.curscc;i++) if(T.sccval[i]>T.sccval[best]) best=i;
    cout<<"max sccval = "<<T.sccval[best]<<"\n";
    return 0;
}
