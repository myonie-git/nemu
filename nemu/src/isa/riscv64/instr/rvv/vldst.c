#include <common.h>

#include "../local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>
#include <cpu/inst.h>
#include "vldst.h"

void vld(Decode *s){
//这里直接调用decode得了
    uint32_t i = s->isa.inst.val;
    int nf = 0, mew = 0, mop = 0, vm = 0, lumop = 0, src2 = 0, src1 = 0, width = 0, dest = 0;

    mop = BITS(i, 27, 26);
    width = BITS(i, 14, 12);


    return;
}



