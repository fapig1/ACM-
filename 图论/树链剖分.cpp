#include <bits/stdc++.h>
#include "../数据结构/线段树.cpp"
using namespace std;

//记得把数据结构里面的线段树一起复制过来！！！
//树链剖分
struct HLD{
    int n, timer;
    vector<int> sz, dep, fa, son, top, dfn, rnk;
    vector<vector<int>> adj;
    Seg seg;

    HLD(int n) : n(n), timer(0), sz(n + 1), dep(n + 1), fa(n + 1), son(n + 1), top(n + 1), dfn(n + 1), rnk(n + 1), adj(n + 1), seg(n) {}

    void addEdge(int u, int v){
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    //处理 sz, dep, fa, son
    void dfs1(int u, int p, int d){
        sz[u] = 1; fa[u] = p; dep[u] = d; son[u] = 0;
        for(int v : adj[u]){
            if(v == p) continue;
            dfs1(v, u, d + 1);
            sz[u] += sz[v];
            if(sz[v] > sz[son[u]]) son[u] = v;
        }
    }

    //处理 top, dfn
    void dfs2(int u, int tp){
        top[u] = tp; dfn[u] = ++timer; rnk[timer] = u;
        if(!son[u]) return;
        dfs2(son[u], tp);
        for(int v : adj[u]){
            if(v == fa[u] || v == son[u]) continue;
            dfs2(v, v);
        }
    }

    //建树
    void build(const vector<int>& a, int root = 1){
        dfs1(root, 0, 1);
        dfs2(root, root);
        vector<int> v(n + 1);
        for(int i = 1; i <= n; i++){
            v[i] = a[rnk[i]];
        }
        seg.build(1, 1, n, v);
    }

    // loglog复杂度太大 可以用树上差分优化
    void updatePath(int u, int v, int val){
        while(top[u] != top[v]){
            if(dep[top[u]] < dep[top[v]]) swap(u, v);
            seg.update(1, 1, n, dfn[top[u]], dfn[u], val);
            u = fa[top[u]];
        }
        if(dep[u] > dep[v]) swap(u, v);
        seg.update(1, 1, n, dfn[u], dfn[v], val);
    }

    //询问路径
    int askPath(int u, int v){
        int res = 0;
        while(top[u] != top[v]){
            if(dep[top[u]] < dep[top[v]]) swap(u, v);
            res += seg.ask(1, 1, n, dfn[top[u]], dfn[u]);
            u = fa[top[u]];
        }
        if(dep[u] > dep[v]) swap(u, v);
        res += seg.ask(1, 1, n, dfn[u], dfn[v]);
        return res;
    }

    //更新树
    void updateTree(int u, int val){
        seg.update(1, 1, n, dfn[u], dfn[u] + sz[u] - 1, val);
    }

    //询问子树
    int askTree(int u){
        return seg.ask(1, 1, n, dfn[u], dfn[u] + sz[u] - 1);
    }

    //获得两点LCA
    int getLCA(int u, int v){
        while(top[u] != top[v]){
            if(dep[top[u]] < dep[top[v]]) swap(u, v);
            u = fa[top[u]];
        }
        return dep[u] < dep[v] ? u : v;
    }
};



signed main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr), cout.tie(nullptr);
    
    return 0;
}
