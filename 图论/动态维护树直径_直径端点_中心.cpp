#include<bits/stdc++.h>
using namespace std;
#define int long long

const int MAXN=2e5+5;
const int LOG=32;

//{to,val}
vector<vector<pair<int,int>>> gra(MAXN);

vector<vector<int>> par(MAXN,vector<int>(LOG));

vector<int> vis(MAXN);
vector<int> dep(MAXN);
vector<int> dis(MAXN);

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
        int cur=u;
        for(int i=LOG-1;i>=0;i--){
            int pp=par[cur][i];
            if(pp && dep[pp]>dep[p]){
                cur=pp;
            }
        }
        nxt=par[cur][0];
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

void solve(){
    int n;
    cin>>n;
    dep[0]=-1;

    for(int i=1;i<n;i++){
        int a,b,w;
        cin>>a>>b>>w;
        gra[a].push_back({b,w});
        gra[b].push_back({a,w});
    }

    int root;
    cin>>root;
    vis[root]=1;
    dep[root]=1;
    dfs(root);

    for(int i=1;i<LOG;i++){
        for(int j=1;j<=n;j++){
            if(par[j][i-1]){
                par[j][i]=par[par[j][i-1]][i-1];
            }
        }
    }

    queue<int> que;
    que.push(root);
    int u=root;//当前直径端点
    int v=root;//当前直径端点
    int diameter=0;//当前直径
    vis.assign(n+1,0);
    vis[root]=1;

    //这里是从根节点开始bfs，每一步都维护对应的直径，直径端点和中心
    //具体题目修改对应的内容
    while(que.size()){
        queue<int> que2;
        while(que.size()){
            int tp=que.front();
            que.pop();
            for(auto [i,w]:gra[tp]){
                if(vis[i])continue;
                vis[i]=1;
                int d1=dist(u,i);
                int d2=dist(v,i);
                if(d1>diameter&&d1>d2){
                    diameter=d1;
                    v=i;
                }
                if(d2>diameter){
                    diameter=d2;
                    u=i;
                }
                pair<int,int> cen=get_center(u,v);
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