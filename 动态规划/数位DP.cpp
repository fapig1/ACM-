#include<bits/stdc++.h>
#define int long long
using namespace std;

// 需要改：维度根据状态设计
// f[dep][state]：剩余 dep 位，状态为 state，且 !tight && started 时的方案数
// 这个状态不能存有前效性的状态，即如果状态与sz有关，即每一个不同的sz对应的f可能不同时，可以尝试多次调用init或者将所有数补全到最高位即可
int f[64][100];

vector<int> num;

// 需要改：进制，二进制写 2，十进制写 10
int base = 2;

void init(){
    memset(f, -1, sizeof(f));
}

// 需要改：最终状态是否合法
bool check(int state){
    return true;
}

// 需要改：状态转移
int trans(int state, int d){
    return state;
}

// 需要改：中间剪枝，简单题直接 return true
bool valid(int state){
    return true;
}

// 需要改：如果状态有多个分量，dfs 参数和 f 维度同步改
int dfs(int dep, int state, bool tight, bool started){
    if(dep == 0){
        if(!started) return 0; // 需要改：是否允许全 0
        return check(state) ? 1 : 0;
    }

    if(!tight && started){
        int &v = f[dep][state];
        if(~v) return v;
    }

    int idx = num.size() - dep;
    int up = tight ? num[idx] : base - 1;

    int ans = 0;

    for(int d = 0; d <= up; d++){
        if(!started && d == 0){
            ans += dfs(dep - 1, 0, tight && (d == up), 0);
        }else{
            int ns = trans(state, d);
            if(valid(ns)){
                ans += dfs(dep - 1, ns, tight && (d == up), 1);
            }
        }
    }

    if(!tight && started){
        f[dep][state] = ans;
    }

    return ans;
}

// 需要改：边界、初始状态
int calc(int x){
    if(x <= 0) return 0; // 需要改：是否允许数字 0
    num.clear();
    while(x){
        num.push_back(x % base);
        x /= base;
    }
    reverse(num.begin(), num.end());
    int sz = num.size();

    // 需要改：初始状态，一般是 0
    return dfs(sz, 0, 1, 0);
}

void solve(){
    int l, r;
    cin >> l >> r;
    cout << calc(r) - calc(l - 1) << "\n";
}

signed main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    init();
    int t = 1;
    // cin >> t; // 多组数据时打开
    while(t--){
        solve();
    }
    return 0;
}