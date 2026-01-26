#include <bits/stdc++.h>
using namespace std;

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
