#include <QCoreApplication>
#include <QDebug>
#include <algorithm>
using namespace std;


// 判断一个字符串是不是回文序列，仅考虑字母，同时忽略大小写
// 如：
// “A man, a plan, a canal: Panama” 是回文序列
// "race a car" 不是回文序列

class Solution
{
public:
    bool isPalindrome(std::string str)
    {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        std::string::iterator left = str.begin();
        std::string::iterator right = str.end() - 1;
        while(left<right){
            if(!::isalnum(*left)) ++left;
            else if(!::isalnum(*right)) --right;
            else if( *left != *right) return false;
            else{
                left++;
                right--;
            }
        }
        return true;
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Solution Palindrome;
    qDebug()<<Palindrome.isPalindrome("A man, a plan, a canal: Panama");
    return a.exec();
}
