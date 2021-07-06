//
// Created by 罗皓天 on 2021/6/29.
//

#ifndef RISC_V_SIMULATOR_REGISTOR_H
#define RISC_V_SIMULATOR_REGISTOR_H
#include <iostream>
#include <cstring>
class Registor {
public:
    int reg[32];
    int fetched_instruct;
    int imme;
    int opt;
    int rd;
    int rs1;
    int rs2;
    int pc;
//    int zimm;
//    int csr;
//    int pred;
//    int succ;
    int l_or_r;
    int shamt;
    int branch;
    Registor(){
        memset(reg , 0 , sizeof reg);
        fetched_instruct = 0;
        pc = imme = opt = rd = rs1 = rs2 = l_or_r = shamt = 0;
        branch = -1;
    }
    void decode();
};


#endif //RISC_V_SIMULATOR_REGISTOR_H
