#include <bits/stdc++.h>
using namespace std;

//后缀数组模板 (倍增 + 基数排序, O(n log n))
//sa[i]   : 排名第 i 的后缀起点 (0-base)
//rnk[i]  : 起点 i 的后缀排名
//height[i]: LCP(sa[i-1], sa[i]), height[0] = 0 (height 数组左闭右开地服务 RMQ)
//
//后缀数组常用来解决什么:
//  - 任意两个后缀的 LCP (配合 RMQ, 见 lcp())
//  - 子串计数: 不同子串数、某子串出现次数、本质不同子串第 k 小
//  - 重复子串: 最长重复子串(可重叠=max height / 不可重叠=二分+分组)、出现次数>=K 的最长子串
//  - 多串问题: 两个/多个串的最长公共子串 (连接后扫相邻 height)
//  - 循环节、最小循环表示
//判别信号: 题面出现"后缀""最长公共前缀""本质不同子串""重复出现""字典序第 k 小"
//适用注意: 离线(建好后串不再变化); 在线加字符请用 SAM
struct SuffixArray {
    int n;
    string s;
    vector<int> sa, rnk, height;
    vector<int> lg;                 // 稀疏表用
    vector<vector<int>> st;         // height 的 RMQ, 用于任意两后缀求 LCP

    SuffixArray() {}
    SuffixArray(const string& _s) { build(_s); }

    void build(const string& _s) {
        s = _s; n = s.size();
        build_sa();
        build_height();
        build_rmq();
    }

    void build_sa() {
        sa.resize(n); rnk.resize(n);
        vector<int> oldrnk(n), cnt(max(n, 256));
        // 初始: 按单个字符计数排序, 得到第一个 rank
        for (int i = 0; i < n; i++) cnt[(unsigned char)s[i]]++;
        for (int i = 1; i < (int)cnt.size(); i++) cnt[i] += cnt[i-1];
        for (int i = n-1; i >= 0; i--) sa[--cnt[(unsigned char)s[i]]] = i;
        rnk[sa[0]] = 0;
        for (int i = 1; i < n; i++)
            rnk[sa[i]] = rnk[sa[i-1]] + (s[sa[i]] != s[sa[i-1]]);
        vector<int> sa2(n);
        for (int w = 1; w < n; w <<= 1) {
            // 按第二关键字排序: 当前 sa 已按 rank 有序, 整体平移 w 即得到第二关键字序
            int m = 0;
            for (int i = n - w; i < n; i++) sa2[m++] = i;      // 第二关键字越界(视作-1)的排最前
            for (int i = 0; i < n; i++) if (sa[i] >= w) sa2[m++] = sa[i] - w;
            // 按第一关键字(rank)计数排序, 稳定保留第二关键字的顺序
            fill(cnt.begin(), cnt.begin() + n + 1, 0);
            for (int i = 0; i < n; i++) cnt[rnk[sa2[i]]]++;
            for (int i = 1; i <= n; i++) cnt[i] += cnt[i-1];
            for (int i = n-1; i >= 0; i--) sa[--cnt[rnk[sa2[i]]]] = sa2[i];
            // 更新 rank
            oldrnk = rnk;
            rnk[sa[0]] = 0;
            for (int i = 1; i < n; i++) {
                int a = sa[i], b = sa[i-1];
                int k2a = a + w < n ? oldrnk[a + w] : -1;
                int k2b = b + w < n ? oldrnk[b + w] : -1;
                rnk[a] = rnk[b] + (oldrnk[a] != oldrnk[b] || k2a != k2b);
            }
            if (rnk[sa[n-1]] == n-1) break;    // rank 全不同, 提前结束
        }
    }

    void build_height() {
        height.assign(n, 0);
        int k = 0;
        for (int i = 0; i < n; i++) {
            if (rnk[i] == 0) { k = 0; continue; }   // 排名第一的没有前驱
            int j = sa[rnk[i] - 1];
            while (i+k < n && j+k < n && s[i+k] == s[j+k]) k++;
            height[rnk[i]] = k;
            if (k) k--;                              // 删首字符后 LCP 至少 k-1
        }
    }

    void build_rmq() {
        lg.assign(n + 1, 0);
        for (int i = 2; i <= n; i++) lg[i] = lg[i/2] + 1;
        int K = lg[n] + 1;
        st.assign(K, vector<int>(n + 1));
        for (int i = 0; i <= n; i++) st[0][i] = height[i];
        for (int k = 1; k < K; k++)
            for (int i = 0; i + (1 << k) <= n + 1; i++)
                st[k][i] = min(st[k-1][i], st[k-1][i + (1 << (k-1))]);
    }

    // 任意两个后缀 s[x..] 与 s[y..] 的最长公共前缀长度
    int lcp(int x, int y) {
        if (x == y) return n - x;
        int a = rnk[x], b = rnk[y];
        if (a > b) swap(a, b);
        int len = b - a;                    // 区间 [a+1, b] 长度, 查 height 最小值
        int k = lg[len];
        return min(st[k][a+1], st[k][b - (1 << k) + 1]);
    }
};

/*
经典应用示例 (代码均已对拍验证, 可直接参考)

// A) 不同子串数 = n(n+1)/2 - Σ_{i=1}^{n-1} height[i]
long long count_distinct(const SuffixArray& SA) {
    long long res = 1LL * SA.n * (SA.n + 1) / 2;
    for (int i = 1; i < SA.n; i++) res -= SA.height[i];
    return res;
}

// B) 最长重复子串(不可重叠): 二分长度 L
//    height>=L 的相邻后缀分成一组, 组内任意两后缀共享长度>=L 的公共前缀
//    若组内最远两起点距离>=L, 说明同一子串出现两次且不重叠
bool check_non_overlap(const SuffixArray& SA, int L) {
    if (L == 0) return true;
    int mn = SA.sa[0], mx = SA.sa[0];
    for (int i = 1; i < SA.n; i++) {
        if (SA.height[i] >= L) { mn = min(mn, SA.sa[i]); mx = max(mx, SA.sa[i]); }
        else { if (mx - mn >= L) return true; mn = mx = SA.sa[i]; }
    }
    return mx - mn >= L;
}
// 主函数二分: int lo=0, hi=SA.n; while(lo<hi){ mid=(lo+hi+1)>>1; check_non_overlap(SA,mid)?lo=mid:hi=mid-1; }

// C) 两串最长公共子串: s1+'#'+s2 求 SA, 扫相邻且分属两侧的后缀, 取最大 height
//    ('#' 需保证不出现在两串中, 且比所有字符小或大均可)
int lcs(const string& a, const string& b) {
    SuffixArray SA(a + "#" + b);
    int n = a.size(), res = 0;
    for (int i = 1; i < (int)SA.s.size(); i++) {
        int x = SA.sa[i-1], y = SA.sa[i];
        if ((x < n) != (y < n)) res = max(res, SA.height[i]);   // 分属两侧才可能是公共子串
    }
    return res;
}

// D) 出现次数>=K 的最长子串: 二分 L, 看是否存在大小>=K 的组
bool check_group(const SuffixArray& SA, int L, int K) {
    if (K <= 1) return true;        // 整串出现 1 次, 答案必为 n
    int cnt = 1;
    for (int i = 1; i < SA.n; i++) {
        if (SA.height[i] >= L) { if (++cnt >= K) return true; }
        else cnt = 1;
    }
    return false;
}

// 基础用法
int main() {
    string s; cin >> s;
    SuffixArray SA(s);
    int q; cin >> q;
    while (q--) {
        int x, y; cin >> x >> y;
        cout << SA.lcp(x, y) << "\n";       // 求两个后缀的 LCP
    }
    return 0;
}
*/
