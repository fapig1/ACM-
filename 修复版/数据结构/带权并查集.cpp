#include<bits/stdc++.h>
using namespace std;

//带权并查集，维护每个连通块根节点到每个节点中某个权值关系
struct DSU{
    int n;
    vector<int> fa, sz, dis;

    DSU(int n):n(n),fa(n+1),sz(n+1,1),dis(n+1,0){
        for(int i=1;i<=n;i++){
            fa[i]=i;
        }
    }

    int find(int x){
        if(fa[x]==x) return x;

        int f=fa[x];
        fa[x]=find(fa[x]);

        dis[x]+=dis[f];

        return fa[x];
    }

    bool merge(int x,int y,int d){
        int fx=find(x);
        int fy=find(y);

        if(fx==fy){
            return dis[y]-dis[x]==d;
        }

        if(sz[fx]>sz[fy]){
            swap(x,y);
            swap(fx,fy);
            d=-d;
        }

        fa[fx]=fy;

        dis[fx]=dis[y]-dis[x]-d;

        sz[fy]+=sz[fx];

        return true;
    }
};