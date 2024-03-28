#include<bits/stdc++.h>

using namespace std;


/*
在编译期求阶乘
准备
既然是在编译期求的量，那么应该是一个常量，我们需要const 来修饰。

因为是编译期的数据，所以需要是static （这样就可以在编译期获得了。

所以我们先写出这样的格式。
*/

template<int N>
struct Fac {
    const static int value = N * Fac<N - 1>::value;
};

template<>
struct Fac<0> {
    const static int value = 1;
};

template<int N>
struct Fib
{
    const static int value = Fib<N - 1>::value + Fib<N - 2>::value;
    /* data */
};


template<>
struct Fib<1> {
    const static int value = 1;
};

template<>
struct Fib<2> {
    const static int value = 1;
};




int main(){

    Fac<3>::value;
    Fib<6>::value;

    return 0;
}