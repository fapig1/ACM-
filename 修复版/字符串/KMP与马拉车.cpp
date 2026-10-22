#include <bits/stdc++.h>
using namespace std;

// 【速查】KMP（前缀函数）
// 用途: 模式串 p 在文本 s 中所有出现位置；border、最小循环节
// 用法: auto pi = prefixFunction(p);   // pi[i] = p[0..i] 的最长相等真前后缀长度（0-based）
//       auto pos = kmpFind(s, p);      // p 在 s 中所有出现的起点（0-based）
// 常用: p 的最小循环节长度 = m - pi[m-1]（若能整除 m 则 p 由它重复构成）
// 复杂度: O(n + m)
// 10.22: 原版写在 main 里、用全局 char 数组，已改为函数
vector<int> prefixFunction(const string& p) {
    int m = p.size();
    vector<int> pi(m, 0);
    for (int i = 1, j = 0; i < m; i++) {
        while (j && p[i] != p[j]) j = pi[j - 1];
        if (p[i] == p[j]) j++;
        pi[i] = j;
    }
    return pi;
}
vector<int> kmpFind(const string& s, const string& p) {
    vector<int> res;
    if (p.empty()) return res;
    auto pi = prefixFunction(p);
    for (int i = 0, j = 0; i < (int)s.size(); i++) {
        while (j && s[i] != p[j]) j = pi[j - 1];
        if (s[i] == p[j]) j++;
        if (j == (int)p.size()) { res.push_back(i - j + 1); j = pi[j - 1]; }
    }
    return res;
}

// 【速查】Manacher 最长回文子串
// 用法: vector<int> d;  int len = manacher(s, d);   // 返回最长回文子串长度（奇偶都算）
// 进阶: d[i] 是变换串 "#a#b#…" 中以 i 为中心的回文臂长（= 原串对应回文的长度），
//       中心为原串 s[k] 的位置是 i = 2k+1，中心在 s[k-1]、s[k] 之间的位置是 i = 2k
// 复杂度: O(n)
int manacher(const string& str, vector<int>& d) {
    string t = "#";
    for (char c : str) { t += c; t += '#'; }
    int m = t.size(), C = 0, R = 0, best = 0;
    d.assign(m, 0);
    for (int i = 0; i < m; i++) {
        int k = (i < R) ? min(d[2 * C - i], R - i) : 0;
        while (i - k - 1 >= 0 && i + k + 1 < m && t[i - k - 1] == t[i + k + 1]) k++;
        d[i] = k;
        if (i + k > R) { C = i; R = i + k; }
        best = max(best, d[i]);
    }
    return best;
}
int manacher(const string& str) { vector<int> d; return manacher(str, d); }

// 示例: 读文本 s 和模式 p，输出每次出现的起点（1-based），再输出 p 的前缀函数
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    string s, p;
    cin >> s >> p;
    for (int x : kmpFind(s, p)) cout << x + 1 << '\n';
    for (int v : prefixFunction(p)) cout << v << ' ';
    return 0;
}
