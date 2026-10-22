#include <bits/stdc++.h>
using namespace std;

// 【速查】单调栈：每个位置左/右第一个比它大（或小）的位置
// 用法: auto R = nextGreater(a);   // a 为 0-based，R[i] = 右边第一个 a[j] > a[i] 的 j，没有为 n
//       auto L = prevGreater(a);   // L[i] = 左边第一个 a[j] > a[i] 的 j，没有为 -1
// 改"更小": 把 while 里 a[st.back()] 与 a[i] 的比较方向反过来；
// 严格/非严格: nextGreater 里 < 改 <= 就变成"右边第一个 >= 的"
// 用途: 以 a[i] 为最大值的区间 (L[i], R[i])、柱状图最大矩形、贡献法计数
// 复杂度: O(n)
vector<int> nextGreater(const vector<long long>& a) {
    int n = a.size();
    vector<int> res(n, n), st;
    for (int i = 0; i < n; i++) {
        while (!st.empty() && a[st.back()] < a[i]) { res[st.back()] = i; st.pop_back(); }
        st.push_back(i);
    }
    return res;
}
vector<int> prevGreater(const vector<long long>& a) {
    int n = a.size();
    vector<int> res(n, -1), st;
    for (int i = 0; i < n; i++) {
        while (!st.empty() && a[st.back()] <= a[i]) st.pop_back();
        if (!st.empty()) res[i] = st.back();
        st.push_back(i);
    }
    return res;
}

// 【速查】单调队列：滑动窗口最大值 / 最小值
// 用法: auto mx = slidingMax(a, k);   // a 为 0-based，mx[i] = max(a[i..i+k-1])，长度 n-k+1
//       slidingMin 同理
// 用途: 窗口最值、单调队列优化 DP（dp[i] = max_{i-k<=j<i} dp[j] + ...，队列里存下标）
// 复杂度: O(n)
vector<long long> slidingMax(const vector<long long>& a, int k) {
    vector<long long> res;
    deque<int> q;
    for (int i = 0; i < (int)a.size(); i++) {
        while (!q.empty() && a[q.back()] <= a[i]) q.pop_back();
        q.push_back(i);
        if (q.front() <= i - k) q.pop_front();
        if (i >= k - 1) res.push_back(a[q.front()]);
    }
    return res;
}
vector<long long> slidingMin(const vector<long long>& a, int k) {
    vector<long long> res;
    deque<int> q;
    for (int i = 0; i < (int)a.size(); i++) {
        while (!q.empty() && a[q.back()] >= a[i]) q.pop_back();
        q.push_back(i);
        if (q.front() <= i - k) q.pop_front();
        if (i >= k - 1) res.push_back(a[q.front()]);
    }
    return res;
}
