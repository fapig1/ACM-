#include <bits/stdc++.h>
using namespace std;
#define int long long

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