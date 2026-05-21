#include <bits/stdc++.h>
using namespace std;
#define int long long


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