#include <bits/stdc++.h>
#define ll long long
#define endl '\n'
using namespace std;

/* ============================================================================
 * 线段树套动态开点权值线段树（SegmentTree × Dynamic PST）
 * ----------------------------------------------------------------------------
 * 一句话：外层位置线段树把 [L,R] 拆成 O(log n) 个节点，
 *        每个节点挂一棵值域线段树，这些"根"一起做值域二分。
 * 适用题型：
 *     带修区间第 K 小 / 区间 ≤ x 个数 / 区间排名 /
 *     带修区间前驱后继 / 二维数点 等
 * 复杂度：
 *     单次修改 O(log²n)
 *     单次查询 O(log²n)
 * 空间：
 *     节点数 ≈ (n + 2q) · log n · log m，比 BIT 版本常数略大
 * ==========================================================================*/

const int N = 1e5 + 5;
/* 【易错点 1】内存池：
 *   初始 n 个数，每个位置走 log n 个外层节点，每个内层再走 log m 个。
 *   n = q = 1e5 全修改的极端负载实测消耗约 9.7e7 节点：
 *     M ≈ (n + 2q) · log n · log m ≈ 1e8 (N*1000)
 *   注意 M=1e8 × 12B/节点 ≈ 1.2GB, 常规 OJ(256~512MB) 必 MLE ——
 *   请按题目真实规模缩小 N/M (例如 n=q=3e4 时 M≈9e6 即够),
 *   或改用"树状数组套"文件中内存更省的 staticPST_BIT 版本。
 * clone 内置 assert: 池耗尽会立刻 RE, 而不是静默写出错误答案。 */
const int M = N * 1000;

int n, q, m;
int a[N];
vector<int> vals;

struct Op{
    int type;      // 1 修改, 2 查询
    int l, r, k;
    int pos, val;
};
vector<Op> ops;

// 【速查】线段树套权值线段树（带修区间第 k 小）
// 用途: 同"树状数组套"，外层换成位置线段树；能做 BIT 做不了的（外层需要区间信息时）
// 用法: 离线 + 离散化后，seg.modify(1, 1, n, pos, id, ±1) 删旧加新；查询 vals[seg.kth(l, r, k) - 1]
// 复杂度: O(log² n)；内存比 BIT 版还大，n=q=1e5 满负载约 1.2GB，按真实规模缩小 N/M
/* =============================== 核心结构 ============================== */
struct SegPST{

    /* ---------------- 内层：动态开点权值线段树 ---------------- */
    struct node{
        int l, r;
        int cnt;      // 【适配点 1】按题型换字段：cnt / sum / mx
    } t[M];

    int root[N << 2]; // 外层线段树每个节点挂一棵值域线段树的根
    int top = 0;      // t[0] 是全零哨兵

    int clone(int x){ assert(top < M); t[++top] = t[x]; return top; }

    /* 在子树 x 上，值 pos 处 += v */
    int update(int x, int l, int r, int pos, int v){
        x = clone(x);
        t[x].cnt += v;                       // 【适配点 1】合并处
        if(l == r) return x;
        int mid = (l + r) >> 1;
        if(pos <= mid) t[x].l = update(t[x].l, l, mid, pos, v);
        else           t[x].r = update(t[x].r, mid + 1, r, pos, v);
        return x;
    }

    /* ---------------- 外层：位置线段树 ---------------- */

    /* 位置 pos 上，值 val += v
     * 沿根到叶路径，所有经过的外层节点都要挂上这次修改 */
    void modify(int nd, int l, int r, int pos, int val, int v){
        root[nd] = update(root[nd], 1, m, val, v);
        if(l == r) return;
        int mid = (l + r) >> 1;
        if(pos <= mid) modify(nd << 1,     l,   mid, pos, val, v);
        else           modify(nd << 1 | 1, mid+1, r, pos, val, v);
    }

    /* ---------------- 查询：收集 + 二分 ---------------- */

    /* 【易错点 2】qNode 大小：区间 [L,R] 在外层分解最多 2·log₂n 个节点。
     * 保守开 N 或 64+ 即可；一定不要开成 2*log n 那种紧贴边界的数。 */
    int qNode[N], qcnt;

    /* 收集外层线段树中覆盖 [L,R] 的所有节点对应的值域根 */
    void collect(int nd, int l, int r, int L, int R){
        if(L <= l && r <= R){
            qNode[++qcnt] = root[nd];
            return;
        }
        int mid = (l + r) >> 1;
        if(L <= mid) collect(nd << 1,     l,   mid, L, R);
        if(R >  mid) collect(nd << 1 | 1, mid+1, r, L, R);
    }

    /* 区间 [L,R] 第 k 小，返回离散化下标
     * 【适配点 2】换语义时主要改这里 */
    int kth(int L, int R, int k){
        qcnt = 0;
        collect(1, 1, n, L, R);

        int l = 1, r = m;
        while(l < r){
            int mid = (l + r) >> 1;
            int cnt = 0;
            for(int i = 1; i <= qcnt; i++)
                cnt += t[t[qNode[i]].l].cnt;

            if(k <= cnt){
                for(int i = 1; i <= qcnt; i++) qNode[i] = t[qNode[i]].l;
                r = mid;
            }else{
                for(int i = 1; i <= qcnt; i++) qNode[i] = t[qNode[i]].r;
                k -= cnt;
                l = mid + 1;
            }
        }
        return l;
    }

    /* ---------- 可选：区间 ≤ x 的个数（对照 BIT 版本照搬） ----------
    int countLE(int L, int R, int pos){
        qcnt = 0;
        collect(1, 1, n, L, R);

        int l = 1, r = m, res = 0;
        while(l < r){
            int mid = (l + r) >> 1;
            if(pos <= mid){
                for(int i = 1; i <= qcnt; i++) qNode[i] = t[qNode[i]].l;
                r = mid;
            }else{
                for(int i = 1; i <= qcnt; i++) res += t[t[qNode[i]].l].cnt;
                for(int i = 1; i <= qcnt; i++) qNode[i] = t[qNode[i]].r;
                l = mid + 1;
            }
        }
        if(l <= pos)
            for(int i = 1; i <= qcnt; i++) res += t[qNode[i]].cnt;
        return res;
    }
    * --------------------------------------------------------- */

} seg;

/* ----------------------------- 辅助 ----------------------------- */
int getId(int x){
    return lower_bound(vals.begin(), vals.end(), x) - vals.begin() + 1;
}

/* ============================================================================
 * 【易错点清单】
 * ----------------------------------------------------------------------------
 * 1) 节点池 M 要按 (n + 2q)·log n · log m 估，比 BIT 版本更大：
 *      · 初始建树：n 个位置 × log n 个外层节点 × log m 个内层节点
 *      · 每次修改：2 次 × log n × log m
 *    n = q = 1e5 时约 9e7 节点，别按 BIT 的 4e7 开，会 RE。
 *
 * 2) 外层线段树索引从 1 开始（1 << 1 = 2），root[] 至少要开 N << 2。
 *
 * 3) modify 时**外层路径上每个节点都要挂上这次修改**：
 *    root[nd] = update(root[nd], ...) 必须在递归前执行，不能只更新叶子。
 *
 * 4) 离散化必须离线读入所有修改后的值；否则 getId 越界。
 *
 * 5) t[0] 是哨兵空节点，clone(0) 天然空树；若 cnt 换成 mx 且需要 -INF 初值，
 *    请手动给 t[0].mx 赋 -INF。
 *
 * 6) kth 中"二分每一步更新 qNode 指针"是关键，漏更新会导致下一层用错子树。
 *
 * 7) collect 是**查询前**一次性收集，不要和 modify 混用同一个缓冲区。
 * ==========================================================================*/

/* ============================================================================
 * 【常见题型的改造点速查】
 * ----------------------------------------------------------------------------
 * · 区间第 K 小                → 直接 kth(L, R, k)
 * · 区间 ≤ x 的个数            → 展开 countLE
 * · 区间中位数                 → kth(L, R, (R-L+2)/2)
 * · 区间 x 的排名（升序）      → countLE(L, R, getId(x)-1) + 1
 * · 区间前驱（< x 的最大值）   → kth(L, R, countLE(L, R, getId(x)-1))
 * · 区间后继（> x 的最小值）   → kth(L, R, countLE(L, R, getId(x)) + 1)
 * · 带"区间修改"的题型        → 把 modify 改成类似 lazy 的做法，
 *                                并在 kth 二分时同步下推（难度上一个台阶）
 * ==========================================================================*/

/* ------------------------------- 主流程 ------------------------------- */
void solve(){
    cin >> n >> q;

    for(int i = 1; i <= n; i++){
        cin >> a[i];
        vals.push_back(a[i]);
    }

    ops.assign(q + 1, Op{});
    for(int i = 1; i <= q; i++){
        char opt; cin >> opt;
        if(opt == 'C'){
            int pos, val; cin >> pos >> val;
            ops[i] = {1, 0, 0, 0, pos, val};
            vals.push_back(val);
        }else{
            int l, r, k; cin >> l >> r >> k;
            ops[i] = {2, l, r, k, 0, 0};
        }
    }

    sort(vals.begin(), vals.end());
    vals.erase(unique(vals.begin(), vals.end()), vals.end());
    m = vals.size();

    /* 初始数组：对每个位置做一次 +1 修改 */
    for(int i = 1; i <= n; i++)
        seg.modify(1, 1, n, i, getId(a[i]), +1);

    for(int i = 1; i <= q; i++){
        Op &op = ops[i];
        if(op.type == 1){
            seg.modify(1, 1, n, op.pos, getId(a[op.pos]), -1);
            seg.modify(1, 1, n, op.pos, getId(op.val),     +1);
            a[op.pos] = op.val;
        }else{
            int idx = seg.kth(op.l, op.r, op.k);
            cout << vals[idx - 1] << endl;
        }
    }
}
