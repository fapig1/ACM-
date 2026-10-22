#include<bits/stdc++.h>
using namespace std;

// 【速查】ST 表（静态区间最值，不支持修改）
// 用法: ST st(a);  st.query(l, r)        // a 为 1-based（a[0] 占位），查 [l, r]
// 改成 min / gcd / & / |: 改构造函数和 query 里的 max（必须是可重复贡献的运算）
// 复杂度: 预处理 O(n log n)，查询 O(1)
//ST表，静态区间最值查询，O(nlogn)预处理，O(1)查询
struct ST{
    int n;
    vector<int> lg;
    vector<vector<int>> f;

    //数组a为1-base，查询同为1-base，若是0-base自行前面补0
    ST(vector<int> a):n(a.size()-1){
        lg.assign(n+1,0);
        for(int i=2;i<=n;i++){
            lg[i]=lg[i/2]+1;
        }

        f.assign(n+1,vector<int>(lg[n]+1));

        for(int i=1;i<=n;i++){
            f[i][0]=a[i];
        }

        for(int j=1;j<=lg[n];j++){
            for(int i=1;i+(1<<j)-1<=n;i++){
                // 修改点：合并方式，max 换成 min / __gcd / & / | 等
                f[i][j]=max(f[i][j-1],f[i+(1<<(j-1))][j-1]);
            }
        }
    }

    // 修改点：返回类型与合并方式需与预处理保持一致
    int query(int l,int r){
        int k=lg[r-l+1];
        return max(f[l][k],f[r-(1<<k)+1][k]);
    }
};