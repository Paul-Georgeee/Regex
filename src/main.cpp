#include<iostream>
#include<string>
#include<RE.h>
using namespace std;

int main(int argc, const char* argv[])
{
    {
        Regex re("(a|b)*a(b|c)*b(a|c)*");
        cout << re.match("aabbccccccc") << endl;
        cout << re.match("aabbccab") << endl << endl;
    }
    {
        Regex re("(0|1)*(01)(0|1)*(01)(0|1)*");
        cout << re.match("0000111") << endl;
        cout << re.match("010000001010111") << endl;
        cout << re.match("22222333") << endl << endl;
    }
    {
        Regex re("((0|1)*1(0|1)*0(0|1)*0(0|1)*)|((0|1)*0(0|1)*1(0|1)*0(0|1)*)|((0|1)*0(0|1)*0(0|1)*1(0|1)*)");
        cout << re.match("00000000000000") << endl;
        cout << re.match("1111111111111111111111110") << endl;
        cout << re.match("1111111111110111011111111111") << endl << endl;
    }
    {
        Regex re("((0|1)(0|1))*((0|1)0|1)");
        cout << re.match("111111000") << endl;
        cout << re.match("10100101") << endl;
        cout << re.match("111110000010101010100100") << endl;
        cout << re.match("100010111011111000011") << endl << endl;
    }
    {
        Regex re("(\\(|\\))*");
        cout << re.match("((((((") << endl;
        cout << re.match("))()()(") << endl;
        cout << re.match("()()()))s()") << endl << endl; 
    }
    {
        Regex re("(0|1|2|3|4|5|6|7|8|9)*@qq.com");
        cout << re.match("2535131331@qq.com") << endl;
        cout << re.match("22231@qqq.com") << endl;
        cout << re.match("12312git@qq.com") << endl;
    }
}