//
// Created by 罗皓天 on 2021/6/29.
//

#ifndef RISC_V_SIMULATOR_MEMORY_H
#define RISC_V_SIMULATOR_MEMORY_H

#include <string>
#include <sstream>
#include <cstring>
class Memory {
public:
    //int *mem;
    int mem[1000000];
public:
    Memory() {
        //mem = new int[1000000];
        memset(mem , 0 , sizeof mem);
    };
    ~Memory() = default;
    void read();
};


#endif //RISC_V_SIMULATOR_MEMORY_H
