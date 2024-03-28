#include <bits/stdc++.h>

using namespace std;

union U
{
    int x;
    float y;
};

union U1
{
    int x;
    float y;
    double z;
};
union U2
{
    int arr[10];
    double z;
};

union U3
{
    struct node
    {
        int x;
        long long y;
    } u;
    double z;
};

union U4
{
    unsigned char bits[4];
    int num;
};

struct short_str
{
    union
    {
        unsigned long long hs;
        char s[8];
    };
};

struct point{
    int x, y;
};
struct line{
    union {
        struct {
            point p1, p2;
        };
        int arr[4];
    };
};

struct var{
    union {
        int iv;
        double dv;
        char* sv;
    };
    var(const int& v) :iv{ v } {};
    var(const double& v) :dv{ v } {};
    var(const char* s) {
        int len = strlen(s);
        sv = new char[len + 1];
        memcpy(sv, s, len + 1);
    }
};

int main()
{

    /*
    C/C++ 为我们提供了强制类型转换，使得我们可以把一块内存看成int，看成double......
    例如下面的代码，我们有个4个字节的数组，然后告诉编译器。
    请把 data 看成一个int* 的指针，并且往里面写入值。

    也就是，data[4] 用来保存了int和float ，当然了不能同时存在。
    */
    char data[4];
    *(int *)data = 114541;
    cout << *(int *)data << endl;
    *(float *)data = 1941.012;
    cout << *(float *)data << endl;

    cout << "========两个变量共用同一块内存=======" << endl;

    /*
    实际上就是两个变量共用同一块内存。

    如果是多个变量的话，union的大小就为最大的那一个变量。
    */
    U u;
    u.x = 114;
    cout << u.x << endl;
    u.y = 5.14;
    cout << u.y << endl;

    cout << "=======多个变量的话，union的大小就为最大的那一个变量========" << endl;

    cout << sizeof(U1) << endl;
    cout << sizeof(U2) << endl;

    /*
    可以union里面放个结构体。里面的结构体是需要满足内存对齐的。
    */

    cout << "=======结构体是需要满足内存对齐========" << endl;

    cout << sizeof(U3) << endl;
    cout << sizeof(U3::node) << endl;

    /*
    union查看内存分布
    看看一个int的数，或者一个结构体，其内存中的样子。
    */

    cout << "=======看看一个int的数，或者一个结构体，其内存中的样子。========" << endl;

    U4 x4;
    x4.num = 64;
    for (int i = 0; i < 4; i++)
    {
        cout << bitset<8>(x4.bits[i]) << " ";
    }
    cout << endl;
    x4.num = 114514;
    for (int i = 0; i < 4; i++)
    {
        cout << bitset<8>(x4.bits[i]) << " ";
    }
    cout << endl;

    /*
    union优化短字符串
    在字符串中，一般情况下比较相等，我们都是通过比较哈希值的。

    而有些库中，对于长度比较短的字符串是有单独的优化的。

    比如对于长度小于等于8的短字符串。可以这样
    */

    cout << "=======union优化短字符串========" << endl;

    short_str sstr;
    memcpy(sstr.s, "hello", 6);
    cout << sstr.s << endl;
    // 我们可以快速的获得短字符串的哈希值(把这个ull看成哈希值)，就能快速的比较两个短字符串是否相等了。
    cout << sstr.hs << endl;

    /*
    union取别名
    在一些二维的问题中，一般都是，定义点，然后点构成了线。
    struct point {
    int x, y;
    };
    struct line {
        point p1, p2;
    };   
    */

   /**
    * 如果我们需要访问具体的数据就要 name.p1.x 这样，比较麻烦。
    * 但我们可以利用union
    */

    cout << "=======union取别名========" << endl;

    line L = {114,514,1919,810};
    cout << L.p1.x << " " << L.p1.y << endl;
    cout << L.p2.x << " " << L.p2.y << endl;
    for (int i = 0; i < 4; i++)L.arr[i] = i;
    cout << L.p1.x << " " << L.p1.y << endl;
    cout << L.p2.x << " " << L.p2.y << endl;
    cout << sizeof(line) << endl;


    /*
    union实现简易的动态类型
    利用union我们可以轻易的写出一个简单的动态类型
    */

   /**
    * struct var{
    union {
        int iv;
        double dv;
        char* sv;
    };
    var(const int& v) :iv{ v } {};
    var(const double& v) :dv{ v } {};
    var(const char* s) {
        int len = strlen(s);
        sv = new char[len + 1];
        memcpy(sv, s, len + 1);
    }
    };
    */

    cout << "=======union实现简易的动态类型========" << endl;

    var x = 1415;
    cout << x.iv << endl;
    x = 3.14;
    cout << x.dv << endl;
    x = "hello world";
    cout << x.sv << endl;
    cout << "sizeof(char*) = " << sizeof(char*) << endl;
    return 0;
}