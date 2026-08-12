#include <bits/stdc++.h>
using namespace std;

//AC自动机模板
// 结构: Trie + fail 指针; 文本在自动机上跑, 状态 u = 当前文本前缀的最长后缀
// 关键: fail[u] 指向"u 代表串的最长真后缀所在状态", build 时补齐 son 转移, 失配自动跳 fail
//
//AC自动机常用来解决什么:
//  - 多模式串在文本中的出现次数 / 出现位置 (洛谷 P3808 / P3796 模板题)
//  - 文本是否包含任一模式串、每个模式串首次出现位置 (见 query_first)
//  - 屏蔽词类: 每个文本位置结尾的最长匹配模式串 -> 区间覆盖 DP (见 covered)
//  - 多模式串 + 自动机 DP (状态含 fail 链的转移)
//判别信号: 模式串数量多(几十~几百个), 需对同一文本批量查询; 模式串总长 1e6 级别
//适用注意: 建好后模式串集合不再变; 模式串会动态变化时不适合(重建太贵)
struct AC {
    static const int SIZE = 2e5 + 6;   // 模式串总长度上界

    struct Node {
        int son[26];      // 转移边 (build 后填满, 未匹配字符自动跳 fail)
        int len;          // 该节点在 Trie 中的深度 = 代表串长度 (应用1/2 需要)
        int ans;          // 文本跑到该状态的次数 (query 时累加)
        int fail;         // 失败指针
        int du;           // fail 树入度, 用于拓扑传播计数
        int idx;          // 模式串编号 (相同模式串映射到同一编号)

        void init() {
            memset(son, 0, sizeof(son));
            len = ans = fail = idx = 0;
        }
    };

    Node tr[SIZE];
    int tot;              // 节点总数
    int ans[SIZE];        // 每个模式串的最终出现次数
    int pidx;             // 模式串编号计数器
    vector<int> qorder;   // build 的 BFS 顺序 (按 Trie 深度递增, 应用1 用)
    int maxLen[SIZE];     // 每个状态结尾能匹配到的最长模式串长度 (应用1 用)

    void init() {
        tot = pidx = 0;
        qorder.clear();
        memset(ans, 0, sizeof(ans));
        memset(maxLen, 0, sizeof(maxLen));
        tr[0].init();
    }

    // 参数: 模式串 s (0-base), id 引用返回该模式串的编号
    void insert(const string& s, int& id) {
        int u = 0;
        for (int i = 0; i < (int)s.size(); i++) {
            int c = s[i] - 'a';
            if (!tr[u].son[c]) {
                tr[u].son[c] = ++tot;
                tr[tot].init();
            }
            u = tr[u].son[c];
            tr[u].len = i + 1;
        }
        if (!tr[u].idx) tr[u].idx = ++pidx;
        id = tr[u].idx;
    }

    void build() {
        queue<int> q;
        for (int i = 0; i < 26; i++) {
            if (tr[0].son[i]) q.push(tr[0].son[i]);
        }
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            qorder.push_back(u);
            for (int i = 0; i < 26; i++) {
                int v = tr[u].son[i];
                if (v) {
                    tr[v].fail = tr[tr[u].fail].son[i];
                    tr[tr[v].fail].du++;        // fail 树入度
                    q.push(v);
                } else {
                    tr[u].son[i] = tr[tr[u].fail].son[i];   // 补齐转移, 查询时无需判失配
                }
            }
        }
    }

    // 跑一遍文本, 每个状态累加"文本前缀以该状态结尾"的次数
    void query(const string& t) {
        int u = 0;
        for (char ch : t) {
            u = tr[u].son[ch - 'a'];
            tr[u].ans++;
        }
    }

    // 拓扑传播计数: 每个模式串出现次数 = 其 fail 子树内所有状态的 ans 之和
    // (每个文本位置都可能在 fail 链上代表多个模式串, 故必须累加)
    void topu() {
        queue<int> q;
        for (int i = 0; i <= tot; i++) {
            if (tr[i].du == 0) q.push(i);
        }
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            if (tr[u].idx) ans[tr[u].idx] = tr[u].ans;
            int v = tr[u].fail;
            tr[v].ans += tr[u].ans;
            if (!--tr[v].du) q.push(v);
        }
    }

    // 应用1: 每个状态结尾能匹配到的最长模式串长度
    // 正确性: maxLen[u] = max(maxLen[fail[u]], 以 u 结尾的模式串长度)
    // 沿 fail 链取 max 即"当前文本前缀的所有后缀里最长的模式串"
    void compute_maxlen() {
        memset(maxLen, 0, sizeof(maxLen));
        for (int u : qorder) {              // BFS 序 = 深度递增, fail 必已算过
            maxLen[u] = maxLen[tr[u].fail];
            if (tr[u].idx) maxLen[u] = max(maxLen[u], tr[u].len);
        }
    }

    // 应用2: 每个模式串首次出现的起始位置 (0-base, 未出现为 -1)
    // 复杂度警告: 每步沿 fail 链走, 最坏 O(文本长 × 最长模式串长); 大数据需优化为 fail 树+倍增
    vector<int> first;
    void query_first(const string& t) {
        first.assign(pidx + 1, -1);
        int u = 0;
        for (int i = 0; i < (int)t.size(); i++) {
            u = tr[u].son[t[i] - 'a'];
            for (int x = u; x; x = tr[x].fail) {   // fail 祖先都是"以 i 结尾的模式串"
                if (tr[x].idx && first[tr[x].idx] == -1) {
                    first[tr[x].idx] = i - tr[x].len + 1;   // 记录起始位置
                }
            }
        }
    }

    // 应用3: 文本是否被模式串完全覆盖
    // 思路: 每个位置 i 用"最长匹配长度 L"给出覆盖区间 [i-L+1, i];
    //       较短匹配的区间是它的子集, 所以合并这些区间即可判定全覆盖
    bool covered(const string& t) {
        compute_maxlen();
        vector<pair<int, int>> segs;
        int u = 0;
        for (int i = 0; i < (int)t.size(); i++) {
            u = tr[u].son[t[i] - 'a'];
            if (maxLen[u]) segs.push_back({i - maxLen[u] + 1, i});
        }
        sort(segs.begin(), segs.end());
        int cur = 0;                        // 已覆盖 [0, cur)
        for (auto [l, r] : segs) {
            if (l > cur) return false;      // 出现断档
            cur = max(cur, r + 1);
            if (cur >= (int)t.size()) return true;
        }
        return cur >= (int)t.size();
    }
} ac;

/*
用法示例
int main() {
    int n;
    cin >> n;
    ac.init();
    vector<int> id(n + 1);
    for (int i = 1; i <= n; i++) {
        string s;
        cin >> s;
        ac.insert(s, id[i]);
    }
    ac.build();

    string t;
    cin >> t;
    ac.query(t);
    ac.topu();
    for (int i = 1; i <= n; i++) {
        cout << ac.ans[id[i]] << "\n";      // 每个模式串出现次数
    }

    ac.query_first(t);
    for (int i = 1; i <= n; i++) {
        cout << ac.first[id[i]] << "\n";    // 每个模式串首次出现位置(-1 表示未出现)
    }

    cout << (ac.covered(t) ? "Yes" : "No") << "\n";   // 文本是否被模式串完全覆盖
    return 0;
}
*/
