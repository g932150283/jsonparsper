#include <bits/stdc++.h>

using namespace std;


int main(){

    tuple<int, char, string> t(113, 'c', "hello");
    cout << get<0>(t) << endl;
    cout << get<1>(t) << endl;
    cout << get<2>(t) << endl;
    return 0;
}