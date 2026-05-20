#include <bits/stdc++.h>
using namespace std;
#define int long long

const int N=1e6+10;
int n,m;
char s1[N],s2[N];
string s,p;
int ne[N];

//马拉车求最长回文串
int d[N];
void getd()
{
  d[1]=1;
  for(int i=2,l=0,r=1;i<=n;i++)
  {
    if(i<=r) d[i]=min(r-i+1,d[r-i+l]);
    while(s[i-d[i]]==s[i+d[i]]) d[i]++;
    if(i+d[i]-1>r) l=i-d[i]+1,r=i+d[i]-1;
  }
}


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

