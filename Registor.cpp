//
// Created by 罗皓天 on 2021/6/29.
//

#include "Registor.h"
void Registor::decode() {
    if (fetched_instruct == 0)return;
    else{
        opt = fetched_instruct & 127;
        rd = 0;
        rs1 = 0;
        rs2 = 0;
        switch (opt) {
            case 55:case 23:{
                branch = (fetched_instruct >> 12) & 7;
                rd = (fetched_instruct >> 7) & 31u;
                imme = (fetched_instruct >> 12) << 12;
                break;
            }
            case 111:{
                rd = (fetched_instruct >> 7) & 31;
                imme = 0;
                //branch = (fetched_instruct >> 12) & 7;
                int a = 0 , b = 0 , c = 0 , d = 0;
                a = (fetched_instruct >> 12) & ((1 << 8) - 1);
                b = (fetched_instruct >> 20) & 1;
                c = (fetched_instruct >> 21) & ((1 << 10) - 1);
                d = (fetched_instruct >> 31) & 1;
                imme += (c << 1) + (b << 11) + (a << 12) + (d << 20);
                break;
            }
            case 103:{
                rd = (fetched_instruct >> 7) & 31;
                rs1 = (fetched_instruct >> 15) & 31;
                branch = (fetched_instruct >> 12) & 7;
                imme = (fetched_instruct >> 20);
                break;
            }
            case 99:{
                rs1 = (fetched_instruct >> 15) & 31;
                rs2 = (fetched_instruct >> 20) & 31;
                branch = (fetched_instruct >> 12) & 7;
                int a = 0 , b = 0 , c = 0 , d = 0;
                imme = 0;
                a = (fetched_instruct >> 7) & 1;
                b = (fetched_instruct >> 8) & 15;
                c = (fetched_instruct >> 25) & 63;
                d = (fetched_instruct >> 31) & 1;
                imme += (b << 1) + (c << 5) + (a << 11) + (d << 12);
                break;
            }
            case 3:{
                rd = (fetched_instruct >> 7) & 31;
                rs1 = (fetched_instruct >> 15) & 31;
                imme = (fetched_instruct >> 20);
                branch = (fetched_instruct >> 12) & 7;
                break;
            }
            case 35:{
                rs1 = (fetched_instruct >> 15) & 31;
                rs2 = (fetched_instruct >> 20) & 31;
                branch = (fetched_instruct >> 12) & 7;
                imme = 0;
                int a = 0 , b = 0;
                a = (fetched_instruct >> 7) & 31;
                b = (fetched_instruct >> 25);
                imme = a + (b << 5);
                break;
            }
            case 19:{
                rd = (fetched_instruct >> 7) & 31;
                rs1 = (fetched_instruct >> 15) & 31;
                rs2 = 0;
                branch = (fetched_instruct >> 12) & 7;
                int div = (fetched_instruct >> 12) & 7;
                switch (div) {
                    case 1:case 5:{
                        shamt = (fetched_instruct >> 20) & 31;
                        if (fetched_instruct >> 25 == 0)l_or_r = 0;//0代表srli ， 1代表srai
                        else l_or_r = 1;
                    }
                    default:imme = (fetched_instruct >> 20);
                }
                break;
            }
            case 51:{
                rd = (fetched_instruct >> 7) & 31;
                rs1 = (fetched_instruct >> 15) & 31;
                rs2 = (fetched_instruct >> 20) & 31;
                branch = (fetched_instruct >> 12) & 7;
                if (branch == 0 || branch == 5){
                    if ((fetched_instruct >> 25) == 0)l_or_r = 0;
                    else l_or_r = 1;//0代表add，1代表sub；0代表srl，1代表sra
                }
                break;
            }
//            case 15:{
//                branch = (fetched_instruct >> 12) & 7;
//                if (branch == 0){
//                    succ = (fetched_instruct >> 20) & 31;
//                    pred = (fetched_instruct >> 25) & 31;//不确定，这个操作的pred到底是什么，不一定是5位
//                    l_or_r = 0;//0代表fence，1代表fence.i
//                } else{
//                    succ = 0;
//                    pred = 0;
//                    l_or_r = 1;
//                }
//                break;
//            }
//            case 115:{
//                branch = (fetched_instruct >> 12) & 7;
//                rd = (fetched_instruct >> 7) & 31;
//                rs1 = (fetched_instruct >> 15) & 31;
//                csr = (fetched_instruct >> 20);
//                if (branch == 0){
//                    if (csr == 0)l_or_r = 0;
//                    else l_or_r = 1;//0代表ecall，1代表ebreak
//                }
//                break;
//            }
            default:break;
        }
    }
}