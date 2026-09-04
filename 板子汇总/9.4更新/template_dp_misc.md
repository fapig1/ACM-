# David_DP&杂项



[TOC]

### 斜率优化DP

```cpp
#define int long long
int mod=1e9+7;
int minProblem=1;
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
        while(cur+1<dp.size()&&!check(dp[cur],dp[cur+1],k)){
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
        while(cur+1<dp.size()&&!check(dp[cur],dp[cur+1],k)){
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
            while(cur+1<pdp.size()&&!check(pdp[cur],pdp[cur+1],k)){
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
```

### 三分

```cpp
#define int long long
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
```

### 高精度

```cpp
string removeLeadingZeros(string s){
    int pos = s.find_first_not_of('0');
    if(pos == string::npos) return "0";
    return s.substr(pos);
}

string addBigNumbers(string num1, string num2){
    int maxLength = max(num1.size(), num2.size());

    while(num1.size() < maxLength) num1 = "0" + num1;
    while(num2.size() < maxLength) num2 = "0" + num2;

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

    while(num1.size() < maxLength) num1 = "0" + num1;
    while(num2.size() < maxLength) num2 = "0" + num2;

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
```

### 对拍

```cpp
// 对拍模板
// 文件结构: data.cpp(数据生成), solve.cpp(正解), bf.cpp(暴力), test.cpp(对拍器)
// 编译脚本: dpW.bat(Windows), dpL.sh(Linux)

// === data.cpp ===
// 数据生成器
mt19937_64 rd(random_device{}());

int rnd(int l, int r){
    return l + rd() % (r - l + 1);
}

// 使用: cout << rnd(1, 100) << " " << rnd(1, 100);

// === solve.cpp ===
// 正解, 读入 n, m, 输出 n + m

// === bf.cpp ===
// 暴力解, 读入 n, m, 输出 ans

// === test.cpp ===
// 对拍器: 循环调用 data.exe, solve.exe, bf.exe, 比较输出
int t = 1000;
for(int i = 1; i <= t; i++){
    system("data.exe > data.in");
    system("solve.exe < data.in > solve.out");
    system("bf.exe < data.in > bf.out");
    if(system("fc solve.out bf.out > nul")){
        cout << "WA";
        break;
    }
    cout << i << " AC\n";
}

// === dpW.bat (Windows) ===
// g++ data.cpp -o data.exe
// g++ solve.cpp -o solve.exe
// g++ bf.cpp -o bf.exe
// g++ test.cpp -o test.exe
// test.exe

// === dpL.sh (Linux) ===
// g++ data.cpp -o data
// g++ solve.cpp -o solve
// g++ bf.cpp -o bf
// g++ test.cpp -o test
// ./test
```
