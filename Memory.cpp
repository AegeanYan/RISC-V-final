//
// Created by 罗皓天 on 2021/6/29.
//

#include "Memory.h"
#include <iostream>
#include <string>
#include <sstream>
using namespace std;
void Memory::read(){
    string ch;
    int i = 0;
    while (cin >> ch) {
        if (ch[0] == '@'){
            i = 0;
            for (int j = 1; j <= 8 ; ++j) {
                if (ch[j] >= 'A' && ch[j] <= 'F')i = (i << 4) + (ch[j] - 'A') + 10;
                else i = (i << 4) + (ch[j] - '0');
            }
        } else if (ch[0] == '#')break;
        else if (ch[0] != '@') {
            stringstream ss;
            ss << ch;
            ss >> hex >> mem[i++];
        }
    }
}