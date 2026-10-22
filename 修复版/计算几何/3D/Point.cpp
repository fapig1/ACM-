#include <bits/stdc++.h>
using namespace std;

// ============================== 3D 计算几何 ==============================
// 内容: 三维点/向量运算（加减、点乘、叉乘、数乘）、距离、夹角，
//       以及增量法三维凸包（返回凸包的三角形集合，每个面法向量朝外）。
// 约定:
//   * 坐标用 double，不用 #define int long long；eps = 1e-8，sgn 为绝对误差。
//   * 叉乘 a % b 遵循右手系: 结果同时垂直于 a、b，方向由右手定则确定。
//   * 三维凸包输出三角形面片（存实际坐标，不合并共面三角形），顶点已按 eps 去重。
//   * 输入点重复、共线、共面等退化情况会返回空 vector。
// 复杂度: 凸包期望 O(n^2)（每次插入扫描当前所有面），空间 O(n)。
// =====================================================================

const double PI = acos(-1);

// 符号函数: |x| < 1e-8 返回 0，否则返回 -1/1；用于叉乘长度、点积的零判定
int sgn(double x) {
    if (fabs(x) < 1e-8) return 0;
    return x < 0 ? -1 : 1;
}

// 【速查】三维点/向量 Point
// 运算: + - 、a * b 点积、a % b 叉积（返回向量）、* k、/ k；dis / ddis 长度、Angle(a, b) 夹角
struct Point {
    double x, y, z;
    Point(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}

    // 向量加减
    Point operator+ (const Point& B) const { return Point(x + B.x, y + B.y, z + B.z); }
    Point operator- (const Point& B) const { return Point(x - B.x, y - B.y, z - B.z); }

    // 点乘（内积）: |a||b|cosθ，结果为标量；= 0 表示垂直
    double operator* (const Point& B) const { return x * B.x + y * B.y + z * B.z; }

    // 叉乘（外积）: 结果垂直于 a、b，方向按右手定则；
    // 模长 = 以 a,b 为邻边的平行四边形面积。三维凸包用它求面法向量。
    Point operator% (const Point& B) const {
        return Point(y * B.z - z * B.y, z * B.x - x * B.z, x * B.y - y * B.x);
    }

    // 数乘 / 数除（除数为 0 会得到 inf/nan，调用方保证非零）
    Point operator* (const double& B) const { return Point(x * B, y * B, z * B); }

    Point operator/ (const double& B) const { return Point(x / B, y / B, z / B); }

    // 相等/不等: 用 sgn 做 eps 比较，避免浮点直接 ==
    bool operator== (const Point& B) const {
        return sgn(x - B.x) == 0 && sgn(y - B.y) == 0 && sgn(z - B.z) == 0;
    }

    bool operator!= (const Point& B) const {
        return sgn(x - B.x) != 0 || sgn(y - B.y) != 0 || sgn(z - B.z) != 0;
    }
};

// 向量长度 |a| 与长度平方 |a|²（比较距离时用平方可避免开方误差）
// 注意: 参数是非 const 引用（历史写法），不能直接传 a-b 这种临时量；
//       需要时写 Point d = a - b; 再 dis(d)。
double dis(Point& a) {
    return sqrt(a * a);
}

double ddis(Point& a) {
    return a * a;
}

// 两点间距离 / 距离平方，同样是非 const 引用参数
double dis(Point& a, Point& b) {
    return sqrt((a - b) * (a - b));
}

double ddis(Point& a, Point& b) {
    return (a - b) * (a - b);
}

// 两向量夹角（弧度），返回 [0, π]
// 用 acos 前把 cos 值夹到 [-1,1]，避免浮点误差导致 nan；
// 零向量会使除法得到 nan，调用前需保证 a、b 非零。
// 若对接近 0/π 的精度敏感，可改用 atan2(|a×b|, a·b)。
double Angle(Point a, Point b) {
    double cos_val = a * b / dis(a) / dis(b);
    if (cos_val >  1.0) cos_val =  1.0;
    if (cos_val < -1.0) cos_val = -1.0;
    return acos(cos_val);
}

// -------------------- 三维凸包模板 --------------------

// 凸包的一个三角形面片: a,b,c 是点表下标，按逆时针（从外部看）排列。
// n 是未归一化的法向量，方向始终指向凸包外部；用 n·(q - p[a]) 的符号判断 q 是否在面外侧。
struct Face {
    int a, b, c;
    Point n; // 法向量，始终指向凸包外部
    Face(int a = 0, int b = 0, int c = 0) : a(a), b(b), c(c) {}
};

// 输出用三角形（存实际坐标，不再带下标）
struct Triangle {
    Point a, b, c;
    Triangle() {}
    Triangle(const Point& a, const Point& b, const Point& c) : a(a), b(b), c(c) {}
};

// 调整面的法向量，使其指向凸包外部。inner 是凸包内部一点。
// 做法: 先算 n = (b-a)×(c-a)；若 n·(inner-a) > 0，说明 n 指向内侧，
//       交换 b,c 并把 n 取反。注意 n 不归一化，长度 = 2 倍三角形面积。
void fix_face(Face& f, const vector<Point>& p, int inner) {
    Point n = (p[f.b] - p[f.a]) % (p[f.c] - p[f.a]);
    // 如果 inner 在法向量正侧，说明法向量指向内部，需要翻转
    if (n * (p[inner] - p[f.a]) > 0) {
        swap(f.b, f.c);
        n = n * -1.0;
    }
    f.n = n;
}

// 【速查】三维凸包（随机增量法）
// 用法: auto faces = convex_hull_3d(p);  返回凸包的三角面片；点数 < 4 或全共面返回空
// 表面积 = Σ |(b-a) % (c-a)| / 2
// 复杂度: O(n²) 最坏（每加一个点扫一遍所有面），n ≤ 2000 左右
// 增量法三维凸包。
// 输入: 点集 p（可含重复点、共线点、共面点）；返回凸包的三角形集合，法向朝外。
// 输出: 不合并共面三角形，因此面数可能多于“最小三角剖分”；顶点坐标已去重。
// 退化: 点数 < 4、所有点共线或共面时返回空 vector。
// 步骤:
//   1) 按 eps 相等去重；
//   2) 随机打乱，降低增量法的最坏情况概率；
//   3) 找 4 个不共面的点作为初始四面体；
//   4) 对剩余每个点，找出它能“看见”的面（点在面外侧），
//      收集可见面的边界有向边（反向边不在可见面集合中），删掉可见面，
//      用边界边和新点连成新面，并用内部点把法向修正朝外；
//   5) 输出所有未退化面。
// 复杂度: 去重 O(n^2)，增量过程期望 O(n^2)（总面积/面数均 O(n)）。
// 注意: 点坐标量级很大（>1e8）时叉积/点积的 double 精度会下降，eps 需相应调整。
vector<Triangle> convex_hull_3d(vector<Point> p) {
    // 去重: 用 Point::operator==（eps 比较），O(n^2)。n 很大时可改用排序去重。
    vector<Point> q;
    for (auto& x : p) {
        bool dup = false;
        for (auto& y : q) {
            if (x == y) {
                dup = true;
                break;
            }
        }
        if (!dup) q.push_back(x);
    }
    p.swap(q);

    int n = (int)p.size();
    if (n < 4) return {};

    // 随机打乱，避免输入顺序导致增量法退化（期望复杂度 O(n^2)）
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(p.begin(), p.end(), rng);

    // 找初始四面体：4 个不共面的点
    // i1: 与 i0 不同的点；i2: 与 i0,i1 不共线的点；i3: 不在 i0,i1,i2 平面上的点。
    int i0 = 0, i1 = -1, i2 = -1, i3 = -1;

    for (int i = 1; i < n; i++) {
        if (!(p[i] == p[i0])) {
            i1 = i;
            break;
        }
    }
    if (i1 == -1) return {};

    for (int i = 1; i < n; i++) {
        if (i == i1) continue;
        Point nrm = (p[i1] - p[i0]) % (p[i] - p[i0]);
        if (sgn(dis(nrm)) != 0) {
            i2 = i;
            break;
        }
    }
    if (i2 == -1) return {};

    for (int i = 1; i < n; i++) {
        if (i == i1 || i == i2) continue;
        Point nrm = (p[i1] - p[i0]) % (p[i2] - p[i0]);
        if (sgn(nrm * (p[i] - p[i0])) != 0) {
            i3 = i;
            break;
        }
    }
    if (i3 == -1) return {};

    // 把选出的 4 个点放到最前面
    vector<Point> pts;
    pts.push_back(p[i0]);
    pts.push_back(p[i1]);
    pts.push_back(p[i2]);
    pts.push_back(p[i3]);
    for (int i = 0; i < n; i++) {
        if (i == i0 || i == i1 || i == i2 || i == i3) continue;
        pts.push_back(p[i]);
    }
    p.swap(pts);
    n = (int)p.size();

    // 初始四面体的 4 个面，法向量全部朝外。
    // 每个面用另外三个点之一作为“内部点”来定方向（四面体内部点必在面内侧）。
    vector<Face> faces;
    faces.emplace_back(0, 1, 2); fix_face(faces.back(), p, 3);
    faces.emplace_back(0, 1, 3); fix_face(faces.back(), p, 2);
    faces.emplace_back(0, 2, 3); fix_face(faces.back(), p, 1);
    faces.emplace_back(1, 2, 3); fix_face(faces.back(), p, 0);

    // 凸包内部一点（初始四面体质心，之后始终在凸包内部），用于判断新面的法向
    Point center = (p[0] + p[1] + p[2] + p[3]) / 4.0;

    // 增量加入剩余点: 每个点只会“撑大”凸包，删除它可见的面，再用边界边补新面
    for (int i = 4; i < n; i++) {
        vector<int> visible(faces.size(), 0);
        bool any = false;

        // 找出当前点能“看见”的面: 点在面外侧 <=> n·(p_i - p_a) > 0。
        // 注意 n 未归一化（长度是 2 倍面积），这里的 1e-8 不是几何距离阈值；
        // 坐标量级变化大时应改成 n·(...) > eps * |n|。
        for (int j = 0; j < (int)faces.size(); j++) {
            if (faces[j].n * (p[i] - p[faces[j].a]) > 1e-8) {
                visible[j] = 1;
                any = true;
            }
        }

        if (!any) continue; // 点在当前凸包内部或表面上，不改变凸包

        // 收集所有可见面的有向边（每个面按逆时针给出三条边）
        map<pair<int, int>, int> edge_cnt;
        for (int j = 0; j < (int)faces.size(); j++) {
            if (!visible[j]) continue;
            int a = faces[j].a, b = faces[j].b, c = faces[j].c;
            edge_cnt[{a, b}]++;
            edge_cnt[{b, c}]++;
            edge_cnt[{c, a}]++;
        }

        // 边界边：反向边不在可见面边集合中。这些边构成可见区域的“地平线”，
        // 接下来用它们和新点 i 组成一圈新三角形。
        vector<pair<int, int>> border;
        for (auto& kv : edge_cnt) {
            int u = kv.first.first;
            int v = kv.first.second;
            if (edge_cnt.find({v, u}) == edge_cnt.end()) {
                border.push_back({u, v});
            }
        }

        // 删除可见面，保留其余面
        vector<Face> new_faces;
        for (int j = 0; j < (int)faces.size(); j++) {
            if (!visible[j]) new_faces.push_back(faces[j]);
        }

        // 用边界边和新点构造新面
        for (auto& e : border) {
            int u = e.first, v = e.second;
            Face f(u, v, i);
            f.n = (p[f.b] - p[f.a]) % (p[f.c] - p[f.a]);

            if (sgn(dis(f.n)) == 0) continue; // 三点共线，退化面直接丢弃

            // 用内部点 center 确保法向量朝外
            if (f.n * (center - p[f.a]) > 0) {
                swap(f.b, f.c);
                f.n = f.n * -1.0;
            }
            new_faces.push_back(f);
        }

        faces.swap(new_faces);
    }

    // 转成三角形集合（p 的前 4 个点是初始四面体，其余按加入顺序排列）
    // 过滤掉法向量长度为 0 的退化面；输出顺序不保证，共面三角形也不合并。
    vector<Triangle> ans;
    for (auto& f : faces) {
        if (sgn(dis(f.n)) == 0) continue; // 过滤退化面
        ans.emplace_back(p[f.a], p[f.b], p[f.c]);
    }
    return ans;
}

// 示例入口（默认不做事，方便直接编译本文件做语法检查）
int main() {
    // 示例：读入点集并求三维凸包
    // vector<Point> pts = { ... };
    // vector<Triangle> hull = convex_hull_3d(pts);
    // for (auto& t : hull) {
    //     cout << t.a.x << " " << t.a.y << " " << t.a.z << " | "
    //          << t.b.x << " " << t.b.y << " " << t.b.z << " | "
    //          << t.c.x << " " << t.c.y << " " << t.c.z << "\n";
    // }
    return 0;
}