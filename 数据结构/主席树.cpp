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
}tree[N*40];
//模版上写20，但是我交了re，写大一点过了，有些玄学

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