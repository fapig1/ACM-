#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

string removeLeadingZeros(string s){
    int pos = s.find_first_not_of('0');
    if(pos == string::npos) return "0";
    return s.substr(pos);
}

string addBigNumbers(string num1, string num2){
    int maxLength = max(num1.size(), num2.size());

    while(num1.size() < maxLength) num1 = "0" + num1;
    while(num2.size() < maxLength) num2 = "0" + num2;

    string result = "";

    int carry = 0;

    for(int i = maxLength - 1; i >= 0; i--){
        int digit1 = num1[i] - '0';
        int digit2 = num2[i] - '0';

        int sum = digit1 + digit2 + carry;

        result.push_back(sum % 10 + '0');

        carry = sum / 10;
    }

    if(carry){
        result.push_back(carry + '0');
    }

    reverse(result.begin(), result.end());

    return removeLeadingZeros(result);
}

string subtractBigNumbers(string num1, string num2){
    bool isNegative = false;

    if(num1.size() < num2.size() || (num1.size() == num2.size() && num1 < num2)){
        swap(num1, num2);
        isNegative = true;
    }

    int maxLength = max(num1.size(), num2.size());

    while(num1.size() < maxLength) num1 = "0" + num1;
    while(num2.size() < maxLength) num2 = "0" + num2;

    string result = "";

    int borrow = 0;

    for(int i = maxLength - 1; i >= 0; i--){
        int digit1 = num1[i] - '0';
        int digit2 = num2[i] - '0';

        digit1 -= borrow;

        borrow = 0;

        if(digit1 < digit2){
            digit1 += 10;
            borrow = 1;
        }

        int diff = digit1 - digit2;

        result.push_back(diff + '0');
    }

    reverse(result.begin(), result.end());

    result = removeLeadingZeros(result);

    if(isNegative && result != "0"){
        result = "-" + result;
    }

    return result;
}

string multiplyBigNumbers(string num1, string num2){
    if(num1 == "0" || num2 == "0") return "0";

    int n = num1.size();
    int m = num2.size();

    string result(n + m, '0');

    for(int i = n - 1; i >= 0; i--){
        for(int j = m - 1; j >= 0; j--){
            int mul = (num1[i] - '0') * (num2[j] - '0');

            int sum = (result[i + j + 1] - '0') + mul;

            result[i + j + 1] = sum % 10 + '0';

            result[i + j] += sum / 10;
        }
    }

    return removeLeadingZeros(result);
}

string divideBigNumbers(string num1, int divisor){
    string result = "";

    long long cur = 0;

    for(int i = 0; i < num1.size(); i++){
        cur = cur * 10 + (num1[i] - '0');

        result.push_back(cur / divisor + '0');

        cur %= divisor;
    }

    return removeLeadingZeros(result);
}

int main(){
    string a, b;

    cin >> a >> b;

    cout << addBigNumbers(a, b) << '\n';

    cout << subtractBigNumbers(a, b) << '\n';

    cout << multiplyBigNumbers(a, b) << '\n';

    cout << divideBigNumbers(a, stoi(b)) << '\n';

    return 0;
}