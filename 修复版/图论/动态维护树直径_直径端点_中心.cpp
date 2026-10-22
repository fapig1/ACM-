#include<bits/stdc++.h>
using namespace std;
#define int long long

// 【速查】倍增 LCA 工具箱 + 逐点加入时动态维护直径/端点/中心
// 用法: TreeTool T;（放全局）  T.gra[a].push_back({b, w}) 建双向边;  T.dep[0] = -1; T.vis[root] = 1; T.dep[root] = 1;
//       T.dfs(root);  T.buildPar(n);  之后可用:
//   T.lca(a, b)  T.dist(a, b)（带边权）  T.jump(x, k) -> x 向上 k 步
//   T.u_to_v_len(u, v, len) -> 从 u 往 v 走 len 能到的最远点;  T.get_center(u, v) -> 路径中点（落在边上返回两端点）
// 动态直径: 新点 x 加入时，新直径只可能是 (u,x) 或 (v,x)，比较 dist 即可（solve 里按 BFS 序加点）
// 复杂度: 预处理 O(n log n)，每次查询 O(log n)
// 10.22: 仅把原来的全局变量 / 函数放进结构体（倍增表的递推从 solve 挪成 buildPar），算法代码未改
const int MAXN=2e5+5;
const int LOG=32;

struct TreeTool{
    //{to,val}
    vector<vector<pair<int,int>>> gra=vector<vector<pair<int,int>>>(MAXN);

    vector<vector<int>> par=vector<vector<int>>(MAXN,vector<int>(LOG));

    vector<int> vis=vector<int>(MAXN);
    vector<int> dep=vector<int>(MAXN);
    vector<int> dis=vector<int>(MAXN);

    //初始化每个节点的父节点，深度，根节点到该节点的边权和
    void dfs(int ind){
        for(auto [i,w]:gra[ind]){
            if(vis[i])continue;

            vis[i]=1;

            par[i][0]=ind;
            dep[i]=dep[ind]+1;
            dis[i]=dis[ind]+w;

            dfs(i);
        }
    }

    //倍增表递推（原 solve 中的循环）
    void buildPar(int n){
        for(int i=1;i<LOG;i++){
            for(int j=1;j<=n;j++){
                if(par[j][i-1]){
                    par[j][i]=par[par[j][i-1]][i-1];
                }
            }
        }
    }

    //求两节点LCA
    int lca(int a,int b){
        if(dep[a]>dep[b])swap(a,b);

        for(int i=LOG-1;i>=0;i--){
            if(dep[par[b][i]]>=dep[a]){
                b=par[b][i];
            }
        }

        if(a==b)return a;

        for(int i=LOG-1;i>=0;i--){
            if(par[a][i]!=par[b][i]){
                a=par[a][i];
                b=par[b][i];
            }
        }

        return par[a][0];
    }

    //求两节点距离
    int dist(int a,int b){
        int l=lca(a,b);
        return dis[a]+dis[b]-2*dis[l];
    }

    //从x节点向根节点移动k步的终点
    int jump(int x,int k){
        for(int i=0;i<LOG;i++){
            if(k&(1LL<<i)){
                x=par[x][i];
            }
        }
        return x;
    }

    //获取从u向v走距离len时最远到达的位置，若落在一条边上返回靠近u的那个端点
    int u_to_v_len(int u,int v,int len){
        int l=lca(u,v);
        int du=dist(u,l);
        if(len<=du){
            int cur=u;
            for(int i=LOG-1;i>=0;i--){
                int p=par[cur][i];
                if(p && dist(u,p)<=len){
                    cur=p;
                }
            }
            return cur;
        }
        int rem=dist(u,v)-len;
        int cur=v;
        for(int i=LOG-1;i>=0;i--){
            int p=par[cur][i];
            if(p && dist(v,p)<=rem){
                cur=p;
            }
        }
        if(dist(cur,v)<rem)cur=par[cur][0];
        return cur;
    }

    //获得u,v两点间中点所在点，若落在一条边上则返回这条边两端点，p1靠近u，p2靠近v
    pair<int,int> get_center(int u,int v){
        int d=dist(u,v);
        int half=d/2;
        int p=u_to_v_len(u,v,half);
        if(dist(u,p)*2==d){
            return {p,p};
        }

        int l=lca(u,v);
        int nxt;
        if(lca(u,p)==p&&p!=l){
            // p 在 u->lca 上行段: 朝 v 方向的下一个点就是 p 的父节点
            nxt=par[p][0];
        }else{
            int cur=v;
            for(int i=LOG-1;i>=0;i--){
                int pp=par[cur][i];
                if(pp && dep[pp]>dep[p]){
                    cur=pp;
                }
            }
            nxt=cur;
        }
        return {p,nxt};
    }
};

TreeTool T;

void solve(){
    int n;
    cin>>n;
    T.dep[0]=-1;

    for(int i=1;i<n;i++){
        int a,b,w;
        cin>>a>>b>>w;
        T.gra[a].push_back({b,w});
        T.gra[b].push_back({a,w});
    }

    int root;
    cin>>root;
    T.vis[root]=1;
    T.dep[root]=1;
    T.dfs(root);
    T.buildPar(n);

    queue<int> que;
    que.push(root);
    int u=root;//当前直径端点
    int v=root;//当前直径端点
    int diameter=0;//当前直径
    T.vis.assign(n+1,0);
    T.vis[root]=1;

    //这里是从根节点开始bfs，每一步都维护对应的直径，直径端点和中心
    //具体题目修改对应的内容
    while(que.size()){
        queue<int> que2;
        while(que.size()){
            int tp=que.front();
            que.pop();
            for(auto [i,w]:T.gra[tp]){
                if(T.vis[i])continue;
                T.vis[i]=1;
                int d1=T.dist(u,i);
                int d2=T.dist(v,i);
                if(d1>diameter&&d1>d2){
                    diameter=d1;
                    v=i;
                }
                if(d2>diameter){
                    diameter=d2;
                    u=i;
                }
                pair<int,int> cen=T.get_center(u,v);
                cout<<u<<" "<<v<<" "<<diameter<<" ";
                if(cen.first==cen.second){
                    cout<<cen.first<<"\n";
                }else{
                    cout<<cen.first<<" "<<cen.second<<"\n";
                }
                que2.push(i);
            }
        }
        swap(que,que2);
    }
    return;
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
