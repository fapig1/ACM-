#include <bits/stdc++.h>
using namespace std;

//AC自动机模板

//本模板用于统计每个模式串出现的次数
struct AC {
    static const int SIZE = 2e5 + 6;
    
    // Trie树节点结构
    struct Node {
        int son[26];      // 子节点数组，存储26个小写字母的转移
        int ans;          // 节点匹配计数（查询时累加）
        int fail;         // 失败指针
        int du;           // 入度，用于拓扑排序
        int idx;          // 模式串编号（相同模式串映射到同一个编号）
        
        void init() {
            memset(son, 0, sizeof(son));
            ans = fail = idx = 0;
        }
    };
    
    Node tr[SIZE];        // Trie树节点数组
    int tot;              // 当前节点总数
    int ans[SIZE];        // 存储每个模式串的最终匹配次数
    int pidx;             // 模式串编号计数器
    
    AC() {
        tot = pidx = 0;
        tr[0].init();
    }

    // 参数：模式串s（下标从1开始），模式串编号的引用
    void insert(string s, int &idx) {
        int u = 0;
        for (int i = 1; s[i]; i++) {
            int c = s[i] - 'a';
            if (!tr[u].son[c]) {
                tr[u].son[c] = ++tot;
                tr[tot].init();
            }
            u = tr[u].son[c];
        }

        if (!tr[u].idx) {
            tr[u].idx = ++pidx;
        }
        idx = tr[u].idx;
    }
    
    void build() {
        queue<int> q;

        for (int i = 0; i < 26; i++) {
            if (tr[0].son[i]) {
                q.push(tr[0].son[i]);
            }
        }
        
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (int i = 0; i < 26; i++) {
                int v = tr[u].son[i];
                if (v) {
                    tr[v].fail = tr[tr[u].fail].son[i];
                    tr[tr[tr[u].fail].son[i]].du++;
                    q.push(v);
                } else {
                    tr[u].son[i] = tr[tr[u].fail].son[i];
                }
            }
        }
    }

    //下标由1开始
    void query(string t) {
        int u = 0;
        for (int i = 1; t[i]; i++) {
            int c = t[i] - 'a';
            u = tr[u].son[c];
            tr[u].ans++;
        }
    }
    
    // 拓扑排序传播匹配计数
    void topu() {
        queue<int> q;
        for (int i = 0; i <= tot; i++) {
            if (tr[i].du == 0) {
                q.push(i);
            }
        }
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            if (tr[u].idx) {
                ans[tr[u].idx] = tr[u].ans;
            }
            int v = tr[u].fail;
            tr[v].ans += tr[u].ans;
            if (!--tr[v].du) {
                q.push(v);
            }
        }
    }
} ac;

const int LEN = 2e6 + 6;  // 最大字符串长度
const int N = 2e5 + 6;    // 最大模式串数量
int idx[N];               // 存储每个模式串的编号
/* 
使用示例
int main() {
    int n;
    cin>>n;
    // 读入模式串
    for (int i = 1; i <= n; i++) {
        string s;
        cin>>s;
        s=' '+s;                     // 字符串下标从1开始
        ac.insert(s, idx[i]);        // 插入模式串，获取编号
    }
    
    ac.build();
    
    string s;
    cin>>s;
    s=' '+s;
    ac.query(s);
    // 拓扑排序传播计数(此处可以查询多个文本串)
    ac.topu();
    
    // 输出每个模式串的匹配次数
    for (int i = 1; i <= n; i++) {
        cout<<ac.ans[idx[i]]<<"\n";
    }
    
    return 0;
}
*/
