#include <bits/stdc++.h>
using namespace std;

// 【速查】Z 函数（扩展 KMP，P5410）
// 用途: z[i] = s 与 s[i..] 的最长公共前缀；ext[i] = t 与 s[i..] 的最长公共前缀（模式串 t 在文本 s 每个位置的匹配长度）
// 用法: auto z = zFunction(s);   // z[0] = |s|
//       auto ext = exKMP(s, t);  // 长度 |s|
// 常用: t 在 s 中出现 <=> ext[i] == |t|；s 的周期 p <=> z[p] == n - p
// 复杂度: O(n + m)，全部 0-based
vector<int> zFunction(const string& s) {
    int n = s.size();
    vector<int> z(n, 0);
    if (n) z[0] = n;
    for (int i = 1, l = 0, r = 0; i < n; i++) {        // [l, r) 为当前最右匹配段
        if (i < r) z[i] = min(r - i, z[i - l]);
        while (i + z[i] < n && s[z[i]] == s[i + z[i]]) z[i]++;
        if (i + z[i] > r) l = i, r = i + z[i];
    }
    return z;
}
vector<int> exKMP(const string& s, const string& t) {
    int n = s.size(), m = t.size();
    vector<int> z = zFunction(t), ext(n, 0);
    for (int i = 0, l = 0, r = 0; i < n; i++) {
        if (i < r) ext[i] = min(r - i, z[i - l]);
        while (ext[i] < m && i + ext[i] < n && t[ext[i]] == s[i + ext[i]]) ext[i]++;
        if (i + ext[i] > r) l = i, r = i + ext[i];
    }
    return ext;
}

// 【速查】最小表示法（P1368）
// 用途: 字符串/数组所有循环同构中字典序最小的那个（判两个环是否同构：各自最小表示相等）
// 用法: int k = minRotation(a);   // a 为 vector<int> 或 string，最小表示是从 a[k] 开始的循环
// 复杂度: O(n)
template <class T>
int minRotation(const T& a) {
    int n = a.size(), i = 0, j = 1, k = 0;
    while (i < n && j < n && k < n) {
        auto x = a[(i + k) % n], y = a[(j + k) % n];
        if (x == y) { k++; continue; }
        if (x > y) i += k + 1; else j += k + 1;
        if (i == j) j++;
        k = 0;
    }
    return min(i, j);
}

// 【速查】回文自动机 PAM / 回文树（P5496、P3649）
// 用途: 本质不同回文子串个数、每个回文子串出现次数、以每个位置结尾的回文串个数
// 用法: PAM p(|s|);  对每个字符 p.add(c - 'a')，返回值 = 以当前位置结尾的回文子串个数;
//       p.size() - 2 = 本质不同回文子串个数;  p.countOcc() 后 p.cnt[v] = 节点 v 代表的回文串出现次数，长度 p.len[v]
// 节点: 0 是长度 -1 的奇根，1 是长度 0 的偶根，真正的回文串从 2 开始
// 复杂度: O(n × 26)；只支持小写字母
struct PAM {
    vector<array<int, 26>> nxt;
    vector<int> len, fail, cnt, num;
    vector<int> s;
    int last;

    PAM(int n = 0) {
        nxt.reserve(n + 2); len.reserve(n + 2); fail.reserve(n + 2); cnt.reserve(n + 2); num.reserve(n + 2);
        newNode(-1); newNode(0);
        fail[0] = 0; fail[1] = 0;
        last = 1;
    }
    int newNode(int l) { nxt.push_back({}); len.push_back(l); fail.push_back(0); cnt.push_back(0); num.push_back(0); return (int)len.size() - 1; }
    int getFail(int v, int i) {
        while (i - len[v] - 1 < 0 || s[i - len[v] - 1] != s[i]) v = fail[v];
        return v;
    }
    int add(int c) {
        s.push_back(c);
        int i = (int)s.size() - 1;
        int cur = getFail(last, i);
        if (!nxt[cur][c]) {
            int now = newNode(len[cur] + 2);
            int f = getFail(fail[cur], i);
            fail[now] = (len[now] == 1) ? 1 : nxt[f][c];
            num[now] = num[fail[now]] + 1;           // 以它结尾的回文后缀个数
            nxt[cur][c] = now;
        }
        last = nxt[cur][c];
        cnt[last]++;
        return num[last];
    }
    int size() { return len.size(); }
    void countOcc() {                                // 出现次数沿 fail 链累加（新节点编号大，倒序即可）
        for (int v = size() - 1; v >= 2; v--) cnt[fail[v]] += cnt[v];
    }
};
