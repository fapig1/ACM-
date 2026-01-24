#include <bits/stdc++.h>
using namespace std;

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

double Angle(Pt a, Pt b){
    double val = a * b / dis(a) / dis(b);
    return acos(max(-1.0, min(1.0, val))); 
}

//在直线的哪边
// 点在直线上, 返回 0 (三点共线)
// 点在直线的逆时针方向, 返回 1
// 点在直线的顺时针方向, 返回 -1
//直线ab, 点c
int Cross(Pt a, Pt b, Pt c){
    return sgn((b - a) % (c - a));
}


double cross(Pt a, Pt b, Pt c){
    return (b - a) % (c - a);
}

struct Line{
    Pt s, e;
    Line(){}
    Line(Pt x, Pt y): s(x), e(y) {}
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
double dist_ptl(Pt p, Pt a, Pt b){
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
bool OnSegment(Pt p, Pt a, Pt b){
    Pt pa = a - p, pb = b - p;
    return sgn(pa % pb) == 0 && sgn(pa * pb) <= 0;
}
//直线与线段相交(线段ab,直线cd)
bool Intersect_line_seg(Pt a, Pt b, Pt c, Pt d){
    return Cross(a, b, c) * Cross(a, b, d) <= 0;
}
//线段与线段相交
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
//直线平行
bool Line_parallel(Line A, Line B){
    return sgn((A.s - A.e) % (B.s - B.e)) == 0;
}
//直线交点
Pt Inter_Line_Pt(Pt a, Pt b, Pt c, Pt d){
    Pt u = b - a, v = d - c;
    double t = ((a - c) % v) / (v % u);
    return a + u * t;
}

//  ------ 多边形 -------
// 三角形面积
double Triangle_area(Pt A, Pt B, Pt C){
    return fabs((B - A) % (C - A)) / 2;
}
//凸多边形面积
// 因为叉积求得的三角形面积是有向的, 在外面的面积可以正负抵消掉
// 所以能够求任意多边形面积(凸, !凸)
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

//点在多边形内(扫描线)
// 适用于任意多边形, 不用考虑精度误差和多边形的给出顺序
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
//是否为凸多边形
bool Is_contex(vector<Pt>& p){
    int n = p.size();
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
//扇形面积
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
vector<Pt> Intersection_line_circle(Pt A, Pt B, Circle c) {
    Pt AB = B - A;
    double len2 = AB * AB;
    Pt pr = A + AB * ((c.o - A) * AB / len2);
    double d2 = ddis(pr - c.o);
    
    int status = sgn(d2 - c.r * c.r);
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
// 相交, 返回两点坐标
// 相切, 返回两个一样的相切点

// 要先判断是否相交或相切再调用
vector<Pt> Intersection_circle_circle(Circle A, Circle B) {
    double d = dis(A.o - B.o);
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
int IntegerPt_on_seg(Pt A, Pt B) {
    int x = abs(A.x - B.x);
    int y = abs(A.y - B.y);
    if (x == 0 || y == 0) return 1;
    return __gcd(x, y) + 1;	// 包含端点
    return __gcd(x, y) - 1;	// 不包含端点
}
// 返回多边形边**上**整点的个数
// 点需要是顺时针(逆时针)给出

// p[] 下标从 0 开始, 长度为 n
int IntegerPt_on_polygon(vector<Pt>& p) {
    int n = p.size();
    int res = 0;
    for (int i = 0, j = n - 1; i < n; j = i++) {
        int x = abs(p[i].x - p[j].x);
        int y = abs(p[i].y - p[j].y);
        res += __gcd(x, y);
    }
    return res;
}
// 返回不包括边界的, 多边形**内**整点个数
int IntegerPt_in_polygon(vector<Pt>& p) {
    int n = p.size();
    double A = area(p);
    double B = IntegerPt_on_polygon(p);
    return A - B / 2 + 1;
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

void polarSort(vector<Pt>& Pts, const Pt& center) {
    sort(Pts.begin(), Pts.end(), [&center](const Pt& a, const Pt& b) {
        int quadA = getQuadrant(a, center);
        int quadB = getQuadrant(b, center);
        
        if (quadA != quadB) {
            return quadA < quadB;
        }
        
        // 同一象限内使用叉积
        double cp = (a - center) % (b - center);
        if (fabs(cp) > eps) {
            return cp > 0; // 逆时针排序
        }
        
        // 共线时，距离近的在前
        return ddis(a - center) < ddis(b - center);
    });
}

//凸包算法
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
