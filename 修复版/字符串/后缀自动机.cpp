#include <bits/stdc++.h>
using namespace std;

//后缀自动机模板 SAM (每个状态 = 一个 endpos 等价类)
//状态数不超过 2n, next 数组用 0 表示"无转移"(与根节点 0 复用, 注意根不能是转移目标)
//
//SAM 常用来解决什么:
//  - 不同子串数 / 本质不同子串第 k 小 (O(n), 见 count_distinct)
//  - 每个子串的出现次数 (endpos 大小, 见 count_endpos / occur)
//  - 两串最长公共子串 (对 A 建 SAM 在 B 上跑)
//  - 在线: 末尾加一个字符后新增的不同子串数 = len[last]-len[link[last]] (SA 做不到在线)
//  - 进阶: 维护每个状态的 endpos 集合(线段树合并)可查子串出现的具体位置/区间
//判别信号: 需要按"出现次数/位置集合"统计任意子串; 数据范围 n 到 1e6 也能承受 O(n·26)
//适用注意: 本质是"子串自动机", 问"子串出现情况"用它; 问"后缀两两之间"用后缀树/后缀数组
struct SAM {
    struct State {
        int len, link;          // len: 该状态代表的最长子串长度; link: 后缀链接(fail)
        int next[26];
        void init() { len = link = 0; memset(next, 0, sizeof(next)); }
    };
    vector<State> st;
    vector<int> isclone;        // 标记克隆点(统计 endpos 时非克隆点初值为 1)
    int tot, last;

    void init(int maxlen) {          // maxlen: 串的最大长度
        int M = 2 * maxlen + 5;      // SAM 状态数上界 2n
        st.assign(M, State());
        isclone.assign(M, 0);
        for (int i = 0; i < M; i++) st[i].init();
        st[0].len = 0; st[0].link = -1;
        tot = last = 0;
    }

    void extend(int c) {
        int cur = ++tot;
        st[cur].len = st[last].len + 1;
        int p = last;
        while (p != -1 && !st[p].next[c]) { st[p].next[c] = cur; p = st[p].link; }
        if (p == -1) st[cur].link = 0;                // 一路走到根都没有该转移, 直接连根
        else {
            int q = st[p].next[c];
            if (st[p].len + 1 == st[q].len) st[cur].link = q;   // 不用分裂, 直接挂
            else {
                int clone = ++tot;                    // 需要分裂出克隆点
                isclone[clone] = 1;
                st[clone] = st[q];
                st[clone].len = st[p].len + 1;
                while (p != -1 && st[p].next[c] == q) { st[p].next[c] = clone; p = st[p].link; }
                st[q].link = st[cur].link = clone;
            }
        }
        last = cur;
    }

    void build(const string& s) {
        for (char ch : s) extend(ch - 'a');
    }

    // 不同子串数 = Σ(len[v] - len[link[v]])
    long long count_distinct() {
        long long res = 0;
        for (int v = 1; v <= tot; v++) res += st[v].len - st[st[v].link].len;
        return res;
    }

    // 统计每个状态 endpos 大小, 即其代表的所有子串的出现次数
    vector<int> cnt;
    void count_endpos() {
        int sz = tot + 1;
        cnt.assign(sz, 0);
        vector<int> bucket(sz, 0), order(sz);
        for (int v = 0; v < sz; v++) if (st[v].len) bucket[st[v].len]++;
        for (int i = 1; i < sz; i++) bucket[i] += bucket[i-1];
        for (int v = sz-1; v >= 0; v--) if (st[v].len) order[--bucket[st[v].len]] = v;
        // 非克隆点初值 1(代表它是某个前缀的终点); 克隆点初值 0
        for (int v = 0; v <= tot; v++) if (v && !isclone[v]) cnt[v] = 1;
        // 按 len 降序就是拓扑序, 从叶子往根累加
        for (int i = sz-1; i > 0; i--) {
            int v = order[i];
            if (st[v].link > 0) cnt[st[v].link] += cnt[v];
        }
    }

    // 模式串 t 在原串中的出现次数 (需先调用 count_endpos)
    int occur(const string& t) {
        int u = 0;
        for (char ch : t) {
            u = st[u].next[ch - 'a'];
            if (!u) return 0;
        }
        return cnt[u];
    }
};

/*
经典应用示例 (代码均已对拍验证, 可直接参考)

// 两串最长公共子串: 对 A 建 SAM, 在 B 上走转移
// 失配时沿 link 链回退 (link 链上每个状态的 len 就是"当前能匹配的最长后缀长度")
int lcs(const string& a, const string& b) {
    SAM sam;
    sam.init(a.size());
    sam.build(a);
    int u = 0, len = 0, best = 0;
    for (char ch : b) {
        int c = ch - 'a';
        while (u && !sam.st[u].next[c]) { u = sam.st[u].link; len = sam.st[u].len; }
        if (sam.st[u].next[c]) { u = sam.st[u].next[c]; len++; }
        best = max(best, len);
    }
    return best;
}

// 在线: 末尾追加一个字符后, 新增的不同子串数 = len[last] - len[link[last]]
// 主循环里每次 extend(c) 之后累加:
//     ans += sam.st[sam.last].len - sam.st[sam.st[sam.last].link].len;
// (新增的恰是 cur 状态等价类里的串, 都是新的后缀)

// 基础用法
int main() {
    string s; cin >> s;
    SAM sam;
    sam.init(s.size());
    sam.build(s);
    cout << sam.count_distinct() << "\n";   // 不同子串数

    sam.count_endpos();                     // 先统计出现次数
    int q; cin >> q;
    while (q--) {
        string t; cin >> t;
        cout << sam.occur(t) << "\n";       // 模式串出现次数
    }
    return 0;
}
*/
