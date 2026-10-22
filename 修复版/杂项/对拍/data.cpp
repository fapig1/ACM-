#include <bits/stdc++.h>
using namespace std;
#define int long long

mt19937_64 rd(random_device{}());

// 【速查】对拍·数据生成器 data.cpp
// 用法: rnd(l, r) 返回 [l, r] 的随机整数，按题目格式输出一组数据
int rnd(int l, int r){
    return l + rd() % (r - l + 1);
}

signed main(){
    cout << rnd(1, 100) << " " << rnd(1, 100);
    return 0;
}