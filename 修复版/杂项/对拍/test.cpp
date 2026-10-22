#include <bits/stdc++.h>
using namespace std;
#define int long long


// 【速查】对拍·驱动 test.cpp
// 用法: 先分别编译 data / solve(待测) / bf(暴力)，再运行本程序；输出 WA 时 data.in 就是反例
//       Windows 用 data.exe + fc，Linux 换成注释里的 ./data + diff
signed main(){
    int t = 1000;
    for(int i = 1; i <= t; i++){

        /*
        system("./data > data.in");
        system("./solve < data.in > solve.out");
        system("./bf < data.in > bf.out");
        */

        system("data.exe > data.in");
        system("solve.exe < data.in > solve.out");
        system("bf.exe < data.in > bf.out");

        /*
        if(system("diff solve.out bf.out > /dev/null"))
        */
        if(system("fc solve.out bf.out > nul")){
            cout << "WA";
            break;
        }

        cout << i << " AC\n";

    }
    return 0;
}