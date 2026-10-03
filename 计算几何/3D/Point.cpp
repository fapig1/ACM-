#include <bits/stdc++.h>
using namespace std;

const double PI = acos(-1);

int sgn(double x) {
    if (fabs(x) < 1e-8) return 0;
    return x < 0 ? -1 : 1;
}

struct Point {
    double x, y, z;
    Point(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}

    Point operator+ (const Point& B) const { return Point(x + B.x, y + B.y, z + B.z); }
    Point operator- (const Point& B) const { return Point(x - B.x, y - B.y, z - B.z); }

    // 点乘
    double operator* (const Point& B) const { return x * B.x + y * B.y + z * B.z; }

    // 叉乘
    Point operator% (const Point& B) const {
        return Point(y * B.z - z * B.y, z * B.x - x * B.z, x * B.y - y * B.x);
    }

    // 数乘
    Point operator* (const double& B) const { return Point(x * B, y * B, z * B); }

    // 数除
    Point operator/ (const double& B) const { return Point(x / B, y / B, z / B); }

    bool operator== (const Point& B) const {
        return sgn(x - B.x) == 0 && sgn(y - B.y) == 0 && sgn(z - B.z) == 0;
    }

    bool operator!= (const Point& B) const {
        return sgn(x - B.x) != 0 || sgn(y - B.y) != 0 || sgn(z - B.z) != 0;
    }
};

double dis(Point& a) {
    return sqrt(a * a);
}

double ddis(Point& a) {
    return a * a;
}

// 两点间距离
double dis(Point& a, Point& b) {
    return sqrt((a - b) * (a - b));
}

// 两点间距离平方
double ddis(Point& a, Point& b) {
    return (a - b) * (a - b);
}

// 向量夹角（弧度）
double Angle(Point a, Point b) {
    double cos_val = a * b / dis(a) / dis(b);
    if (cos_val >  1.0) cos_val =  1.0;
    if (cos_val < -1.0) cos_val = -1.0;
    return acos(cos_val);
}

// -------------------- 三维凸包模板 --------------------

struct Face {
    int a, b, c;
    Point n; // 法向量，始终指向凸包外部
    Face(int a = 0, int b = 0, int c = 0) : a(a), b(b), c(c) {}
};

struct Triangle {
    Point a, b, c;
    Triangle() {}
    Triangle(const Point& a, const Point& b, const Point& c) : a(a), b(b), c(c) {}
};

// 调整面的法向量，使其指向凸包外部。inner 是凸包内部一点。
void fix_face(Face& f, const vector<Point>& p, int inner) {
    Point n = (p[f.b] - p[f.a]) % (p[f.c] - p[f.a]);
    // 如果 inner 在法向量正侧，说明法向量指向内部，需要翻转
    if (n * (p[inner] - p[f.a]) > 0) {
        swap(f.b, f.c);
        n = n * -1.0;
    }
    f.n = n;
}

// 输入点集，返回三维凸包的三角形集合。
// 如果点数小于 4 或所有点共面/共线，返回空。
vector<Triangle> convex_hull_3d(vector<Point> p) {
    // 去重
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

    // 随机打乱，避免最坏情况
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(p.begin(), p.end(), rng);

    // 找初始四面体：4 个不共面的点
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

    // 初始四面体的 4 个面，法向量全部朝外
    vector<Face> faces;
    faces.emplace_back(0, 1, 2); fix_face(faces.back(), p, 3);
    faces.emplace_back(0, 1, 3); fix_face(faces.back(), p, 2);
    faces.emplace_back(0, 2, 3); fix_face(faces.back(), p, 1);
    faces.emplace_back(1, 2, 3); fix_face(faces.back(), p, 0);

    // 凸包内部一点，用于判断新面的法向量方向
    Point center = (p[0] + p[1] + p[2] + p[3]) / 4.0;

    // 增量加入剩余点
    for (int i = 4; i < n; i++) {
        vector<int> visible(faces.size(), 0);
        bool any = false;

        // 找出当前点能“看见”的面
        for (int j = 0; j < (int)faces.size(); j++) {
            if (faces[j].n * (p[i] - p[faces[j].a]) > 1e-8) {
                visible[j] = 1;
                any = true;
            }
        }

        if (!any) continue; // 点在当前凸包内部

        // 收集所有可见面的有向边
        map<pair<int, int>, int> edge_cnt;
        for (int j = 0; j < (int)faces.size(); j++) {
            if (!visible[j]) continue;
            int a = faces[j].a, b = faces[j].b, c = faces[j].c;
            edge_cnt[{a, b}]++;
            edge_cnt[{b, c}]++;
            edge_cnt[{c, a}]++;
        }

        // 边界边：反向边不在可见面边集合中
        vector<pair<int, int>> border;
        for (auto& kv : edge_cnt) {
            int u = kv.first.first;
            int v = kv.first.second;
            if (edge_cnt.find({v, u}) == edge_cnt.end()) {
                border.push_back({u, v});
            }
        }

        // 删除可见面
        vector<Face> new_faces;
        for (int j = 0; j < (int)faces.size(); j++) {
            if (!visible[j]) new_faces.push_back(faces[j]);
        }

        // 用边界边和新点构造新面
        for (auto& e : border) {
            int u = e.first, v = e.second;
            Face f(u, v, i);
            f.n = (p[f.b] - p[f.a]) % (p[f.c] - p[f.a]);

            if (sgn(dis(f.n)) == 0) continue; // 退化面

            // 确保法向量朝外
            if (f.n * (center - p[f.a]) > 0) {
                swap(f.b, f.c);
                f.n = f.n * -1.0;
            }
            new_faces.push_back(f);
        }

        faces.swap(new_faces);
    }

    // 转成三角形集合
    vector<Triangle> ans;
    for (auto& f : faces) {
        if (sgn(dis(f.n)) == 0) continue; // 过滤退化面
        ans.emplace_back(p[f.a], p[f.b], p[f.c]);
    }
    return ans;
}

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