//
// Created by 罗皓天 on 2021/6/28.
//

#include "simulator.h"
#include <string>
#include <sstream>
using namespace std;
void simulator::read() {
    RAM.read();
}
void simulator::Hazard_detect() {
    if (ex_result.rd != 0  && ex_result.opt != 3 && ex_result.opt != 99){
        if (ex_result.rd == registor.rs1){
//            registor.reg[registor.rs1] = ex_result.imme;
            forwardCap.rd = registor.rs1;
            forwardCap.imme = ex_result.imme;
        } else if (ex_result.rd == registor.rs2){
//            registor.reg[registor.rs2] = ex_result.imme;
            forwardCap.rd = registor.rs2;
            forwardCap.imme = ex_result.imme;
        }
    }
    else if (mem_data.rd != 0  && mem_data.opt != 3 && mem_data.opt != 99){
        if (mem_data.rd == registor.rs1){
//            registor.reg[registor.rs1] = mem_data.imme;
            forwardCap.rd = registor.rs1;
            forwardCap.imme = mem_data.imme;
        } else if (mem_data.rd == registor.rs2){
//            registor.reg[registor.rs2] = mem_data.imme;
            forwardCap.rd = registor.rs2;
            forwardCap.imme = mem_data.imme;
        }
    }
    if (ex_result.opt == 3 && ex_result.rd != 0){
        if (ex_result.rd == registor.rs1 || ex_result.rd == registor.rs2){
            buBle.IF = true;
            buBle.ID = true;
            buBle.EX = true;
        }
    }
}
void simulator::Hazard_forward() {
    registor.reg[forwardCap.rd] = forwardCap.imme;
    forwardCap.rd = 0;
    forwardCap.imme = 0;
}
void simulator::IF() {
    if (buBle.IF){
        buBle.IF = false;
        registor.fetched_instruct = 0;
        return;
    }
    if (ifEnd.if_end)return;
    registor.fetched_instruct = RAM.mem[registor.pc] + (RAM.mem[registor.pc + 1] << 8) + (RAM.mem[registor.pc + 2] << 16) + (RAM.mem[registor.pc + 3] << 24);
    registor.pc += 4;
    if (registor.fetched_instruct == 0x0ff00513){
        ifEnd.if_end = true;
        idEnd.id_end = true;
    }
}
void simulator::ID() {
    if (buBle.ID){
        buBle.ID = false;
        idEnd.fetched = registor.fetched_instruct;
        buffer.rd = registor.rd;
        buffer.rs1 = registor.rs1;
        buffer.rs2 = registor.rs2;
        registor.rd = 0;
        registor.rs1 = 0;
        registor.rs2 = 0;
        //registor.opt = -1;
        return;
    }
    if (registor.fetched_instruct == 0 && idEnd.fetched == 0){
        registor.opt = -1;
        registor.rd = 0;
        registor.rs1 = 0;
        registor.rs2 = 0;
        return;
    }
//    if (registor.fetched_instruct == 0x0ff00513){
//        //cout << (((unsigned int)registor.reg[10]) & 255u);
//        idEnd.id_end = true;
//        return;
//    }
    if (idEnd.fetched != 0)registor.fetched_instruct = idEnd.fetched;
    registor.decode();
    idEnd.fetched = 0;
    idEnd.pc = registor.pc;
    if (registor.opt == 111 || registor.opt == 103 || registor.opt == 99){
        buBle.IF = true;
        registor.fetched_instruct = 0;
    }
//    cout << "opt= " << registor.opt << " rd= " << registor.rd << " rs1= " << registor.rs1 <<
//    " rs2= " << registor.rs2 << " imme= " << registor.imme << " branch= " << registor.branch <<endl;
//    IF();
}
int simulator::sext(const int &im,const int &maxw) {//maxw 0-based
    if (im < 0)return im;
    int ans = im;
    if (((im >> maxw) & 1) == 0)return ans;
    else{
        for (int i = maxw + 1; i <= 31; ++i) {
            ans += (1 << i);
        }
    }
    return ans;
}
void simulator::EX(int option) {
    if (buBle.EX){
        buBle.EX = false;
        ex_result.opt = -1;
        ex_result.rd = 0;
        return;
    }
    ex_result.ex_end = idEnd.id_end;
    ex_result.opt = registor.opt;
    if (registor.opt == -1 || registor.opt == 0){
        return;
    }
    if (buffer.rd != 0 || buffer.rs1 !=0 || buffer.rs2 != 0){
        registor.rd = buffer.rd;
        registor.rs1 = buffer.rs1;
        registor.rs2 = buffer.rs2;
        buffer.rd = 0;
        buffer.rs1 = 0;
        buffer.rs2 = 0;
    }
    switch (option) {
        case 55:{//lui
            //registor.reg[registor.rd] = registor.imme;
            ex_result.rd = registor.rd;
            ex_result.imme = registor.imme;
            ex_result.pc = idEnd.pc;
            break;
        }
        case 23:{//auipc
//            registor.reg[registor.rd] = pc + sext(registor.imme , 31);
            ex_result.rd = registor.rd;
            ex_result.imme = idEnd.pc + sext(registor.imme , 31);
            ex_result.pc = idEnd.pc;
            break;
        }
        case 111:{//jal
//            registor.reg[registor.rd] = pc + 4;
//            pc += sext(registor.imme , 20);
            if (registor.rd != 0)ex_result.imme = idEnd.pc;
            else ex_result.imme = 0;
            ex_result.pc = idEnd.pc + sext(registor.imme , 20) - 4;
            ex_result.rd = registor.rd;
            registor.pc = ex_result.pc;
            break;
        }
        case 103:{//jalr
//            int t = pc + 4;
//            pc = (registor.reg[registor.rs1] + sext(registor.imme , 11))&~ 1;
//            registor.reg[registor.rd] = t;
            if(registor.rd != 0)ex_result.imme = idEnd.pc;
            else ex_result.imme = 0;
            ex_result.pc = (registor.reg[registor.rs1] + sext(registor.imme , 11))&~ 1;
            ex_result.rd = registor.rd;
            registor.pc = ex_result.pc;
            break;
        }
        case 99:{
            switch (registor.branch) {
                case 0:{//beq
                    if (registor.reg[registor.rs1] == registor.reg[registor.rs2]) //pc += sext(registor.imme , 12);
                    {ex_result.pc = idEnd.pc + sext(registor.imme , 12) - 4;ex_result.pcflag = true;}
                    else {ex_result.pc = idEnd.pc;ex_result.pcflag = false;}
                    break;
                }
                case 1:{//bne
                    if (registor.reg[registor.rs1] != registor.reg[registor.rs2]) //pc += sext(registor.imme , 12);
                    {ex_result.pc = idEnd.pc + sext(registor.imme , 12) - 4;ex_result.pcflag = true;}
                    else {ex_result.pc = idEnd.pc;ex_result.pcflag = false;}
                    break;
                }
                case 4:{//blt
                    if (registor.reg[registor.rs1] < registor.reg[registor.rs2]) //pc += sext(registor.imme , 12);
                    {ex_result.pc = idEnd.pc + sext(registor.imme , 12) - 4;ex_result.pcflag = true;}
                    else {ex_result.pc = idEnd.pc;ex_result.pcflag = false;}
                    break;
                }
                case 5:{//bge
                    if (registor.reg[registor.rs1] >= registor.reg[registor.rs2]) //pc += sext(registor.imme , 12);
                    {ex_result.pc = idEnd.pc + sext(registor.imme , 12) - 4;ex_result.pcflag = true;}
                    else {ex_result.pc = idEnd.pc;ex_result.pcflag = false;}
                    break;
                }
                case 6:{//bltu
                    if ((unsigned int)registor.reg[registor.rs1] < (unsigned int)registor.reg[registor.rs2]) //pc += sext(registor.imme , 12);
                    {ex_result.pc = idEnd.pc + sext(registor.imme , 12) - 4;ex_result.pcflag = true;}
                    else {ex_result.pc = idEnd.pc;ex_result.pcflag = false;}
                    break;
                }
                case 7:{//bgeu
                    if ((unsigned int)registor.reg[registor.rs1] >= (unsigned int)registor.reg[registor.rs2]) //pc += sext(registor.imme , 12);
                    {ex_result.pc = idEnd.pc + sext(registor.imme , 12) - 4;ex_result.pcflag = true;}
                    else {ex_result.pc = idEnd.pc;ex_result.pcflag = false;}
                    break;
                }
                default: {ex_result.pc = idEnd.pc;ex_result.pcflag = false;}
            }
            registor.pc = ex_result.pc;
            break;
        }
        case 3:{
            switch (registor.branch) {
                case 0:{//lb
//                    int pos = registor.reg[registor.rs1] + sext(registor.imme , 11);
//                    int ans = 0;
//                    ans = RAM.mem[pos];
//                    registor.reg[registor.rd] = sext(ans , 7);
                    ex_result.rd = registor.rd;
                    ex_result.imme = registor.reg[registor.rs1] + sext(registor.imme , 11);
                    ex_result.branch = 0;
                    ex_result.pc = idEnd.pc;
                    break;
                }
                case 1:{//lh
//                    int pos = registor.reg[registor.rs1] + sext(registor.imme , 11);
//                    int ans = 0;
//                    ans = RAM.mem[pos] + (RAM.mem[pos + 1] << 8);
//                    registor.reg[registor.rd] = sext(ans , 15);
                    ex_result.rd = registor.rd;
                    ex_result.imme = registor.reg[registor.rs1] + sext(registor.imme , 11);
                    ex_result.branch = 1;
                    ex_result.pc = idEnd.pc;
                    break;
                }
                case 2:{//lw
//                    int pos = registor.reg[registor.rs1] + sext(registor.imme , 11);
//                    int ans = 0;
//                    ans = RAM.mem[pos] + (RAM.mem[pos + 1] << 8) + (RAM.mem[pos + 2] << 16) + (RAM.mem[pos + 3] << 24);
//                    registor.reg[registor.rd] = sext(ans , 31);
                    ex_result.rd = registor.rd;
                    ex_result.imme = registor.reg[registor.rs1] + sext(registor.imme , 11);
                    ex_result.branch = 2;
                    ex_result.pc = idEnd.pc;
                    break;
                }
                case 4:{//lbu
//                    int pos = registor.reg[registor.rs1] + sext(registor.imme , 11);
//                    int ans = 0;
//                    ans = RAM.mem[pos];
//                    registor.reg[registor.rd] = ans;
                    ex_result.rd = registor.rd;
                    ex_result.imme = registor.reg[registor.rs1] + sext(registor.imme , 11);
                    ex_result.branch = 4;
                    ex_result.pc = idEnd.pc;
                    break;
                }
                case 5:{//lhu
//                    int pos = registor.reg[registor.rs1] + sext(registor.imme , 11);
//                    int ans = 0;
//                    ans = RAM.mem[pos] + (RAM.mem[pos + 1] << 8);
//                    registor.reg[registor.rd] = ans;
                    ex_result.rd = registor.rd;
                    ex_result.imme = registor.reg[registor.rs1] + sext(registor.imme , 11);
                    ex_result.branch = 5;
                    ex_result.pc = idEnd.pc;
                    break;
                }
                default:break;
            }
            break;
        }
        case 35:{
            switch (registor.branch) {
                case 0:{//sb
//                    int ans = 0;
//                    ans = registor.reg[registor.rs2] & 255;
//                    int pos = registor.reg[registor.rs1] + sext(registor.imme , 11);
//                    RAM.mem[pos] = ans;
                    ex_result.branch = 0;
                    ex_result.rd = 0;
                    ex_result.imme = registor.reg[registor.rs1] + sext(registor.imme , 11);
                    ex_result.ans = registor.reg[registor.rs2] & 255;//这里rd被我拿来当ans用了
                    ex_result.pc = idEnd.pc;
                    break;
                }
                case 1:{//sh
//                    int pos = registor.reg[registor.rs1] + sext(registor.imme , 11);
//                    int l = registor.reg[registor.rs2] & 255;
//                    int r = (registor.reg[registor.rs2] >> 8) & 255;
//                    RAM.mem[pos] = l;
//                    RAM.mem[pos + 1] = r;
                    ex_result.branch = 1;
                    ex_result.rd = 0;
                    ex_result.imme = registor.reg[registor.rs1] + sext(registor.imme , 11);
                    ex_result.ans = registor.reg[registor.rs2];
                    ex_result.pc = idEnd.pc;
                    break;
                }
                case 2:{//sw
//                    int pos = registor.reg[registor.rs1] + sext(registor.imme , 11);
//                    int a = registor.reg[registor.rs2] & 255;
//                    int b = (registor.reg[registor.rs2] >> 8) & 255;
//                    int c = (registor.reg[registor.rs2] >> 16) & 255;
//                    int d = (registor.reg[registor.rs2] >> 24) & 255;
//                    RAM.mem[pos] = a;
//                    RAM.mem[pos + 1] = b;
//                    RAM.mem[pos + 2] = c;
//                    RAM.mem[pos + 3] = d;
                    ex_result.branch = 2;
                    ex_result.rd = 0;
                    ex_result.imme = registor.reg[registor.rs1] + sext(registor.imme , 11);
                    ex_result.ans = registor.reg[registor.rs2];
                    ex_result.pc = idEnd.pc;
                    break;
                }
                default:break;
            }
            break;
        }
        case 19:{
            ex_result.pc = idEnd.pc;
            switch (registor.branch) {
                case 0:{//addi
                    //registor.reg[registor.rd] = registor.reg[registor.rs1] + sext(registor.imme , 11);
                    ex_result.branch = 0;
                    ex_result.rd = registor.rd;
                    ex_result.imme = registor.reg[registor.rs1] + sext(registor.imme , 11);
                    break;
                }
                case 2:{//slti
                    ex_result.rd = registor.rd;
                    ex_result.branch = 2;
                    if (registor.reg[registor.rs1] < sext(registor.imme , 11))//registor.reg[registor.rd] = 1;
                        ex_result.imme = 1;
                    else ex_result.imme = 0;
                        //registor.reg[registor.rd] = 0;
                    break;
                }
                case 3:{//sltiu
                    ex_result.rd = registor.rd;
                    ex_result.branch = 3;
                    if (registor.reg[registor.rs1] < (unsigned int)sext(registor.imme , 11))//registor.reg[registor.rd] = 1;
                        ex_result.imme = 1;
                    else ex_result.imme = 0;
                        //registor.reg[registor.rd] = 0;
                    break;
                }
                case 4:{//xori
                    ex_result.branch = 4;
                    ex_result.rd = registor.rd;
                    //registor.reg[registor.rd] = registor.reg[registor.rs1] ^ sext(registor.imme , 11);
                    ex_result.imme = registor.reg[registor.rs1] ^ sext(registor.imme , 11);
                    break;
                }
                case 6:{//ori
                    ex_result.branch = 6;
                    ex_result.rd = registor.rd;
                    //registor.reg[registor.rd] = registor.reg[registor.rs1] | sext(registor.imme , 11);
                    ex_result.imme = registor.reg[registor.rs1] ^ sext(registor.imme , 11);
                    break;
                }
                case 7:{//andi
                    ex_result.branch = 7;
                    ex_result.rd = registor.rd;
                    //registor.reg[registor.rd] = registor.reg[registor.rs1] & sext(registor.imme , 11);
                    ex_result.imme = registor.reg[registor.rs1] & sext(registor.imme , 11);
                    break;//hehe 手贱了
                }
                case 1:{//slli
                    ex_result.branch = 1;
                    ex_result.rd = registor.rd;
                    if ((registor.shamt & 32) == 0){
                        //registor.reg[registor.rd] = registor.reg[registor.rs1] << registor.shamt;
                        ex_result.imme = registor.reg[registor.rs1] << registor.shamt;
                    } else{
                        ex_result.imme = registor.reg[registor.rd];//重写一次
                    }
                    break;
                }
                case 5:{//srli
                    ex_result.branch = 5;
                    ex_result.rd = registor.rd;
                    if (registor.l_or_r == 0){
                        if ((registor.shamt & 32) == 0){
                            //registor.reg[registor.rd] = registor.reg[registor.rs1] >> registor.shamt;
                            ex_result.imme = (unsigned int)registor.reg[registor.rs1] >> registor.shamt;
                        } else ex_result.imme = registor.reg[registor.rd];
                    } else{//srai
                        if ((registor.shamt & 32) == 0){
                            //registor.reg[registor.rd] = sext(registor.reg[registor.rs1] >> registor.shamt , 31 - registor.shamt);
                            ex_result.imme = sext((unsigned int)registor.reg[registor.rs1] >> registor.shamt , 31 - registor.shamt);
                        } else ex_result.imme = registor.reg[registor.rd];
                    }
                    break;
                }
                default:break;
            }
            break;
        }
        case 51:{
            ex_result.pc = idEnd.pc;
            switch (registor.branch) {
                case 0:{//add and sub
                    ex_result.branch = 0;
                    ex_result.rd = registor.rd;
                    if (registor.l_or_r == 0)//registor.reg[registor.rd] = registor.reg[registor.rs1] + registor.reg[registor.rs2];
                        ex_result.imme = registor.reg[registor.rs1] + registor.reg[registor.rs2];
                    else ex_result.imme = registor.reg[registor.rs1] - registor.reg[registor.rs2];
                        //registor.reg[registor.rd] = registor.reg[registor.rs1] - registor.reg[registor.rs2];
                    break;
                }
                case 1:{//sll
                    ex_result.branch = 1;
                    ex_result.rd = registor.rd;
                    ex_result.imme = registor.reg[registor.rs1] << (registor.reg[registor.rs2] & 31);
                    //registor.reg[registor.rd] = registor.reg[registor.rs1] << (registor.reg[registor.rs2] & 31);
                    break;
                }
                case 2:{//slt
                    ex_result.branch = 2;
                    ex_result.rd = registor.rd;
                    if (registor.reg[registor.rs1] < registor.reg[registor.rs2])//registor.reg[registor.rd] = 1;
                        ex_result.imme = 1;
                    else ex_result.imme = 0;
                        //registor.reg[registor.rd] = 0;
                    break;
                }
                case 3:{//sltu
                    ex_result.branch = 3;
                    ex_result.rd = registor.rd;
                    if ((unsigned int)registor.reg[registor.rs1] < (unsigned int)registor.reg[registor.rs2])ex_result.imme = 1;
                        //registor.reg[registor.rd] = 1;
                    else ex_result.imme = 0;
                        //registor.reg[registor.rd] = 0;
                    break;
                }
                case 4:{//xor
                    ex_result.branch = 4;
                    ex_result.rd = registor.rd;
                    ex_result.imme = registor.reg[registor.rs1] ^ registor.reg[registor.rs2];
                    //registor.reg[registor.rd] = registor.reg[registor.rs1] ^ registor.reg[registor.rs2];
                    break;
                }
                case 5:{//srl and sra
                    ex_result.branch = 5;
                    ex_result.rd = registor.rd;
                    if (registor.l_or_r == 0)//registor.reg[registor.rd] = registor.reg[registor.rs1] >> (registor.reg[registor.rs2] & 31);
                        ex_result.imme = registor.reg[registor.rs1] >> (registor.reg[registor.rs2] & 31);
                    else ex_result.imme = sext(registor.reg[registor.rs1] >> (registor.reg[registor.rs2] & 31), 31 - (registor.reg[registor.rs2] & 31));
                        //registor.reg[registor.rd] = sext(registor.reg[registor.rs1] >> (registor.reg[registor.rs2] & 31), 31 - (registor.reg[registor.rs2] & 31));
                    break;
                }
                case 6:{//or
                    ex_result.branch = 6;
                    ex_result.rd = registor.rd;
                    //registor.reg[registor.rd] = registor.reg[registor.rs1] | registor.reg[registor.rs2];
                    ex_result.imme = registor.reg[registor.rs1] | registor.reg[registor.rs2];
                    break;
                }
                case 7:{
                    ex_result.branch = 7;
                    ex_result.rd = registor.rd;
                    //registor.reg[registor.rd] = registor.reg[registor.rs1] & registor.reg[registor.rs2];
                    ex_result.imme = registor.reg[registor.rs1] & registor.reg[registor.rs2];
                    break;
                }
                default:break;
            }
            break;
        }
        default:break;
    }
//    ID();
}
void simulator::MEM(int option) {
    mem_data.opt = ex_result.opt;
    mem_data.mem_end = ex_result.ex_end;
    switch (option) {
        case 55:case 23:case 111:case 103:case 19:case 51:{//lui//auipc//jal//jalr
            mem_data.pc = ex_result.pc;
            mem_data.rd = ex_result.rd;
            mem_data.imme = ex_result.imme;
            mem_data.branch = ex_result.branch;
            break;
        }
        case 99:{//b类
            mem_data.pc = ex_result.pc;
            mem_data.pcflag = ex_result.pcflag;
            break;
        }
        case 3:{
            mem_data.pc = ex_result.pc;
            switch (ex_result.branch) {
                case 0:{
                    mem_data.rd = ex_result.rd;
                    mem_data.imme = RAM.mem[ex_result.imme];
                    mem_data.branch = ex_result.branch;
                    break;
                }
                case 1:{
                    mem_data.rd = ex_result.rd;
                    mem_data.imme = RAM.mem[ex_result.imme] + (RAM.mem[ex_result.imme + 1] << 8);
                    mem_data.branch = ex_result.branch;
                    break;
                }
                case 2:{
                    mem_data.rd = ex_result.rd;
                    mem_data.imme = RAM.mem[ex_result.imme] + (RAM.mem[ex_result.imme + 1] << 8) + (RAM.mem[ex_result.imme + 2] << 16) + (RAM.mem[ex_result.imme + 3] << 24);
                    mem_data.branch = ex_result.branch;
                    break;
                }
                case 4:{
                    mem_data.rd = ex_result.rd;
                    mem_data.imme = RAM.mem[ex_result.imme];
                    mem_data.branch = ex_result.branch;
                    break;
                }
                case 5:{
                    mem_data.rd = ex_result.rd;
                    mem_data.imme = RAM.mem[ex_result.imme] + (RAM.mem[ex_result.imme + 1] << 8);
                    mem_data.branch = ex_result.branch;
                    break;
                }
                default:break;
            }
            break;
        }
        case 35:{
            mem_data.pc = ex_result.pc;
            mem_data.branch = ex_result.branch;
            switch (ex_result.branch) {
                case 0:{
                    RAM.mem[ex_result.imme] = ex_result.ans;
                    break;
                }
                case 1:{
                    int l = ex_result.ans & 255;
                    int r = (ex_result.ans >> 8) & 255;
                    RAM.mem[ex_result.imme] = l;
                    RAM.mem[ex_result.imme + 1] = r;
                    break;
                }
                case 2:{
                    int a = ex_result.ans & 255;
                    int b = (ex_result.ans >> 8) & 255;
                    int c = (ex_result.ans >> 16) & 255;
                    int d = (ex_result.ans >> 24) & 255;
                    RAM.mem[ex_result.imme] = a;
                    RAM.mem[ex_result.imme + 1] = b;
                    RAM.mem[ex_result.imme + 2] = c;
                    RAM.mem[ex_result.imme + 3] = d;
                    break;
                }
                default:break;
            }
            break;
        }
        default:break;
    }
//    EX(registor.opt);
}
void simulator::WB(int option) {
    //mem_data.opt = ex_result.opt;
    wbEnd.wb_end = mem_data.mem_end;
    switch (option) {
        case 55:case 23:case 19:case 51:{//lui//auipc//jal
            registor.reg[mem_data.rd] = mem_data.imme;
            break;
        }
        case 111:case 103:{
            registor.reg[mem_data.rd] = mem_data.imme;
            //registor.pc = mem_data.pc;
            break;
        }
        case 99:{
            if (mem_data.pcflag){mem_data.pcflag = false;}
            break;
        }
        case 3:{
            switch (mem_data.branch) {
                case 0:{
                    registor.reg[mem_data.rd] = sext(mem_data.imme , 7);
                    break;
                }
                case 1:{
                    registor.reg[mem_data.rd] = sext(mem_data.imme , 15);
                    break;
                }
                case 2:{
                    registor.reg[mem_data.rd] = sext(mem_data.imme , 31);
                    break;
                }
                case 4:{
                    registor.reg[mem_data.rd] = mem_data.imme;
                    break;
                }
                case 5:{
                    registor.reg[mem_data.rd] = mem_data.imme;
                    break;
                }
                default:break;
            }
        }
        case 35:{
            break;
        }
        default:break;
    }
    if (option != 0 && option != -1){
//        cout << j++ << ' ' << "pc = " << mem_data.pc << ' ' << "loa=" << registor.l_or_r << ' ' << mem_data.opt << ' ' << mem_data.branch << ' ' << ' ' << mem_data.rd << ' '  << endl;
//        for (int i = 0; i < 32; ++i) {
//            cout << "reg[" << i << "]=" << registor.reg[i] << endl;
//        }
        j++;
    }
    if (wbEnd.wb_end)throw 1;
//    MEM(ex_result.opt);
}
//void simulator::IF_ID() {
//    ID();
//    IF();
//}
//void simulator::ID_EX() {
//    EX(registor.opt);
//    ID();
//}
//void simulator::EX_MEM() {
//    MEM(ex_result.opt);
//    EX(registor.opt);
//}
//void simulator::MEM_WB() {
//    WB(mem_data.opt);
//    MEM(ex_result.opt);
//}
void simulator::run() {
//    while (true){
//    IF();
//    ID();
//    EX(registor.opt);
//    MEM(ex_result.opt);
//    while (!end)WB(mem_data.opt);
    while (true){
        try {
            WB(mem_data.opt);
            Hazard_forward();
            MEM(ex_result.opt);
            EX(registor.opt);
            ID();
            IF();
            Hazard_detect();
        } catch (...) {
            cout << (unsigned int)(registor.reg[10] & (0b11111111u)) << endl;
            break;
        }
//        MEM_WB();
//        EX_MEM();
//        ID_EX();
//        IF_ID();
    }
//    WB(mem_data.opt);
//    MEM(ex_result.opt);
//    EX(registor.opt);
//    WB(mem_data.opt);
//    MEM(ex_result.opt);
//    WB(mem_data.opt);

//        ID();
//        if (registor.fetched_instruct == 0x0ff00513){
//            cout << (((unsigned int)registor.reg[10]) & 255u);
//            break;
//        }
//        EX(registor.opt);
//        MEM(registor.opt);
//        WB(registor.opt);
//    }
}