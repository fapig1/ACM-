#include <bits/stdc++.h>
using namespace std;

// 【速查】并查集（连通性 + 连通块计数）
// 用法: SDU d(n);  d.merge(x, y);  d.check(x, y) -> 是否连通;  d.cnt -> 当前连通块个数;  d.find(x) -> 根
// 复杂度: 近似 O(1)（路径压缩）；1-based。注意 find 是递归，1e6 深链可能爆栈
//并查集
struct SDU{
    int n, cnt;//cnt为连通块数量
    vector<int> fa;
    SDU(int m = 0){
        n = m;
        fa.resize(n + 1);
        for(int i = 1; i <= n; i++) fa[i] = i;//1-based
        cnt = n;
    }

    int find(int x){
        return x == fa[x] ? x : fa[x] = find(fa[x]);
    }

    void merge(int x, int y){
        int xx = find(x);
        int yy = find(y);
        if(xx != yy){
            fa[xx] = yy;
            cnt--;
        }
    }

    bool check(int x, int y){
        return find(x) == find(y);
    }
};