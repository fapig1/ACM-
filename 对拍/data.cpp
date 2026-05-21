#include <bits/stdc++.h>
using namespace std;
#define int long long

mt19937_64 rd(random_device{}());

int rnd(int l, int r){
    return l + rd() % (r - l + 1);
}

signed main(){
    cout << rnd(1, 100);
    return 0;
}