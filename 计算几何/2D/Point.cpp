#include <bits/stdc++.h>
using namespace std;
//注意所有有关vector数组都不可开过大如 n+5，必须是有多少点n就是多少
//注意所有有关vector数组都不可开过大如 n+5，必须是有多少点n就是多少
//注意所有有关vector数组都不可开过大如 n+5，必须是有多少点n就是多少
//注意所有有关vector数组都不可开过大如 n+5，必须是有多少点n就是多少
//注意所有有关vector数组都不可开过大如 n+5，必须是有多少点n就是多少
//注意所有有关vector数组都不可开过大如 n+5，必须是有多少点n就是多少

const double PI = acos(-1), eps = 1e-9;

int sgn(double x) {
    if (fabs(x) < eps) return 0;
    return x < 0 ? -1 : 1;
}

struct Pt{
    double x, y;
    Pt(double x = 0.0, double y = 0.0) : x(x), y(y) {}
    
    Pt operator- (const Pt& b) const {return Pt(x - b.x, y - b.y);}

    Pt operator+ (const Pt& b) const {return Pt(x + b.x, y + b.y);}

    double operator% (const Pt& b) const {return x * b.y - y * b.x;}

    Pt operator* (const double b) const {return Pt(x * b, y * b);}

    double operator* (const Pt& b) const {return x * b.x + y * b.y;}

    Pt operator/ (const double b) const {return Pt(x / b, y / b);}

    bool operator< (const Pt& b) const {return x < b.x || (x == b.x && y < b.y);}

    bool operator== (const Pt& b) const {return sgn(x - b.x) == 0 && sgn(y - b.y) == 0;}

    bool operator!= (const Pt& b) const {return sgn(x - b.x) != 0 || sgn(y - b.y) != 0;}
};

double dis(Pt a){
    return sqrt(a * a);
}

double ddis(Pt a){
    return a * a;
}

// 两向量夹角, 返回 [0, π], 不带方向
// !!! a 或 b 是零向量时会**静默返回 0**(而不是 nan), 需要自己先判
// 两向量夹角, 返回 [0, π], 不带方向
// !!! a 或 b 是零向量时会**静默返回 0**(而不是 nan), 需要自己先判
// 用 atan2 而不是 acos: acos 在接近 ±1 处丢精度,
// 老写法 acos(a*b/|a|/|b|) 对 Angle((1,0),(1,1e-8)) 会直接返回 0
double Angle(Pt a, Pt b){
    return atan2(fabs(a % b), a * b);
}

//在直线的哪边
// 点在直线上, 返回 0 (三点共线)
// 点在直线的逆时针方向, 返回 1
// 点在直线的顺时针方向, 返回 -1
//直线ab, 点c
// !!! 坐标绝对值必须 <= 9.4e7 (= sqrt(2^53))
//     再大叉积就超出 double 的 53 位有效位, 会把不共线的三点判成共线:
//     K = 1e8 时 A(0,0) B(K,K-1) C(K-1,K-2) 的精确叉积是 -1, 这里算出来是 0,
//     Andrew 凸包会因此丢掉真实顶点, 皮克定理跟着一起错
//     整数坐标且范围大时, 请单独用 long long / __int128 算叉积
int Cross(Pt a, Pt b, Pt c){
    return sgn((b - a) % (c - a));
}


double cross(Pt a, Pt b, Pt c){
    return (b - a) % (c - a);
}

struct Line {
    Pt s, e;
    double ang;
    Line() {}
    Line(Pt x, Pt y) : s(x), e(y) {
        ang = atan2(e.y - s.y, e.x - s.x);
    }

    // 检查点 P 是否在向量 se 的右侧（非法区域）
    // 使用叉积判断：(e-s) ^ (P-s) < 0 则在右侧
    bool onRight(const Pt& P) const {
        return sgn((e - s) % (P - s)) < 0;
    }

    // 极角排序：极角相同时, 把"更靠左"(即约束更强)的那条排在前面
    // 去重只保留每组极角里的第一条, 所以这里必须是 < 0
    // 写成 > 0 会保留约束最弱的那条, 半平面交的结果整体偏大(随机对拍 1963 组错 1910 组)
    bool operator< (const Line& b) const {
        if (sgn(ang - b.ang) != 0) return ang < b.ang;
        return sgn((e - s) % (b.e - s)) < 0;
    }
};


int Cross(Pt c, Line l){
    Pt a = l.s, b = l.e;
    return sgn((b - a) % (c - a));
}

//三点共线
bool In_one_line(Pt a, Pt b, Pt c){
    return !sgn((b - a) % (c - a));
}
//点到直线距离
// a == b 时退化: 不加保护会返回 nan, 而 sgn(nan) == 1, 会让 LWC 把"圆心处的退化直线"判成相离
double dist_ptl(Pt p, Pt a, Pt b){
    if (a == b) return dis(p - a);
    Pt v1 = p - a, v2 = b - a;
    return fabs((v1 % v2) / dis(v2));
}
//点到线段距离
double dist_pts(Pt p, Pt a, Pt b){
    if(a == b) return dis(p - a);
    Pt v1 = b - a, v2 = p - a, v3 = p - b;
    if(sgn(v1 * v2) < 0) return dis(p - a);
    if(sgn(v1 * v3) > 0) return dis(p - b);
    return fabs((v1 % v2) / dis(v1));
}
//点在线段上
// 注意 sgn 用的是绝对 eps, 叉积量级是坐标的平方: 坐标量级远小于 1 时会误判,
// 例如 OnSegment((5e-5, 5e-6), (0,0), (1e-4,0)) 返回 true, 但那个点偏离了线段长度的 5%
bool OnSegment(Pt p, Pt a, Pt b){
    Pt pa = a - p, pb = b - p;
    return sgn(pa % pb) == 0 && sgn(pa * pb) <= 0;
}
//直线与线段相交: 直线 ab, 线段 cd
//(判断线段 cd 的两个端点是否分居直线 ab 两侧, 参数顺序别写反)
bool Intersect_line_seg(Pt a, Pt b, Pt c, Pt d){
    return Cross(a, b, c) * Cross(a, b, d) <= 0;
}
//线段与线段是否相交
bool Intersect_seg(Pt a, Pt b, Pt c, Pt d){
    if (OnSegment(a, c, d) || OnSegment(b, c, d) || OnSegment(c, a, b) || OnSegment(d, a, b)) return 1;
    if (Cross(a, b, c) * Cross(a, b, d) >= 0) return 0;
    if (Cross(c, d, a) * Cross(c, d, b) >= 0) return 0;
    return 1;
}
//线段到线段距离
double dist_sts(Pt a, Pt b, Pt c, Pt d){
    if(Intersect_seg(a, b, c, d)){
        return 0.0;
    }
    return min({
        dist_pts(a, c, d),
        dist_pts(b, c, d),
        dist_pts(c, a, b),
        dist_pts(d, a, b),
    });
}
//直线平行(两直线重合时也返回 true)
// 叉积必须除以两个方向向量的长度再和 eps 比, 否则短线段会被误判:
// 长 1e-5 的水平线和长 1e-5 的竖直线互相垂直, 但叉积只有 1e-10 < eps, 不归一化就判成平行
bool Line_parallel(Line A, Line B){
    Pt u = A.s - A.e, v = B.s - B.e;
    double du = dis(u), dv = dis(v);
    if (sgn(du) == 0 || sgn(dv) == 0) return true;  // 退化的"直线", 当作平行
    return sgn((u % v) / (du * dv)) == 0;
}
//直线交点（四个点）
Pt Inter_Line_Pt(Pt a, Pt b, Pt c, Pt d){
    Pt u = b - a, v = d - c;
    double t = ((a - c) % v) / (v % u);
    return a + u * t;
}

// 直线交点：输入两条 Line 对象（方便半平面交调用）
Pt Inter_Line_Pt(Line a, Line b) {
    Pt u = a.s - b.s;
    Pt v1 = a.e - a.s;
    Pt v2 = b.e - b.s;
    double t = (v2 % u) / (v1 % v2);
    return a.s + v1 * t;
}

//半平面交
//保留每一条直线的左侧区域
//!!!注意，设置边界的时候，limit一定要比eps大一点，不然可能卡精度，例如limit=1e-11,eps=1e-18
//!!!Line 只有 (Pt, Pt) 一个构造函数, 下面的例子别再传第三个参数
//!!!必须自己保证区域有界(先塞一圈大边界框), 否则返回的顶点里会混进 nan/inf 且不报错
//!!!下面这四行是**第二象限专用**的框(只框住 x<0 且 y>0), 是从某道旧题里留下来的,
//   当成通用边界框直接用会把可行域裁成空集。通用的框应该是:
// L.push_back(Line(Pt(-INF,-INF), Pt(INF,-INF)));
// L.push_back(Line(Pt(INF,-INF), Pt(INF, INF)));
// L.push_back(Line(Pt(INF, INF), Pt(-INF, INF)));
// L.push_back(Line(Pt(-INF, INF), Pt(-INF,-INF)));
//   第二象限版本(原样保留):
// L.push_back(Line(Pt(-INF, limit), Pt(-limit, limit)));
// L.push_back(Line(Pt(-limit, limit), Pt(-limit, INF)));
// L.push_back(Line(Pt(-limit, INF), Pt(-INF, INF)));
// L.push_back(Line(Pt(-INF, INF), Pt(-INF, limit)));
vector<Pt> getHalfPlaneIntersection(vector<Line>& L) {
    if (L.empty()) return {};   // 不判空的话下面 ql[0] 越界, 直接段错误

    // 1. 排序
    sort(L.begin(), L.end());

    // 2. 去重（对于极角相同的直线，只保留排序后第一条，即最靠左的那条）
    vector<Line> ql;
    for (int i = 0; i < L.size(); i++) {
        if (i == 0 || sgn(L[i].ang - L[i-1].ang) != 0) {
            ql.push_back(L[i]);
        }
    }

    // 3. 双端队列求解
    deque<Line> dq;
    deque<Pt> pts; // pts[i] 是 dq[i] 和 dq[i+1] 的交点

    dq.push_back(ql[0]);
    for (int i = 1; i < ql.size(); i++) {
        // 如果队尾的交点在当前直线的右侧（非法），弹出队尾
        while (!pts.empty() && ql[i].onRight(pts.back())) {
            pts.pop_back();
            dq.pop_back();
        }
        // 如果队首的交点在当前直线的右侧（非法），弹出队首
        // 这一步是为了处理“环绕一圈后覆盖了起始点”的情况
        while (!pts.empty() && ql[i].onRight(pts.front())) {
            pts.pop_front();
            dq.pop_front();
        }
        
        dq.push_back(ql[i]);
        // 计算新加入直线与上一条直线的交点
        if (dq.size() > 1) {
            // 相邻两条直线方向平行(去重后只可能是极角差 pi 的反向平行)时,
            // Inter_Line_Pt 会除以 0 得到 (inf, nan); 而 sgn(nan) == 1 使得 onRight 永远为 false,
            // 这个脏点再也弹不出去, 最后混进结果里让 area() 变成 nan。
            // 出现这种情况说明可行域无界或为空, 直接判无解
            Line& u = dq[dq.size() - 2];
            Line& v = dq.back();
            if (sgn((u.e - u.s) % (v.e - v.s)) == 0) return {};
            pts.push_back(Inter_Line_Pt(u, v));
        }
    }

    // 4. 收尾检查：队首的直线可能会切掉队尾的点
    while (!pts.empty() && dq.front().onRight(pts.back())) {
        pts.pop_back();
        dq.pop_back();
    }

    // 如果队列中直线少于3条，无法构成多边形
    if (dq.size() < 3) return {};

    // 5. 计算首尾交点，封闭多边形
    // 同样要防反向平行: 三条直线 y>=0 / x<=1 / y<=1 会走到这里, dq.size() == 3 躲过上面的判断,
    // 首尾两条极角差 pi, 不拦的话返回的第三个顶点是 (inf, nan)
    {
        Line& u = dq.back();
        Line& v = dq.front();
        if (sgn((u.e - u.s) % (v.e - v.s)) == 0) return {};
    }
    pts.push_back(Inter_Line_Pt(dq.back(), dq.front()));

    // 将 deque 转为 vector 返回
    vector<Pt> res;
    for (auto p : pts) res.push_back(p);
    return res;
}

//  ------ 多边形 -------
// 三角形面积
double Triangle_area(Pt A, Pt B, Pt C){
    return fabs((B - A) % (C - A)) / 2;
}
//凸多边形面积
// 因为叉积求得的三角形面积是有向的, 在外面的面积可以正负抵消掉
// 所以能够求任意多边形面积(凸, !凸)
// !!! 但必须是**简单多边形**(边不自交)。自交多边形会正负相消, 比如蝴蝶结形状返回 0
// p[]下标从 0 开始, 长度为 n
double area(vector<Pt>& p){
    int n = p.size();
    double S = 0;
    for(int i = 1; i <= n - 2; i++){
        S += (p[i] - p[0]) % (p[i + 1] - p[0]);
    }
    return fabs(S / 2);// 无向面积
    //return S / 2;//有向面积
}

//点在多边形内(射线法)
// 适用于任意简单多边形, 顺时针逆时针给出都可以
// 注意: 内部要除以一条边两端点的 y 差, 近水平且 x 跨度极大的边仍会掉精度,
//       不是完全不用管误差(整数坐标下随机 6 万次查询无误)
// 点在多边形边上, 返回 -1
// 点在多边形内, 返回 1
// 点在多边形外, 返回 0

// p[] 的下标从 0 开始, 长度为 n
int InPolygon(Pt P, vector<Pt>& p) {
    int n = p.size();
    bool flag = false;		
    for (int i = 0, j = n - 1; i < n; j = i++) {
        Pt p1 = p[i], p2 = p[j];
        if (OnSegment(P, p1, p2)) return -1;
        if (sgn(P.y - p1.y) > 0 == sgn(P.y - p2.y) > 0) continue;
        if (sgn((P.y - p1.y) * (p1.x - p2.x) / (p1.y - p2.y) + p1.x - P.x) > 0) 
            flag = !flag;
    }
    return flag;
}
//是否为凸多边形 (convex; 函数名 Is_contex 是历史拼写错误, 搜 convex 搜不到它)
// !!! 只检查拐向是否一致, **自交多边形会被判成凸**: 五角星实测返回 true
//     调用前必须自己保证是简单多边形; 全部顶点共线的退化多边形也返回 true
bool Is_contex(vector<Pt>& p){
    int n = p.size();
    if (n < 3) return false;    // n == 1 时下面的 k = n - 2 = -1, 会越界读 p[-1]
    bool s[3] = {0, 0, 0};
    for (int i = 0, j = n - 1, k = n - 2; i < n; k = j, j = i++) {
        int cnt = sgn((p[i] - p[j]) % (p[k] - p[j])) + 1;
        s[cnt] = true;
        if (s[0] && s[2]) return false;
    }
    return true;
}


// ------ 圆 -------
struct Circle{
    Pt o;
    double r;
    Circle(Pt o = Pt(), double r = 0) : o(o), r(r) {}

    double S(){return PI * r * r;}
    double C(){return PI * 2 * r;}
};
//扇形面积(有向, 逆时针为正)
// !!! A、B 必须是**相对圆心**的向量。圆心不在原点时要写 SectorArea(A - c.o, B - c.o, c.r)
//     直接传绝对坐标不会报错, 但结果完全是错的(实测圆心(10,10)、R=5 的 90 度扇形
//     正确值 19.635, 传绝对坐标得到 4.935)
// !!! 只对夹角 <= π 的扇形成立(Angle 只返回 [0, π]), 优角要自己拆成两块加起来
double SectorArea(Pt A, Pt B, double R){
    double angle = Angle(A, B);
    if(sgn(A % B) < 0) angle = -angle;
    return R * R * angle / 2;
}
//点和圆的位置关系
// 点在圆上, 返回 0
// 点在圆外, 返回 -1
// 点在圆内, 返回 1
int PWC(Pt p, Circle c){
    double d = dis(p - c.o);
    if(sgn(d - c.r) == 0) return 0;
    if(sgn(d - c.r) > 0) return -1;
    return 1;
}
//直线和圆的位置关系
// 相切, 返回 0
// 相交, 返回 1
// 相离, 返回 -1
int LWC(Pt A, Pt B, Circle c) {
    double d = dist_ptl(c.o, A, B);
    if (sgn(d - c.r) == 0) return 0;
    if (sgn(d - c.r) > 0) return -1;
    return 1;
}

//直线和圆的交点
// 判据必须和 LWC 一致, 都比"未平方"的距离。
// 直接比 d2 与 r*r 的话, 半径小于约 3.2e-5 时 |d2 - r*r| 恒小于 eps, 所有相交都被判成相切:
// LWC 说相交(返回 1), 这里却只返回 1 个点, if(LWC(..)==1){ 用 v[0], v[1]; } 会越界读 v[1]
vector<Pt> Intersection_line_circle(Pt A, Pt B, Circle c) {
    if (A == B) return {};              // 退化, 两点重合构不成直线
    Pt AB = B - A;
    double len2 = AB * AB;
    Pt pr = A + AB * ((c.o - A) * AB / len2);
    double d2 = ddis(pr - c.o);

    int status = sgn(sqrt(d2) - c.r);
    if (status > 0) return {}; // 相离，返回空 vector

    double base = sqrt(max(0.0, c.r * c.r - d2));
    if (status == 0) return {pr}; // 相切，返回 1 个点

    // 相交，返回 2 个点
    Pt e = AB / sqrt(len2);
    return {pr + e * base, pr - e * base};
}
//圆与圆的位置关系
// 相离, 返回 -1
// 外切, 返回 0
// 内切(A 包含 B), 返回 1
// 内切(B 包含 A), 返回 2
// 内含(A 包含 B), 返回 3
// 内含(B 包含 A), 返回 4
// 相交, 返回 5
// !!! 两圆完全重合(圆心半径都相同)时返回 1(内切), 没有单独的"重合"返回值, 需要自己先判

int Circle_with_circle(Circle A, Circle B) {
    double len1 = dis(A.o - B.o);
    double len2 = A.r + B.r;
    if (sgn(len1 - len2) > 0) return -1;
    if (sgn(len1 - len2) == 0) return 0;
    if (sgn(len1 + len2 - 2 * A.r) == 0) return 1;
    if (sgn(len1 + len2 - 2 * B.r) == 0) return 2;
    if (sgn(len1 + len2 - 2 * A.r) < 0) return 3;
    if (sgn(len1 + len2 - 2 * B.r) < 0) return 4;
    return 5;
}

//圆与圆的交点
// 相交, 返回 2 个点
// 相切, 返回 1 个点(不是两个重复的点, 用 .size() 区分)
// 相离/内含, 返回空 vector

// 函数内部已经判过位置关系, 可以直接调用
vector<Pt> Intersection_circle_circle(Circle A, Circle B) {
    double d = dis(A.o - B.o);
    // 圆心重合: 半径相同是无穷多交点, 半径不同是无交点, 统一返回空
    // 不加这句的话, 两圆完全重合时会走到下面的 (B.o - A.o) / d 除以 0, 返回 (nan, nan)
    if (sgn(d) == 0) return {};
    // 情况 1: 相离或内含 (无交点)
    if (sgn(d - (A.r + B.r)) > 0 || sgn(d - fabs(A.r - B.r)) < 0) {
        return {};
    }

    // 情况 2: 相切 (1 个交点)
    if (sgn(d - (A.r + B.r)) == 0 || sgn(d - fabs(A.r - B.r)) == 0) {
        Pt e = (B.o - A.o) / d;
        return {A.o + e * A.r};
    }

    // 情况 3: 相交 (2 个交点)
    double a = acos(max(-1.0, min(1.0, (A.r * A.r + d * d - B.r * B.r) / (2.0 * A.r * d))));
    double t = atan2(B.o.y - A.o.y, B.o.x - A.o.x);
    return {
        A.o + Pt(A.r * cos(t + a), A.r * sin(t + a)),
        A.o + Pt(A.r * cos(t - a), A.r * sin(t - a))
    };
}
//求圆外一点对圆的两个切点
vector<Pt> TangentPt_Pt_circle(Pt p, Circle c) {
    double d2 = ddis(p - c.o);
    int status = sgn(d2 - c.r * c.r);
    
    if (status < 0) return {}; // 点在圆内，无切点
    if (status == 0) return {p}; // 点在圆上，切点就是自身

    // 点在圆外，2 个切点
    double d = sqrt(d2);
    double l = sqrt(d2 - c.r * c.r);
    double angle = asin(max(-1.0, min(1.0, c.r / d)));
    Pt e = (c.o - p) / d;

    // 旋转向量得到切点方向
    auto rotate = [](Pt v, double ang) {
        return Pt(v.x * cos(ang) - v.y * sin(ang), v.x * sin(ang) + v.y * cos(ang));
    };

    return {
        p + rotate(e, angle) * l,
        p + rotate(e, -angle) * l
    };
}

//求三角形外接圆
// !!! 前置条件: A、B、C 不共线。共线时 D = 0, 返回的圆心是 (-inf, inf)、半径 inf, 没有任何提示
Circle get_circumcircle(Pt A, Pt B, Pt C) {
    double Bx = B.x - A.x, By = B.y - A.y;
    double Cx = C.x - A.x, Cy = C.y - A.y;
    double D = 2 * (Bx * Cy - By * Cx);

    double x = (Cy * (Bx * Bx + By * By) - By * (Cx * Cx + Cy * Cy)) / D + A.x;
    double y = (Bx * (Cx * Cx + Cy * Cy) - Cx * (Bx * Bx + By * By)) / D + A.y;
    Pt P(x, y);
    return Circle(P, dis(A - P));
}
//三角形内切圆
Circle get_incircle(Pt A, Pt B, Pt C) {
    double a = dis(B - C);
    double b = dis(A - C);
    double c = dis(A - B);
    Pt p = (A * a + B * b + C * c) / (a + b + c);
    return Circle(p, dist_ptl(p, A, B));
}
// 要保证传入的点是整点
//线段上的整点个数
//注意按照要求修改返回值
// 端点相同时(退化成一个点) gcd(0, 0) = 0, "包含端点" 版本返回 1, 是对的
// 但改用下面"不包含端点"那一行时, 退化线段会返回 -1, 得自己特判
int IntegerPt_on_seg(Pt A, Pt B) {
    int x = abs(llround(A.x) - llround(B.x));
    int y = abs(llround(A.y) - llround(B.y));
    return __gcd(x, y) + 1;	// 包含端点
    return __gcd(x, y) - 1;	// 不包含端点
}
// 返回多边形边**上**整点的个数(含顶点)
// 点需要是顺时针(逆时针)给出

// p[] 下标从 0 开始, 长度为 n
// 记得 #define int long long: res 会累加到 n * 2e9 量级, 32 位 int 直接溢出
int IntegerPt_on_polygon(vector<Pt>& p) {
    int n = p.size();
    int res = 0;
    for (int i = 0, j = n - 1; i < n; j = i++) {
        int x = abs(llround(p[i].x) - llround(p[j].x));
        int y = abs(llround(p[i].y) - llround(p[j].y));
        res += __gcd(x, y);	// 每条边只算一个端点, 绕一圈刚好不重不漏
    }
    return res;
}

// ------ 皮克定理 (Pick's Theorem) -------
// S = a + b / 2 - 1
//     S : 多边形面积     a : 内部整点数     b : 边界整点数(含顶点)
// 前提: 所有顶点都是整点, 且是简单多边形(边不自交)
//       凸多边形(比如 Andrew 求出来的凸包)天然满足;
//       凸包把边上的共线点删掉也不影响 b, 因为 gcd 是按整条边算的
// 注意: 千万不要拿 double 的 area() 直接套公式, 1e-9 的误差取整后整体就差 1
//       下面统一用 "两倍面积" 走纯整数运算(记得 #define int long long)
// 用法: vector<Pt> hull = Andrew(p); int cnt = IntegerPt_polygon(hull);

// 多边形的两倍面积(顶点是整点时一定是整数), 顺/逆时针给出都可以
// 坐标绝对值超过 1e9 时把 int 换成 __int128 防溢出
int area2(vector<Pt>& p) {
    int n = p.size();
    int S = 0;
    for (int i = 0, j = n - 1; i < n; j = i++) {
        int x1 = llround(p[j].x), y1 = llround(p[j].y);
        int x2 = llround(p[i].x), y2 = llround(p[i].y);
        S += x1 * y2 - y1 * x2;
    }
    return abs(S);
}

// 返回不包括边界的, 多边形**内**整点个数
// a = S - b / 2 + 1 = (2S - b + 2) / 2, 分子恒为偶数, 整除不掉精度
int IntegerPt_in_polygon(vector<Pt>& p) {
    return (area2(p) - IntegerPt_on_polygon(p) + 2) / 2;
}

// 返回多边形整点总数(内部 + 边界)
// a + b = (2S + b + 2) / 2
// 退化情况(所有点共线, 面积为 0): 本函数返回的仍是那条线段上的整点数,
// 但 IntegerPt_in_polygon 会返回 0 或负数, 需要自行特判
int IntegerPt_polygon(vector<Pt>& p) {
    return (area2(p) + IntegerPt_on_polygon(p) + 2) / 2;
}

int getQuadrant(Pt p, Pt center) {
    double x = p.x - center.x;
    double y = p.y - center.y;
    if (x > 0 && y >= 0) return 1; // 第一象限
    if (x <= 0 && y > 0) return 2; // 第二象限
    if (x < 0 && y <= 0) return 3; // 第三象限
    if (x >= 0 && y < 0) return 4; // 第四象限
    return 0;
}

// !!! center 必须按值传, 不能按引用。polarSort(p, p[0]) 这种写法下 sort 会移动 p[0],
//     按引用捕获的 center 排到一半就变了, 比较函数自相矛盾 -> 结果错乱 + std::sort UB
//     (实测 n = 30 就能和"绕副本排序"给出不同结果)
void polarSort(vector<Pt>& Pts, Pt center) {
    sort(Pts.begin(), Pts.end(), [center](const Pt& a, const Pt& b) {
        int quadA = getQuadrant(a, center);
        int quadB = getQuadrant(b, center);

        if (quadA != quadB) {
            return quadA < quadB;
        }

        // 同一象限内使用叉积
        // 这里必须用精确的 != 0 而不是 fabs(cp) > eps:
        // eps 版本的"等价"不满足传递性(a~b 且 b~c 但 a<c), 是 std::sort 的 UB
        double cp = (a - center) % (b - center);
        if (cp != 0) {
            return cp > 0; // 逆时针排序
        }

        // 共线时，距离近的在前
        return ddis(a - center) < ddis(b - center);
    });
}

//凸包算法
// 返回逆时针序, 边上的共线点会被删掉(只留角点)
// !!! 会就地 sort 传入的 p, 打乱调用方的数组; 要保留原顺序就先自己复制一份
// !!! 坐标绝对值 > 9.4e7 时 Cross 会失真丢顶点, 见 Cross 处的说明
// 退化情况: 所有点共线或重合时返回 2 个点, 0/1 个点原样返回
vector<Pt> Andrew(vector<Pt>& p){
    sort(p.begin(), p.end());
    int n = p.size();
    vector<Pt> ans;
    for(int i = 0; i < n; i++){
        while(ans.size() >= 2 && Cross(ans[ans.size() - 2], ans.back(), p[i]) <= 0){
            ans.pop_back();
        }
        ans.push_back(p[i]);
    }

    int t = ans.size();
    for(int i = n - 2; i >= 0; i--){
        while(ans.size() > t && Cross(ans[ans.size() - 2], ans.back(), p[i]) <= 0){
            ans.pop_back();
        }
        ans.push_back(p[i]);
    }

    if(ans.size() > 1) ans.pop_back();
    return ans;

}

int norm_idx(int x, int n){
    x %= n;
    if(x < 0) x += n;
    return x;
}

// 点在已排序凸包中的位置：
// 返回 -1 表示外部，0 表示边界上，1 表示内部
// 要求 hull 为逆时针排序的凸包
int InConvex(Pt q, const vector<Pt>& hull){
    int n = hull.size();
    if(n == 0) return -1;
    if(n == 1) return q == hull[0] ? 0 : -1;
    if(n == 2) return OnSegment(q, hull[0], hull[1]) ? 0 : -1;

    int s1 = Cross(hull[0], hull[1], q);
    int s2 = Cross(hull[0], hull[n - 1], q);
    if(s1 < 0 || s2 > 0) return -1;
    if(OnSegment(q, hull[0], hull[1]) || OnSegment(q, hull[0], hull[n - 1])) return 0;

    int l = 1, r = n - 1;
    while(l + 1 < r){
        int mid = (l + r) >> 1;
        if(Cross(hull[0], hull[mid], q) >= 0) l = mid;
        else r = mid;
    }

    int side = Cross(hull[l], hull[(l + 1) % n], q);
    if(side < 0) return -1;
    if(OnSegment(q, hull[l], hull[(l + 1) % n])) return 0;
    return side == 0 ? 0 : 1;
}

// 若 q 在凸包外部，返回一个可见边的编号 i，表示边 hull[i] -> hull[i+1] 对 q 可见
// 若 q 在凸包内部或边界上，返回 -1
int FindVisibleEdge(Pt q, const vector<Pt>& hull){
    int n = hull.size();
    if(n < 3) return -1;

    if(Cross(hull[0], hull[1], q) < 0) return 0;
    if(Cross(hull[0], hull[n - 1], q) > 0) return n - 1;

    int l = 1, r = n - 1;
    while(l + 1 < r){
        int mid = (l + r) >> 1;
        if(Cross(hull[0], hull[mid], q) >= 0) l = mid;
        else r = mid;
    }

    return Cross(hull[l], hull[(l + 1) % n], q) < 0 ? l : -1;
}

bool VisibleEdge(Pt q, const vector<Pt>& hull, int idx){
    int n = hull.size();
    return Cross(hull[norm_idx(idx, n)], hull[norm_idx(idx + 1, n)], q) < 0;
}

// 返回外点 q 到已排序凸包的两个切点下标
// 返回值中的 first, second 分别是可见边链的起点和终点，按凸包逆时针顺序给出
// 若 q 不在凸包外部，则返回 {-1, -1}
pair<int, int> TangentIndexConvex(Pt q, const vector<Pt>& hull){
    int n = hull.size();
    if(n == 0) return {-1, -1};
    if(n == 1) return {0, 0};
    if(n == 2) return {0, 1};

    int k = FindVisibleEdge(q, hull);
    if(k == -1) return {-1, -1};

    int lo = 0, hi = n - 1;
    while(lo < hi){
        int mid = (lo + hi + 1) >> 1;
        if(VisibleEdge(q, hull, k + mid)) lo = mid;
        else hi = mid - 1;
    }
    int right_len = lo;

    lo = 0, hi = n - 1;
    while(lo < hi){
        int mid = (lo + hi + 1) >> 1;
        if(VisibleEdge(q, hull, k - mid)) lo = mid;
        else hi = mid - 1;
    }
    int left_len = lo;

    int left_edge = norm_idx(k - left_len, n);
    int right_edge = norm_idx(k + right_len, n);
    return {left_edge, norm_idx(right_edge + 1, n)};
}

pair<Pt, Pt> TangentPointConvex(Pt q, const vector<Pt>& hull){
    auto idx = TangentIndexConvex(q, hull);
    if(idx.first == -1) return {Pt(), Pt()};
    return {hull[idx.first], hull[idx.second]};
}

bool InTangentCone(Pt a, Pt b, const vector<Pt>& hull){
    auto idx = TangentIndexConvex(a, hull);
    if(idx.first == -1) return InConvex(a, hull) >= 0;
    return Cross(a, hull[idx.first], b) <= 0 && Cross(a, hull[idx.second], b) >= 0;
}

// 判断线段 ab 是否与已排序凸包相交
// 要求 hull 为逆时针排序的凸包
bool Intersect_seg_convex(Pt a, Pt b, const vector<Pt>& hull){
    int n = hull.size();
    if(n == 0) return false;
    if(n == 1) return OnSegment(hull[0], a, b);
    if(n == 2) return Intersect_seg(a, b, hull[0], hull[1]);

    if(InConvex(a, hull) >= 0 || InConvex(b, hull) >= 0) return true;
    return InTangentCone(a, b, hull) && InTangentCone(b, a, hull);
}
