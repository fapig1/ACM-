#include <bits/stdc++.h>
using namespace std;
#define int long long

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