#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// 【速查】数位 DP（记忆化搜索框架，结构体封装）
// 用途: 统计 [l, r] 内满足某种"按位条件"的数的个数
// 用法: DigitDP dp;  dp.base = 进制;  ll ans = dp.calc(r) - dp.calc(l - 1);
// 改题: 只改"按题意修改"那一段：状态个数 S、trans（加一位后的新状态）、valid（剪枝）、check（终态是否合法）、
//       是否计入 0（calc 开头）；状态有多维就把 state 编码成一个整数
// 记忆化: f[dep][state] 只在 !tight && started 时有效，和上界无关，所以多次 calc 不用清空
//         （若状态和数字总位数有关，就每次 calc 前 reset()）
// 复杂度: O(位数 × S × base)
// 10.22: 原版是全局 f / num / base + 全局函数，已封装为结构体
struct DigitDP {
    // ---- 按题意修改 ----
    static const int S = 100;                  // 状态个数上限
    int base = 2;
    bool check(int state) { return true; }      // 最终状态是否合法
    int trans(int state, int d) { return state; }   // 在末尾接上数字 d 之后的状态
    bool valid(int state) { return true; }      // 中间剪枝，不需要就 return true
    // --------------------
    vector<int> num;
    vector<array<ll, S>> f;

    DigitDP() { reset(); }
    void reset() { f.assign(64, {}); for (auto& row : f) row.fill(-1); }

    ll dfs(int dep, int state, bool tight, bool started) {
        if (dep == 0) return started && check(state) ? 1 : 0;   // 改: 是否允许全 0
        if (!tight && started && f[dep][state] != -1) return f[dep][state];
        int up = tight ? num[num.size() - dep] : base - 1;
        ll ans = 0;
        for (int d = 0; d <= up; d++) {
            if (!started && d == 0) ans += dfs(dep - 1, 0, tight && d == up, false);
            else {
                int ns = trans(state, d);
                if (valid(ns)) ans += dfs(dep - 1, ns, tight && d == up, true);
            }
        }
        if (!tight && started) f[dep][state] = ans;
        return ans;
    }
    ll calc(ll x) {                            // [1, x] 中合法的数的个数
        if (x <= 0) return 0;                   // 改: 是否计入数字 0
        num.clear();
        for (; x; x /= base) num.push_back(x % base);
        reverse(num.begin(), num.end());
        return dfs(num.size(), 0, true, false);  // 改: 初始状态
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    DigitDP dp;
    ll l, r;
    cin >> l >> r;
    cout << dp.calc(r) - dp.calc(l - 1) << "\n";
    return 0;
}
