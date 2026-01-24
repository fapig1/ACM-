#include <bits/stdc++.h>
using namespace std;

struct SDU{
    int n, cnt;//cnt为连通块数量
    vector<int> fa;
    SDU(int m = 0){
        n = m;
        fa.resize(n + 1);
        for(int i = 1; i <= n; i++) fa[i] = i;//1-based
        cnt = n;
    }

    int find(int x){
        return x == fa[x] ? x : fa[x] = find(fa[x]);
    }

    void merge(int x, int y){
        int xx = find(x);
        int yy = find(y);
        if(xx != yy){
            fa[xx] = yy;
            cnt--;
        }
    }

    bool check(int x, int y){
        return find(x) == find(y);
    }
};

//AC自动机模板
struct AC{
    static const int maxn = 1e6 + 5;
    int tr[maxn][26];
    int fail[maxn];
    int end[maxn];
    int tot;

    void init(){
        for(int i = 0; i <= tot; i++){
            memset(tr[i], 0, sizeof(tr[i]));
            fail[i] = end[i] = 0;
        }
        tot = 0;
    }

    void insert(string s){
        int u = 0;
        for(auto c : s){
            int v = c - 'a';
            if(!tr[u][v]) tr[u][v] = ++tot;
            u = tr[u][v];
        }
        end[u]++;
    }

    void build(){
        queue<int> q;
        for(int i = 0; i < 26; i++){
            if(tr[0][i]){
                q.push(tr[0][i]);
            }
        }

        while(q.size()){
            int u = q.front();
            q.pop();
            for(int i = 0; i < 26; i++){
                if(tr[u][i]){
                    fail[tr[u][i]] = tr[fail[u]][i];
                    q.push(tr[u][i]);
                } else {
                    tr[u][i] = tr[fail[u]][i];
                }
            }
        }
    }

    int query(string t){
        int u = 0, res = 0;
        for(auto c : t){
            u = tr[u][c - 'a'];
            for(int j = u; j && end[j] != -1; j = fail[j]){
                res += end[j];
                end[j] = -1;
            }
        }
        return res;
    }
} ac;



signed main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr), cout.tie(nullptr);
    
    return 0;
}
