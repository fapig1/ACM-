#include <bits/stdc++.h>
using namespace std;

// 【速查】字典树 Trie（前缀计数 / 精确计数）
// 用法: Trie t;  t.insert(s);  t.prefixCount(s) -> 以 s 为前缀的已插入串个数;  t.count(s) -> 恰好等于 s 的个数
//       多组数据: t.clear()；字符集默认 A-Z a-z 0-9（改 SIGMA 和 id() 即可换成纯小写等）
// 复杂度: 插入 / 查询 O(|s|)；节点按需开（vector），空间 = 总串长 × SIGMA 个 int
// 10.22: 原版是全局 ch[N][70] 数组 + 手动清空，已封装为结构体
struct Trie {
    static const int SIGMA = 62;
    vector<array<int, SIGMA>> ch;
    vector<int> pass, ends;                    // pass: 经过该节点的串数（前缀计数）；ends: 在此结束的串数

    Trie() { clear(); }
    void clear() { ch.assign(1, {}); pass.assign(1, 0); ends.assign(1, 0); }
    static int id(char c) {
        if (c >= 'A' && c <= 'Z') return c - 'A';
        if (c >= 'a' && c <= 'z') return c - 'a' + 26;
        return c - '0' + 52;
    }
    void insert(const string& s) {
        int p = 0;
        for (char c : s) {
            int j = id(c);
            if (!ch[p][j]) {
                ch[p][j] = ch.size();               // 先记下新编号再扩容，避免引用失效
                ch.push_back({}); pass.push_back(0); ends.push_back(0);
            }
            p = ch[p][j];
            pass[p]++;
        }
        ends[p]++;
    }
    int walk(const string& s) {
        int p = 0;
        for (char c : s) { p = ch[p][id(c)]; if (!p) return 0; }
        return p;
    }
    int prefixCount(const string& s) { int p = walk(s); return p ? pass[p] : 0; }
    int count(const string& s) { int p = walk(s); return p ? ends[p] : 0; }
};

// 示例: 读 T 组，每组 n q、n 个串、q 个询问，输出以询问串为前缀的串个数
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int T;
    cin >> T;
    Trie t;
    while (T--) {
        t.clear();
        int n, q;
        cin >> n >> q;
        string s;
        for (int i = 0; i < n; i++) { cin >> s; t.insert(s); }
        while (q--) { cin >> s; cout << t.prefixCount(s) << '\n'; }
    }
    return 0;
}
