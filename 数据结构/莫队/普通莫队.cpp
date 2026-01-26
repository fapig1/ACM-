#include <bits/stdc++.h>
using namespace std;
#define int long long
#define double long double

const int maxn = 5e4;
int B;
int cur = 0;

struct Q{
    int l, r, id;

    bool operator<(const Q& b)const{
        int pos1 = l / B, pos2 = b.l / B;
        if(pos1 != pos2) return pos1 < pos2;
        if(pos1 & 1) return r < b.r;
        return r > b.r;
    }
};

vector<int> cnt(maxn + 5);
vector<int> a(maxn + 5);
vector<Q> ask(maxn + 5);
vector<pair<int,int>> ans(maxn + 5);

int l = 1, r = 0;

void add(int pos){
    int val = a[pos];
    cur += cnt[val];
    cnt[val]++; 
}

void del(int pos){
    int val = a[pos];
    cnt[val]--;
    cur -= cnt[val];
}

void solve(){
    int n, m;
    cin >> n >> m;
    B = max(1.0, n / sqrt(m * 2.0 / 3.0));
    for(int i = 1; i <= n; i++){
        cin >> a[i];
    }
    for(int i = 0; i < m; i++){
        cin >> ask[i].l >> ask[i].r;
        ask[i].id = i;
    }
    sort(ask.begin(), ask.begin() + m);
    for(int i = 0; i < m; i++){
        auto[ql, qr, id] = ask[i];
        while(l > ql) add(--l);
        while(r < qr) add(++r);
        while(l < ql) del(l++);
        while(r > qr) del(r--);
        int len = r - l + 1;
        int fm = cur == 0 ? 1 : len * (len - 1) / 2;
        int g = cur == 0 ? 1 : __gcd(cur, fm);
        ans[id] = {cur / g, fm / g};
    }
    for(int i = 0; i < m; i++){
        cout << ans[i].first << '/' << ans[i].second << '\n';
    }


}

