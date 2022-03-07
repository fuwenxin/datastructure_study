// 本文件用于测试仿函数
// https://blog.csdn.net/a130737/article/details/38147143
#include <string>
#include <iostream>
using namespace std;

class X{
public:
    X(){
        cout << "123" << endl;
    }
    void operator()(string str){
        cout << str << endl;
    }
};

int main(){
    
    string s = "1234";
    X()(s);
}