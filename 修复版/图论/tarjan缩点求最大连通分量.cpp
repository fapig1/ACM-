#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// 【速查】Tarjan 强连通分量 + 缩点（有向图 → DAG）
// 用途: 有向图里"互相可达"的点缩成一个点，之后在 DAG 上做拓扑 / DP（如最大点权路径，P3387）
// 用法: SCC s(n);  s.addEdge(u, v);  s.build();            // 点 1-based
// 结果: s.comp[u] = 点 u 所属分量 (0..s.cnt-1)；分量编号是"逆拓扑序"（编号大的指向编号小的）
//       auto dag = s.dag();  缩点后的 DAG 邻接表（已去重），dag[c] 是分量 c 的出边
// 复杂度: O(n + m)；迭代实现，1e6 深链也不会爆栈
struct SCC {
    int n, cnt = 0;
    vector<vector<int>> g;
    vector<int> comp;

    SCC(int n) : n(n), g(n + 1), comp(n + 1, -1) {}
    void addEdge(int u, int v) { g[u].push_back(v); }

    void build() {
        int timer = 0;
        vector<int> dfn(n + 1, -1), low(n + 1), it(n + 1, 0), st, call;
        vector<char> inStack(n + 1, 0);
        for (int s = 1; s <= n; s++) {
            if (dfn[s] != -1) continue;
            dfn[s] = low[s] = timer++; st.push_back(s); inStack[s] = 1; call.push_back(s);
            while (!call.empty()) {
                int u = call.back();
                if (it[u] < (int)g[u].size()) {
                    int v = g[u][it[u]++];
                    if (dfn[v] == -1) { dfn[v] = low[v] = timer++; st.push_back(v); inStack[v] = 1; call.push_back(v); }
                    else if (inStack[v]) low[u] = min(low[u], dfn[v]);
                } else {
                    call.pop_back();
                    if (!call.empty()) low[call.back()] = min(low[call.back()], low[u]);
                    if (low[u] == dfn[u]) {          // u 是一个分量的根，弹栈
                        while (true) {
                            int w = st.back(); st.pop_back(); inStack[w] = 0;
                            comp[w] = cnt;
                            if (w == u) break;
                        }
                        cnt++;
                    }
                }
            }
        }
    }
    vector<vector<int>> dag() {
        vector<vector<int>> d(cnt);
        for (int u = 1; u <= n; u++)
            for (int v : g[u]) if (comp[u] != comp[v]) d[comp[u]].push_back(comp[v]);
        for (auto& e : d) { sort(e.begin(), e.end()); e.erase(unique(e.begin(), e.end()), e.end()); }
        return d;
    }
};

// 示例: 读入 n m、m 条有向边、n 个点权；输出分量数与缩点图边数（含重边）、每个分量的点集与点权和、最大分量点权和
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n, m;
    cin >> n >> m;
    SCC s(n);
    vector<pair<int, int>> edges(m);
    for (auto& [a, b] : edges) { cin >> a >> b; s.addEdge(a, b); }
    vector<ll> val(n + 1);
    for (int i = 1; i <= n; i++) cin >> val[i];
    s.build();
    vector<ll> sccval(s.cnt, 0);
    for (int i = 1; i <= n; i++) sccval[s.comp[i]] += val[i];
    int cross = 0;
    for (auto [a, b] : edges) if (s.comp[a] != s.comp[b]) cross++;
    cout << s.cnt << " " << cross << "\n";
    for (int c = 0; c < s.cnt; c++) {
        cout << "SCC " << c + 1 << " (val=" << sccval[c] << "):";
        for (int j = 1; j <= n; j++) if (s.comp[j] == c) cout << " " << j;
        cout << "\n";
    }
    int best = 0;
    for (int c = 1; c < s.cnt; c++) if (sccval[c] > sccval[best]) best = c;
    cout << "max sccval = " << (s.cnt ? sccval[best] : 0) << "\n";
    return 0;
}
