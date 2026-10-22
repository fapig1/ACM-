#include <bits/stdc++.h>
using namespace std;
#define int long long

const int N=1e6+10;
int n,m;
char s1[N],s2[N];
string s,p;
int ne[N];

//马拉车(Manacher)求最长回文子串长度
//原版 getd() 无 '#' 分隔符且无边界检查(不能处理偶回文, 越界为 UB), 且从未被调用 —— 已重写
//d[i]: 变换串 t = "#a#b#..." 中以 i 为中心的回文臂长, 恰等于原串对应回文的长度
int d[N * 2];
// 【速查】Manacher 最长回文子串
// 用法: int len = manacher(s);    // 返回最长回文子串长度（奇偶都算）
// 进阶: 调用后 d[i] 是变换串 "#a#b#" 中以 i 为中心的回文长度（= 原串回文长度）
// 复杂度: O(n)
int manacher(const string& str){
    string t = "#";
    for(char c : str){ t += c; t += '#'; }
    int m = t.size(), C = 0, R = 0, best = 0;
    for(int i = 0; i < m; i++){
        int k = (i < R) ? min(d[2 * C - i], R - i) : 0;
        while(i - k - 1 >= 0 && i + k + 1 < m && t[i - k - 1] == t[i + k + 1]) k++;
        d[i] = k;
        if(i + k > R){ C = i; R = i + k; }
        best = max(best, (int)d[i]);
    }
    return best;
}


// 【速查】KMP（写在 main 里，1-based）
// 用途: 模式串 p 在文本 s 中所有出现位置；ne[i] = p[1..i] 的最长相等真前后缀长度（border）
// 用法: s1、s2 是 1-based 的文本/模式；第一个循环求 ne，第二个循环匹配，j == n 时找到一次（起点 i-n+1）
// 常用: 最小循环节长度 = n - ne[n]
// 复杂度: O(n + m)
signed main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
  
    cin>>s>>p;
    int m=s.length(),n=p.length();
    ne[1]=0;
    for(int i=1;i<=m;i++)
    {
      s1[i]=s[i-1];
    }
    for(int i=1;i<=n;i++)
    {
      s2[i]=p[i-1];
    }
    for(int i=2,j=0;i<=n;i++)
    {
      while(j&&s2[i]!=s2[j+1]) j=ne[j];
      if(s2[i]==s2[j+1]) j++;
      ne[i]=j;
    }
    for(int i=1,j=0;i<=m;i++)
    {
      while(j&&s1[i]!=s2[j+1]) j=ne[j];
      if(s1[i]==s2[j+1]) j++;
      if(j==n) 
      {
        cout<<i-n+1<<endl;
        j=ne[j];
      }
    }

    for(int i=1;i<=n;i++)
    {
      cout<<ne[i]<<' ';
    }

    return 0;
}

