# David_数据结构



[TOC]

### 区间加乘线段树

```cpp
//线段树，支持区间加与区间乘
struct Seg{
    int n, mod;
    vector<int> tr, lazyAdd, lazyMul;

    Seg(int n, int mod) : n(n), mod(mod),
    tr(n * 4 + 5),
    lazyAdd(n * 4 + 5),
    lazyMul(n * 4 + 5, 1) {}

    void build(int p, int l, int r, const vector<int>& a){
        if(l == r){
            tr[p] = a[l] % mod;
            return;
        }
        int mid = l + r >> 1;
        build(p << 1, l, mid, a);
        build(p << 1 | 1, mid + 1, r, a);
        pushup(p);
    }

    void clear(){
        fill(tr.begin(), tr.end(), 0);
        fill(lazyAdd.begin(), lazyAdd.end(), 0);
        fill(lazyMul.begin(), lazyMul.end(), 1);
    }

    void pushup(int p){
        tr[p] = (tr[p << 1] + tr[p << 1 | 1]) % mod;
    }

    void apply(int p, int l, int r, int mul, int add){
        tr[p] = (1LL * tr[p] * mul % mod + 1LL * (r - l + 1) * add % mod) % mod;
        lazyMul[p] = 1LL * lazyMul[p] * mul % mod;
        lazyAdd[p] = (1LL * lazyAdd[p] * mul % mod + add) % mod;
    }

    void pushdown(int p, int l, int r){
        if(lazyMul[p] != 1 || lazyAdd[p] != 0){
            int mid = l + r >> 1;
            apply(p << 1, l, mid, lazyMul[p], lazyAdd[p]);
            apply(p << 1 | 1, mid + 1, r, lazyMul[p], lazyAdd[p]);
            lazyMul[p] = 1;
            lazyAdd[p] = 0;
        }
    }

    void updateMul(int p, int l, int r, int ql, int qr, int val){
        if(ql <= l && r <= qr){
            apply(p, l, r, val, 0);
            return;
        }
        pushdown(p, l, r);
        int mid = l + r >> 1;
        if(ql <= mid) updateMul(p << 1, l, mid, ql, qr, val);
        if(mid < qr) updateMul(p << 1 | 1, mid + 1, r, ql, qr, val);
        pushup(p);
    }

    void updateAdd(int p, int l, int r, int ql, int qr, int val){
        if(ql <= l && r <= qr){
            apply(p, l, r, 1, val);
            return;
        }
        pushdown(p, l, r);
        int mid = l + r >> 1;
        if(ql <= mid) updateAdd(p << 1, l, mid, ql, qr, val);
        if(mid < qr) updateAdd(p << 1 | 1, mid + 1, r, ql, qr, val);
        pushup(p);
    }

    int ask(int p, int l, int r, int ql, int qr){
        if(ql <= l && r <= qr){
            return tr[p];
        }
        pushdown(p, l, r);
        int mid = l + r >> 1;
        int res = 0;
        if(ql <= mid) res = (res + ask(p << 1, l, mid, ql, qr)) % mod;
        if(mid < qr) res = (res + ask(p << 1 | 1, mid + 1, r, ql, qr)) % mod;
        return res;
    }
};

//非递归线段树，适用于单点修改区间查询的问题，常数更小
struct Tree {
    int sz;                // 大于值域上限的 2 的幂
    vector<int> tree;

    // n: 值域右端点（1‑based），实际会用到的最大下标
    Tree(int n) {
        sz = 1;
        while (sz < n) sz <<= 1;
        tree.assign(sz * 2, 0);      // 1‑based 索引，叶子从 sz 开始
    }

    // 清空线段树（全部置零），供下一轮重用
    void clear() {
        fill(tree.begin(), tree.end(), 0);
    }

    // 单点更新：忽略前三个无用参数，只保留 tar, val
    void update(int /*ind*/, int /*l*/, int /*r*/, int tar, int val) {
        int p = tar + sz - 1;         // tar 是 1‑based
        tree[p] = val;
        p >>= 1;
        while (p) {
            tree[p] = max(tree[p << 1], tree[p << 1 | 1]);
            p >>= 1;
        }
    }

    // 区间最大值查询：忽略前三个无用参数，ql,qr 为 1‑based
    int query(int /*ind*/, int /*l*/, int /*r*/, int ql, int qr) {
        if (ql > qr) return 0;
        int l = ql + sz - 1;
        int r = qr + sz - 1;
        int ans = 0;
        while (l <= r) {
            if (l & 1) ans = max(ans, tree[l++]);
            if (!(r & 1)) ans = max(ans, tree[r--]);
            l >>= 1;
            r >>= 1;
        }
        return ans;
    }
};
```

### 二维线段树

```cpp
// 二维线段树
struct Seg2D {
    int n, m;
    vector<vector<int>> tr, lazy;
    
    Seg2D(int n, int m) : n(n), m(m) {
        tr.assign(n * 4 + 5, vector<int>(m * 4 + 5, 0));
        lazy.assign(n * 4 + 5, vector<int>(m * 4 + 5, 0));
    }
    
    void build(int p, int l, int r, const vector<vector<int>>& a) {
        build_y(p, 1, 1, m, l, r, a);
    }
    
    // 构建Y轴
    void build_y(int px, int py, int ly, int ry, int lx, int rx, const vector<vector<int>>& a) {
        if(ly == ry) {
            if(lx == rx) {
                tr[px][py] = a[lx][ly];
            } else {
                tr[px][py] = tr[px << 1][py] + tr[px << 1 | 1][py];
            }
            return;
        }
        int mid = ly + ry >> 1;
        build_y(px, py << 1, ly, mid, lx, rx, a);
        build_y(px, py << 1 | 1, mid + 1, ry, lx, rx, a);
        pushup_y(px, py);
    }
    
    void build(int px, int lx, int rx, const vector<vector<int>>& a) {
        if(lx == rx) {
            build_y(px, 1, 1, m, lx, rx, a);
            return;
        }
        int mid = lx + rx >> 1;
        build(px << 1, lx, mid, a);
        build(px << 1 | 1, mid + 1, rx, a);
        build_y(px, 1, 1, m, lx, rx, a);
    }
    
    void clear() {
        for(auto& row : tr) fill(row.begin(), row.end(), 0);
        for(auto& row : lazy) fill(row.begin(), row.end(), 0);
    }
    
    void pushup_y(int px, int py) {
        tr[px][py] = tr[px][py << 1] + tr[px][py << 1 | 1];
    }
    
    void pushdown_y(int px, int py, int ly, int ry) {
        if(lazy[px][py]) {
            int mid = ly + ry >> 1;
            tr[px][py << 1] += (mid - ly + 1) * lazy[px][py];
            tr[px][py << 1 | 1] += (ry - mid) * lazy[px][py];
            lazy[px][py << 1] += lazy[px][py];
            lazy[px][py << 1 | 1] += lazy[px][py];
            lazy[px][py] = 0;
        }
    }
    
    // 内层Y轴更新
    void update_y(int px, int py, int ly, int ry, int qly, int qry, int val) {
        if(qly <= ly && ry <= qry) {
            tr[px][py] += (ry - ly + 1) * val;
            lazy[px][py] += val;
            return;
        }
        pushdown_y(px, py, ly, ry);
        int mid = ly + ry >> 1;
        if(qly <= mid) update_y(px, py << 1, ly, mid, qly, qry, val);
        if(mid < qry) update_y(px, py << 1 | 1, mid + 1, ry, qly, qry, val);
        pushup_y(px, py);
    }
    
    // 外层X轴更新
    void update(int p, int l, int r, int ql, int qr, int qly, int qry, int val) {
        if(ql <= l && r <= qr) {
            update_y(p, 1, 1, m, qly, qry, val);
            return;
        }
        int mid = l + r >> 1;
        if(ql <= mid) update(p << 1, l, mid, ql, qr, qly, qry, val);
        if(mid < qr) update(p << 1 | 1, mid + 1, r, ql, qr, qly, qry, val);
        // 更新当前节点
        for(int py = 1; py <= m * 4; py++) {
            tr[p][py] = tr[p << 1][py] + tr[p << 1 | 1][py];
        }
    }
    
    // 内层Y轴查询
    int ask_y(int px, int py, int ly, int ry, int qly, int qry) {
        if(qly <= ly && ry <= qry) {
            return tr[px][py];
        }
        pushdown_y(px, py, ly, ry);
        int mid = ly + ry >> 1;
        int res = 0;
        if(qly <= mid) res += ask_y(px, py << 1, ly, mid, qly, qry);
        if(mid < qry) res += ask_y(px, py << 1 | 1, mid + 1, ry, qly, qry);
        return res;
    }
    
    // 外层X轴查询
    int ask(int p, int l, int r, int ql, int qr, int qly, int qry) {
        if(ql <= l && r <= qr) {
            return ask_y(p, 1, 1, m, qly, qry);
        }
        int mid = l + r >> 1;
        int res = 0;
        if(ql <= mid) res += ask(p << 1, l, mid, ql, qr, qly, qry);
        if(mid < qr) res += ask(p << 1 | 1, mid + 1, r, ql, qr, qly, qry);
        return res;
    }
};
```

### 李超线段树

```cpp
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
    // 查询 x 处的最大 y 值
    double ask(int p, int l, int r, int x){
        if(x < l || x > r || !tr[p].exists) return INF;//更改处-INF
        double res = tr[p].cal(x);
        if(l == r) return res;
        int mid = l + r >> 1;
                        //更改处 max
        if(x <= mid) return min(res, ask(p << 1, l, mid, x));
        return min(res, ask(p << 1 | 1, mid + 1, r, x));
    }
};


//离散化李超线段树（对所有可能的x值进行离散化处理）（最大值版本）
const int INF = 2e18; // 维护最大值，初始b设为-INF

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
        if (!tr[p].exists) return -INF;
        int cur_x = coords[idx - 1];
        int res = tr[p].cal(cur_x);
        if (l == r) return res;
        
        int mid = l + r >> 1;
        if (idx <= mid) return max(res, ask(p << 1, l, mid, idx));
        return max(res, ask(p << 1 | 1, mid + 1, r, idx));
    }
};

// ==================== 分数版李超线段树 ====================
// k、b 都可以是分数 p/q。查询点为整数，所有比较用 __int128 精确完成，没有浮点误差。
// 下面实现的是最小值版本；若需要最大值，把 add 和 ask 中标注"方向"处的比较符号反过来即可。
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
```

### 主席树

```cpp
//https://www.luogu.com.cn/problem/P3919
//主席树单点修改单点查询

const int N=1e6+5;

struct node{
    int l,r;
    long long val;
}tree[N*40];
//模版上写20，但是我交了re，写大一点过了，有些玄学

vector<long long> vec(N);
vector<int> root(N);//root表示第i个版本的根节点的index

int top=0;
//在更新操作时进行动态开点的函数
int clone(int ind){
    top++;
    tree[top]=tree[ind];
    return top;
}

//建树
int build(int ind,int l,int r){
    top++;
    ind=top;

    if(l==r){
        tree[ind].val=vec[l];
        return ind;
    }

    int mid=(l+r)/2;
    //实际上更新点可以有很多种方式，我使用的方式是为了统一更新建树和查询函数参数的格式
    //build里面的第一个参数意义不大，为了统一参数那么写的，写一样的就行
    tree[ind].l = build(tree[ind].l,l,mid);
    tree[ind].r = build(tree[ind].r,mid+1,r);
    return ind;
}

//更新
int update(int ind,int l,int r,int t,long long v){
    ind=clone(ind);

    if(l==r){
        tree[ind].val=v;
        return ind;
    }
    int mid=(l+r)/2;
    if(t<=mid){
        tree[ind].l=update(tree[ind].l,l,mid,t,v);
    }else{
        tree[ind].r=update(tree[ind].r,mid+1,r,t,v);
    }
    return ind;
}

//查询
long long query(int ind,int l,int r,int t){
    if(l==r){
        return tree[ind].val; 
    }

    int mid=(l+r)/2;
    if(t<=mid){
        return query(tree[ind].l,l,mid,t);
    }else{
        return query(tree[ind].r,mid+1,r,t);
    }
}
```

### 并查集

```cpp
//并查集
struct SDU{
    int n, cnt;//cnt为连通块数量
    vector<int> fa;
    SDU(int m = 0){
        n = m;
        fa.resize(n + 1);
        for(int i = 1; i <= n; i++) fa[i] = i;//1-based
        cnt = n;
    }

    int find(int x){
        return x == fa[x] ? x : fa[x] = find(fa[x]);
    }

    void merge(int x, int y){
        int xx = find(x);
        int yy = find(y);
        if(xx != yy){
            fa[xx] = yy;
            cnt--;
        }
    }

    bool check(int x, int y){
        return find(x) == find(y);
    }
};
```

### 带权并查集

```cpp
//带权并查集，维护每个连通块根节点到每个节点中某个权值关系
struct DSU{
    int n;
    vector<int> fa, sz, dis;

    DSU(int n):n(n),fa(n+1),sz(n+1,1),dis(n+1,0){
        for(int i=1;i<=n;i++){
            fa[i]=i;
        }
    }

    int find(int x){
        if(fa[x]==x) return x;

        int f=fa[x];
        fa[x]=find(fa[x]);

        dis[x]+=dis[f];

        return fa[x];
    }

    bool merge(int x,int y,int d){
        int fx=find(x);
        int fy=find(y);

        if(fx==fy){
            return dis[y]-dis[x]==d;
        }

        if(sz[fx]>sz[fy]){
            swap(x,y);
            swap(fx,fy);
            d=-d;
        }

        fa[fx]=fy;

        dis[fx]=dis[y]-dis[x]-d;

        sz[fy]+=sz[fx];

        return true;
    }
};
```

### 普通莫队

```cpp
#define int long long
#define double long double
const int maxn = 5e4;
int B;
int cur = 0;

struct Q{
    int l, r, id;

    bool operator<(const Q& b)const{
        int pos1 = l / B, pos2 = b.l / B;
        if(pos1 != pos2) return pos1 < pos2;
        if(pos1 & 1) return r < b.r;
        return r > b.r;
    }
};

vector<int> cnt(maxn + 5);
vector<int> a(maxn + 5);
vector<Q> ask(maxn + 5);
vector<pair<int,int>> ans(maxn + 5);

int l = 1, r = 0;

void add(int pos){
    int val = a[pos];
    cur += cnt[val];
    cnt[val]++; 
}

void del(int pos){
    int val = a[pos];
    cnt[val]--;
    cur -= cnt[val];
}

void solve(){
    int n, m;
    cin >> n >> m;
    B = max(1.0, n / sqrt(m * 2.0 / 3.0));
    for(int i = 1; i <= n; i++){
        cin >> a[i];
    }
    for(int i = 0; i < m; i++){
        cin >> ask[i].l >> ask[i].r;
        ask[i].id = i;
    }
    sort(ask.begin(), ask.begin() + m);
    for(int i = 0; i < m; i++){
        auto[ql, qr, id] = ask[i];
        while(l > ql) add(--l);
        while(r < qr) add(++r);
        while(l < ql) del(l++);
        while(r > qr) del(r--);
        int len = r - l + 1;
        int fm = cur == 0 ? 1 : len * (len - 1) / 2;
        int g = cur == 0 ? 1 : __gcd(cur, fm);
        ans[id] = {cur / g, fm / g};
    }
    for(int i = 0; i < m; i++){
        cout << ans[i].first << '/' << ans[i].second << '\n';
    }
}

```

### 带修莫队

```cpp
#define int long long
#define double long double
int n, m, B;
struct Q{
    int l, r, t, id;

    bool operator<(const Q& b)const{
        if(l / B != b.l / B) return l / B < b.l / B;
        if(r / B != b.r / B) return r / B < b.r / B;
        return t < b.t;
    }

    Q(int l, int r, int t, int id) : l(l), r(r), t(t), id(id) {}
};

struct OP{
    int p, x;

    OP(int p, int x): p(p), x(x) {}
};

vector<int> a;
vector<Q> ask;
vector<OP> op; 
vector<int> cnt(1000006);
int res = 0;

void add(int val){
    if(cnt[val] == 0) res++;
    cnt[val]++; 
}

void del(int val){
    cnt[val]--;
    if(cnt[val] == 0) res--;
}

void solve(){
    cin >> n >> m;
    a.resize(n);
    for(int i = 0; i < n; i++){
        cin >> a[i];
    }

    for(int i = 0; i < m; i++){
        char c;
        int x, y;
        cin >> c >> x >> y;
        if(c == 'Q'){
            ask.push_back(Q(x - 1, y - 1, op.size(), ask.size()));
        } else {
            op.push_back(OP(x - 1, y));
        }
    }
    B = pow(n, 2.0 / 3.0);
    sort(ask.begin(), ask.end());
    vector<int> ans(ask.size());
    int curT = 0, l = 0, r = -1;
    for(int i = 0; i < ask.size(); i++){
        auto[ql, qr, t, id] = ask[i];
        while(l > ql) add(a[--l]);
        while(r < qr) add(a[++r]);
        while(l < ql) del(a[l++]);
        while(r > qr) del(a[r--]);
        while(curT < t){
            int pos = op[curT].p;
            if(l <= pos && pos <= r){
                del(a[pos]), add(op[curT].x);
            }
            swap(a[pos], op[curT].x);
            curT++;
        }
        while(curT > t){
            curT--;
            int pos = op[curT].p;
            if(l <= pos && pos <= r){
                del(a[pos]), add(op[curT].x);
            }     
            swap(a[pos], op[curT].x);
        }
        ans[id] = res;
    }
    for(int i : ans) cout << i << '\n';
}

```

### 回滚莫队

```cpp
#define int long long
#define double long double
int n, q, B;
struct Q{
    int l, r, id;

    bool operator<(const Q& b)const{
        if(l / B != b.l / B) return l / B < b.l / B;
        return r < b.r;
    }
    Q(int l, int r, int id) : l(l), r(r), id(id) {}
};

vector<int> a;
vector<Q> ask;
vector<int> fst, lst;
vector<int> cnt(200005);

void solve(){
    cin >> n;
    a.resize(n);
    fst.resize(n + 5, - 1);
    lst.resize(n + 5, -1);
    for(int i = 0; i < n; i++){
        cin >> a[i];
    }    
    cin >> q;
    B = max(1.0, n / sqrt(q * 2.0 / 3.0));
    auto b = a;
    sort(b.begin(), b.end());
    b.erase(unique(b.begin(), b.end()), b.end());
    for(int i = 0; i < n; i++){
        a[i] = lower_bound(b.begin(), b.end(), a[i]) - b.begin();
    }
    for(int i = 0; i < q; i++){
        int x, y;
        cin >> x >> y;
        ask.push_back(Q(x - 1, y - 1, i));
    }
    sort(ask.begin(), ask.end());
    vector<int> ans(q);
    int lb = -1, r = -1, res = 0;
    for(int i = 0; i < q; i++){
        auto[ql, qr, id] = ask[i];
        int cur = ql / B;

        if(cur != lb){
            fill(fst.begin(), fst.end(), -1);
            fill(lst.begin(), lst.end(), -1);
            res = 0;
            lb = cur;
            r = (cur + 1) * B - 1;
        }
        
        if(cur == qr / B){
            int temp = 0;
            vector<int> wait;
            for(int j = ql; j <= qr; j++){
                int val = a[j];
                if(fst[val] == -1){
                    fst[val] = j;
                    wait.push_back(val);
                }
                lst[val] = j;
                temp = max(temp, j - fst[val]);
            }
            ans[id] = temp;
            for(int val : wait){
                fst[val] = -1;
                lst[val] = -1;
            }
            continue;
        }


        while(r < qr){
            r++;
            int val = a[r];
            if(fst[val] == -1) fst[val] = r;
            lst[val] = r;
            res = max(res, lst[val] - fst[val]);
        }

        int l = min(n, (cur + 1) * B);
        int tmp = res;
        stack<array<int,3>> wait;// val fst lst 
        while(l > ql){
            l--;
            int val = a[l];
            wait.push({val, fst[val], lst[val]});
            if(lst[val] == -1) lst[val] = l;
            fst[val] = l;
            res = max(res, lst[val] - fst[val]);
        }

        ans[id] = res;
        res = tmp;

        while(wait.size()){
            auto[val, l, r] = wait.top();
            wait.pop();
            fst[val] = l;
            lst[val] = r; 
        }

    }

    for(int i : ans) cout << i << '\n';

}

```
