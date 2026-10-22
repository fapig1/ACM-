// 依赖: 2D/Point.cpp 里的 Pt、sgn、eps、PI、dis、ddis、Andrew、Circle、get_circumcircle
//      （粘贴时放在 2D 模板之后；单独编译测试时先 #include "Point.cpp"）

// 【速查】旋转卡壳：凸包直径（最远点对，P1452）
// 用法: auto h = Andrew(pts);  double d2 = diameter2(h);   // 返回最远点对距离的平方
// 套路: 同样的双指针还能求凸包宽度、最小矩形覆盖（每条边找对踵点）
// 复杂度: 凸包 O(n log n) + 卡壳 O(n)
double diameter2(const vector<Pt>& h) {
    int n = h.size();
    if (n == 1) return 0;
    if (n == 2) return ddis(h[0] - h[1]);
    double res = 0;
    for (int i = 0, j = 1; i < n; i++) {
        Pt e = h[(i + 1) % n] - h[i];
        while (fabs(e % (h[(j + 1) % n] - h[i])) > fabs(e % (h[j] - h[i]))) j = (j + 1) % n;   // 面积单峰，j 单调前进
        res = max({res, ddis(h[i] - h[j]), ddis(h[(i + 1) % n] - h[j])});
    }
    return res;
}

// 【速查】最小圆覆盖（随机增量，P1742）
// 用法: Circle c = minCircle(pts);   // c.o 圆心，c.r 半径
// 复杂度: 期望 O(n)（必须先随机打乱，代码里已做）
Circle minCircle(vector<Pt> p) {
    shuffle(p.begin(), p.end(), mt19937(1022));
    int n = p.size();
    Circle c(p[0], 0);
    for (int i = 1; i < n; i++) {
        if (dis(p[i] - c.o) <= c.r + eps) continue;
        c = Circle(p[i], 0);
        for (int j = 0; j < i; j++) {
            if (dis(p[j] - c.o) <= c.r + eps) continue;
            c = Circle((p[i] + p[j]) / 2, dis(p[i] - p[j]) / 2);
            for (int k = 0; k < j; k++) {
                if (dis(p[k] - c.o) <= c.r + eps) continue;
                c = get_circumcircle(p[i], p[j], p[k]);   // 三点不共线（共线时第三点一定已在以 i,j 为直径的圆内）
            }
        }
    }
    return c;
}

// 【速查】平面最近点对（分治，P1429）
// 用法: double d = closestPair(pts);   // 返回最近两点距离；点数 < 2 返回 1e18
// 复杂度: O(n log n)
double closestPair(vector<Pt> p) {
    int n = p.size();
    if (n < 2) return 1e18;
    sort(p.begin(), p.end(), [](const Pt& a, const Pt& b) { return a.x < b.x || (a.x == b.x && a.y < b.y); });
    vector<Pt> tmp(n);
    double best = 1e18;
    function<void(int, int)> rec = [&](int l, int r) {        // [l, r)，结束时按 y 有序
        if (r - l <= 3) {
            for (int i = l; i < r; i++) for (int j = i + 1; j < r; j++) best = min(best, dis(p[i] - p[j]));
            sort(p.begin() + l, p.begin() + r, [](const Pt& a, const Pt& b) { return a.y < b.y; });
            return;
        }
        int m = (l + r) / 2;
        double midx = p[m].x;
        rec(l, m); rec(m, r);
        merge(p.begin() + l, p.begin() + m, p.begin() + m, p.begin() + r, tmp.begin(), [](const Pt& a, const Pt& b) { return a.y < b.y; });
        copy(tmp.begin(), tmp.begin() + (r - l), p.begin() + l);
        int k = 0;
        for (int i = l; i < r; i++) {
            if (fabs(p[i].x - midx) >= best) continue;
            for (int j = k - 1; j >= 0 && p[i].y - tmp[j].y < best; j--) best = min(best, dis(p[i] - tmp[j]));
            tmp[k++] = p[i];
        }
    };
    rec(0, n);
    return best;
}

// 【速查】圆的面积并（格林公式，精确，SP8073 / HDU 类题）
// 用法: double S = circleUnionArea(cs);   // cs 为 vector<Circle>
// 原理: 面积 = ∮(x dy - y dx)/2，只沿"不被别的圆覆盖的圆弧"积分；每段弧的积分有闭式
// 复杂度: O(n² log n)
double circleUnionArea(vector<Circle> cs) {
    // 去掉重复圆、被别的圆包含的圆
    vector<Circle> c;
    for (int i = 0; i < (int)cs.size(); i++) {
        if (cs[i].r < eps) continue;
        bool drop = false;
        for (int j = 0; j < (int)cs.size() && !drop; j++) {
            if (i == j) continue;
            double d = dis(cs[i].o - cs[j].o);
            bool same = d < eps && fabs(cs[i].r - cs[j].r) < eps;
            if (same ? j < i : d + cs[i].r <= cs[j].r + eps) drop = true;
        }
        if (!drop) c.push_back(cs[i]);
    }
    double res = 0;
    for (int i = 0; i < (int)c.size(); i++) {
        vector<pair<double, double>> cover;          // 被覆盖的角度区间，[-π, π]
        for (int j = 0; j < (int)c.size(); j++) {
            if (i == j) continue;
            Pt v = c[j].o - c[i].o;
            double d = dis(v);
            if (d >= c[i].r + c[j].r - eps) continue;   // 相离或外切
            double base = atan2(v.y, v.x);
            double a = acos(max(-1.0, min(1.0, (c[i].r * c[i].r + d * d - c[j].r * c[j].r) / (2 * c[i].r * d))));
            double l = base - a, r = base + a;
            if (l < -PI) { cover.push_back({l + 2 * PI, PI}); cover.push_back({-PI, r}); }
            else if (r > PI) { cover.push_back({l, PI}); cover.push_back({-PI, r - 2 * PI}); }
            else cover.push_back({l, r});
        }
        sort(cover.begin(), cover.end());
        auto arc = [&](double a, double b) {          // 沿第 i 个圆从角 a 到角 b 的积分
            const Circle& C = c[i];
            return 0.5 * (C.r * C.r * (b - a) + C.o.x * C.r * (sin(b) - sin(a)) - C.o.y * C.r * (cos(b) - cos(a)));
        };
        double cur = -PI;
        for (auto [l, r] : cover) {
            if (l > cur) res += arc(cur, l);
            cur = max(cur, r);
        }
        if (cur < PI) res += arc(cur, PI);
    }
    return res;
}
