# David_字符串（10.21 修复版）

> **通用前导**：本文档所有代码块为片段，需配合以下前导使用：

```cpp
#include <bits/stdc++.h>
using namespace std;
```

标注 **【本次修复】** 的模板经过 2026-10-21 全量审查修复，并全部通过随机对拍/压测回归验证；各修复点在代码注释中亦有说明。

[TOC]

### 字典树

> 【本次修复】移除 #define int long long（原版静态数组因此占 1.68GB）；N=1e5(28MB)，按题目总长调整。

```cpp
/* 原版有 #define int long long, 导致 ch[3e6][70] 每格 8B 共 1.68GB —— 已移除。
 * 字符集 62 (大写/小写/数字), 每个节点 70 个儿子槽, int(4B) 下:
 *   N=1e5 → 28MB; N=5e5 → 140MB。按题目"所有串总长"上限调整 N。 */
const int N=1e5+10;
const int base=13331;
const int mod=1e9+7;
int n,_T,q;
string s;
int ch[N][70]={0};
int idx;
int cnt[N];

int hashn(char x)
{
  if(x>='A'&&x<='Z')
  {
    return x-'A';
  }
  else if(x>='a'&&x<='z')
  {
    return x-'a'+26;
  }
  else 
  return x-'0'+52;
}

void insert(string s)
{
  int p=0;
  int len=s.length();
  for(int i=0;i<len;i++)
  {
    int j=hashn(s[i]);
    if(!ch[p][j]) ch[p][j]=++idx;
    p=ch[p][j];
    cnt[p]++;
  }
  
}

int query(string s)
{
  int p=0;
  int len=s.length();
  for(int i=0;i<len;i++)
  {
    int j=hashn(s[i]);
    if(!ch[p][j]) return 0;
    p=ch[p][j];
  }
  return cnt[p];
}

void solve()
{
  idx=0;
  cin>>n>>q;
  for(int i=1;i<=n;i++)
  {
    cin>>s;
    insert(s);
  }
  while(q--)
  {
    cin>>s;
    cout<<query(s)<<endl;
  }
  for(int i=0;i<=idx;i++)
  {
    for(int j=0;j<=65;j++)
    {
      ch[i][j]=0;
    }
  }
  for(int i=0;i<=idx;i++)
  {
    cnt[i]=0;
  }
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    
    cin>>_T;
    while(_T--)
    {
      solve();
    }

    return 0;
}
```

### AC自动机

> fail 树拓扑传播计数；含首次出现位置与全覆盖判定两个应用。

```cpp
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
```

### KMP与马拉车

> 【本次修复】原版马拉车 getd() 为无分隔符/无边界的死代码，已重写为标准 manacher(str)（返回最长回文长度，奇偶通用）；main 仍为 KMP 示例。

```cpp
#define int long long

const int N=1e6+10;
int n,m;
char s1[N],s2[N];
string s,p;
int ne[N];

//马拉车(Manacher)求最长回文子串长度
//原版 getd() 无 '#' 分隔符且无边界检查(不能处理偶回文, 越界为 UB), 且从未被调用 —— 已重写
//d[i]: 变换串 t = "#a#b#..." 中以 i 为中心的回文臂长, 恰等于原串对应回文的长度
int d[N * 2];
int manacher(const string& str){
    string t = "#";
    for(char c : str){ t += c; t += '#'; }
    int m = t.size(), C = 0, R = 0, best = 0;
    for(int i = 0; i < m; i++){
        int k = (i < R) ? min(d[2 * C - i], R - i) : 0;
        while(i - k - 1 >= 0 && i + k + 1 < m && t[i - k - 1] == t[i + k + 1]) k++;
        d[i] = k;
        if(i + k > R){ C = i; R = i + k; }
        best = max(best, (int)d[i]);
    }
    return best;
}


signed main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
  
    cin>>s>>p;
    int m=s.length(),n=p.length();
    ne[1]=0;
    for(int i=1;i<=m;i++)
    {
      s1[i]=s[i-1];
    }
    for(int i=1;i<=n;i++)
    {
      s2[i]=p[i-1];
    }
    for(int i=2,j=0;i<=n;i++)
    {
      while(j&&s2[i]!=s2[j+1]) j=ne[j];
      if(s2[i]==s2[j+1]) j++;
      ne[i]=j;
    }
    for(int i=1,j=0;i<=m;i++)
    {
      while(j&&s1[i]!=s2[j+1]) j=ne[j];
      if(s1[i]==s2[j+1]) j++;
      if(j==n) 
      {
        cout<<i-n+1<<endl;
        j=ne[j];
      }
    }

    for(int i=1;i<=n;i++)
    {
      cout<<ne[i]<<' ';
    }

    return 0;
}
```

### 字符串哈希（含二维）

> 双哈希 1e9+7/1e9+9，BASE=277；二维版支持子矩阵哈希。

```cpp
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
```

### 后缀数组

> 【本次修复】原版计数数组 cnt 在 n≥256 时前缀和越界一格；已扩容。含 height+ST表任意两后缀 LCP。

```cpp
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
        vector<int> oldrnk(n), cnt(max(n, 256) + 1);
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
```

### 后缀自动机

> 含 endpos 计数与出现次数查询。

```cpp
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
```

### 后缀树

> 反串 SAM 的 link 树；迭代 DFS 建倍增 LCA（防链爆栈）；up[20] 支持到约 1e6。

```cpp
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
```
