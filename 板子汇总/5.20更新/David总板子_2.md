# David总板子_2

主要包含**数论**、**字符串**、**计算几何**、**动态规划**、**杂项**等模板

---

[TOC]



## 数论

### 排列组合

```c++
//模逆元 排列数
vector<int> fac(maxn + 5), inv(maxn + 5);
int ksm(int x, int a, int mod){
    int ans = 1;
    while(a){
        if(a & 1) ans = ans * x % mod;
        x = x * x % mod;
        a >>= 1;
    }
    return ans;
}

void init(){
    fac[0] = 1;
    for(int i = 1; i <= maxn; i++){
        fac[i] = fac[i - 1] * i % mod;
    }
    inv[maxn] = ksm(fac[maxn], mod - 2, mod);
    for(int i = maxn - 1; i >= 0; i--){
        inv[i] = inv[i + 1] * (i + 1) % mod;
    }
}

int A(int n, int m){
    return fac[n] * inv[m] % mod;
}

int C(int n, int m){
    return fac[n] * inv[m] % mod * inv[n - m] % mod;
}

//错位排列Derangement
int Derangement(int n){
    vector<int> dp(n + 1);
    dp[1] = 0;
    dp[2] = 1;
    for(int i = 3; i <= n; i++){
        dp[i] = (n - 1) * (dp[i - 1] + dp[i - 2]) % mod;
    }
}

//卡特兰数CatalanNumber 解决不交叉问题的种类数
//有 n 个左括号与 n 个右括号。能组成多少个合法括号序列？
int CatalanNumber(int n){
    int ans = C(2 * n, n) - C(2 * n, n + 1);
    return (ans % mod + mod) % mod;
}

```



### 数论杂项

```c++
//欧拉筛
const int mod = 1e9 + 7;
const int maxn = 1e6;
vector<int> primes;
//欧拉筛 同时 预处理最小质因数 与 目标数分解质因数

int min_primes[maxn + 5];
void init(){
    for(int i = 2; i <= maxn; i++){
        if(!min_primes[i]){
            min_primes[i] = i;
            primes.push_back(i);
        }
        for(int p : primes){
            if(p > min_primes[i] || i * p > maxn) break;
            min_primes[i * p] = p;
        }
    }
}
//分解质因数
vector<int> getPrimes(int x){
    vector<int> res;
    while(x > 1){
        int p = min_primes[x];
        res.push_back(p);
        while(x % p == 0) x /= p;
    }
    return res;
}
//分解质因数带次数（幂）{p, cnt}
vector<pair<int,int>> getPrimesCnt(int x){
    vector<pair<int,int>> res;
    while(x > 1){
        int p = min_primes[x];
        int cnt = 0;
        while(x % p == 0){
            x /= p;
            cnt++;
        }
        res.push_back({p, cnt});
    }
    return res;
}


// 旧欧拉筛单独函数
// void init(){
//     vector<bool> nop(maxn + 5);
//     nop[0] = nop[1] = true;
//     for(int i = 2; i <= maxn; i++){
//         if(!nop[i]){
//             primes.push_back(i);
//             if(i > sqrt(maxn)) continue;
//             for(int j = i * i; j <= maxn; j++){
//                 nop[j] = true;
//             }
//         }
//     }
// }


int ksm(int x, int a, int m){
    int ans = 1;
    while(a){
        if(a & 1) ans = ans * x % m;
        x = x * x % m;
        a >>= 1;
    }
    return ans;
}


//扩展欧几里得算法
int exgcd(int a, int b, int &x, int &y){
    if(!b){
        x = 1;
        y = 0;
    }
    int d = exgcd(b, a % b, x, y);
    int t = x;
    x = y;
    y = t - (a / b) * y;
    return d;//最大公约数
}

//中国剩余定理(x === a (mod r) )
int CRT(int k, vector<int> a, vector<int> r){
    int n = 1, ans = 0;
    for(int i = 0; i < k; i++){
        n = n * r[i];
    }
    for(int i = 0; i < k; i++){
        int m = n / r[i], b, y;
        exgcd(m, r[i], b, y);
        ans = (ans + a[i] * m * b % n) % n;
    }
    return (ans % n + n) % n;
}

//扩展中国剩余定理
//求解方程组 x ≡ a[i] (mod m[i])
int excrt(vector<int> m, vector<int> a, int n){
    int M = m[0];
    int ans =  a[0];
    for(int i = 1; i < n; i++){
        int c = (a[i] - ans % m[i] + m[i]) % m[i];
        int x, y;
        int d = exgcd(M, m[i], x, y);
        if(c % d != 1) return -1;
        int k = m[i] % d;
        x = ksm(x, c / d, k);
        ans += x * M;
        M *= k;
        ans = (ans % M + M) % M;
    }
    return ans;
}

//一个数的欧拉函数
int oula_phi(int n){
    int ans = n;
    for(int i = 2; i * i <= n; i++){
        if(n % i == 0){
            ans = ans / i * (i - 1);
            while(n % i == 0) n /= i;
        }
        if(n > 1) ans = ans / n * (n - 1);
        return ans;
    }
}

//筛法求欧拉函数
vector<int> sieve_phi(int n) {
    vector<int> phi(n + 1);
    vector<bool> st(n + 1, false);
    vector<int> primes;
    
    phi[1] = 1; // 特殊情况：1的欧拉函数值为1[1,3,5](@ref)
    
    for (int i = 2; i <= n; i++) {
        if (!st[i]) { // i是质数
            primes.push_back(i);
            phi[i] = i - 1; // 质数的欧拉函数值为i-1[1,2](@ref)
        }
        
        for (int j = 0; primes[j] <= n / i; j++) {
            st[primes[j] * i] = true;
            
            if (i % primes[j] == 0) {
                // primes[j]是i的最小质因子
                phi[primes[j] * i] = phi[i] * primes[j]; // 情况1[2,5](@ref)
                break;
            } else {
                // primes[j]与i互质
                phi[primes[j] * i] = phi[i] * (primes[j] - 1); // 情况2[1,3](@ref)
            }
        }
    }
    
    return phi;
}

// 狄利克雷卷积快速幂 - 计算f的k次卷积幂
void dirichlet_power(const vector<int>& f, vector<int>& res, int n, int k) {
    // 初始化结果为狄利克雷卷积的单位元ε
    res.assign(n + 1, 0);
    res[1] = 1;  // ε(1)=1, ε(n)=0(n>1)
    
    vector<int> temp(f), base(f);
    
    while (k) {
        if (k & 1) {
            vector<int> new_res(n + 1, 0);
            // 计算res = res * base
            for (int i = 1; i <= n; i++) {
                for (int j = 1; i * j <= n; j++) {
                    new_res[i * j] = (new_res[i * j] + res[i] * base[j]) % mod;
                }
            }
            res = new_res;
        }
        
        // 计算base = base * base
        vector<int> new_base(n + 1, 0);
        for (int i = 1; i <= n; i++) {
            for (int j = 1; i * j <= n; j++) {
                new_base[i * j] = (new_base[i * j] + base[i] * base[j]) % mod;
            }
        }
        base = new_base;
        
        k >>= 1;
    }
}

// 阶乘表，通常n不会太大，可以预先计算好
// 实际使用时可以根据需要动态计算或传入
const int fact[10] = {1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880};

/**
 * 康托展开，求排列的字典序排第几，O(n^2)
 * @param permutation 排列，元素为0..n-1或1..n均可，但需是连续不重复的整数
 * @return 康托展开值（即比当前排列小的排列个数）
 */
int cantor_expand(const std::vector<int>& permutation) {
    int n = permutation.size();
    int code = 0;
    
    for (int i = 0; i < n; ++i) {
        int smaller = 0;  // 统计未出现的元素中有多少个比当前元素小
        for (int j = i + 1; j < n; ++j) {
            if (permutation[j] < permutation[i]) {
                smaller++;
            }
        }
        code += smaller * fact[n - i - 1];
    }
    
    return code;
}

/**
 * 逆康托展开
 * @param code 康托展开值（比目标排列小的排列个数）
 * @param n 排列长度
 * @param elements 可选的元素集合，默认使用0..n-1；如果是1..n则需传入对应集合
 * @return 对应的排列
 */
std::vector<int> cantor_expand_inv(int code, int n, const std::vector<int>& elements = {}) {
    std::vector<int> result;
    
    // 确定可用的元素集合
    std::vector<int> available;
    if (elements.empty()) {
        // 默认使用0..n-1
        for (int i = 0; i < n; ++i) {
            available.push_back(i);
        }
    } else {
        available = elements;
    }
    
    for (int i = n; i > 0; --i) {
        int fact_val = fact[i - 1];
        int index = code / fact_val;      // 确定应该取第几小的数
        code %= fact_val;                  // 更新余数
        
        result.push_back(available[index]);
        available.erase(available.begin() + index);
    }
    
    return result;
}


// FFT 需要用 double，所以局部不建议用全局的 int int 覆盖所有变量
struct Complex {
    double r, i;
    Complex(double r = 0, double i = 0) : r(r), i(i) {}
    Complex operator + (const Complex& t) const { return {r + t.r, i + t.i}; }
    Complex operator - (const Complex& t) const { return {r - t.r, i - t.i}; }
    Complex operator * (const Complex& t) const { return {r * t.r - i * t.i, r * t.i + i * t.r}; }
};

const double PI = acos(-1.0);
vector<int> rev; // 预处理位逆序

void fft_init(int len) {
    rev.assign(len, 0);
    for (int i = 0; i < len; i++)
        rev[i] = (rev[i >> 1] >> 1) | ((i & 1) ? (len >> 1) : 0);
}

void fft(vector<Complex>& a, int type) {
    int n = a.size();
    for (int i = 0; i < n; i++) if (i < rev[i]) swap(a[i], a[rev[i]]);
    for (int mid = 1; mid < n; mid <<= 1) {
        Complex wn(cos(PI / mid), type * sin(PI / mid));
        for (int i = 0; i < n; i += (mid << 1)) {
            Complex w(1, 0);
            for (int j = 0; j < mid; j++, w = w * wn) {
                Complex x = a[i + j], y = w * a[i + j + mid];
                a[i + j] = x + y;
                a[i + j + mid] = x - y;
            }
        }
    }
    if (type == -1) for (auto& x : a) x.r /= n;
}

// 多项式乘法封装
vector<int> multiply_fft(vector<int>& A, vector<int>& B) {
    int n = A.size(), m = B.size(), total = n + m - 1;
    int len = 1; while (len < total) len <<= 1;
    fft_init(len);
    vector<Complex> fa(len), fb(len);
    for (int i = 0; i < n; i++) fa[i].r = A[i];
    for (int i = 0; i < m; i++) fb[i].r = B[i];
    fft(fa, 1); fft(fb, 1);
    for (int i = 0; i < len; i++) fa[i] = fa[i] * fb[i];
    fft(fa, -1);
    vector<int> res(total);
    for (int i = 0; i < total; i++) res[i] = (int)(fa[i].r + 0.5);
    return res;
}

// // 假设输入两个多项式的系数
//     vector<int> a = {1, 2, 1}; // x^2 + 2x + 1
//     vector<int> b = {1, 1};    // x + 1
    
//     int n = a.size(), m = b.size();
//     int len = 1;
//     while (len < n + m - 1) len <<= 1; // 补齐到2的幂次

//     vector<Complex> fa(len), fb(len);
//     for (int i = 0; i < n; i++) fa[i] = Complex(a[i], 0);
//     for (int i = 0; i < m; i++) fb[i] = Complex(b[i], 0);

//     fft(fa, len, 1);
//     fft(fb, len, 1);
    
//     // 点值相乘
//     for (int i = 0; i < len; i++) fa[i] = fa[i] * fb[i];

//     fft(fa, len, -1);

//     // 输出结果 (四舍五入)
//     for (int i = 0; i < n + m - 1; i++) {
//         cout << (int)(fa[i].r + 0.5) << " ";
//     }
//     return 0;


const int NTT_MOD = 998244353;
const int G = 3;   // 原根
const int GI = 332748118; // 原根的逆元

// 借用你原本的 ksm，但注意模数要用 NTT_MOD
int ntt_ksm(int a, int b) {
    int res = 1; a %= NTT_MOD;
    while (b) {
        if (b & 1) res = res * a % NTT_MOD;
        a = a * a % NTT_MOD;
        b >>= 1;
    }
    return res;
}

void ntt(vector<int>& a, int type) {
    int n = a.size();
    for (int i = 0; i < n; i++) if (i < rev[i]) swap(a[i], a[rev[i]]);
    for (int mid = 1; mid < n; mid <<= 1) {
        int wn = ntt_ksm(type == 1 ? G : GI, (NTT_MOD - 1) / (mid << 1));
        for (int i = 0; i < n; i += (mid << 1)) {
            int w = 1;
            for (int j = 0; j < mid; j++, w = w * wn % NTT_MOD) {
                int x = a[i + j], y = w * a[i + j + mid] % NTT_MOD;
                a[i + j] = (x + y) % NTT_MOD;
                a[i + j + mid] = (x - y + NTT_MOD) % NTT_MOD;
            }
        }
    }
    if (type == -1) {
        int inv_n = ntt_ksm(n, NTT_MOD - 2);
        for (int& x : a) x = x * inv_n % NTT_MOD;
    }
}

// NTT 多项式乘法封装
vector<int> multiply_ntt(vector<int> A, vector<int> B) {
    int n = A.size(), m = B.size(), total = n + m - 1;
    int len = 1; while (len < total) len <<= 1;
    fft_init(len); // rev数组计算逻辑与FFT一致
    A.resize(len); B.resize(len);
    ntt(A, 1); ntt(B, 1);
    for (int i = 0; i < len; i++) A[i] = A[i] * B[i] % NTT_MOD;
    ntt(A, -1);
    A.resize(total);
    return A;
}

// // 多项式 A: 1 + 2x + x^2
//     vector<int> A = {1, 2, 1};
//     // 多项式 B: 1 + x
//     vector<int> B = {1, 1};

//     // 直接调用封装好的乘法函数
//     vector<int> C = multiply_ntt(A, B);  
//     vector<int> D = multiply_fft(A, B);

//     for (int x : C) {
//         cout << x << " ";
//     }
//     cout << endl;
//     // 输出预期: 1 3 3 1


// 类欧几里得算法 log求和 sum = i(0 - n-1) floor [(a*i + b) / c]
// 求i从0到n-1的ai+b/c向下取整的总和
// n >= 0, m > 0, a >= 0, b >= 0
int floor_sum(int n, int m, int a, int b) {
    int ans = 0;
    if (a >= m) {
        ans += (n - 1) * n / 2 * (a / m);
        a %= m;
    }
    if (b >= m) {
        ans += n * (b / m);
        b %= m;
    }

    int y_max = (a * n + b) / m;
    int x_max = (y_max * m - b);
    if (y_max == 0) return ans;

    ans += (n - (x_max + a - 1) / a) * y_max;
    ans += floor_sum(y_max, a, m, (a - x_max % a) % a);
    return ans;
}

//SOSDP
int main() {
    int n = 50;
    int total_states = (1 << n);
    vector<int> dp(total_states, 0); // dp数组初始化，根据题意可能初始为A[i]

    // SOS DP 计算超集和
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < total_states; ++j) {
            if ((j >> i) & 1) { // 如果状态j的第i位是1
                dp[j] += dp[j ^ (1 << i)]; // 累加不含第i位的状态的值
            }
        }
    }

    // 此时dp[mask]存储了所有满足 (i & mask) == mask 的A[i]之和
    return 0;
}
```



### 线性代数

```c++
const int mod = 1e9 + 7;

//线性基LinearBasis
vector<int> p(64);
bool insert(int x){
    for(int i = 63; i >= 0; i--){
        if(!(x >> i)) continue;
        if(!p[i]){
            p[i] = x;
            return true;
        }
        x ^= p[i];
    }
    return false;
}

//矩阵 (0-based)
struct mat{
    int n, m;
    vector<vector<int>> a;
    mat(int x = 0, int y = 0){
        n = x;
        m = y;
        a.resize(x, vector<int> (y));
    }

    mat operator+ (const mat& b){
        mat c(n, m);
        for(int i = 0; i < n; i++){
            for(int j = 0; j < m; j++){
                c.a[i][j] = (a[i][j] + b.a[i][j]) % mod;
            }
        }
        return c;
    } 

    mat operator- (const mat& b){
        mat c(n, m);
        for(int i = 0; i < n; i++){
            for(int j = 0; j < m; j++){
                c.a[i][j] = ((a[i][j] - b.a[i][j]) % mod + mod) % mod;
            }
        }
        return c;
    } 

    mat operator* (const mat& b){
        mat c(n, b.m);
        for(int i = 0; i < n; i++){
            for(int j = 0; j < b.m; j++){
                for(int k = 0; k < m; k++){
                    c.a[i][j] = (c.a[i][j] + a[i][k] * b.a[k][j]) % mod;
                }
            }
        }
        return c;
    } 

    mat transpose(){
        mat res(m, n);
        for(int i = 0; i < n; i++){
            for(int j = 0; j < m; j++){
                res.a[j][i] = a[i][j];
            }
        }
        return res;
    }

    void print(){
        for(int i = 0; i < n; i++){
            for(int j = 0; j < m; j++){
                cout << a[i][j] << " ";
            }
            cout << '\n';
        }
    }

};

mat ksm(mat x, int a){
    int n = x.a.size();
    mat res(n, n);
    for(int i = 0; i < n; i++){
        res.a[i][i] = 1;
    }
    while(a){
        if(a & 1) res = res * x;
        x = x * x;
        a >>= 1;
    }
    return res;
}

```



## 字符串

### 字典树

```c++
const int N=3e6+10;
const int base=13331;
const int mod=1e9+7;
int n,_T,q;
string s;
int ch[N][70]={0};
int idx;
int cnt[N];

int hashn(char x)
{
  if(x>='A'&&x<='Z')
  {
    return x-'A';
  }
  else if(x>='a'&&x<='z')
  {
    return x-'a'+26;
  }
  else 
  return x-'0'+52;
}

void insert(string s)
{
  int p=0;
  int len=s.length();
  for(int i=0;i<len;i++)
  {
    int j=hashn(s[i]);
    if(!ch[p][j]) ch[p][j]=++idx;
    p=ch[p][j];
    cnt[p]++;
  }
  
}

int query(string s)
{
  int p=0;
  int len=s.length();
  for(int i=0;i<len;i++)
  {
    int j=hashn(s[i]);
    if(!ch[p][j]) return 0;
    p=ch[p][j];
  }
  return cnt[p];
}

void solve()
{
  idx=0;
  cin>>n>>q;
  for(int i=1;i<=n;i++)
  {
    cin>>s;
    insert(s);
  }
  while(q--)
  {
    cin>>s;
    cout<<query(s)<<endl;
  }
  for(int i=0;i<=idx;i++)
  {
    for(int j=0;j<=65;j++)
    {
      ch[i][j]=0;
    }
  }
  for(int i=0;i<=idx;i++)
  {
    cnt[i]=0;
  }
}

```



### AC自动机

```c++
const int N=(1e4+10)*55;
const int M=1e6+10;
int n,k,_n,T,idx;
char s[M];
int cnt[N];
int ne[N];
int ch[N][26];

void insert()
{
    int p=0;
    for(int i=0;s[i];i++)
    {
        int j=s[i]-'a';
        if(!ch[p][j]) ch[p][j]=++idx;
        p=ch[p][j];
    }
    cnt[p]++;
}

void build()
{
    queue<int> q;
    for(int i=0;i<26;i++)
    {
        if(ch[0][i]) q.push(ch[0][i]);
    }
    while(!q.empty())
    {
        int u=q.front();
        q.pop();
        for(int i=0;i<26;i++)
        {
            int v=ch[u][i];
            if(v) ne[v]=ch[ne[u]][i],q.push(v);
            else ch[u][i]=ch[ne[u]][i];
        }
    }
}

int query()
{
    int ans=0;
    for(int k=0,i=0;s[k];k++)
    {
        i=ch[i][s[k]-'a'];
        for(int j=i;j&&~cnt[j];j=ne[j])
        {
            ans+=cnt[j];
            cnt[j]=-1;
        }
    }
    return ans;
}


void solve()
{
    idx=0;
  memset(cnt,0,sizeof cnt);
  memset(ne,0,sizeof ne);
  memset(ch,0,sizeof ch);
  cin>>n;
  for(int i=1;i<=n;i++)
  {
      cin>>s;
      insert();
  }
  build();
  cin>>s;
  cout<<query()<<endl;
}

```



### 马拉车求最长回文串

```c++
{
  d[1]=1;
  for(int i=2,l=0,r=1;i<=n;i++)
  {
    if(i<=r) d[i]=min(r-i+1,d[r-i+l]);
    while(s[i-d[i]]==s[i+d[i]]) d[i]++;
    if(i+d[i]-1>r) l=i-d[i]+1,r=i+d[i]-1;
  }
}

```



### KMP

```c++
const int N=1e6+10;
int n,m;
char s1[N],s2[N];
string s,p;
int ne[N];

signed main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
  
    cin>>s>>p;
    int m=s.length(),n=p.length();
    ne[1]=0;
    for(int i=1;i<=m;i++)
    {
      s1[i]=s[i-1];
    }
    for(int i=1;i<=n;i++)
    {
      s2[i]=p[i-1];
    }
    for(int i=2,j=0;i<=n;i++)
    {
      while(j&&s2[i]!=s2[j+1]) j=ne[j];
      if(s2[i]==s2[j+1]) j++;
      ne[i]=j;
    }
    for(int i=1,j=0;i<=m;i++)
    {
      while(j&&s1[i]!=s2[j+1]) j=ne[j];
      if(s1[i]==s2[j+1]) j++;
      if(j==n) 
      {
        cout<<i-n+1<<endl;
        j=ne[j];
      }
    }

    for(int i=1;i<=n;i++)
    {
      cout<<ne[i]<<' ';
    }

    return 0;
}
```



 ### 字符串哈希

```c++
//字符串哈希
struct StringHash {
    static const int MAXN = 1e6 + 5;
    static const int BASE = 277;
    static const int MOD1 = 1e9 + 7;
    static const int MOD2 = 1e9 + 9;

    inline static vector<int> p1, p2;
    vector<int> h1, h2;
    int n;

    // 初始化静态成员数组 p1 和 p2
    static void get_power() {
        static bool initialized = false;
        if (!initialized) {
            p1.assign(MAXN, 0);
            p2.assign(MAXN, 0);
            p1[0] = 1;
            p2[0] = 1;
            for (int i = 1; i < MAXN; i++) {
                p1[i] = p1[i - 1] * BASE % MOD1;
                p2[i] = p2[i - 1] * BASE % MOD2;
            }
            initialized = true;
        }
    }

    // 构造函数：传入字符串，自动计算前缀哈希
    StringHash(const string& s) {
        get_power();
        n = s.size();
        h1.assign(n + 1, 0);
        h2.assign(n + 1, 0);
        for (int i = 0; i < n; i++) {
            h1[i + 1] = (h1[i] * BASE + s[i]) % MOD1;
            h2[i + 1] = (h2[i] * BASE + s[i]) % MOD2;
        }
    }

    // 获取子串 [l, r] 的双哈希值 (0-indexed)
    // 例如 s = "abcde", get(1, 3) 返回 "bcd" 的哈希
    pair<int, int> get(int l, int r) {
        int len = r - l + 1;
        int res1 = (h1[r + 1] - h1[l] * p1[len] % MOD1 + MOD1) % MOD1;
        int res2 = (h2[r + 1] - h2[l] * p2[len] % MOD2 + MOD2) % MOD2;
        return {res1, res2};
    }
};
/*
    使用示例
    string s1;
    string s2;
    StringHash hash1(s1);
    StringHash hash2(s2);

    pair<int,int> get1=hash1.get(1,2);
    pair<int,int> get2=hash2.get(5,6);

    //判断字符串s1的[1,2]与字符串s2的[5,6]是否完全匹配
    if(get1.first==get2.first&&get1.second==get2.second){
        cout<<"Yes\n";
    }else{
        cout<<"No\n";
    }
*/

struct MatrixHash {
    static const int MAXN = 1005; // 最大行数
    static const int MAXM = 1005; // 最大列数
    static const int BASE_R = 13331; // 行基数
    static const int BASE_C = 277;   // 列基数
    static const int MOD1 = 1e9 + 7;
    static const int MOD2 = 1e9 + 9;

    // 静态成员数组：行幂次与列幂次
    inline static vector<int> pr1, pr2, pc1, pc2;
    
    // 二维前缀哈希表
    vector<vector<int>> h1, h2;
    int R, C;

    // 初始化静态幂次数组
    static void get_power() {
        static bool initialized = false;
        if (!initialized) {
            pr1.assign(MAXN, 0); pr2.assign(MAXN, 0);
            pc1.assign(MAXM, 0); pc2.assign(MAXM, 0);
            pr1[0] = pr2[0] = pc1[0] = pc2[0] = 1;
            
            for (int i = 1; i < MAXN; i++) {
                pr1[i] = 1LL * pr1[i - 1] * BASE_R % MOD1;
                pr2[i] = 1LL * pr2[i - 1] * BASE_R % MOD2;
            }
            for (int i = 1; i < MAXM; i++) {
                pc1[i] = 1LL * pc1[i - 1] * BASE_C % MOD1;
                pc2[i] = 1LL * pc2[i - 1] * BASE_C % MOD2;
            }
            initialized = true;
        }
    }

    // 注意传入数组是 字符串 还是 int ！！！！
    // 构造函数：传入 vector<string> 或 vector<vector<int>>
    MatrixHash(const vector<string>& mat) {
        get_power();
        R = mat.size();
        C = mat[0].size();
        h1.assign(R + 1, vector<int>(C + 1, 0));
        h2.assign(R + 1, vector<int>(C + 1, 0));

        for (int i = 0; i < R; i++) {
            for (int j = 0; j < C; j++) {
                // 二维哈希递推公式：H[i][j] = H[i-1][j]*Br + H[i][j-1]*Bc - H[i-1][j-1]*Br*Bc + val
                h1[i + 1][j + 1] = (1LL * h1[i][j + 1] * BASE_R % MOD1 + 
                                    1LL * h1[i + 1][j] * BASE_C % MOD1 - 
                                    1LL * h1[i][j] * BASE_R % MOD1 * BASE_C % MOD1 + 
                                    mat[i][j] + MOD1) % MOD1;

                h2[i + 1][j + 1] = (1LL * h2[i][j + 1] * BASE_R % MOD2 + 
                                    1LL * h2[i + 1][j] * BASE_C % MOD2 - 
                                    1LL * h2[i][j] * BASE_R % MOD2 * BASE_C % MOD2 + 
                                    mat[i][j] + MOD2) % MOD2;
            }
        }
    }

    // 获取子矩阵 [(r1, c1), (r2, c2)] 的双哈希值 (0-indexed)！！！
    pair<int, int> get(int r1, int c1, int r2, int c2) {
        int dr = r2 - r1 + 1; // 行高
        int dc = c2 - c1 + 1; // 列宽

        // 提取公式：H = h[r2][c2] - h[r1-1][c2]*Br^dr - h[r2][c1-1]*Bc^dc + h[r1-1][c1-1]*Br^dr*Bc^dc
        int res1 = (h1[r2 + 1][c2 + 1] - 1LL * h1[r1][c2 + 1] * pr1[dr] % MOD1 + MOD1) % MOD1;
        res1 = (res1 - 1LL * h1[r2 + 1][c1] * pc1[dc] % MOD1 + MOD1) % MOD1;
        res1 = (res1 + 1LL * h1[r1][c1] * pr1[dr] % MOD1 * pc1[dc] % MOD1) % MOD1;

        int res2 = (h2[r2 + 1][c2 + 1] - 1LL * h2[r1][c2 + 1] * pr2[dr] % MOD2 + MOD2) % MOD2;
        res2 = (res2 - 1LL * h2[r2 + 1][c1] * pc2[dc] % MOD2 + MOD2) % MOD2;
        res2 = (res2 + 1LL * h2[r1][c1] * pr2[dr] % MOD2 * pc2[dc] % MOD2) % MOD2;

        return {res1, res2};
    }
};

```





### 字符串常用库函数

```c++
#include <cstring>

char str1[20] = "hello";
char str2[20] = "world";

// 字符串长度
size_t len = strlen(str1);           // 5

// 字符串拷贝
strcpy(str1, "new");                 // str1 = "new"
strncpy(str1, "hello", 3);           // 安全拷贝

// 字符串连接
strcat(str1, " world");              // str1 = "hello world"
strncat(str1, "!!!", 2);             // 安全连接

// 字符串比较
int cmp = strcmp(str1, str2);        // 返回0相等，<0小于，>0大于
int cmp_n = strncmp(str1, str2, 3);  // 比较前n个字符

// 字符串查找
char* pos = strchr(str1, 'l');       // 查找字符第一次出现
char* pos2 = strrchr(str1, 'l');     // 查找字符最后一次出现
char* substr = strstr(str1, "world"); // 查找子串

// 内存操作（常用于字符串）
memset(str1, 0, sizeof(str1));       // 清空
memcpy(str2, str1, len);             // 内存拷贝
memmove(str2, str1, len);            // 安全内存移动


#include <cctype>

char ch = 'A';

// 字符分类
isalpha(ch);    // 是否是字母
isdigit(ch);    // 是否是数字
isalnum(ch);    // 是否是字母或数字
isspace(ch);    // 是否是空白字符
isupper(ch);    // 是否是大写字母
islower(ch);    // 是否是小写字母
ispunct(ch);    // 是否是标点符号

// 字符转换
tolower(ch);    // 转小写 → 'a'
toupper(ch);    // 转大写 → 'A'

// 实际应用：字符串处理
string s = "Hello World 123!";
for (char& c : s) {
    if (isupper(c)) c = tolower(c);  // 转小写
}


#include <string>
using namespace std;

string s = "hello world";

// 查找操作
size_t pos = s.find("world");        // 查找子串，返回位置
pos = s.find('o');                   // 查找字符
pos = s.rfind('o');                  // 从后往前查找
pos = s.find_first_of("aeiou");      // 查找任何匹配字符
pos = s.find_last_of("aeiou");       // 从后往前查找任何匹配字符
pos = s.find_first_not_of("helo ");  // 查找第一个不匹配字符

// 子串操作
string sub = s.substr(6, 5);         // 从位置6开始取5个字符 → "world"
s.substr(6);                         // 从位置6到结尾 → "world"

// 修改操作
s.insert(5, " beautiful");           // 插入 → "hello beautiful world"
s.erase(5, 10);                      // 删除 → "hello world"
s.replace(6, 5, "everyone");         // 替换 → "hello everyone"

// 追加和连接
s.append("!!!");                     // 追加 → "hello world!!!"
s += "!!!";                          // 同样效果
s.push_back('!');                    // 追加单个字符

// 比较
int result = s.compare("hello");     // 比较字符串
bool equal = (s == "hello world");   // 直接比较

// 数值转换
string num_str = to_string(123);     // 数字转字符串 → "123"
int num = stoi("456");               // 字符串转int
double d = stod("3.14");             // 字符串转double
long l = stol("1000000");            // 字符串转long



#include <sstream>
#include <string>
using namespace std;

// 字符串分割
string s = "apple,banana,orange";
stringstream ss(s);
string token;
while (getline(ss, token, ',')) {
    cout << token << endl;  // 输出: apple banana orange
}

// 数字和字符串转换
stringstream ss2;
ss2 << "年龄: " << 25 << " 分数: " << 95.5;
string result = ss2.str();  // "年龄: 25 分数: 95.5"

// 从字符串提取数字
string data = "100 200 300";
stringstream ss3(data);
int a, b, c;
ss3 >> a >> b >> c;  // a=100, b=200, c=300



#include <bits/stdc++.h>
using namespace std;

// 快速字符串处理函数
void string_demo() {
    string s = "Hello World";
    
    // 1. 删除所有空格
    s.erase(remove(s.begin(), s.end(), ' '), s.end());
    cout << s << endl;  // "HelloWorld"
    
    // 2. 字符串转小写
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    cout << s << endl;  // "helloworld"
    
    // 3. 字符串分割（简单版）
    string data = "apple,banana,orange";
    replace(data.begin(), data.end(), ',', ' ');
    stringstream ss(data);
    string token;
    while (ss >> token) {
        cout << token << endl;
    }
    
    // 4. 检查回文
    string palindrome = "racecar";
    bool is_pal = equal(palindrome.begin(), palindrome.end(), palindrome.rbegin());
    cout << (is_pal ? "是回文" : "不是回文") << endl;
    
    // 5. 字符串反转
    reverse(s.begin(), s.end());
    cout << s << endl;  // "dlrowolleh"
}


#include <string>
#include <algorithm>

// 自定义便捷函数
inline string to_lower(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

inline string to_upper(string s) {
    transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}

inline string trim(const string& s) {
    auto start = s.find_first_not_of(" \t\n\r");
    auto end = s.find_last_not_of(" \t\n\r");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

inline vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        if (!token.empty()) tokens.push_back(token);
    }
    return tokens;
}
```





## 计算几何

### 2D

```c++
//注意所有有关vector数组都不可开过大如 n+5，必须是有多少点n就是多少
//注意所有有关vector数组都不可开过大如 n+5，必须是有多少点n就是多少
//注意所有有关vector数组都不可开过大如 n+5，必须是有多少点n就是多少
//注意所有有关vector数组都不可开过大如 n+5，必须是有多少点n就是多少
//注意所有有关vector数组都不可开过大如 n+5，必须是有多少点n就是多少
//注意所有有关vector数组都不可开过大如 n+5，必须是有多少点n就是多少

const double PI = acos(-1), eps = 1e-9;

int sgn(double x) {
    if (fabs(x) < eps) return 0;
    return x < 0 ? -1 : 1;
}

struct Pt{
    double x, y;
    Pt(double x = 0.0, double y = 0.0) : x(x), y(y) {}
    
    Pt operator- (const Pt& b) const {return Pt(x - b.x, y - b.y);}

    Pt operator+ (const Pt& b) const {return Pt(x + b.x, y + b.y);}

    double operator% (const Pt& b) const {return x * b.y - y * b.x;}

    Pt operator* (const double b) const {return Pt(x * b, y * b);}

    double operator* (const Pt& b) const {return x * b.x + y * b.y;}

    Pt operator/ (const double b) const {return Pt(x / b, y / b);}

    bool operator< (const Pt& b) const {return x < b.x || (x == b.x && y < b.y);}

    bool operator== (const Pt& b) const {return sgn(x - b.x) == 0 && sgn(y - b.y) == 0;}

    bool operator!= (const Pt& b) const {return sgn(x - b.x) != 0 || sgn(y - b.y) != 0;}
};

double dis(Pt a){
    return sqrt(a * a);
}

double ddis(Pt a){
    return a * a;
}

double Angle(Pt a, Pt b){
    double val = a * b / dis(a) / dis(b);
    return acos(max(-1.0, min(1.0, val))); 
}

//在直线的哪边
// 点在直线上, 返回 0 (三点共线)
// 点在直线的逆时针方向, 返回 1
// 点在直线的顺时针方向, 返回 -1
//直线ab, 点c
int Cross(Pt a, Pt b, Pt c){
    return sgn((b - a) % (c - a));
}


double cross(Pt a, Pt b, Pt c){
    return (b - a) % (c - a);
}

struct Line {
    Pt s, e;
    double ang;
    Line() {}
    Line(Pt x, Pt y) : s(x), e(y) {
        ang = atan2(e.y - s.y, e.x - s.x);
    }

    // 检查点 P 是否在向量 se 的右侧（非法区域）
    // 使用叉积判断：(e-s) ^ (P-s) < 0 则在右侧
    bool onRight(const Pt& P) const {
        return sgn((e - s) % (P - s)) < 0;
    }

    // 极角排序：极角相同则保留靠左的那条
    bool operator< (const Line& b) const {
        if (sgn(ang - b.ang) != 0) return ang < b.ang;
        return sgn((e - s) % (b.e - s)) > 0;
    }
};


int Cross(Pt c, Line l){
    Pt a = l.s, b = l.e;
    return sgn((b - a) % (c - a));
}

//三点共线
bool In_one_line(Pt a, Pt b, Pt c){
    return !sgn((b - a) % (c - a));
}
//点到直线距离
double dist_ptl(Pt p, Pt a, Pt b){
    Pt v1 = p - a, v2 = b - a;
    return fabs((v1 % v2) / dis(v2));
}
//点到线段距离
double dist_pts(Pt p, Pt a, Pt b){
    if(a == b) return dis(p - a);
    Pt v1 = b - a, v2 = p - a, v3 = p - b;
    if(sgn(v1 * v2) < 0) return dis(p - a);
    if(sgn(v1 * v3) > 0) return dis(p - b);
    return fabs((v1 % v2) / dis(v1));
}
//点在线段上
bool OnSegment(Pt p, Pt a, Pt b){
    Pt pa = a - p, pb = b - p;
    return sgn(pa % pb) == 0 && sgn(pa * pb) <= 0;
}
//直线与线段相交(线段ab,直线cd)
bool Intersect_line_seg(Pt a, Pt b, Pt c, Pt d){
    return Cross(a, b, c) * Cross(a, b, d) <= 0;
}
//线段与线段是否相交
bool Intersect_seg(Pt a, Pt b, Pt c, Pt d){
    if (OnSegment(a, c, d) || OnSegment(b, c, d) || OnSegment(c, a, b) || OnSegment(d, a, b)) return 1;
    if (Cross(a, b, c) * Cross(a, b, d) >= 0) return 0;
    if (Cross(c, d, a) * Cross(c, d, b) >= 0) return 0;
    return 1;
}
//线段到线段距离
double dist_sts(Pt a, Pt b, Pt c, Pt d){
    if(Intersect_seg(a, b, c, d)){
        return 0.0;
    }
    return min({
        dist_pts(a, c, d),
        dist_pts(b, c, d),
        dist_pts(c, a, b),
        dist_pts(d, a, b),
    });
}
//直线平行
bool Line_parallel(Line A, Line B){
    return sgn((A.s - A.e) % (B.s - B.e)) == 0;
}
//直线交点（四个点）
Pt Inter_Line_Pt(Pt a, Pt b, Pt c, Pt d){
    Pt u = b - a, v = d - c;
    double t = ((a - c) % v) / (v % u);
    return a + u * t;
}

// 直线交点：输入两条 Line 对象（方便半平面交调用）
Pt Inter_Line_Pt(Line a, Line b) {
    Pt u = a.s - b.s;
    Pt v1 = a.e - a.s;
    Pt v2 = b.e - b.s;
    double t = (v2 % u) / (v1 % v2);
    return a.s + v1 * t;
}

//半平面交
//保留每一条直线的左侧区域
//!!!注意，设置边界的时候，limit一定要比eps大一点，不然可能卡精度，例如limit=1e-11,eps=1e-18
// L.push_back(Line(Pt(-INF, limit), Pt(-limit, limit), 0));       
// L.push_back(Line(Pt(-limit, limit), Pt(-limit, INF), 0));         
// L.push_back(Line(Pt(-limit, INF), Pt(-INF, INF), 0));    
// L.push_back(Line(Pt(-INF, INF), Pt(-INF, limit), 0));
vector<Pt> getHalfPlaneIntersection(vector<Line>& L) {
    // 1. 排序
    sort(L.begin(), L.end());

    // 2. 去重（对于极角相同的直线，只保留排序后第一条，即最靠左的那条）
    vector<Line> ql;
    for (int i = 0; i < L.size(); i++) {
        if (i == 0 || sgn(L[i].ang - L[i-1].ang) != 0) {
            ql.push_back(L[i]);
        }
    }

    // 3. 双端队列求解
    deque<Line> dq;
    deque<Pt> pts; // pts[i] 是 dq[i] 和 dq[i+1] 的交点

    dq.push_back(ql[0]);
    for (int i = 1; i < ql.size(); i++) {
        // 如果队尾的交点在当前直线的右侧（非法），弹出队尾
        while (!pts.empty() && ql[i].onRight(pts.back())) {
            pts.pop_back();
            dq.pop_back();
        }
        // 如果队首的交点在当前直线的右侧（非法），弹出队首
        // 这一步是为了处理“环绕一圈后覆盖了起始点”的情况
        while (!pts.empty() && ql[i].onRight(pts.front())) {
            pts.pop_front();
            dq.pop_front();
        }
        
        dq.push_back(ql[i]);
        // 计算新加入直线与上一条直线的交点
        if (dq.size() > 1) {
            pts.push_back(Inter_Line_Pt(dq[dq.size() - 2], dq.back()));
        }
    }

    // 4. 收尾检查：队首的直线可能会切掉队尾的点
    while (!pts.empty() && dq.front().onRight(pts.back())) {
        pts.pop_back();
        dq.pop_back();
    }
    
    // 如果队列中直线少于3条，无法构成多边形
    if (dq.size() < 3) return {};

    // 5. 计算首尾交点，封闭多边形
    pts.push_back(Inter_Line_Pt(dq.back(), dq.front()));

    // 将 deque 转为 vector 返回
    vector<Pt> res;
    for (auto p : pts) res.push_back(p);
    return res;
}

//  ------ 多边形 -------
// 三角形面积
double Triangle_area(Pt A, Pt B, Pt C){
    return fabs((B - A) % (C - A)) / 2;
}
//凸多边形面积
// 因为叉积求得的三角形面积是有向的, 在外面的面积可以正负抵消掉
// 所以能够求任意多边形面积(凸, !凸)
// p[]下标从 0 开始, 长度为 n
double area(vector<Pt>& p){
    int n = p.size();
    double S = 0;
    for(int i = 1; i <= n - 2; i++){
        S += (p[i] - p[0]) % (p[i + 1] - p[0]);
    }
    return fabs(S / 2);// 无向面积
    //return S / 2;//有向面积
}

//点在多边形内(扫描线)
// 适用于任意多边形, 不用考虑精度误差和多边形的给出顺序
// 点在多边形边上, 返回 -1
// 点在多边形内, 返回 1
// 点在多边形外, 返回 0

// p[] 的下标从 0 开始, 长度为 n
int InPolygon(Pt P, vector<Pt>& p) {
    int n = p.size();
    bool flag = false;		
    for (int i = 0, j = n - 1; i < n; j = i++) {
        Pt p1 = p[i], p2 = p[j];
        if (OnSegment(P, p1, p2)) return -1;
        if (sgn(P.y - p1.y) > 0 == sgn(P.y - p2.y) > 0) continue;
        if (sgn((P.y - p1.y) * (p1.x - p2.x) / (p1.y - p2.y) + p1.x - P.x) > 0) 
            flag = !flag;
    }
    return flag;
}
//是否为凸多边形
bool Is_contex(vector<Pt>& p){
    int n = p.size();
    bool s[3] = {0, 0, 0};
    for (int i = 0, j = n - 1, k = n - 2; i < n; k = j, j = i++) {
        int cnt = sgn((p[i] - p[j]) % (p[k] - p[j])) + 1;
        s[cnt] = true;
        if (s[0] && s[2]) return false;
    }
    return true;
}


// ------ 圆 -------
struct Circle{
    Pt o;
    double r;
    Circle(Pt o = Pt(), double r = 0) : o(o), r(r) {}

    double S(){return PI * r * r;}
    double C(){return PI * 2 * r;}
};
//扇形面积
double SectorArea(Pt A, Pt B, double R){
    double angle = Angle(A, B);
    if(sgn(A % B) < 0) angle = -angle;
    return R * R * angle / 2;
}
//点和圆的位置关系
// 点在圆上, 返回 0
// 点在圆外, 返回 -1
// 点在圆内, 返回 1
int PWC(Pt p, Circle c){
    double d = dis(p - c.o);
    if(sgn(d - c.r) == 0) return 0;
    if(sgn(d - c.r) > 0) return -1;
    return 1;
}
//直线和圆的位置关系
// 相切, 返回 0
// 相交, 返回 1
// 相离, 返回 -1
int LWC(Pt A, Pt B, Circle c) {
    double d = dist_ptl(c.o, A, B);
    if (sgn(d - c.r) == 0) return 0;
    if (sgn(d - c.r) > 0) return -1;
    return 1;
}

//直线和圆的交点
vector<Pt> Intersection_line_circle(Pt A, Pt B, Circle c) {
    Pt AB = B - A;
    double len2 = AB * AB;
    Pt pr = A + AB * ((c.o - A) * AB / len2);
    double d2 = ddis(pr - c.o);
    
    int status = sgn(d2 - c.r * c.r);
    if (status > 0) return {}; // 相离，返回空 vector

    double base = sqrt(max(0.0, c.r * c.r - d2));
    if (status == 0) return {pr}; // 相切，返回 1 个点

    // 相交，返回 2 个点
    Pt e = AB / sqrt(len2);
    return {pr + e * base, pr - e * base};
}
//圆与圆的位置关系
// 相离, 返回 -1
// 外切, 返回 0
// 内切(A 包含 B), 返回 1
// 内切(B 包含 A), 返回 2
// 内含(A 包含 B), 返回 3
// 内含(B 包含 A), 返回 4
// 相交, 返回 5

int Circle_with_circle(Circle A, Circle B) {
    double len1 = dis(A.o - B.o);
    double len2 = A.r + B.r;
    if (sgn(len1 - len2) > 0) return -1;
    if (sgn(len1 - len2) == 0) return 0;
    if (sgn(len1 + len2 - 2 * A.r) == 0) return 1;
    if (sgn(len1 + len2 - 2 * B.r) == 0) return 2;
    if (sgn(len1 + len2 - 2 * A.r) < 0) return 3;
    if (sgn(len1 + len2 - 2 * B.r) < 0) return 4;
    return 5;
}

//圆与圆的交点
// 相交, 返回两点坐标
// 相切, 返回两个一样的相切点

// 要先判断是否相交或相切再调用
vector<Pt> Intersection_circle_circle(Circle A, Circle B) {
    double d = dis(A.o - B.o);
    // 情况 1: 相离或内含 (无交点)
    if (sgn(d - (A.r + B.r)) > 0 || sgn(d - fabs(A.r - B.r)) < 0) {
        return {};
    }

    // 情况 2: 相切 (1 个交点)
    if (sgn(d - (A.r + B.r)) == 0 || sgn(d - fabs(A.r - B.r)) == 0) {
        Pt e = (B.o - A.o) / d;
        return {A.o + e * A.r};
    }

    // 情况 3: 相交 (2 个交点)
    double a = acos(max(-1.0, min(1.0, (A.r * A.r + d * d - B.r * B.r) / (2.0 * A.r * d))));
    double t = atan2(B.o.y - A.o.y, B.o.x - A.o.x);
    return {
        A.o + Pt(A.r * cos(t + a), A.r * sin(t + a)),
        A.o + Pt(A.r * cos(t - a), A.r * sin(t - a))
    };
}
//求圆外一点对圆的两个切点
vector<Pt> TangentPt_Pt_circle(Pt p, Circle c) {
    double d2 = ddis(p - c.o);
    int status = sgn(d2 - c.r * c.r);
    
    if (status < 0) return {}; // 点在圆内，无切点
    if (status == 0) return {p}; // 点在圆上，切点就是自身

    // 点在圆外，2 个切点
    double d = sqrt(d2);
    double l = sqrt(d2 - c.r * c.r);
    double angle = asin(max(-1.0, min(1.0, c.r / d)));
    Pt e = (c.o - p) / d;

    // 旋转向量得到切点方向
    auto rotate = [](Pt v, double ang) {
        return Pt(v.x * cos(ang) - v.y * sin(ang), v.x * sin(ang) + v.y * cos(ang));
    };

    return {
        p + rotate(e, angle) * l,
        p + rotate(e, -angle) * l
    };
}

//求三角形外接圆
Circle get_circumcircle(Pt A, Pt B, Pt C) {
    double Bx = B.x - A.x, By = B.y - A.y;
    double Cx = C.x - A.x, Cy = C.y - A.y;
    double D = 2 * (Bx * Cy - By * Cx);

    double x = (Cy * (Bx * Bx + By * By) - By * (Cx * Cx + Cy * Cy)) / D + A.x;
    double y = (Bx * (Cx * Cx + Cy * Cy) - Cx * (Bx * Bx + By * By)) / D + A.y;
    Pt P(x, y);
    return Circle(P, dis(A - P));
}
//三角形内切圆
Circle get_incircle(Pt A, Pt B, Pt C) {
    double a = dis(B - C);
    double b = dis(A - C);
    double c = dis(A - B);
    Pt p = (A * a + B * b + C * c) / (a + b + c);
    return Circle(p, dist_ptl(p, A, B));
}
// 要保证传入的点是整点
//线段上的整点个数
//注意按照要求修改返回值
int IntegerPt_on_seg(Pt A, Pt B) {
    int x = abs(A.x - B.x);
    int y = abs(A.y - B.y);
    if (x == 0 || y == 0) return 1;
    return __gcd(x, y) + 1;	// 包含端点
    return __gcd(x, y) - 1;	// 不包含端点
}
// 返回多边形边**上**整点的个数
// 点需要是顺时针(逆时针)给出

// p[] 下标从 0 开始, 长度为 n
int IntegerPt_on_polygon(vector<Pt>& p) {
    int n = p.size();
    int res = 0;
    for (int i = 0, j = n - 1; i < n; j = i++) {
        int x = abs(p[i].x - p[j].x);
        int y = abs(p[i].y - p[j].y);
        res += __gcd(x, y);
    }
    return res;
}
// 返回不包括边界的, 多边形**内**整点个数
int IntegerPt_in_polygon(vector<Pt>& p) {
    int n = p.size();
    double A = area(p);
    double B = IntegerPt_on_polygon(p);
    return A - B / 2 + 1;
}

int getQuadrant(Pt p, Pt center) {
    double x = p.x - center.x;
    double y = p.y - center.y;
    if (x > 0 && y >= 0) return 1; // 第一象限
    if (x <= 0 && y > 0) return 2; // 第二象限
    if (x < 0 && y <= 0) return 3; // 第三象限
    if (x >= 0 && y < 0) return 4; // 第四象限
    return 0;
}

void polarSort(vector<Pt>& Pts, const Pt& center) {
    sort(Pts.begin(), Pts.end(), [&center](const Pt& a, const Pt& b) {
        int quadA = getQuadrant(a, center);
        int quadB = getQuadrant(b, center);
        
        if (quadA != quadB) {
            return quadA < quadB;
        }
        
        // 同一象限内使用叉积
        double cp = (a - center) % (b - center);
        if (fabs(cp) > eps) {
            return cp > 0; // 逆时针排序
        }
        
        // 共线时，距离近的在前
        return ddis(a - center) < ddis(b - center);
    });
}

//凸包算法
vector<Pt> Andrew(vector<Pt>& p){
    sort(p.begin(), p.end());
    int n = p.size();
    vector<Pt> ans;
    for(int i = 0; i < n; i++){
        while(ans.size() >= 2 && Cross(ans[ans.size() - 2], ans.back(), p[i]) <= 0){
            ans.pop_back();
        }
        ans.push_back(p[i]);
    }

    int t = ans.size();
    for(int i = n - 2; i >= 0; i--){
        while(ans.size() > t && Cross(ans[ans.size() - 2], ans.back(), p[i]) <= 0){
            ans.pop_back();
        }
        ans.push_back(p[i]);
    }

    if(ans.size() > 1) ans.pop_back();
    return ans;

}

int norm_idx(int x, int n){
    x %= n;
    if(x < 0) x += n;
    return x;
}

// 点在已排序凸包中的位置：
// 返回 -1 表示外部，0 表示边界上，1 表示内部
// 要求 hull 为逆时针排序的凸包
int InConvex(Pt q, const vector<Pt>& hull){
    int n = hull.size();
    if(n == 0) return -1;
    if(n == 1) return q == hull[0] ? 0 : -1;
    if(n == 2) return OnSegment(q, hull[0], hull[1]) ? 0 : -1;

    int s1 = Cross(hull[0], hull[1], q);
    int s2 = Cross(hull[0], hull[n - 1], q);
    if(s1 < 0 || s2 > 0) return -1;
    if(OnSegment(q, hull[0], hull[1]) || OnSegment(q, hull[0], hull[n - 1])) return 0;

    int l = 1, r = n - 1;
    while(l + 1 < r){
        int mid = (l + r) >> 1;
        if(Cross(hull[0], hull[mid], q) >= 0) l = mid;
        else r = mid;
    }

    int side = Cross(hull[l], hull[(l + 1) % n], q);
    if(side < 0) return -1;
    if(OnSegment(q, hull[l], hull[(l + 1) % n])) return 0;
    return side == 0 ? 0 : 1;
}

// 若 q 在凸包外部，返回一个可见边的编号 i，表示边 hull[i] -> hull[i+1] 对 q 可见
// 若 q 在凸包内部或边界上，返回 -1
int FindVisibleEdge(Pt q, const vector<Pt>& hull){
    int n = hull.size();
    if(n < 3) return -1;

    if(Cross(hull[0], hull[1], q) < 0) return 0;
    if(Cross(hull[0], hull[n - 1], q) > 0) return n - 1;

    int l = 1, r = n - 1;
    while(l + 1 < r){
        int mid = (l + r) >> 1;
        if(Cross(hull[0], hull[mid], q) >= 0) l = mid;
        else r = mid;
    }

    return Cross(hull[l], hull[(l + 1) % n], q) < 0 ? l : -1;
}

bool VisibleEdge(Pt q, const vector<Pt>& hull, int idx){
    int n = hull.size();
    return Cross(hull[norm_idx(idx, n)], hull[norm_idx(idx + 1, n)], q) < 0;
}

// 返回外点 q 到已排序凸包的两个切点下标
// 返回值中的 first, second 分别是可见边链的起点和终点，按凸包逆时针顺序给出
// 若 q 不在凸包外部，则返回 {-1, -1}
pair<int, int> TangentIndexConvex(Pt q, const vector<Pt>& hull){
    int n = hull.size();
    if(n == 0) return {-1, -1};
    if(n == 1) return {0, 0};
    if(n == 2) return {0, 1};

    int k = FindVisibleEdge(q, hull);
    if(k == -1) return {-1, -1};

    int lo = 0, hi = n - 1;
    while(lo < hi){
        int mid = (lo + hi + 1) >> 1;
        if(VisibleEdge(q, hull, k + mid)) lo = mid;
        else hi = mid - 1;
    }
    int right_len = lo;

    lo = 0, hi = n - 1;
    while(lo < hi){
        int mid = (lo + hi + 1) >> 1;
        if(VisibleEdge(q, hull, k - mid)) lo = mid;
        else hi = mid - 1;
    }
    int left_len = lo;

    int left_edge = norm_idx(k - left_len, n);
    int right_edge = norm_idx(k + right_len, n);
    return {left_edge, norm_idx(right_edge + 1, n)};
}

pair<Pt, Pt> TangentPointConvex(Pt q, const vector<Pt>& hull){
    auto idx = TangentIndexConvex(q, hull);
    if(idx.first == -1) return {Pt(), Pt()};
    return {hull[idx.first], hull[idx.second]};
}

bool InTangentCone(Pt a, Pt b, const vector<Pt>& hull){
    auto idx = TangentIndexConvex(a, hull);
    if(idx.first == -1) return InConvex(a, hull) >= 0;
    return Cross(a, hull[idx.first], b) <= 0 && Cross(a, hull[idx.second], b) >= 0;
}

// 判断线段 ab 是否与已排序凸包相交
// 要求 hull 为逆时针排序的凸包
bool Intersect_seg_convex(Pt a, Pt b, const vector<Pt>& hull){
    int n = hull.size();
    if(n == 0) return false;
    if(n == 1) return OnSegment(hull[0], a, b);
    if(n == 2) return Intersect_seg(a, b, hull[0], hull[1]);

    if(InConvex(a, hull) >= 0 || InConvex(b, hull) >= 0) return true;
    return InTangentCone(a, b, hull) && InTangentCone(b, a, hull);
}

```



### 3D

```c++
const double PI = acos(-1);

int sgn(double x) {
    if (fabs(x) < 1e-8) return 0;
    return x < 0 ? -1 : 1;
}

struct Point{
    double x, y, z;
    Point(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}

    Point operator+ (const Point& B) const {return Point(x + B.x, y + B.y, z + B.z);}

    Point operator- (const Point& B) const {return Point(x - B.x, y - B.y, z - B.z);}
    //点乘
    double operator* (const Point& B) const {return x * B.x + y * B.y + z * B.z;}
    //叉乘
    Point operator% (const Point& B) const {return Point(y * B.z - z * B.y, z * B.x - x * B.z, x * B.y - y * B.x);}
    //数乘
    Point operator* (const double& B) const {return Point(x * B, y * B, z * B);}
    //数除（
    Point operator/ (const double& B) const {return Point(x / B, y / B, z / B);}
    
    bool operator== (const Point& B) const {return sgn(x - B.x) == 0 && sgn(y - B.y) == 0 && sgn(z - B.z) == 0;}
    
    bool operator!= (const Point& B) const {return sgn(x - B.x) != 0 || sgn(y - B.y) != 0 || sgn(z - B.z) != 0;}


};

double dis(Point& a){
	return sqrt(a * a);
}

double ddis(Point& a){
	return a * a;
}

// 两点间距离
double dis(Point& a, Point& b){
    return sqrt((a - b) * (a - b));
}
//两点间距离平方
double ddis(Point& a, Point& b){
    return (a - b) * (a - b);
}
//向量夹角
double Angle(Point& a, Point& b){
    return acos(a * b / dis(a) / dis(b));
}
```



## 动态规划

### 斜率优化DP

```c++
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
```





## 杂项

### 高精度加减乘除

```c++
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



### 对拍

#### bf.cpp

```c++
#include <bits/stdc++.h>
using namespace std;

//暴力内容
int main(){
    int n, m;
    cin >> n >> m;
    int ans = 0;
    while(n--) ans++;
    while(m--) ans++;
    cout << ans;
}
```



#### solve.cpp

```c++
#include <bits/stdc++.h>
using namespace std;

//程序正解内容
int main(){
    int n, m;
    cin >> n >> m;
    cout << n + m;
}
```



#### data.cpp

```c++
#include <bits/stdc++.h>
using namespace std;
#define int long long

mt19937_64 rd(random_device{}());

//生成随机数据作为输入
int rnd(int l, int r){
    return l + rd() % (r - l + 1);
}

signed main(){
    cout << rnd(1, 100) << " " << rnd(1, 100);
    return 0;
}
```



#### test.cpp

```c++
#include <bits/stdc++.h>
using namespace std;
#define int long long

//链接以上程序
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



#### dpL.sh

```sh
g++ data.cpp -o data 
g++ solve.cpp -o solve 
g++ bf.cpp -o bf 
g++ test.cpp -o test 
./test
```



#### dpW.bat

```bat
g++ data.cpp -o data.exe
g++ solve.cpp -o solve.exe
g++ bf.cpp -o bf.exe
g++ test.cpp -o test.exe
test.exe
```



### STL库函数

```c++
#include <numeric>
#include <algorithm>
#include <cmath>

// 最大公约数
__gcd(a, b);        // GNU扩展，最快
gcd(a, b);          // C++17标准

// 最小公倍数
lcm(a, b);          // C++17

// 累积计算
accumulate(v.begin(), v.end(), 0);                          // 求和
accumulate(v.begin(), v.end(), 1, multiplies<int>());       // 求积
partial_sum(v.begin(), v.end(), result.begin());            // 前缀和
adjacent_difference(v.begin(), v.end(), result.begin());    // 差分

// 其他数学
__builtin_popcount(x);      // 二进制中1的个数
abs(x); pow(x, y); sqrt(x); // 绝对值、幂、平方根


#include <algorithm>

// 二分查找（要求有序）
lower_bound(v.begin(), v.end(), x);     // 第一个≥x的位置
upper_bound(v.begin(), v.end(), x);     // 第一个>x的位置
binary_search(v.begin(), v.end(), x);   // 是否存在x
equal_range(v.begin(), v.end(), x);     // 返回[x,x]的区间

// 统计
count(v.begin(), v.end(), x);           // 统计x出现次数
count_if(v.begin(), v.end(), pred);     // 统计满足条件的元素

// 最值
max_element(v.begin(), v.end());        // 最大元素位置
min_element(v.begin(), v.end());        // 最小元素位置


// 填充
fill(v.begin(), v.end(), value);        // 填充值
iota(v.begin(), v.end(), start);        // 填充递增序列

// 变换
transform(v.begin(), v.end(), result.begin(), func); // 对每个元素应用函数

// 交换
swap(a, b);                             // 交换两个值
iter_swap(it1, it2);                    // 交换迭代器指向的值

// 反转
reverse(v.begin(), v.end());            // 反转序列



// 下一个排列（字典序）
next_permutation(v.begin(), v.end());   // 变为下一个排列
prev_permutation(v.begin(), v.end());   // 变为上一个排列

// 使用示例：生成所有排列
sort(v.begin(), v.end());
do {
    // 处理当前排列
} while (next_permutation(v.begin(), v.end()));



// 集合运算（要求有序）
set_union(a.begin(), a.end(), b.begin(), b.end(), result.begin());      // 并集
set_intersection(a.begin(), a.end(), b.begin(), b.end(), result.begin()); // 交集
set_difference(a.begin(), a.end(), b.begin(), b.end(), result.begin()); // 差集
includes(a.begin(), a.end(), b.begin(), b.end());                       // 包含关系



#include <bitset>
#include <bit>  // C++20

bitset<32> bs(x);           // 位集操作
__builtin_clz(x);           // 前导0个数
__builtin_ctz(x);           // 后缀0个数
__builtin_ffs(x);           // 最低位1的位置
popcount(x);                // C++20, 1的个数



ios::sync_with_stdio(false);
cin.tie(0);
cout.tie(0);
```



### 迭代器操作

```c++
#include <iterator>

// 前进n步（不改变原迭代器）
auto it2 = next(it, n);      // 返回it后面第n个位置的迭代器
auto it2 = next(it);         // 相当于next(it, 1)

// 后退n步（不改变原迭代器）
auto it2 = prev(it, n);      // 返回it前面第n个位置的迭代器
auto it2 = prev(it);         // 相当于prev(it, 1)

// 前进n步（改变原迭代器）
advance(it, n);              // 将it前进n步

#include <iterator>

// 计算两个迭代器之间的距离
int dist = distance(it1, it2);

// 获取迭代器在容器中的位置索引
int pos = distance(container.begin(), it);

int main() {
    vector<int> v = {1, 2, 3, 4, 5};
    
    // 正向迭代器转反向迭代器
    auto it = v.begin() + 2;  // 指向3
    auto rit = reverse_iterator<vector<int>::iterator>(it);
    cout << *rit << endl;     // 输出: 2（指向前一个元素）
    
    // 反向迭代器转正向迭代器
    auto rit2 = v.rbegin() + 1;  // 指向4
    auto it2 = rit2.base();      // 指向5
    cout << *it2 << endl;        // 输出: 5
    
    return 0;
}

```

