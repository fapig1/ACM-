#include <bits/stdc++.h>
using namespace std;
#define int long long

/* ============================================================================
 * 李超线段树 —— 三个版本
 * ----------------------------------------------------------------------------
 * 原版缺陷: 三个版本放在同一作用域, Line / LCSeg / INF 重名, 整个文件无法编译。
 * 修复: 版本1/2 各自封装进命名空间, 版本3 原名本就唯一, 一并入命名空间统一风格。
 *
 * 用法:
 *   LC_double :: LCSeg seg(n);          // double 最小值版, x∈[1,n] 整数域
 *   LC_disc   :: LCSeg seg(m, coords);  // 离散化 int 最大值版, coords 为升序去重 x 值表
 *   LC_frac   :: LCSegFrac seg(n);      // __int128 分数精确比较最小值版
 * 注意: 文件顶部 #define int long long 会影响其后粘贴的代码,
 *       scanf("%d") 读 int 变量会因类型不符得到垃圾值, main 请写 signed main。
 * ==========================================================================*/

// ==================== 版本1: double 最小值版 ====================
namespace LC_double {

//普通李超线段树
//用于查询若干线段或者直线在某点处的最值
const double eps = 1e-9, INF = 1e18;

int sgn(double x){
    if(fabs(x) < eps) return 0;
    return x < 0 ? -1 : 1;
}

struct Line{
    double k, b;
    bool exists;
                                //更改处（-INF）
    Line(double k = 0, double b = INF) : k(k), b(b), exists(false) {}

    double cal(int x)const{
        return k * x + b;
    }
};
//插入线段复杂度为 log * log 其余为 log (当前为维护最小值版本)
struct LCSeg{
    int n;
    vector<Line> tr;
    LCSeg(int n) : n(n), tr(n * 4) {}

    void add(int p, int l, int r, Line val){
        if(!tr[p].exists){
            tr[p] = val;
            tr[p].exists = true;
            return;
        }
        int mid = l + r >> 1;
                        //更改处 <
        if(tr[p].cal(mid) > val.cal(mid)) swap(tr[p], val);
        if(l == r) return;
                        //更改处 >
        if(val.cal(l) < tr[p].cal(l)){
            add(p << 1, l, mid, val);
        } else {
            add(p << 1 | 1, mid + 1, r, val);
        }
    }

    // 插入一条全局直线
    void addLine(double k, double b){
        Line l(k, b);
        l.exists = true;
        add(1, 1, n, l);
    }

    // 插入一条在 [L, R] 范围内的线段
    void addSeg(int p, int l, int r, int ql, int qr, Line val){
        if(ql <= l && r <= qr){
            add(p, l, r, val);
            return;
        }
        int mid = l + r >> 1;
        if(ql <= mid) addSeg(p << 1, l, mid, ql, qr, val);
        if(mid < qr) addSeg(p << 1 | 1, mid + 1, r, ql, qr, val);
    }
    // 查询 x 处的最小 y 值（最大值版本把 return 里的 min 改成 max）
    double ask(int p, int l, int r, int x){
        if(x < l || x > r) return INF;//更改处-INF
        // 注意：本节点为空时不能直接 return！addSeg 只把线段存在被完全覆盖的节点上，
        // 祖先可能为空、而子孙节点里存着线段，必须继续往下递归才能查到。
        double res = tr[p].exists ? tr[p].cal(x) : INF;//更改处-INF
        if(l == r) return res;
        int mid = l + r >> 1;
                        //更改处 max
        if(x <= mid) return min(res, ask(p << 1, l, mid, x));
        return min(res, ask(p << 1 | 1, mid + 1, r, x));
    }
};

} // namespace LC_double

// ==================== 版本2: 离散化 int 最大值版 ====================
namespace LC_disc {

//离散化李超线段树（对所有可能的x值进行离散化处理）（最大值版本）
const int INF = 2e18; // 维护最大值，初始b设为-INF (在 #define int long long 下为 64 位, 2e18 安全)

struct Line {
    int k, b;
    bool exists;
    Line(int k = 0, int b = -INF) : k(k), b(b), exists(false) {}
    int cal(int x) const {
        return k * x + b;
    }
};

struct LCSeg {
    int n;
    vector<Line> tr;
    vector<int>& coords; //离散化后的数组，sort+erase（unique）

    LCSeg(int n, vector<int>& c) : n(n), tr(n * 4), coords(c) {}

    void add(int p, int l, int r, Line val) {
        if (!tr[p].exists) {
            tr[p] = val; tr[p].exists = true;
            return;
        }

        int mid = l + r >> 1;

        int mid_x = coords[mid - 1];
        int l_x = coords[l - 1];
        int r_x = coords[r - 1];

        if (tr[p].cal(mid_x) < val.cal(mid_x)) swap(tr[p], val);
        if (l == r) return;

        if (val.cal(l_x) > tr[p].cal(l_x)) add(p << 1, l, mid, val);
        else if (val.cal(r_x) > tr[p].cal(r_x)) add(p << 1 | 1, mid + 1, r, val);
    }

    void addLine(int k, int b) {
        Line l(k, b);
        l.exists = true;
        add(1, 1, n, l);
    }

    int ask(int p, int l, int r, int idx) {
        // 同样：空节点不能提前 return，子孙节点里可能存着线段（若以后加了 addSeg 就会错）
        int cur_x = coords[idx - 1];
        int res = tr[p].exists ? tr[p].cal(cur_x) : -INF;
        if (l == r) return res;

        int mid = l + r >> 1;
        if (idx <= mid) return max(res, ask(p << 1, l, mid, idx));
        return max(res, ask(p << 1 | 1, mid + 1, r, idx));
    }
};

// auto get_id = [&](int x) {
//     return lower_bound(coords.begin(), coords.end(), x) - coords.begin() + 1;
// };

// for (int i = 1; i <= n; i++) {
//     int x_idx = get_id(pre[i]);  ！！！要引用离散化后的角标（1-based）
//     dp[i] = seg.ask(1, 1, m, x_idx) + a * pre[i] * pre[i] + b * pre[i];
//     int new_k = -2 * a * pre[i];
//     int new_b = dp[i] + a * pre[i] * pre[i] - b * pre[i] + c;
//     seg.addLine(new_k, new_b);
// }

} // namespace LC_disc

// ==================== 版本3: __int128 分数精确版 ====================
namespace LC_frac {

// ==================== 分数版李超线段树 ====================
// k、b 都可以是分数 p/q。查询点为整数，所有比较用 __int128 精确完成，没有浮点误差。
// 下面实现的是最小值版本；若需要最大值，把 add 和 ask 中标注“方向”处的比较符号反过来即可。
struct Frac {
    __int128 p, q; // 表示 p/q，始终 q > 0 且已约分

    Frac(__int128 p = 0, __int128 q = 1) : p(p), q(q) { norm(); }

    static __int128 gcd128(__int128 a, __int128 b) {
        if (a < 0) a = -a;
        if (b < 0) b = -b;
        while (b) {
            __int128 t = a % b;
            a = b;
            b = t;
        }
        return a;
    }

    void norm() {
        if (q == 0) { // 实际使用中不会出现分母为 0
            exit(1);
        }
        if (q < 0) {
            p = -p;
            q = -q;
        }
        if (p == 0) {
            q = 1;
            return;
        }
        __int128 g = gcd128(p, q);
        p /= g;
        q /= g;
    }

    Frac operator*(long long x) const { return Frac(p * x, q); }
    Frac operator+(const Frac& o) const { return Frac(p * o.q + o.p * q, q * o.q); }
    Frac operator-() const { return Frac(-p, q); }

    bool operator<(const Frac& o) const { return p * o.q < o.p * q; }
    bool operator>(const Frac& o) const { return p * o.q > o.p * q; }
    bool operator==(const Frac& o) const { return p == o.p && q == o.q; }
    bool operator<=(const Frac& o) const { return !(*this > o); }
    bool operator>=(const Frac& o) const { return !(*this < o); }

    long long toLL() const { return (long long)(p / q); }
    double toDouble() const { return (double)p / (double)q; }
};

struct LineFrac {
    Frac k, b;
    bool exists;
    // 空节点初始化为一个足够大的值，用于最小值版本
    // [方向] 最小值用 +INF；若改最大值，这里改为足够小的值，如 -(1LL << 62)
    LineFrac(Frac k = 0, Frac b = Frac(1LL << 62, 1)) : k(k), b(b), exists(false) {}

    Frac cal(int x) const {
        return k * x + b;
    }
};

struct LCSegFrac {
    int n;
    vector<LineFrac> tr;

    LCSegFrac(int n) : n(n), tr(n * 4) {}

    void add(int p, int l, int r, LineFrac val) {
        if (!tr[p].exists) {
            tr[p] = val;
            tr[p].exists = true;
            return;
        }
        int mid = l + r >> 1;
        // 最小值版本：中点更小/更优的留在当前节点
        // [方向] 最小值：当前中点值 > 新线中点值时交换（让更小的留在当前）
        if (tr[p].cal(mid) > val.cal(mid)) swap(tr[p], val);
        if (l == r) return;

        // [方向] 最小值：新线在左端点更小则往左走（改最大值时改为 >）
        if (val.cal(l) < tr[p].cal(l)) {
            add(p << 1, l, mid, val);
        // [方向] 最小值：新线在右端点更小则往右走（改最大值时改为 >）
        } else if (val.cal(r) < tr[p].cal(r)) {
            add(p << 1 | 1, mid + 1, r, val);
        }
    }

    // 插入全局直线 y = k*x + b，k 和 b 都可以是分数
    void addLine(const Frac& k, const Frac& b) {
        LineFrac l(k, b);
        l.exists = true;
        add(1, 1, n, l);
    }

    // 直接传 p/q 形式：y = (p/q)*x + (u/v)
    void addLine(long long p, long long q, long long u, long long v) {
        addLine(Frac(p, q), Frac(u, v));
    }

    // 插入一条在 [L,R] 范围内的线段
    void addSeg(int p, int l, int r, int ql, int qr, LineFrac val) {
        if (ql <= l && r <= qr) {
            add(p, l, r, val);
            return;
        }
        int mid = l + r >> 1;
        if (ql <= mid) addSeg(p << 1, l, mid, ql, qr, val);
        if (mid < qr) addSeg(p << 1 | 1, mid + 1, r, ql, qr, val);
    }

    void addSeg(int ql, int qr, const Frac& k, const Frac& b) {
        LineFrac l(k, b);
        l.exists = true;
        addSeg(1, 1, n, ql, qr, l);
    }

    Frac ask(int p, int l, int r, int x) {
        // [方向] 最小值：空节点返回 +INF（改最大值时改为 -INF）
        if (x < l || x > r) return Frac(1LL << 62, 1);
        Frac res = tr[p].exists ? tr[p].cal(x) : Frac(1LL << 62, 1);
        if (l == r) return res;
        int mid = l + r >> 1;
        // [方向] 最小值：合并时取 min（改最大值时改为 max）
        if (x <= mid) return min(res, ask(p << 1, l, mid, x));
        return min(res, ask(p << 1 | 1, mid + 1, r, x));
    }

    Frac ask(int x) {
        return ask(1, 1, n, x);
    }
};

} // namespace LC_frac
