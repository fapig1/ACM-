#include<bits/stdc++.h>
#define ll long long
#define endl '\n'
using namespace std;

//https://www.luogu.com.cn/problem/P3919
//主席树单点修改单点查询

const int N=1e6+5;

struct node{
    int l,r;
    ll val;
}tree[N*25];
//节点数按 (n + q·log n) 估计: n=1e6,q=1e6 时 ≈ 1e6+2e7 = 2.1e7, N*25=2.5e7 留有余量
//原版 N*40 = 4e7 节点 × 16B = 640MB, 512MB 内存限制的评测机会 MLE

vector<ll> vec(N);
vector<int> root(N);//root表示第i个版本的根节点的index

int top=0;
//在更新操作时进行动态开点的函数
int clone(int ind){
    top++;
    tree[top]=tree[ind];
    return top;
}

//建树
int build(int ind,int l,int r){
    top++;
    ind=top;

    if(l==r){
        tree[ind].val=vec[l];
        return ind;
    }

    int mid=(l+r)/2;
    //实际上更新点可以有很多种方式，我使用的方式是为了统一更新建树和查询函数参数的格式
    //build里面的第一个参数意义不大，为了统一参数那么写的，写一样的就行
    tree[ind].l = build(tree[ind].l,l,mid);
    tree[ind].r = build(tree[ind].r,mid+1,r);
    return ind;
}

//更新
int update(int ind,int l,int r,int t,ll v){
    ind=clone(ind);

    if(l==r){
        tree[ind].val=v;
        return ind;
    }
    int mid=(l+r)/2;
    if(t<=mid){
        tree[ind].l=update(tree[ind].l,l,mid,t,v);
    }else{
        tree[ind].r=update(tree[ind].r,mid+1,r,t,v);
    }
    return ind;
}

//查询
ll query(int ind,int l,int r,int t){
    if(l==r){
        return tree[ind].val; 
    }

    int mid=(l+r)/2;
    if(t<=mid){
        return query(tree[ind].l,l,mid,t);
    }else{
        return query(tree[ind].r,mid+1,r,t);
    }
}

int n,q;
void solve(){
    cin>>n>>q;

    for(int i = 1;i <= n;i ++){
        cin>>vec[i];
    }

    root[0]=build(1,1,n);
    for(int i=1;i<=q;i++){
        int version;
        int command;
        cin>>version>>command;
        if(command==1){
            int t;
            ll v;
            cin>>t>>v;
            root[i]=update(root[version],1,n,t,v);
        }else{
            int t;
            cin>>t;
            ll ans=query(root[version],1,n,t);
            cout<<ans<<endl;
            root[i]=root[version];
        }

    }

}


namespace Pseg{
    const int N = 1e6 + 5;      // 版本数上限
    const int M = N * 25;       // 节点数上限，按需调整

    struct PSeg{
        struct node{
            int l, r;
            ll val;             // 按题型可改成 cnt
        }tree[M];

        int root[N];            // root[i] 表示第 i 个版本的根
        int top;                // 当前节点编号

        PSeg(){
            top = 0;
        }

        // 动态开点：复制旧节点，返回新节点编号
        int clone(int t){
            top++;
            tree[top] = tree[t];
            return top;
        }

        // 建树：带初值，建版本 0
        // vec 是初始数组，下标从 1 开始
        int build(int t, int l, int r, const vector<ll>& vec){
            t = ++top;
            if(l == r){
                tree[t].val = vec[l];
                return t;
            }
            int mid = (l + r) / 2;
            tree[t].l = build(tree[t].l, l, mid, vec);
            tree[t].r = build(tree[t].r, mid + 1, r, vec);
            return t;
        }

        // 建空树：所有权值为 0
        int buildEmpty(int t, int l, int r){
            t = ++top;
            tree[t].val = 0;
            if(l == r) return t;
            int mid = (l + r) / 2;
            tree[t].l = buildEmpty(tree[t].l, l, mid);
            tree[t].r = buildEmpty(tree[t].r, mid + 1, r);
            return t;
        }

        // 单点修改：在 t 版本基础上，把位置 pos 改成 v
        int update(int t, int l, int r, int pos, ll v){
            t = clone(t);
            if(l == r){
                tree[t].val = v;
                return t;
            }
            int mid = (l + r) / 2;
            if(pos <= mid)
                tree[t].l = update(tree[t].l, l, mid, pos, v);
            else
                tree[t].r = update(tree[t].r, mid + 1, r, pos, v);
            return t;
        }

        // 单点查询：查询 t 版本中位置 pos 的值
        ll query(int t, int l, int r, int pos){
            if(l == r) return tree[t].val;
            int mid = (l + r) / 2;
            if(pos <= mid)
                return query(tree[t].l, l, mid, pos);
            else
                return query(tree[t].r, mid + 1, r, pos);
        }

        // 针对于区间第K小问题的插入
        // int insert(int t, int l, int r, int pos){
        //     t = clone(t);
        //     tree[t].val++;
        //     if(l == r) return t;
        //     int mid = (l + r) / 2;
        //     if(pos <= mid)
        //         tree[t].l = insert(tree[t].l, l, mid, pos);
        //     else
        //         tree[t].r = insert(tree[t].r, mid + 1, r, pos);
        //     return t;
        // }

        // 针对于区间第K小问题的查询，返回ind
        // int queryKth(int u, int v, int l, int r, int k){
        //     if(l == r) return l;
        //     int mid = (l + r) / 2;
        //     int leftCnt = tree[tree[v].l].val - tree[tree[u].l].val;
        //     if(k <= leftCnt)
        //         return queryKth(tree[u].l, tree[v].l, l, mid, k);
        //     else
        //         return queryKth(tree[u].r, tree[v].r, mid + 1, r, k - leftCnt);
        // }
    };

    // 第K小使用示例
    // int n, q;
    // vector<ll> a, vec;

    // 放全局不会爆栈
    // PSeg pst;

    // void solve(){
    //     // 快捷清空
    //     pst.top=0;
    
    //     cin >> n >> q;
    //     a.assign(n + 1, 0);
    //     for(int i = 1; i <= n; i++){
    //         cin >> a[i];
    //         vec.push_back(a[i]);
    //     }

    //     sort(vec.begin(), vec.end());
    //     vec.erase(unique(vec.begin(), vec.end()), vec.end());
    //     int m = vec.size();

    //     //这句话很关键！！！！！
    //     pst.root[0] = pst.buildEmpty(0, 1, m);

    //     for(int i = 1; i <= n; i++){
    //         int pos = lower_bound(vec.begin(), vec.end(), a[i]) - vec.begin() + 1;
    //         pst.root[i] = pst.insert(pst.root[i-1], 1, m, pos);
    //     }

    //     while(q--){
    //         int l, r, k;
    //         cin >> l >> r >> k;
    //         int idx = pst.queryKth(pst.root[l-1], pst.root[r], 1, m, k);
    //         cout << vec[idx - 1] << endl;
    //     }
    // }
}
