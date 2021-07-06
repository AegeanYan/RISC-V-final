//
// Created by 罗皓天 on 2021/6/29.
//

#ifndef RISC_V_SIMULATOR_ALU_H
#define RISC_V_SIMULATOR_ALU_H


class ALU {
public:
    bool overflow;
    bool zero;
    bool negative;
    bool flag;
public:
    ALU():overflow(false),zero(false),negative(false),flag(false){};
};


#endif //RISC_V_SIMULATOR_ALU_H
