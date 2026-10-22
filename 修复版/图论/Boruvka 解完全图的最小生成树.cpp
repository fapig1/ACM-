#include <bits/stdc++.h>
#define int long long
#define endl "\n"
using namespace std;

// 【速查】Boruvka 完全图最小生成树
// 用途: n 个点两两有边、边权由公式给出（本例 w(i,j) = (a_i + a_j) mod k），边太多不能全建
// 用法: Boruvka B;  B.solve() 读入一组 n k 和 n 个数，返回 MST 权值和
//       每轮给每个连通块找"连到别的块的最小边"（multiset 里二分），全部合并；改 mine 更新处的边权公式即可套题
// 复杂度: O(n log² n)（log n 轮，每轮 n 次 set 查询）
// 10.22: 仅把原来的全局并查集与 solve 放进结构体，算法代码未改
//Boruvka 求解边权为某个数学条件下的完全图的最小生成树
//使用类似启发式的思想，每轮寻找每个并查集对于其他并查集的最小出边
//每轮并查集数量/=2，最多log轮，在每轮中需要遍历每一个点以及二分寻找找到在其他集合中的点对这个点的最小边
//总复杂度O(n(logn)^2)

struct edge{
    int val,a,b;// 边权，起点，终点
};

struct Boruvka{
    //并查集模板
    vector<int> par;
    int find(int a){
        if(a==par[a])return a;
        return par[a]=find(par[a]);
    }

    int unite(int a,int b){
        int pa=find(a);
        int pb=find(b);

        if(pa==pb)return 1;
        par[pb]=pa;
        return 0;
    }

    int solve(){
        int n,k;
        cin>>n>>k;
        par.assign(n,0);
        vector<int> val(n);

        multiset<pair<int,int>> st;
        vector<vector<int>> block(n);
        vector<edge> mine(n);
        for(int i=0;i<n;i++){
            cin>>val[i];
            val[i]%=k;
            par[i]=i;
            st.insert({val[i],i});
        }

        int num=0;
        int ans=0;
        while(num<n-1){
            //初始化，将每一个并查集块中的元素分类，同时初始化每个并查集块的最小出边
            for(int i=0;i<n;i++){
                block[i].clear();
            }
            for(int i=0;i<n;i++){
                block[find(i)].push_back(i);
            }
            for(int i=0;i<n;i++){
                mine[i]={(int)2e18,-1,-1};
            }

            for(int i=0;i<n;i++){
                //删除当前并查集块的点
                for(int j:block[i]){
                    st.erase({val[j],j});
                }

                for(int j:block[i]){
                    int tar=k-val[j];
                    pair<int,int> pr={tar,0};
                    auto id=st.lower_bound(pr);

                    if(id!=st.end()){
                        pair<int,int> p=*id;

                        //修改这里，取决于边权如何决定
                        if(mine[i].val>(p.first+val[j])%k){
                            mine[i].val=(p.first+val[j])%k;
                            mine[i].b=p.second;
                            mine[i].a=j;
                        }
                    }

                    tar=0;
                    pr={tar,0};
                    id=st.lower_bound(pr);

                    if(id!=st.end()){
                        pair<int,int> p=*id;

                        //修改这里，取决于边权如何决定
                        if(mine[i].val>(p.first+val[j])%k){
                            mine[i].val=(p.first+val[j])%k;
                            mine[i].b=p.second;
                            mine[i].a=j;
                        }
                    }
                }

                //重新插入当前元素的点
                for(int j:block[i]){
                    st.insert({val[j],j});
                }
            }

            //合并新建边的集合
            for(int i=0;i<n;i++){
                auto [v,a,b]=mine[i];
                if(mine[i].a!=-1){
                    if(!unite(a,b)){
                        ans+=v;
                        num++;
                    }
                }
            }
        }
        return ans;


    }
};

Boruvka B;

signed main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    int t=1;
    cin>>t;
    while(t--){
        cout<<B.solve()<<"\n";
    }
    return 0;
}
