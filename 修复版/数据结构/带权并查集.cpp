#include<bits/stdc++.h>
using namespace std;

// 【速查】带权并查集（维护"差值"关系：val[y] - val[x] = d）
// 用法: DSU d(n);  d.merge(x, y, d0) -> 加约束 val[y]-val[x]=d0，与已有约束矛盾返回 false
//       查差值: 先 d.find(x), d.find(y)，同根时 val[y]-val[x] = d.dis[y] - d.dis[x]
// 含义: find 后 dis[x] = val[x] - val[根]
// 复杂度: 近似 O(1)；1-based
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