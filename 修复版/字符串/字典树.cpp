#include<bits/stdc++.h>
using namespace std;
// 【速查】字典树 Trie（前缀计数）
// 用法: Trie T;（放全局）  T.insert(s) 插入；T.query(s) -> 以 s 为前缀的已插入串个数；字符集 A-Z a-z 0-9（hashn 映射）
// 多组数据: 每组结束调用 T.clear()（只清用过的节点）
// 复杂度: 插入/查询 O(|s|)；空间 N × 70 个 int，N 按"所有串总长"开
// 10.22: 仅把原来的全局变量 / 函数放进结构体（solve 末尾的清空循环挪成 clear），算法代码未改
/* 原版有 #define int long long, 导致 ch[3e6][70] 每格 8B 共 1.68GB —— 已移除。
 * 字符集 62 (大写/小写/数字), 每个节点 70 个儿子槽, int(4B) 下:
 *   N=1e5 → 28MB; N=5e5 → 140MB。按题目"所有串总长"上限调整 N。 */
const int N=1e5+10;
const int base=13331;
const int mod=1e9+7;

struct Trie{
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

  void clear()
  {
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
    idx=0;
  }
};

Trie T;
int n,_T,q;
string s;

void solve()
{
  T.idx=0;
  cin>>n>>q;
  for(int i=1;i<=n;i++)
  {
    cin>>s;
    T.insert(s);
  }
  while(q--)
  {
    cin>>s;
    cout<<T.query(s)<<endl;
  }
  T.clear();
}

int main(){
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
