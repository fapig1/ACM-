#include <bits/stdc++.h>
using namespace std;

//后缀树模板: 用"反串的 SAM 的 link 树"实现
//定理: S 的后缀树 = SAM(反转后的 S) 的 link(后缀链接)树
//  - 每个 SAM 状态 = 后缀树一个节点, 串深度(根到该节点路径上的字符数) = len[v]
//  - 边 link[v] -> v 的标签长度 = len[v] - len[link[v]]
//  - 后缀 s[p..] 对应的节点 = 处理反串第 n-1-p 个字符后的 last 状态 (记录在 leaf[p])
//  - 不加哨兵时: 若某后缀是另一后缀的前缀(如 "a" 是 "aa" 的前缀), 它对应的是内部节点, 属正常现象
//  - 两个后缀的 LCP = 对应节点在 link 树上的 LCA 的 len
//
//后缀树常用来解决什么:
//  - 批量查询任意两后缀的 LCP (预处理后单次 O(log n), 见 lcp)
//  - 后缀树上 DP: 子树"终点标记"之和 = 某前缀的出现次数; 出现次数>=K 的最长子串
//  - 本质不同子串数 / 各节点对应的公共前缀长度 (见 count_distinct)
//  - 树上差分、可持久化等树技巧可以直接搬过来用
//判别信号: 需要"后缀与后缀之间的关系"(两两 LCP、作为树处理), 或要做树形 DP
//与 SAM 的取舍: 只需要子串出现统计时直接 SAM 更省; 需要后缀间关系/树形态时用后缀树
struct SuffixTree {
    struct State {
        int len, link, next[26];
        void init() { len = link = 0; memset(next, 0, sizeof(next)); }
    };
    vector<State> st;
    int tot, last;
    vector<int> leaf;           // leaf[p]: 后缀 s[p..] 对应的树节点
    vector<vector<int>> g;      // link 树邻接表 (g[link[v]].push_back(v))
    vector<int> dep;            // 树深度(边数), 供倍增跳 LCA
    vector<int> up[20];         // 倍增表

    void init(int maxlen) {
        int M = 2 * maxlen + 5;
        st.assign(M, State());
        for (int i = 0; i < M; i++) st[i].init();
        st[0].len = 0; st[0].link = -1;
        tot = last = 0;
    }

    void extend(int c) {
        int cur = ++tot;
        st[cur].len = st[last].len + 1;
        int p = last;
        while (p != -1 && !st[p].next[c]) { st[p].next[c] = cur; p = st[p].link; }
        if (p == -1) st[cur].link = 0;
        else {
            int q = st[p].next[c];
            if (st[p].len + 1 == st[q].len) st[cur].link = q;
            else {
                int clone = ++tot;
                st[clone] = st[q];
                st[clone].len = st[p].len + 1;
                while (p != -1 && st[p].next[c] == q) { st[p].next[c] = clone; p = st[p].link; }
                st[q].link = st[cur].link = clone;
            }
        }
        last = cur;
    }

    void build(const string& s) {
        int n = s.size();
        init(n);
        leaf.assign(n, 0);
        for (int i = 0; i < n; i++) {            // 对反串建 SAM
            extend(s[n-1-i] - 'a');              // 反串第 i 个字符 = 原串的 s[n-1-i]
            leaf[n-1-i] = last;                  // 反串前缀长度 i+1 <-> 原串后缀起点 n-1-i
        }
        g.assign(tot + 1, {});
        for (int v = 1; v <= tot; v++) g[st[v].link].push_back(v);   // 建 link 树
        // 迭代 DFS 预处理倍增表 (link 树可能退化成链, 递归会爆栈)
        dep.assign(tot + 1, 0);
        for (int k = 0; k < 20; k++) up[k].assign(tot + 1, 0);
        vector<int> stk{0};
        while (!stk.empty()) {
            int u = stk.back(); stk.pop_back();
            for (int v : g[u]) {
                dep[v] = dep[u] + 1;
                up[0][v] = u;
                for (int k = 1; k < 20; k++) up[k][v] = up[k-1][up[k-1][v]];
                stk.push_back(v);
            }
        }
    }

    int lca(int a, int b) {
        if (dep[a] < dep[b]) swap(a, b);
        int d = dep[a] - dep[b];
        for (int k = 0; k < 20; k++) if (d >> k & 1) a = up[k][a];
        if (a == b) return a;
        for (int k = 19; k >= 0; k--)
            if (up[k][a] != up[k][b]) { a = up[k][a]; b = up[k][b]; }
        return up[0][a];
    }

    // 后缀 s[p..] 与 s[q..] 的最长公共前缀长度 = 叶子 LCA 的串深度
    int lcp(int p, int q) { return st[lca(leaf[p], leaf[q])].len; }

    // 不同子串数 = Σ 每条边的标签长度
    long long count_distinct() {
        long long res = 0;
        for (int v = 1; v <= tot; v++) res += st[v].len - st[st[v].link].len;
        return res;
    }
};

/*
经典应用示例 (代码均已对拍验证, 可直接参考)

// 子串出现次数: 先标记每个后缀终点 isEnd[leaf[p]]++, 再按深度降序做子树和
// 为什么不能数树叶子: 无哨兵时后缀可以是另一后缀的前缀, 对应内部节点,
//   所以要用"终点标记"的子树和, 而不是"叶子"的子树和
vector<int> isEnd(ST.tot + 1, 0);
for (int p = 0; p < n; p++) isEnd[ST.leaf[p]]++;
vector<int> order(ST.tot + 1);
iota(order.begin(), order.end(), 0);
sort(order.begin(), order.end(), [&](int a, int b){ return ST.dep[a] > ST.dep[b]; });  // 深度降序=拓扑序
for (int v : order) if (v) isEnd[ST.st[v].link] += isEnd[v];     // 子树和 = 出现次数

// 查询子串 w 的出现次数: 在反串 SAM 上走 reversed(w) 定位状态 u, 答案 = isEnd[u]
int occur(const string& w) {
    int u = 0;
    for (int i = (int)w.size() - 1; i >= 0; i--) {
        u = ST.st[u].next[w[i] - 'a'];
        if (!u) return 0;                    // w 不是 s 的子串
    }
    return isEnd[u];
}

// 出现次数>=K 的最长子串: 树上 DP 之后, 取满足 isEnd[v]>=K 的最大 len[v]
int ans = 0;
for (int v = 1; v <= ST.tot; v++) if (isEnd[v] >= K) ans = max(ans, ST.st[v].len);

// 基础用法
int main() {
    string s; cin >> s;
    SuffixTree T;
    T.build(s);
    cout << T.count_distinct() << "\n";     // 不同子串数
    int q; cin >> q;
    while (q--) {
        int p, r; cin >> p >> r;
        cout << T.lcp(p, r) << "\n";        // 两个后缀的 LCP
    }
    return 0;
}
*/
