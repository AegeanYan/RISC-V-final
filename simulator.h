//
// Created by 罗皓天 on 2021/6/28.
//

#ifndef RISC_V_SIMULATOR_SIMULATOR_H
#define RISC_V_SIMULATOR_SIMULATOR_H

#include <iostream>
#include "Memory.h"
#include "ALU.h"
#include "Registor.h"
#include <unordered_map>
class simulator {
private:
    Registor registor;
    ALU alu;
    Memory RAM;
    int pc;
    int j = 1;
    struct BuBle{
    public:
        bool IF = false;
        bool ID = false;
        bool EX = false;
        bool MEM = false;
        bool WB = false;
        BuBle() = default;
    };
    struct IF_end{
    public:
        bool if_end = false;
    };
    struct ID_end{
    public:
        int fetched = 0;
        bool id_end = false;
        int pc = 0;
    };
    struct WB_end{
    public:
        bool wb_end = false;
    };
    struct Buffer{
    public:
        int rd = 0;
        int rs1 = 0;
        int rs2 = 0;
    };
    struct forward_cap{
    public:
        int rd = 0;
        int imme = 0;
    };
    struct EX_Result{
    public:
        int rd = 0;
        int imme = 0;
        int branch = 0;
        int pc = 0;
        int opt = 0;
        int ans = 0;
        bool ex_end = false;
        bool pcflag = false;
        EX_Result() = default;
    };
    struct MEM_Data{
    public:
        int opt = 0;
        int rd = 0;
        int imme = 0;
        int branch = 0;
        int pc = 0;
        bool mem_end = false;
        bool pcflag = false;
        MEM_Data() = default;
    };
    EX_Result ex_result{};
    MEM_Data mem_data{};
    ID_end idEnd{};
    WB_end wbEnd{};
    IF_end ifEnd{};
    BuBle buBle{};
    forward_cap forwardCap{};
    Buffer buffer{};
public:
    simulator() {
        pc = 0;
    }
    void read();
    void IF();
    void ID();
    void EX(int option);
    void MEM(int option);
    void WB(int option);
    void run();
    void Hazard_detect();
    void Hazard_forward();
    static int sext(const int &im ,const int &maxw);
};


#endif //RISC_V_SIMULATOR_SIMULATOR_H
