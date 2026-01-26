#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

// 高精度减法函数 (计算 num1 - num2)
string subtractBigNumbers(string num1, string num2) {
    // 检查结果是否为负
    bool isNegative = false;
    // 确保 num1 >= num2，否则交换并标记结果为负[2,3](@ref)
    if (num1.size() < num2.size() || (num1.size() == num2.size() && num1 < num2)) {
        swap(num1, num2);
        isNegative = true;
    }

    // 将两个数字补零至相同长度[1,2](@ref)
    int maxLength = max(num1.size(), num2.size());
    while (num1.size() < maxLength) num1 = "0" + num1;
    while (num2.size() < maxLength) num2 = "0" + num2;

    string result = "";
    int borrow = 0; // 借位值

    // 从最低位（字符串末尾）开始逐位相减[2](@ref)
    for (int i = maxLength - 1; i >= 0; i--) {
        int digit1 = num1[i] - '0';
        int digit2 = num2[i] - '0';
        
        // 减去借位
        digit1 -= borrow;
        borrow = 0; // 重置借位
        
        // 如果当前位不够减，则需要借位[2,4](@ref)
        if (digit1 < digit2) {
            digit1 += 10;
            borrow = 1;
        }
        int diff = digit1 - digit2;
        result.push_back(diff + '0'); // 将结果位加入字符串
    }

    // 反转结果字符串（因为是从低位开始计算）
    reverse(result.begin(), result.end());
    
    // 移除前导零[1,2](@ref)
    size_t nonZeroIndex = result.find_first_not_of('0');
    if (nonZeroIndex != string::npos) {
        result = result.substr(nonZeroIndex);
    } else {
        result = "0"; // 如果结果全零，则返回"0"
    }

    // 添加负号（如果需要）[2](@ref)
    if (isNegative && result != "0") {
        result = "-" + result;
    }
    
    return result;
}

int main() {
    string a, b;
    cin >> a >> b;
    
    string result = subtractBigNumbers(a, b);
    cout << result << '\n';
    
    return 0;
}