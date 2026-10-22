#include <bits/stdc++.h>
using namespace std;
#define int long long
#define double long double

const int maxn = 5e4;

// 【速查】普通莫队（离线区间查询，无修改）
// 用途: 区间信息能 O(1) 加/删一个元素时，离线回答 m 个区间询问。本例: 区间内任取两数相同的概率（P1494）
// 用法: Mo mo;（放全局）  mo.solve()；套题时改 add(pos) / del(pos) 维护 cur；询问按 (l/B, 奇偶 r) 排序后移动指针
// 复杂度: O(n√m)，B = n/√(2m/3)
// 10.22: 仅把原来的全局变量 / 函数放进结构体（B 改为静态成员供 Q 排序使用），算法代码未改
struct Mo{
    inline static int B;
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

    vector<int> cnt = vector<int>(maxn + 5);
    vector<int> a = vector<int>(maxn + 5);
    vector<Q> ask = vector<Q>(maxn + 5);
    vector<pair<int,int>> ans = vector<pair<int,int>>(maxn + 5);

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
        // 离散化: a_i 值域可能很大(如 1e9), cnt 数组只按种类数开
        {
            vector<int> all(a.begin() + 1, a.begin() + n + 1);
            sort(all.begin(), all.end());
            all.erase(unique(all.begin(), all.end()), all.end());
            for(int i = 1; i <= n; i++)
                a[i] = lower_bound(all.begin(), all.end(), a[i]) - all.begin() + 1;
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
};

Mo mo;
void solve(){ mo.solve(); }
