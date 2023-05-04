#include <common.h>

#include "../local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>
#include <cpu/inst.h>
#include <isa.h>
#include "vldst.h"
#include "vreg.h"

//implemented
#define MODE_UNIT 0
#define MODE_INDEXED_UNORDERED 1 
//unimplemented
#define MODE_STRIDED 2  
#define MODE_INDEXED_ORDERED 3 //只有这种方式保证导入的顺序

#define WIDTH_8 0
#define WIDTH_16 5
#define WIDTH_32 6
#define WIDTH_64 7

#define R(i) gpr(i)
#define V(i) vgpr(i)
#define Mr vaddr_read
#define Mw vaddr_write
//#define src1R() do { src1 = R(rs1); } while (0)
//#define src2R() do { src2 = R(rs2); } while (0)

void vld(Decode *s){
    uint32_t i = s->isa.inst.val;
    int nf = 0, mew = 0, mop = 0, vm = 0, lumop = 0, rs1 = 0, rs2 = 0, width = 0, dest = 0;
    word_t src1 = 0;
    int stride = 0;
    int64_t len = 0;
    uint64_t mask = 0;
    mop = BITS(i, 27, 26);
    width = BITS(i, 14, 12);
    vm = BITS(i, 25, 25);
    rs1 = BITS(i, 19, 15);
    src1 = R(rs1);
    mew = BITS(i, 28, 28);
    dest = BITS(i, 11, 7);
    
    assert(!mew);
    // Error检测
    //vld loop:
    switch(mop){
        case MODE_UNIT: lumop = BITS(i, 24, 20); stride = 1;break;
        case MODE_STRIDED:  rs2 = BITS(i, 24, 20); stride = R(rs2); break;
        case MODE_INDEXED_UNORDERED: case MODE_INDEXED_ORDERED: default: assert(0); 
        /*TODO()*/
    }

    switch(width){
        case WIDTH_8: len = 1; break; 
        case WIDTH_16: len = 2; break;
        case WIDTH_32: len = 4; break;
        case WIDTH_64: len = 8; break;
        default: assert(0); break;
    }

    switch(vm){
        case true: mask = cpu.p; break;
        case false: mask = -1; break; //设置掩码为全1 
        default: assert(0);
    }
    //lumop
    //mew
    //
    if(cpu.vstart + cpu.vl > MCLEN) assert(0); 
    /*TODO: 带nfield的校验*/
    int offset = 0;
    for(int idx = cpu.vstart; idx < cpu.vl; idx++){
        
        if(!vm ||BITS(mask, idx, idx)) {
            switch(width){
                case WIDTH_8:  V(dest)._64[idx] = Mr(src1 + offset, 1) + R(lumop); break; //Mr已经完成类型转换了，不用我们进行操纵
                case WIDTH_16: V(dest)._64[idx] = Mr(src1 + offset, 2) + R(lumop); break;
                case WIDTH_32: V(dest)._64[idx] = Mr(src1 + offset, 4) + R(lumop); break;
                case WIDTH_64: V(dest)._64[idx] = Mr(src1 + offset, 8) + R(lumop); break;
            }
        }
        /*TODO: 无符号的导入*/
        /*TODO: 跨符号的导入*/
        //printf("idx: %d, addr: %lx, Load Value: %d\n", idx, src1 + offset, V(dest)._8[idx * 8]);
        offset += len * stride;

    }

    //printf("src1: %lx\n", src1);
    //assert(0);
    return;
}


void vsd(Decode *s){
//这里直接调用decode得了
    uint32_t i = s->isa.inst.val;
    int nf = 0, mew = 0, mop = 0, vm = 0, sumop = 0, rs1 = 0, rs2 = 0, width = 0, dest = 0;
    word_t src1 = 0;
    int stride = 0;
    int64_t len = 0;
    uint64_t mask = 0;
    mop = BITS(i, 27, 26);
    width = BITS(i, 14, 12);
    vm = BITS(i, 25, 25);
    rs1 = BITS(i, 19, 15);
    src1 = R(rs1);
    mew = BITS(i, 28, 28);
    dest = BITS(i, 11, 7);
    
    assert(!mew);
    // Error检测
    //vld loop:
    switch(mop){
        case MODE_UNIT: sumop = BITS(i, 24, 20); stride = 1;break;
        case MODE_STRIDED:  rs2 = BITS(i, 24, 20); stride = R(rs2); break;
        case MODE_INDEXED_UNORDERED: case MODE_INDEXED_ORDERED: default: assert(0); 
        /*TODO()*/
    }
    
    switch(width){
        case WIDTH_8: len = 1; break; 
        case WIDTH_16: len = 2; break;
        case WIDTH_32: len = 4; break;
        case WIDTH_64: len = 8; break;
        default: assert(0); break;
    }

    switch(vm){
        case true: mask = cpu.p; break;
        case false: mask = -1; break; //设置掩码为全1 
        default: assert(0);
    }
    //lumop
    //mew
    //
    if(cpu.vstart + cpu.vl > MCLEN) assert(0); 
    /*TODO: 带nfield的校验*/
    int offset = 0;
    for(int idx = cpu.vstart; idx < cpu.vl; idx++){
        
        if( !vm || BITS(mask, idx, idx)) {
            switch(width){
                case WIDTH_8:  Mw(src1 + offset, 1, V(dest)._8[idx * 8]); break;
                case WIDTH_16: Mw(src1 + offset, 2, V(dest)._16[idx * 4]); break;
                case WIDTH_32: Mw(src1 + offset, 4, V(dest)._32[idx * 2]); break;
                case WIDTH_64: Mw(src1 + offset, 8, V(dest)._64[idx]); break;
            }
            //printf("VSD: idx: %d, res: %ld\n", idx, V(dest)._64[idx]);
        }

        /*TODO: 无符号的导入*/
        /*TODO: 如何验证指令是否合规？*/
        /*TODO: NFIELDS是否要实现*/
        //printf("idx: %d, addr: %lx, Load Value: %d\n", idx, src1 + offset, V(dest)._8[idx]);
        
        offset += len * stride ;
    }
    //printf("len is: %d\n strides is: %d\n", len, stride);
    
    return;
}

