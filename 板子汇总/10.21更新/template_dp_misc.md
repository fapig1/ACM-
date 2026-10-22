# David_DP&杂项（10.21 修复版）

> **通用前导**：本文档所有代码块为片段，需配合以下前导使用：

```cpp
#include <bits/stdc++.h>
using namespace std;
```

标注 **【本次修复】** 的模板经过 2026-10-21 全量审查修复，并全部通过随机对拍/压测回归验证；各修复点在代码注释中亦有说明。

[TOC]

### 斜率优化DP

> 【本次修复】查询前进条件 !check→check（原版全错），solve3 补弹栈后 cur 保护（原版段错误）。含三例：通用转移/P6047 丝之割/P4072 方差。

```cpp
#define int long long
int mod=1e9+7;
int minProblem=1;
// 【速查】斜率优化 DP（单调队列维护凸包）
// 用途: dp[i] = min/max_j { y(j) - k(i)·x(j) } + 只和 i 有关的项，且 k(i) 单调、x(j) 单调
// 套路: 化成 b = y - k·x：b 含 dp[i]，(x, y) 只含 j，k 只含 i；求 min 维护下凸包(minProblem=1)，max 维护上凸包
// 用法: 照 solve3 / solve(P6047) / solve2(P4072) 改 x、y、k 的计算；check 判斜率，cur 指向最优点
// 复杂度: O(n)（k 不单调时改成凸包上二分，O(n log n)）
//由于dp的形式千变万化，这里仅供思路参考
struct pt{
    int x,y;
};

//点p1,p2的斜率是否小于k
bool check(pt p1,pt p2,int k){
    int c=(p2.y-p1.y);
    int d=k*(p2.x-p1.x);
    if(minProblem)return (p2.y-p1.y)<=k*(p2.x-p1.x);
    else return (p2.y-p1.y)>=k*(p2.x-p1.x);
}

//点p1,p2的斜率是否大于p2,p3
bool check(pt p1,pt p2,pt p3){
    if(minProblem)return (p2.y-p1.y)*(p3.x-p2.x)>=(p3.y-p2.y)*(p2.x-p1.x);
    else return (p2.y-p1.y)*(p3.x-p2.x)<=(p3.y-p2.y)*(p2.x-p1.x);
}


void solve3(){
    int n;
    cin>>n;
    vector<int> s(n+1);
    vector<int> k(n+1);
    vector<int> c(n+1);
    vector<int> kk(n+1);
    vector<int> ss(n+1);
    for(int i=1;i<=n;i++){
        cin>>s[i]>>k[i]>>c[i];
        kk[i]=k[i]+kk[i-1];
        ss[i]=k[i]*s[i]+ss[i-1];
    }


    //dp[i]=min{dp[j]+ss[j]-ss[i]+s[i]*kk[i]-s[i]*kk[j]+c[i]}
    //dp[i]+ss[i]-s[i]*kk[i]-c[i]=dp[j]+ss[j]-s[i]*kk[j];
    //因为s[i]已知且单调不减，使用斜率优化DP
    //设b=y-k*x   b=dp[i]+ss[i]-s[i]*kk[i]-c[i]    y=dp[j]+ss[j]    k=s[i]  x=kk[j]

    //如果维护max问题，则维护上凸包，k取负数，minproblem=0
    //如果维护min问题，则维护下凸包，k取正数，minproblem=1

    // (注意负号！我们保证k是正数，所以x可能为了拼凑负号变为负数)
    //得到的新点{kk[i],dp[i]+ss[i]}插入单调队列中，保证单调队列中两点间斜率单调递增
    //使用一个cur维护当前符合要求的点（要么是终点，要么cur与cur+1两点间斜率刚好是单调队列中比k稍大的点）
    //时间复杂度O(n)
    //以下部分可以作为模板参考，替换其中的x,y,k,b的计算方式，只要保证k单调就可以应用
    vector<pt> dp;  //dp单调队列
    vector<int> adp(n+1);   //dp答案
    dp.push_back({0,0});
    int cur=0;
    for(int i=1;i<=n;i++){
        int k=s[i];
        //下凸包求 min: 斜率(邻边)<=k 时可以右移 (原版 !check 方向写反)
        while(cur+1<dp.size()&&check(dp[cur],dp[cur+1],k)){
            cur++;
        }

        int x=dp[cur].x;
        int y=dp[cur].y;
        int b=y-x*k;
        adp[i]=b-ss[i]+s[i]*kk[i]+c[i];

        //将转移方程中x,y项中的j替换为i即可
        int nx=kk[i];
        int ny=adp[i]+ss[i];

        while(dp.size()>=2&&check(dp[dp.size()-2],dp[dp.size()-1],{nx,ny})){
            dp.pop_back();
        }
        while(cur>=(int)dp.size())cur--;   //弹栈可能使 cur 越界,必须回收 (原版缺失会段错误)

        dp.push_back({nx,ny});
    }
    int ans=adp[n];
    cur=n;

    //此题特判最后k为0无贡献
    while(cur&&k[cur]==0){
        cur--;
        ans=min(ans,adp[cur]);
    }
    cout<<ans<<"\n";
    
}

//https://www.luogu.com.cn/problem/P6047 丝之割
void solve(){
    int n,m;
    cin>>n>>m;
    vector<int> veca(n+1);
    vector<int> vecb(n+1);
    vector<int> mina(n+1,INT_MAX);
    vector<int> minb(n+2,INT_MAX);
    for(int i=1;i<=n;i++){
        cin>>veca[i];
        mina[i]=min(mina[i-1],veca[i]);
    }
    for(int i=1;i<=n;i++){
        cin>>vecb[i];
    }
    for(int i=n;i>=0;i--){
        minb[i]=min(minb[i+1],vecb[i]);
    }
    vector<pt> p(m); 
    for(int i=0;i<m;i++){
        cin>>p[i].x>>p[i].y;
    }
    sort(p.begin(),p.end(),[&](const pt &a,const pt &b){
        if(a.x==b.x){
            return a.y>b.y;
        }else{
            return a.x<b.x;
        }
    });

    vector<pt> tar;
    for(int i=0;i<m;i++){
        if(tar.empty()||(tar.back().y<p[i].y&&tar.back().x<p[i].x)){
            tar.push_back(p[i]);
        }
    }
    //预处理出每个点左侧最小值，右侧最小值，以及去除交叉线段

    //dp[i]=dp[j]+minb(i+1)*mina(j)
    //设b=y-k*x   b=dp[i]    y=dp[j]    k=minb(i+1)  x=-mina(j)
    int len=tar.size();
    vector<pt> dp;
    int cur=0;
    dp.push_back({-mina[tar[0].x-1],0});
    for(int i=0;i<tar.size();i++){
        int k=minb[tar[i].y+1];
        while(cur+1<dp.size()&&check(dp[cur],dp[cur+1],k)){
            cur++;      
        }

        int x=dp[cur].x;
        int y=dp[cur].y;
        int b=y-x*k;

        //下面处理构造出的新点，主要根据x=-mina(j)和y=dp[j]，将所有的j替换为i
        int nx=0;
        if(i<tar.size()-1)nx=-mina[tar[i+1].x-1];   //防止末尾越界
        int ny=b;

        while(dp.size()>=2&&check(dp[dp.size()-2],dp[dp.size()-1],{nx,ny})){
            dp.pop_back();
        }
        while(cur>=dp.size())cur--;

        dp.push_back({nx,ny});
    }
    cout<<dp.back().y<<"\n";
    
}

//https://www.luogu.com.cn/problem/P4072 求将n个数分成m段，使得整体方差最小
void solve2(){
    int n,m;
    cin>>n>>m;
    vector<int> vec(n+1);
    vector<int> pre(n+1);
    for(int i=1;i<=n;i++){
        cin>>vec[i];
        pre[i]=vec[i]+pre[i-1];
    }
    //dp[i][j]-sum(j)^2=-2sum(j)sum(k)+dp[i-1][k]+sum(k)^2
    //设b=y-k*x   b=dp[i][j]-sum(j)^2    y=dp[i-1][k]+sum(k)^2    k=2sum(j)  x=sum(k)
    //特别注意k*x项本身带一个负号，不要多余处理这个负号
    //其中dp[i-1]表示上一维的答案，需要两个单调队列维护当前维的队列内容和上一维的队列内容
    vector<vector<int>> dp(m+1,vector<int>(n+1));
    for(int k=0;k<=n;k++){
        dp[0][k]=0;
    }
    vector<pt> pdp;
    pdp.push_back({0,0});
    for(int i=1;i<=m;i++){
        vector<pt> cdp;
        int cur=0;
        for(int j=i;j<=n;j++){
            int k=2*pre[j];
            while(cur+1<pdp.size()&&check(pdp[cur],pdp[cur+1],k)){
                cur++;      
            }
            
            int x=pdp[cur].x;
            int y=pdp[cur].y;
            int b=y-x*k;
            dp[i][j]=b+pre[j]*pre[j];
            
            //由于有多维，x=sum(k)和y=dp[i-1][k]+sum(k)^2无视i-1，将k替换为j
            int nx=pre[j];
            int ny=dp[i][j]+pre[j]*pre[j];
            
            while(cdp.size()>=2&&check(cdp[cdp.size()-2],cdp[cdp.size()-1],{nx,ny})){
                cdp.pop_back();
            }
            
            cdp.push_back({nx,ny});
        }
        swap(cdp,pdp);
    }
    cout<<dp[m][n]*m-pre[n]*pre[n]<<"\n";
    
}

signed main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    int t=1;
    while(t--)solve();
    return 0;
}
```

### 数位DP

> tight/started 记忆化框架，按注释改 check/trans/valid。
> 【10.22 结构体封装】原全局变量 + 全局函数改为结构体 / 传参函数，用法见块内速查卡。

```cpp
using ll = long long;

// 【速查】数位 DP（记忆化搜索框架，结构体封装）
// 用途: 统计 [l, r] 内满足某种"按位条件"的数的个数
// 用法: DigitDP dp;  dp.base = 进制;  ll ans = dp.calc(r) - dp.calc(l - 1);
// 改题: 只改"按题意修改"那一段：状态个数 S、trans（加一位后的新状态）、valid（剪枝）、check（终态是否合法）、
//       是否计入 0（calc 开头）；状态有多维就把 state 编码成一个整数
// 记忆化: f[dep][state] 只在 !tight && started 时有效，和上界无关，所以多次 calc 不用清空
//         （若状态和数字总位数有关，就每次 calc 前 reset()）
// 复杂度: O(位数 × S × base)
// 10.22: 原版是全局 f / num / base + 全局函数，已封装为结构体
struct DigitDP {
    // ---- 按题意修改 ----
    static const int S = 100;                  // 状态个数上限
    int base = 2;
    bool check(int state) { return true; }      // 最终状态是否合法
    int trans(int state, int d) { return state; }   // 在末尾接上数字 d 之后的状态
    bool valid(int state) { return true; }      // 中间剪枝，不需要就 return true
    // --------------------
    vector<int> num;
    vector<array<ll, S>> f;

    DigitDP() { reset(); }
    void reset() { f.assign(64, {}); for (auto& row : f) row.fill(-1); }

    ll dfs(int dep, int state, bool tight, bool started) {
        if (dep == 0) return started && check(state) ? 1 : 0;   // 改: 是否允许全 0
        if (!tight && started && f[dep][state] != -1) return f[dep][state];
        int up = tight ? num[num.size() - dep] : base - 1;
        ll ans = 0;
        for (int d = 0; d <= up; d++) {
            if (!started && d == 0) ans += dfs(dep - 1, 0, tight && d == up, false);
            else {
                int ns = trans(state, d);
                if (valid(ns)) ans += dfs(dep - 1, ns, tight && d == up, true);
            }
        }
        if (!tight && started) f[dep][state] = ans;
        return ans;
    }
    ll calc(ll x) {                            // [1, x] 中合法的数的个数
        if (x <= 0) return 0;                   // 改: 是否计入数字 0
        num.clear();
        for (; x; x /= base) num.push_back(x % base);
        reverse(num.begin(), num.end());
        return dfs(num.size(), 0, true, false);  // 改: 初始状态
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    DigitDP dp;
    ll l, r;
    cin >> l >> r;
    cout << dp.calc(r) - dp.calc(l - 1) << "\n";
    return 0;
}
```

### 三分

> 整数三分求单峰极值；对带平台函数会失效（三分固有局限）。

```cpp
#define int long long

// 【速查】整数三分（单峰函数求极值）
// 用途: cal(x) 在 [l, r] 上先增后减（单峰），求最大值；求最小值把 v1>=v2 的比较反过来、max 改 min
// 用法: 改 cal 和初始 l、r；区间缩到 ≤3 后暴力枚举
// 注意: 有平台（一段相等）时三分会失效
// 复杂度: O(log r · 单次 cal)
int cal(int num){return 0;}
void solve(){
    int l=0,r=2e18;
    while(r-l>2){
        int m1=l+(r-l)/3;
        int m2=r-(r-l)/3;

        int v1=cal(m1);
        int v2=cal(m2);

        //求极大值，改为极小值修改此处符号
        if(v1>=v2){
            r=m2;
        }else{
            l=m1;
        }
    }
    int ans=0;
    for(int j=l;j<=r;j++){
        int cur=cal(j);
        ans=max(ans,cur);
    }
    cout<<ans<<"\n";
}

signed main()
{
    ios::sync_with_stdio(false);
    cin.tie(0);

    int t=1;
    cin>>t;
    while(t--)solve();

    return 0;
}
```

### 高精度

> 字符串四则（除法为高精度÷int）；非负整数输入。

```cpp
#include <iostream>
#include <string>
#include <algorithm>

// 【速查】高精度（非负整数字符串）
// 用法: addBigNumbers(a, b) / subtractBigNumbers(a, b)（a<b 时结果带负号）
//       multiplyBigNumbers(a, b) / divideBigNumbers(a, d)（d 为 int，向下取整）
// 注意: 只支持非负数输入，不要带前导零或符号
// 复杂度: 加减 O(n)，乘 O(nm)，除以小整数 O(n)
string removeLeadingZeros(string s){
    int pos = s.find_first_not_of('0');
    if(pos == string::npos) return "0";
    return s.substr(pos);
}

string addBigNumbers(string num1, string num2){
    int maxLength = max(num1.size(), num2.size());

    num1.insert(0, maxLength - num1.size(), '0');   // 10.22 修复: 原版逐位 "0"+s，位数差大时 O(n²)
    num2.insert(0, maxLength - num2.size(), '0');

    string result = "";

    int carry = 0;

    for(int i = maxLength - 1; i >= 0; i--){
        int digit1 = num1[i] - '0';
        int digit2 = num2[i] - '0';

        int sum = digit1 + digit2 + carry;

        result.push_back(sum % 10 + '0');

        carry = sum / 10;
    }

    if(carry){
        result.push_back(carry + '0');
    }

    reverse(result.begin(), result.end());

    return removeLeadingZeros(result);
}

string subtractBigNumbers(string num1, string num2){
    bool isNegative = false;

    if(num1.size() < num2.size() || (num1.size() == num2.size() && num1 < num2)){
        swap(num1, num2);
        isNegative = true;
    }

    int maxLength = max(num1.size(), num2.size());

    num1.insert(0, maxLength - num1.size(), '0');   // 10.22 修复: 原版逐位 "0"+s，位数差大时 O(n²)
    num2.insert(0, maxLength - num2.size(), '0');

    string result = "";

    int borrow = 0;

    for(int i = maxLength - 1; i >= 0; i--){
        int digit1 = num1[i] - '0';
        int digit2 = num2[i] - '0';

        digit1 -= borrow;

        borrow = 0;

        if(digit1 < digit2){
            digit1 += 10;
            borrow = 1;
        }

        int diff = digit1 - digit2;

        result.push_back(diff + '0');
    }

    reverse(result.begin(), result.end());

    result = removeLeadingZeros(result);

    if(isNegative && result != "0"){
        result = "-" + result;
    }

    return result;
}

string multiplyBigNumbers(string num1, string num2){
    if(num1 == "0" || num2 == "0") return "0";

    int n = num1.size();
    int m = num2.size();

    string result(n + m, '0');

    for(int i = n - 1; i >= 0; i--){
        for(int j = m - 1; j >= 0; j--){
            int mul = (num1[i] - '0') * (num2[j] - '0');

            int sum = (result[i + j + 1] - '0') + mul;

            result[i + j + 1] = sum % 10 + '0';

            result[i + j] += sum / 10;
        }
    }

    return removeLeadingZeros(result);
}

string divideBigNumbers(string num1, int divisor){
    string result = "";

    long long cur = 0;

    for(int i = 0; i < num1.size(); i++){
        cur = cur * 10 + (num1[i] - '0');

        result.push_back(cur / divisor + '0');

        cur %= divisor;
    }

    return removeLeadingZeros(result);
}

int main(){
    string a, b;

    cin >> a >> b;

    cout << addBigNumbers(a, b) << '\n';

    cout << subtractBigNumbers(a, b) << '\n';

    cout << multiplyBigNumbers(a, b) << '\n';

    cout << divideBigNumbers(a, stoi(b)) << '\n';

    return 0;
}
```

### 对拍（四件套，四个独立程序分别编译）


**data.cpp（数据生成）**

```cpp
#define int long long

mt19937_64 rd(random_device{}());

// 【速查】对拍·数据生成器 data.cpp
// 用法: rnd(l, r) 返回 [l, r] 的随机整数，按题目格式输出一组数据
int rnd(int l, int r){
    return l + rd() % (r - l + 1);
}

signed main(){
    cout << rnd(1, 100) << " " << rnd(1, 100);
    return 0;
}
```

**solve.cpp（正解，示例为 a+b）**

```cpp
int main(){
    int n, m;
    cin >> n >> m;
    cout << n + m;
}
```

**bf.cpp（暴力，示例为逐个累加）**

```cpp
int main(){
    int n, m;
    cin >> n >> m;
    int ans = 0;
    while(n--) ans++;
    while(m--) ans++;
    cout << ans;
}
```

**test.cpp（对拍器，Windows 用 fc，Linux 用 diff，二选一）**

```cpp
#define int long long


// 【速查】对拍·驱动 test.cpp
// 用法: 先分别编译 data / solve(待测) / bf(暴力)，再运行本程序；输出 WA 时 data.in 就是反例
//       Windows 用 data.exe + fc，Linux 换成注释里的 ./data + diff
signed main(){
    int t = 1000;
    for(int i = 1; i <= t; i++){

        /*
        system("./data > data.in");
        system("./solve < data.in > solve.out");
        system("./bf < data.in > bf.out");
        */

        system("data.exe > data.in");
        system("solve.exe < data.in > solve.out");
        system("bf.exe < data.in > bf.out");

        /*
        if(system("diff solve.out bf.out > /dev/null"))
        */
        if(system("fc solve.out bf.out > nul")){
            cout << "WA";
            break;
        }

        cout << i << " AC\n";

    }
    return 0;
}
```

### 基础 DP 合集（背包 / 区间 / 决策单调性 / 树形 / 状压）

> 【10.22 新增】01、完全、多重（二进制 / 单调队列）、分组背包；石子合并（含 Knuth 优化）；分治优化；树形 DP；TSP。

```cpp
using ll = long long;

// 【速查】背包合集（容量 W，物品 (重量 w, 价值 v)，求最大价值）
// knap01(W, items)            01 背包，每个物品最多 1 个。倒序枚举容量。O(nW)
// knapComplete(W, items)      完全背包，每个物品无限个。正序枚举容量。O(nW)
// knapMultiBinary(W, items3)  多重背包 (w, v, 个数 c)，二进制拆分成 01 背包。O(W Σlog c)
// knapMultiQueue(W, items3)   多重背包单调队列优化。O(nW)，c 很大时用它
// knapGroup(W, groups)        分组背包，每组最多选 1 个。O(W × 总物品数)
// 返回: dp[0..W]，dp[j] = 容量不超过 j 的最大价值（要"恰好装满"就把 dp 初值改为 -INF、dp[0]=0）
vector<ll> knap01(int W, const vector<pair<int, ll>>& items) {
    vector<ll> dp(W + 1, 0);
    for (auto [w, v] : items) for (int j = W; j >= w; j--) dp[j] = max(dp[j], dp[j - w] + v);
    return dp;
}
vector<ll> knapComplete(int W, const vector<pair<int, ll>>& items) {
    vector<ll> dp(W + 1, 0);
    for (auto [w, v] : items) for (int j = w; j <= W; j++) dp[j] = max(dp[j], dp[j - w] + v);
    return dp;
}
vector<ll> knapMultiBinary(int W, const vector<array<ll, 3>>& items) {
    vector<pair<int, ll>> split;
    for (auto [w, v, c] : items)
        for (ll k = 1; c > 0; k <<= 1) { ll t = min(k, c); split.push_back({(int)(w * t), v * t}); c -= t; }
    vector<pair<int, ll>> ok;
    for (auto& p : split) if (p.first <= W) ok.push_back(p);
    return knap01(W, ok);
}
vector<ll> knapMultiQueue(int W, const vector<array<ll, 3>>& items) {
    vector<ll> dp(W + 1, 0), pre;
    for (auto [w, v, c] : items) {
        if (w == 0) { for (auto& x : dp) x += v * c; continue; }
        pre = dp;
        for (int r = 0; r < w; r++) {               // 按 j mod w 分组，组内是滑动窗口最大值
            deque<pair<ll, ll>> q;                   // (k, pre[r + k w] - k v)
            for (ll k = 0; r + k * w <= W; k++) {
                ll val = pre[r + k * w] - k * v;
                while (!q.empty() && q.back().second <= val) q.pop_back();
                q.push_back({k, val});
                while (q.front().first < k - c) q.pop_front();
                dp[r + k * w] = q.front().second + k * v;
            }
        }
    }
    return dp;
}
vector<ll> knapGroup(int W, const vector<vector<pair<int, ll>>>& groups) {
    vector<ll> dp(W + 1, 0), pre;
    for (auto& g : groups) {
        pre = dp;                                    // 只从"上一组"的状态转移，保证每组只选一个（重量为 0 的物品也安全）
        for (int j = 0; j <= W; j++)
            for (auto [w, v] : g) if (w <= j) dp[j] = max(dp[j], pre[j - w] + v);
    }
    return dp;
}

// 【速查】区间 DP：石子合并（相邻两堆合并，代价为两堆之和，求最小总代价，P1775）
// stoneMerge(a)       朴素 O(n³)：dp[l][r] = min_k dp[l][k] + dp[k+1][r] + sum(l, r)
// stoneMergeKnuth(a)  四边形不等式优化 O(n²)：最优断点 opt[l][r-1] <= opt[l][r] <= opt[l+1][r]
// 环形: 把数组复制一份接在后面，取所有长度为 n 的区间的最小值
// 适用 Knuth 的条件: 代价 w 满足四边形不等式且区间包含单调（区间和、区间平方和等）
ll stoneMerge(const vector<ll>& a) {
    int n = a.size();
    if (n <= 1) return 0;
    vector<ll> s(n + 1, 0);
    for (int i = 0; i < n; i++) s[i + 1] = s[i] + a[i];
    vector<vector<ll>> dp(n, vector<ll>(n, 0));
    for (int len = 2; len <= n; len++)
        for (int l = 0; l + len - 1 < n; l++) {
            int r = l + len - 1; dp[l][r] = LLONG_MAX;
            for (int k = l; k < r; k++) dp[l][r] = min(dp[l][r], dp[l][k] + dp[k + 1][r]);
            dp[l][r] += s[r + 1] - s[l];
        }
    return dp[0][n - 1];
}
ll stoneMergeKnuth(const vector<ll>& a) {
    int n = a.size();
    if (n <= 1) return 0;
    vector<ll> s(n + 1, 0);
    for (int i = 0; i < n; i++) s[i + 1] = s[i] + a[i];
    vector<vector<ll>> dp(n, vector<ll>(n, 0));
    vector<vector<int>> opt(n, vector<int>(n, 0));
    for (int i = 0; i < n; i++) opt[i][i] = i;
    for (int len = 2; len <= n; len++)
        for (int l = 0; l + len - 1 < n; l++) {
            int r = l + len - 1; dp[l][r] = LLONG_MAX;
            for (int k = opt[l][r - 1]; k <= min(r - 1, opt[l + 1][r]); k++) {
                ll v = dp[l][k] + dp[k + 1][r];
                if (v < dp[l][r]) dp[l][r] = v, opt[l][r] = k;
            }
            dp[l][r] += s[r + 1] - s[l];
        }
    return dp[0][n - 1];
}

// 【速查】决策单调性 · 分治优化（把序列分成 k 段，最小化每段代价之和，CF321E / P4072 类）
// 用法: ll ans = divideConquerDP(n, k, cost);   // cost(j, i) = 第 j+1..i 个元素组成一段的代价（1-based 前缀意义）
// 条件: 最优断点随 i 单调不减（cost 满足四边形不等式即可，如 (段和)²、区间逆序对等）
// 复杂度: O(k n log n) 次 cost 调用（cost 要 O(1)，一般用前缀和）
ll divideConquerDP(int n, int k, const function<ll(int, int)>& cost) {
    const ll INF = LLONG_MAX / 4;
    vector<ll> prv(n + 1, INF), cur(n + 1, INF);
    prv[0] = 0;
    function<void(int, int, int, int)> solve = [&](int l, int r, int optl, int optr) {
        if (l > r) return;
        int m = (l + r) / 2, best = optl;
        ll bv = INF;
        for (int j = optl; j <= min(m - 1, optr); j++) {
            if (prv[j] >= INF) continue;
            ll v = prv[j] + cost(j, m);
            if (v < bv) bv = v, best = j;
        }
        cur[m] = bv;
        solve(l, m - 1, optl, best);
        solve(m + 1, r, best, optr);
    };
    for (int t = 1; t <= k; t++) {
        fill(cur.begin(), cur.end(), INF);
        solve(1, n, 0, n - 1);
        swap(prv, cur);
    }
    return prv[n];
}

// 【速查】树形 DP（迭代 DFS 序，不爆栈）
// treeMIS(n, edges, w)        带权最大独立集（没有上司的舞会 P1352）：选的点两两不相邻，点权和最大
// treeKnapsack(n, fa, val, m) 树上背包（选课 P2014）：选一个点必须先选它的父亲（fa[i]=0 表示可直接选），
//                              恰好选 m 个点的最大价值；O(n·m)（按子树大小合并）
// 点 1-based
ll treeMIS(int n, const vector<pair<int, int>>& edges, const vector<ll>& w) {
    vector<vector<int>> g(n + 1);
    for (auto [u, v] : edges) g[u].push_back(v), g[v].push_back(u);
    vector<int> order, fa(n + 1, 0);
    vector<char> vis(n + 1, 0);
    vector<array<ll, 2>> dp(n + 1, {0, 0});            // dp[u][0] 不选 u，dp[u][1] 选 u
    ll total = 0;
    for (int s = 1; s <= n; s++) {
        if (vis[s]) continue;
        order.clear(); order.push_back(s); vis[s] = 1;
        for (size_t i = 0; i < order.size(); i++)
            for (int v : g[order[i]]) if (!vis[v]) vis[v] = 1, fa[v] = order[i], order.push_back(v);
        for (int i = order.size() - 1; i >= 0; i--) {   // BFS 序倒过来 = 先儿子后父亲
            int u = order[i];
            dp[u][1] += w[u];
            if (fa[u]) { dp[fa[u]][0] += max(dp[u][0], dp[u][1]); dp[fa[u]][1] += dp[u][0]; }
        }
        total += max(dp[s][0], dp[s][1]);
    }
    return total;
}
ll treeKnapsack(int n, const vector<int>& fa, const vector<ll>& val, int m) {
    // 0 号点是虚根（必选，价值 0），答案 = dp[0][m + 1]
    const ll NEG = LLONG_MIN / 4;
    vector<vector<int>> ch(n + 1);
    for (int i = 1; i <= n; i++) ch[fa[i]].push_back(i);
    vector<int> order = {0};
    for (size_t i = 0; i < order.size(); i++) for (int c : ch[order[i]]) order.push_back(c);
    vector<vector<ll>> dp(n + 1);
    vector<int> sz(n + 1, 1);
    for (int i = order.size() - 1; i >= 0; i--) {
        int u = order[i];
        dp[u] = {NEG, u ? val[u] : 0};                // dp[u][j]: u 子树里选 j 个（含 u）的最大价值
        for (int c : ch[u]) {
            int lim = min(sz[u] + sz[c], m + 1);
            vector<ll> nd(lim + 1, NEG);
            for (int a = 1; a <= min(sz[u], m + 1); a++) if (dp[u][a] > NEG)
                for (int b = 0; b <= sz[c] && a + b <= lim; b++) {
                    ll vb = b == 0 ? 0 : dp[c][b];
                    if (vb > NEG) nd[a + b] = max(nd[a + b], dp[u][a] + vb);
                }
            sz[u] += sz[c];
            dp[u] = nd;
            dp[c].clear(); dp[c].shrink_to_fit();
        }
        if ((int)dp[u].size() > m + 2) dp[u].resize(m + 2);
    }
    return (int)dp[0].size() > m + 1 ? dp[0][m + 1] : NEG;
}

// 【速查】状压 DP：旅行商 TSP（从 0 出发经过所有点恰好一次再回到 0 的最短回路）
// 用法: ll ans = tsp(d);   // d 为 n×n 距离矩阵（可不对称），n ≤ 20；不可达用很大的数
// 改"路径"（不回起点）: 答案改成 min_i dp[full][i]
// 复杂度: O(2^n n²)，内存 2^n × n 个 long long
ll tsp(const vector<vector<ll>>& d) {
    int n = d.size();
    if (n == 1) return 0;
    const ll INF = LLONG_MAX / 4;
    vector<vector<ll>> dp(1 << n, vector<ll>(n, INF));
    dp[1][0] = 0;
    for (int mask = 1; mask < (1 << n); mask++) {
        if (!(mask & 1)) continue;
        for (int u = 0; u < n; u++) {
            if (dp[mask][u] >= INF || !(mask >> u & 1)) continue;
            for (int v = 0; v < n; v++) if (!(mask >> v & 1))
                dp[mask | 1 << v][v] = min(dp[mask | 1 << v][v], dp[mask][u] + d[u][v]);
        }
    }
    ll ans = INF;
    for (int u = 1; u < n; u++) ans = min(ans, dp[(1 << n) - 1][u] + d[u][0]);
    return ans;
}
```
