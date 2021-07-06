//
// Created by 罗皓天 on 2021/7/2.
//
#include <iostream>
#include "simulator.h"
#include <string>
#include <cstdio>
using namespace std;
int main(){
    freopen("IN.data" , "r" , stdin);
    freopen("MYOUT.data" , "w" , stdout);
    simulator s;
    s.read();
    //cout << 12222222;
    s.run();
}
