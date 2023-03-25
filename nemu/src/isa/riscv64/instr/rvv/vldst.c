#include <common.h>

#include "../local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>
#include <cpu/inst.h>
#include <isa.h>
#include "vldst.h"

//implemented
#define MODE_UNIT 0
#define MODE_INDEXED_UNORDERED 1 
//unimplemented
#define MODE_STRIDED 2  
#define MODE_INDEXED_ORDERED 3 //只有这种方式保证导入的顺序

#define R(i) gpr(i)
#define src1R() do { src1 = R(rs1); } while (0)
#define src2R() do { src2 = R(rs2); } while (0)

void vld(Decode *s){
//这里直接调用decode得了
    uint32_t i = s->isa.inst.val;
    int nf = 0, mew = 0, mop = 0, vm = 0, lumop = 0, rs1 = 0, rs2 = 0, width = 0, dest = 0;
    word_t src1 = 0, src2 = 0;
    uint64_t mask = 0;
    mop = BITS(i, 27, 26);
    width = BITS(i, 14, 12);
    vm = BITS(i, 25, 25);
    rs1 = BITS(i, 19, 15);
    
    // Error检测
    //vld loop:

    switch(mop){
        case MODE_UNIT: lumop = BITS(i, 24, 20); src1R(); break;
        case MODE_STRIDED:  rs2 = BITS(i, 24, 20); src1R(); src2R(); break;
        case MODE_INDEXED_UNORDERED: case MODE_INDEXED_ORDERED: default: assert(0); 
        /*TODO()*/
    }

    switch(vm){
        case true: mask = cpu.p; break;
        case false: mask = -1; break; //设置掩码为全1 
        default: assert(0);
    }
    
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



