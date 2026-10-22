#include <bits/stdc++.h>
using namespace std;

// 【速查】可持久化 01-Trie（区间最大异或，P4735）
// 用途: 依次插入若干数，询问"第 l..r 个插入的数里，与 x 异或的最大值"
// 用法: PersistentTrie T(最大插入次数);  T.insert(y);   // 第 i 次插入后版本号为 i（版本 0 为空）
//       T.query(l, r, x) -> max_{l <= i <= r} (x ^ 第 i 个数)，要求 1 <= l <= r <= 已插入个数
// 套路(P4735): 令 s[i] 为前缀异或，先插入 s[0]=0；问 max(s[p-1] ^ s[n] ^ x)，p∈[l,r] 就是查
//       插入序号 [l, r]（s[l-1]..s[r-1] 对应序号 l..r）
// 复杂度: 插入 / 查询 O(BITS)；节点数 = 插入次数 × (BITS + 1)
struct PersistentTrie {
    static const int BITS = 30;              // 值域 < 2^30；long long 值域改成 62 并把 int 换 long long
    vector<array<int, 2>> ch;
    vector<int> cnt, root;

    PersistentTrie(int maxInsert = 0) {
        ch.reserve((size_t)(maxInsert + 1) * (BITS + 1) + 1);
        cnt.reserve(ch.capacity());
        ch.push_back({0, 0}); cnt.push_back(0);   // 0 号为空节点
        root.push_back(0);                         // 版本 0: 空树
    }
    int newNode() { ch.push_back({0, 0}); cnt.push_back(0); return (int)ch.size() - 1; }

    void insert(int x) {
        int p = root.back(), q = newNode();
        root.push_back(q);
        for (int i = BITS - 1; i >= 0; i--) {
            int b = x >> i & 1;
            ch[q] = ch[p]; cnt[q] = cnt[p] + 1;
            int nq = newNode();
            ch[q][b] = nq;
            p = ch[p][b]; q = nq;
        }
        ch[q] = ch[p]; cnt[q] = cnt[p] + 1;
    }
    int query(int l, int r, int x) {
        int u = root[r], v = root[l - 1], res = 0;
        for (int i = BITS - 1; i >= 0; i--) {
            int b = x >> i & 1;
            if (cnt[ch[u][!b]] - cnt[ch[v][!b]] > 0) { res |= 1 << i; u = ch[u][!b]; v = ch[v][!b]; }
            else { u = ch[u][b]; v = ch[v][b]; }
        }
        return res;
    }
};
