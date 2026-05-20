#include<bits/stdc++.h>
#define int long long
using namespace std;
const int N=3e6+10;
const int base=13331;
const int mod=1e9+7;
int n,_T,q;
string s;
int ch[N][70]={0};
int idx;
int cnt[N];

int hashn(char x)
{
  if(x>='A'&&x<='Z')
  {
    return x-'A';
  }
  else if(x>='a'&&x<='z')
  {
    return x-'a'+26;
  }
  else 
  return x-'0'+52;
}

void insert(string s)
{
  int p=0;
  int len=s.length();
  for(int i=0;i<len;i++)
  {
    int j=hashn(s[i]);
    if(!ch[p][j]) ch[p][j]=++idx;
    p=ch[p][j];
    cnt[p]++;
  }
  
}

int query(string s)
{
  int p=0;
  int len=s.length();
  for(int i=0;i<len;i++)
  {
    int j=hashn(s[i]);
    if(!ch[p][j]) return 0;
    p=ch[p][j];
  }
  return cnt[p];
}

void solve()
{
  idx=0;
  cin>>n>>q;
  for(int i=1;i<=n;i++)
  {
    cin>>s;
    insert(s);
  }
  while(q--)
  {
    cin>>s;
    cout<<query(s)<<endl;
  }
  for(int i=0;i<=idx;i++)
  {
    for(int j=0;j<=65;j++)
    {
      ch[i][j]=0;
    }
  }
  for(int i=0;i<=idx;i++)
  {
    cnt[i]=0;
  }
}

signed main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    
    cin>>_T;
    while(_T--)
    {
      solve();
    }

    return 0;
}
