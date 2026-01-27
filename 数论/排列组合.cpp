#include <bits/stdc++.h>
using namespace std;
#define int long long


const int mod = 1e9 + 7;
const int maxn = 2e5;


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

//卡特兰数CatalanNumber
int CatalanNumber(int n){
    int ans = C(2 * n, n) - C(2 * n, n + 1);
    return (ans % mod + mod) % mod;
}


