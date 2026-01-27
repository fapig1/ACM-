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
