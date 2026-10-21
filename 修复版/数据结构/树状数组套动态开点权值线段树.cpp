#include <bits/stdc++.h>
#define ll long long
#define endl '\n'
using namespace std;
namespace PST_BIT{
    /* ============================================================================
    * 树状数组套动态开点权值线段树（BIT × Dynamic PST）
    * ----------------------------------------------------------------------------
    * 一句话：外层 BIT 管"位置区间"，内层值域线段树管"值域统计"。
    * 适用题型：
    *     带修区间第 K 小 / 带修区间 ≤ x 的个数 / 带修区间排名 /
    *     带修区间前驱后继 / 带修二维数点 等
    * 复杂度：
    *     单次修改 O(log²n)
    *     单次查询 O(log²n)
    * 空间：
    *     节点数 ≈ (n + 2q) · log²n，按最坏情况开池子
    * ==========================================================================*/

    const int N = 1e5 + 5;    // 序列长度上限
    /* 节点池容量与内存代价(节点 12B):
     *   M = N*600 = 6e7 ≈ 720MB —— 实测 n=q=1e5(9.9万次修改)消耗约 5e7
     *   若修改占比超过 ~7 成, 需按 (n + 2·#modify)·log n·log m 估算, 极端全修改
     *   约 8.7e7 ≈ 1GB, 常规 OJ 不可行, 请改用下方 staticPST_BIT 或整体二分。
     * clone 内置 assert: 池耗尽会立刻 RE 报错, 而不是静默写出错误答案。 */
    const int M = N * 600;

    int n, q, m;              // n 序列长，q 操作数，m 离散化后值域大小
    int a[N];                 // 当前数组，随修改实时更新
    vector<int> vals;         // 离散化容器（初始值 + 所有修改后的值）

    /* ---------------------------- 离线操作记录 ---------------------------- */
    struct Op{
        int type;             // 1 = 修改(C), 2 = 查询(Q)
        int l, r, k;          // 查询参数：区间 [L,R]，第 k 小
        int pos, val;         // 修改参数：a[pos] = val
    };
    vector<Op> ops;

    /* =============================== 核心结构 ============================== */
    struct BitPST{

        /* -------- 内层：动态开点权值线段树 -------- */
        struct node{
            int l, r;
            int cnt;          // 【适配点 1】题型不同时改这个字段：
                            //   · 第K小 / ≤x个数      → cnt（计数）
                            //   · 区间和              → sum（ll）
                            //   · 区间最值            → mx（带初值）
        } t[M];

        int root[N];          // root[i] = BIT 第 i 个节点挂的线段树根编号
        int top = 0;          // 节点池指针；t[0] 是零值哨兵，天然代表空树

        /* 复制旧节点并返回新节点编号，主席树的关键操作 */
        int clone(int x){ assert(top < M); t[++top] = t[x]; return top; }

        /* 在子树 x 上做单点 pos += v（v 可为负） */
        int update(int x, int l, int r, int pos, int v){
            x = clone(x);
            t[x].cnt += v;                       // 【适配点 1】对应字段修改处
            if(l == r) return x;
            int mid = (l + r) >> 1;
            if(pos <= mid) t[x].l = update(t[x].l, l, mid, pos, v);
            else           t[x].r = update(t[x].r, mid + 1, r, pos, v);
            return x;
        }

        /* -------- 外层：树状数组 -------- */
        /* 在位置 i 上，对值 pos 做 +v（v 可为负） */
        void add(int i, int pos, int v){
            for(; i <= n; i += i & -i)
                root[i] = update(root[i], 1, m, pos, v);
        }

        /* -------- 查询：两条 BIT 链缓存 -------- */
        /* 大小只需 log n + 1，写到 N 是保守写法 */
        int qL[N], qR[N];

        /* 区间 [L,R] 的第 k 小，返回离散化后的下标
        * 【适配点 2】要换查询语义时主要改这里： */
        int kth(int L, int R, int k){
            int cl = 0, cr = 0;
            for(int i = L - 1; i > 0; i -= i & -i) qL[++cl] = root[i];
            for(int i = R;     i > 0; i -= i & -i) qR[++cr] = root[i];

            int l = 1, r = m;
            while(l < r){
                int mid = (l + r) >> 1;

                /* 【核心】左子树计数 = 右链求和 - 左链求和 */
                int cnt = 0;
                for(int i = 1; i <= cr; i++) cnt += t[t[qR[i]].l].cnt;
                for(int i = 1; i <= cl; i++) cnt -= t[t[qL[i]].l].cnt;

                if(k <= cnt){
                    for(int i = 1; i <= cr; i++) qR[i] = t[qR[i]].l;
                    for(int i = 1; i <= cl; i++) qL[i] = t[qL[i]].l;
                    r = mid;
                }else{
                    for(int i = 1; i <= cr; i++) qR[i] = t[qR[i]].r;
                    for(int i = 1; i <= cl; i++) qL[i] = t[qL[i]].r;
                    k -= cnt;
                    l = mid + 1;
                }
            }
            return l;
        }

        /* ---------- 可选：带修区间 ≤ x 的个数（按需展开） ----------
        * 思路：沿值域走，每次完全落在 [1,pos] 内的子树直接累加 cnt。
        *
        int countLE(int L, int R, int pos){
            int cl = 0, cr = 0;
            for(int i = L - 1; i > 0; i -= i & -i) qL[++cl] = root[i];
            for(int i = R;     i > 0; i -= i & -i) qR[++cr] = root[i];

            auto sum = [&](){
                int s = 0;
                for(int i = 1; i <= cr; i++) s += t[qR[i]].cnt;
                for(int i = 1; i <= cl; i++) s -= t[qL[i]].cnt;
                return s;
            };

            int l = 1, r = m, res = 0;
            while(l < r){
                int mid = (l + r) >> 1;
                if(pos <= mid){
                    for(int i = 1; i <= cr; i++) qR[i] = t[qR[i]].l;
                    for(int i = 1; i <= cl; i++) qL[i] = t[qL[i]].l;
                    r = mid;
                }else{
                    for(int i = 1; i <= cr; i++) qR[i] = t[qR[i]].l;
                    for(int i = 1; i <= cl; i++) qL[i] = t[qL[i]].l;
                    res += sum();
                    for(int i = 1; i <= cr; i++) qR[i] = t[qR[i]].r;
                    for(int i = 1; i <= cl; i++) qL[i] = t[qL[i]].r;
                    l = mid + 1;
                }
            }
            if(l <= pos) res += sum();
            return res;
        }
        * --------------------------------------------------------- */

    } bit;

    /* ------------------------- 离散化辅助 ------------------------- */
    int getId(int x){
        return lower_bound(vals.begin(), vals.end(), x) - vals.begin() + 1;
    }

    /* ============================================================================
    * 【易错点清单】—— 每一条都是真实踩过的坑
    * ----------------------------------------------------------------------------
    * 1) 必须"离线读入所有操作"再离散化。修改后的值也要 push 进 vals，
    *    否则 getId 时越界或找不到。
    *
    * 2) t[0] 是全零哨兵节点，clone(0) 天然得到空树，无需额外 buildEmpty。
    *    但如果你把 cnt 换成 mx 并需要负数初值，要手动初始化 t[0].mx = -INF。
    *
    * 3) BIT 下标从 1 开始。L-1 = 0 时 for 循环自然不进入，无需特判。
    *
    * 4) 节点池 M 一定要按 (n + 2q)·log²n 估算。
    *    n = q = 1e5 时，纯 BIT 做法约 3e5 次 add × log²n(≈289) ≈ 8.7e7 节点，
    *    实际常常 MLE。若 MLE，改"静态 PST + BIT 动态增量"的写法能省一半。
    *
    * 5) kth 里 cnt 统计的是"左子树"的贡献，不是当前节点的。
    *    换成 countLE 时，注意"完全落区间"的那部分要立刻累加，不能继续下探。
    *
    * 6) 修改 = 删旧值 + 加新值，两次 add，别忘了同步更新 a[pos]。
    *
    * 7) qL / qR 是每次查询临时缓存，不要设成 static，也不要跨查询复用。
    *
    * 8) 查询时别把 kth 的第一个参数写成 L（那是静态主席树的写法）；
    *    BIT 版本的"游标链"要从 L-1 开始。
    * ==========================================================================*/

    /* ============================================================================
    * 【常见题型的改造点速查】
    * ----------------------------------------------------------------------------
    * · 区间第 K 小（P2617 本尊）        → 直接使用 kth
    * · 区间 ≤ x 的个数                  → 展开 countLE（上文注释块）
    * · 区间中位数                       → kth(L, R, (R-L+2)/2)
    * · 区间 x 的排名（升序）            → countLE(L, R, getId(x)-1) + 1
    * · 区间前驱（严格 < x 的最大值）    → kth(L, R, countLE(L, R, getId(x)-1))
    * · 区间后继（严格 > x 的最小值）    → kth(L, R, countLE(L, R, getId(x)) + 1)
    *     注意：k > 区间长度时要输出无解
    * · 二维数点 / 带修矩阵和            → 把"位置"换成第一维坐标，
    *                                      外层 BIT 沿第一维走，内层沿第二维
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
                vals.push_back(val);            // ← 别忘了修改后的值
            }else{
                int l, r, k; cin >> l >> r >> k;
                ops[i] = {2, l, r, k, 0, 0};
            }
        }

        sort(vals.begin(), vals.end());
        vals.erase(unique(vals.begin(), vals.end()), vals.end());
        m = vals.size();

        /* 初始数组等价于"对每个位置做一次 +1 修改" */
        for(int i = 1; i <= n; i++)
            bit.add(i, getId(a[i]), +1);

        for(int i = 1; i <= q; i++){
            Op &op = ops[i];
            if(op.type == 1){
                bit.add(op.pos, getId(a[op.pos]), -1);
                bit.add(op.pos, getId(op.val),     +1);
                a[op.pos] = op.val;
            }else{
                int idx = bit.kth(op.l, op.r, op.k);
                cout << vals[idx - 1] << endl;
            }
        }
    }
}

namespace staticPST_BIT{
    /* ============================================================================
    * 静态主席树 + 树状数组套动态开点权值线段树
    *     (Static PST + BIT × Dynamic PST)
    * ----------------------------------------------------------------------------
    * 一句话：
    *     静态 PST 保存"初始数组"的前缀版本；
    *     BIT 套动态 PST 只保存"修改量"；
    *     查询时两部分贡献相加，得到 [L,R] 的当前值域分布。
    *
    * 相比纯 BIT 版本的优势：
    *     · 初始 n 个数只花 n·log m 节点，不再花 n·log n·log m
    *     · 内存约省 60%，查询常数更小
    *
    * 复杂度：
    *     单次修改 O(log²n)
    *     单次查询 O(log²n)
    * 空间：
    *     静态 PST： n · log m
    *     BIT 增量： 2q · log n · log m
    * ==========================================================================*/

    const int N  = 1e5 + 5;
    const int M1 = N * 25;     // 静态 PST 节点池：n · log m (实测 n=1e5 消耗 1.87e6, N*25 留余量)
    const int M2 = N * 400;    // 动态 BIT 节点池：2q · log n · log m (实测全修改负载消耗 3.36e7/4e7)

    int n, q, m;               // n 序列长，q 操作数，m 离散化后值域大小
    int a[N];                  // 当前数组，随修改实时更新
    vector<int> vals;          // 离散化容器（初始值 + 所有修改后的值）

    /* ---------------------------- 离线操作记录 ---------------------------- */
    struct Op{
        int type;              // 1 = 修改(C), 2 = 查询(Q)
        int l, r, k;           // 查询参数：区间 [L,R]，第 k 小
        int pos, val;          // 修改参数：a[pos] = val
    };
    vector<Op> ops;

    /* =============================== 核心结构 ============================== */
    struct BitPST{

        /* ======================= 静态部分：普通主席树 =======================
        * 用途：保存初始数组的前缀版本 st.root[i] = 前 i 个数的值域分布
        * 特点：不带修改，纯查询 */
        struct StaticPST{
            struct node{
                int l, r;
                int cnt;       // 【适配点 1】按题型换：cnt / sum / mx
            } t[M1];

            int root[N];       // root[i]：前缀 [1..i] 的版本根
            int top = 0;       // t[0] 是零值哨兵，天然代表空树

            int clone(int x){ assert(top < M1); t[++top] = t[x]; return top; }

            int update(int x, int l, int r, int pos, int v){
                x = clone(x);
                t[x].cnt += v;                       // 【适配点 1】合并处
                if(l == r) return x;
                int mid = (l + r) >> 1;
                if(pos <= mid) t[x].l = update(t[x].l, l, mid, pos, v);
                else           t[x].r = update(t[x].r, mid + 1, r, pos, v);
                return x;
            }
        } st;

        /* ================ 动态部分：BIT 套动态开点权值线段树 ================
        * 用途：每个 BIT 节点挂一棵值域线段树，只记录"修改量"（+1/-1）
        * 特点：初始为空；每次修改走 log n 个 BIT 节点 */
        struct DiffPST{
            struct node{
                int l, r;
                int cnt;       // 【适配点 1】同上
            } t[M2];

            int root[N];       // root[i]：BIT 节点 i 挂的动态线段树根
            int top = 0;

            int clone(int x){ assert(top < M2); t[++top] = t[x]; return top; }

            int update(int x, int l, int r, int pos, int v){
                x = clone(x);
                t[x].cnt += v;
                if(l == r) return x;
                int mid = (l + r) >> 1;
                if(pos <= mid) t[x].l = update(t[x].l, l, mid, pos, v);
                else           t[x].r = update(t[x].r, mid + 1, r, pos, v);
                return x;
            }

            /* 在位置 i 上，对值 pos 做 +v */
            void add(int i, int pos, int v){
                for(; i <= n; i += i & -i)
                    root[i] = update(root[i], 1, m, pos, v);
            }
        } dif;

        /* ========================= 查询缓存 ========================= */
        /* 两条 BIT 游标链，大小只需 log n + 1，N 是保守写法 */
        int qL[N], qR[N];

        /* ------------------- 区间 [L,R] 第 k 小 -------------------
        * 【适配点 2】换查询语义时主要改这里 */
        int kth(int L, int R, int k){
            /* 动态部分：两条 BIT 链 */
            int cl = 0, cr = 0;
            for(int i = L - 1; i > 0; i -= i & -i) qL[++cl] = dif.root[i];
            for(int i = R;     i > 0; i -= i & -i) qR[++cr] = dif.root[i];

            /* 静态部分：两个版本根
            * 这里和纯 BIT 版本唯一不同的地方：
            *   纯 BIT 的 u、v 从 root[L-1]、root[R] 拿，
            *   这里从 st.root[L-1]、st.root[R] 拿。 */
            int u = st.root[L - 1], v = st.root[R];

            int l = 1, r = m;
            while(l < r){
                int mid = (l + r) >> 1;

                /* 【核心】左子树计数 = 静态差值 + 动态差值 */
                int cnt = st.t[st.t[v].l].cnt - st.t[st.t[u].l].cnt;
                for(int i = 1; i <= cr; i++) cnt += dif.t[dif.t[qR[i]].l].cnt;
                for(int i = 1; i <= cl; i++) cnt -= dif.t[dif.t[qL[i]].l].cnt;

                if(k <= cnt){
                    u = st.t[u].l;
                    v = st.t[v].l;
                    for(int i = 1; i <= cr; i++) qR[i] = dif.t[qR[i]].l;
                    for(int i = 1; i <= cl; i++) qL[i] = dif.t[qL[i]].l;
                    r = mid;
                }else{
                    u = st.t[u].r;
                    v = st.t[v].r;
                    for(int i = 1; i <= cr; i++) qR[i] = dif.t[qR[i]].r;
                    for(int i = 1; i <= cl; i++) qL[i] = dif.t[qL[i]].r;
                    k -= cnt;
                    l = mid + 1;
                }
            }
            return l;
        }

        /* ---------- 可选：带修区间 ≤ x 的个数 ----------
        * 思路和 kth 相同，只是把"二分行进"改成"沿值域下探 + 累加"。
        *
        int countLE(int L, int R, int pos){
            int cl = 0, cr = 0;
            for(int i = L - 1; i > 0; i -= i & -i) qL[++cl] = dif.root[i];
            for(int i = R;     i > 0; i -= i & -i) qR[++cr] = dif.root[i];
            int u = st.root[L - 1], v = st.root[R];

            int l = 1, r = m, res = 0;
            while(l < r){
                int mid = (l + r) >> 1;
                if(pos <= mid){
                    u = st.t[u].l; v = st.t[v].l;
                    for(int i = 1; i <= cr; i++) qR[i] = dif.t[qR[i]].l;
                    for(int i = 1; i <= cl; i++) qL[i] = dif.t[qL[i]].l;
                    r = mid;
                }else{
                    int s = st.t[st.t[v].l].cnt - st.t[st.t[u].l].cnt;
                    for(int i = 1; i <= cr; i++) s += dif.t[dif.t[qR[i]].l].cnt;
                    for(int i = 1; i <= cl; i++) s -= dif.t[dif.t[qL[i]].l].cnt;
                    res += s;

                    u = st.t[u].r; v = st.t[v].r;
                    for(int i = 1; i <= cr; i++) qR[i] = dif.t[qR[i]].r;
                    for(int i = 1; i <= cl; i++) qL[i] = dif.t[qL[i]].r;
                    l = mid + 1;
                }
            }
            if(l <= pos){
                int s = st.t[v].cnt - st.t[u].cnt;
                for(int i = 1; i <= cr; i++) s += dif.t[qR[i]].cnt;
                for(int i = 1; i <= cl; i++) s -= dif.t[qL[i]].cnt;
                res += s;
            }
            return res;
        }
        * --------------------------------------------------------- */

    } pst;

    /* ------------------------- 离散化辅助 ------------------------- */
    int getId(int x){
        return lower_bound(vals.begin(), vals.end(), x) - vals.begin() + 1;
    }

    /* ============================================================================
    * 【易错点清单】
    * ----------------------------------------------------------------------------
    * 1) 静态 PST 和动态 BIT 共用同一个 m（离散化值域大小），
    *    所以初始数组、修改后的值必须一起参与离散化。
    *
    * 2) st.root[0] = 0 天然代表"空前缀"，不需要显式 build 空树。
    *    静态 PST 的版本链表是 root[i] = update(root[i-1], ...)。
    *
    * 3) 动态 BIT 的 root 初始全 0，因为它只记录修改量；
    *    若某位置从未被修改，该 BIT 链上所有 root 都是 0，等价空树。
    *
    * 4) kth 中要同时推进两组游标：
    *      · 静态：u = st.t[u].l / st.t[u].r
    *      · 动态：qL[i] / qR[i] = dif.t[...].l / .r
    *    少写一组会导致下一层用错子树。
    *
    * 5) 计数公式：左子树 cnt = 静态差值 + 动态差值。
    *    顺序不能颠倒（其实是加法交换律，但意思是两者都要算进去），
    *    别只写了静态差就完事。
    *
    * 6) 修改 = "删旧值 + 加新值"，两次 dif.add，别忘了同步更新 a[pos]。
    *
    * 7) M1 / M2 分开估：
    *      M1 ≈ n · log m
    *      M2 ≈ 2q · log n · log m
    *    n = q = 1e5 时，M1 ≈ 2e6，M2 ≈ 6e7。
    *    若仍 MLE，改"二分 + 离线 CDQ"或"整体二分"。
    *
    * 8) qL / qR 是每次查询临时缓存，不要设成 static，也不要跨查询复用。
    * ==========================================================================*/

    /* ============================================================================
    * 【常见题型的改造点速查】
    * ----------------------------------------------------------------------------
    * · 区间第 K 小（P2617 本尊）        → 直接使用 kth
    * · 区间 ≤ x 的个数                  → 展开 countLE
    * · 区间中位数                       → kth(L, R, (R-L+2)/2)
    * · 区间 x 的排名（升序）            → countLE(L, R, getId(x)-1) + 1
    * · 区间前驱（严格 < x 的最大值）    → kth(L, R, countLE(L, R, getId(x)-1))
    * · 区间后继（严格 > x 的最小值）    → kth(L, R, countLE(L, R, getId(x)) + 1)
    * · 二维数点 / 带修矩阵和            → 把"位置"换成第一维坐标，
    *                                      静态 PST 沿第一维按前缀建，
    *                                      动态 BIT 沿第一维走
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

        /* 静态 PST：链式建前缀版本
        * 初始数组 a[i] 全部进静态部分，一个都不进 BIT */
        for(int i = 1; i <= n; i++)
            pst.st.root[i] = pst.st.update(pst.st.root[i-1], 1, m, getId(a[i]), +1);

        /* 动态 BIT：初始为空。只有"修改"才会往里加东西 */

        for(int i = 1; i <= q; i++){
            Op &op = ops[i];
            if(op.type == 1){
                /* 修改 = 在 BIT 里记一笔"删旧 + 加新" */
                pst.dif.add(op.pos, getId(a[op.pos]), -1);
                pst.dif.add(op.pos, getId(op.val),     +1);
                a[op.pos] = op.val;
            }else{
                int idx = pst.kth(op.l, op.r, op.k);
                cout << vals[idx - 1] << endl;
            }
        }
    }

}