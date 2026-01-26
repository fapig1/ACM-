#include <bits/stdc++.h>
using namespace std;
#define int long long
#define double long double

int n, q, B;
struct Q{
    int l, r, id;

    bool operator<(const Q& b)const{
        if(l / B != b.l / B) return l / B < b.l / B;
        return r < b.r;
    }
    Q(int l, int r, int id) : l(l), r(r), id(id) {}
};

vector<int> a;
vector<Q> ask;
vector<int> fst, lst;
vector<int> cnt(200005);

void solve(){
    cin >> n;
    a.resize(n);
    fst.resize(n + 5, - 1);
    lst.resize(n + 5, -1);
    for(int i = 0; i < n; i++){
        cin >> a[i];
    }    
    cin >> q;
    B = max(1.0, n / sqrt(q * 2.0 / 3.0));
    auto b = a;
    sort(b.begin(), b.end());
    b.erase(unique(b.begin(), b.end()), b.end());
    for(int i = 0; i < n; i++){
        a[i] = lower_bound(b.begin(), b.end(), a[i]) - b.begin();
    }
    for(int i = 0; i < q; i++){
        int x, y;
        cin >> x >> y;
        ask.push_back(Q(x - 1, y - 1, i));
    }
    sort(ask.begin(), ask.end());
    vector<int> ans(q);
    int lb = -1, r = -1, res = 0;
    for(int i = 0; i < q; i++){
        auto[ql, qr, id] = ask[i];
        int cur = ql / B;

        if(cur != lb){
            fill(fst.begin(), fst.end(), -1);
            fill(lst.begin(), lst.end(), -1);
            res = 0;
            lb = cur;
            r = (cur + 1) * B - 1;
        }
        
        if(cur == qr / B){
            int temp = 0;
            vector<int> wait;
            for(int j = ql; j <= qr; j++){
                int val = a[j];
                if(fst[val] == -1){
                    fst[val] = j;
                    wait.push_back(val);
                }
                lst[val] = j;
                temp = max(temp, j - fst[val]);
            }
            ans[id] = temp;
            for(int val : wait){
                fst[val] = -1;
                lst[val] = -1;
            }
            continue;
        }


        while(r < qr){
            r++;
            int val = a[r];
            if(fst[val] == -1) fst[val] = r;
            lst[val] = r;
            res = max(res, lst[val] - fst[val]);
        }

        int l = min(n, (cur + 1) * B);
        int tmp = res;
        stack<array<int,3>> wait;// val fst lst 
        while(l > ql){
            l--;
            int val = a[l];
            wait.push({val, fst[val], lst[val]});
            if(lst[val] == -1) lst[val] = l;
            fst[val] = l;
            res = max(res, lst[val] - fst[val]);
        }

        ans[id] = res;
        res = tmp;

        while(wait.size()){
            auto[val, l, r] = wait.top();
            wait.pop();
            fst[val] = l;
            lst[val] = r; 
        }

    }

    for(int i : ans) cout << i << '\n';

}
