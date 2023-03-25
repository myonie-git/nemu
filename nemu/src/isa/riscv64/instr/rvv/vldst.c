#include <common.h>

#include "../local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>
#include <cpu/inst.h>
#include <isa.h>
#include "vldst.h"

void vld(Decode *s){
//这里直接调用decode得了
    uint32_t i = s->isa.inst.val;
    int nf = 0, mew = 0, mop = 0, vm = 0, lumop = 0, rs1 = 0, rs2 = 0, width = 0, dest = 0;
    word_t src1 = 0, src2 = 0;
    mop = BITS(i, 27, 26);
    width = BITS(i, 14, 12);
    rs1 = BITS(i, 19, 15);
    // Error检测
    //vld loop:
    for(int idx = cpu.vstart; idx < cpu.vl; idx++){
        /*TODO: 对vl + vstart 进行校验*/
        
    }


    /*
    width: 
    000: 8b
    101: 16b
    110: 32b
    111: 64b
    */
   
    return;
}



