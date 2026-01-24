#include <bits/stdc++.h>
using namespace std;

const double PI = acos(-1);

int sgn(double x) {
    if (fabs(x) < 1e-8) return 0;
    return x < 0 ? -1 : 1;
}

struct Point{
    double x, y, z;
    Point(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}

    Point operator+ (const Point& B) const {return Point(x + B.x, y + B.y, z + B.z);}

    Point operator- (const Point& B) const {return Point(x - B.x, y - B.y, z - B.z);}
    //点乘
    double operator* (const Point& B) const {return x * B.x + y * B.y + z * B.z;}
    //叉乘
    Point operator% (const Point& B) const {return Point(y * B.z - z * B.y, z * B.x - x * B.z, x * B.y - y * B.x);}
    //数乘
    Point operator* (const double& B) const {return Point(x * B, y * B, z * B);}
    //数除（
    Point operator/ (const double& B) const {return Point(x / B, y / B, z / B);}
    
    bool operator== (const Point& B) const {return sgn(x - B.x) == 0 && sgn(y - B.y) == 0 && sgn(z - B.z) == 0;}
    
    bool operator!= (const Point& B) const {return sgn(x - B.x) != 0 || sgn(y - B.y) != 0 || sgn(z - B.z) != 0;}


};

double dis(Point& a){
	return sqrt(a * a);
}

double ddis(Point& a){
	return a * a;
}

// 两点间距离
double dis(Point& a, Point& b){
    return sqrt((a - b) * (a - b));
}
//两点间距离平方
double ddis(Point& a, Point& b){
    return (a - b) * (a - b);
}
//向量夹角
double Angle(Point& a, Point& b){
    return acos(a * b / dis(a) / dis(b));
}




int main(){

    return 0;
}
