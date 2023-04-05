#include "../local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>
#include <cpu/inst.h>
#include "decode.h"

#define MODE_MCTYPE_WIDTH_8 0
#define MODE_MCTYPE_WIDTH_16 1
#define MODE_MCTYPE_WIDTH_32 2 
#define MODE_MCTYPE_WIDTH_64 3

#define OPICC 0
#define OPFCC 1
#define OPICV 2
#define OPICX 3
#define OPICI 4
#define OPFCV 5
#define OPFCF 6
#define OPSET 7


#define R(i) gpr(i)
#define V(i) vgpr(i)
#define MX(i) mgpr_x(i)
#define MY(i) mgpr_y(i)


#define srcC(name, src) do {name.x = mgpr_x(src); name.y = mgpr_y(src); } while(0)
#define srcV(name, src) do {name = src; } while(0)
#define srcR(name, src) do {name = R(src); } while(0)
#define srcI(name, src) do {name = src; } while(0) //TODO, 可以进一步扩展imm

typedef struct {
    int vdest, vsrc2;
    int vm;
    word_t rs2, imm;
    MC_POSITION_REG m0, m1;
} RV64MC_Operand;

enum{
    TYPE_V,
    TYPE_VSET,
    TYPE_N,
};

static void decode_operand_rvmc(Decode *s, RV64MC_Operand* op, int type){
    uint32_t i = s->isa.inst.val;
        
    int src1 = BITS(i, 19, 15);
    int src2 = BITS(i, 24, 20);

    srcC(op->m0, src1);
    srcV(op->vdest, BITS(i, 11, 7));
    
    op->vm = BITS(i, 25, 25);
    switch(type){
        case OPICC: srcC(op->m1, src2)   ; break;
        case OPFCC: assert(0)            ; break;
        case OPICV: srcV(op->vsrc2, src2); break;
        case OPICX: srcR(op->rs2, src2)  ; break;
        case OPICI: srcI(op->imm, src2)  ; break;
        case OPFCV: assert(0)            ; break;
        case OPFCF: assert(0)            ; break;
        default:    assert(0)            ; break;
    }
}


void madd(Decode *s, int TYPE){
    RV64MC_Operand op;
    decode_operand_rvmc(s, &op, TYPE);
    uint64_t mask = 0;

    switch(op.vm){
        case true: mask = cpu.p; break;
        case false: mask = 0xffffffffffffffff; break;
    }

    if(op.m0.x != op.m1.x) {assert(0); }
    if(op.m0.x + cpu.mvl.xlen >= MCLEN) assert(0);
    if(cpu.mvl.ylen > 1) assert(0);
    
    for(int idx = op.m0.x; idx < cpu.mvl.xlen + op.m0.x; idx++){
        if(!op.vm || BITS(mask, idx, idx)){
            switch(cpu.mctype){
                case MODE_MCTYPE_WIDTH_8: V(op.vdest)._64[idx] = (uint8_t)(cpu.mc[op.m0.x + idx][op.m0.y] + cpu.mc[op.m1.x + idx][op.m1.y]); break;
                case MODE_MCTYPE_WIDTH_16: V(op.vdest)._64[idx] = (uint16_t)(cpu.mc[op.m0.x + idx][op.m0.y] + cpu.mc[op.m1.x + idx][op.m1.y]); break;
                case MODE_MCTYPE_WIDTH_32: V(op.vdest)._64[idx] = (uint32_t)(cpu.mc[op.m0.x + idx][op.m0.y] + cpu.mc[op.m1.x + idx][op.m1.y]); break;
                case MODE_MCTYPE_WIDTH_64: V(op.vdest)._64[idx] = (uint64_t)(cpu.mc[op.m0.x + idx][op.m0.y] + cpu.mc[op.m1.x + idx][op.m1.y]); break;
            }
        }
    }
}

void msub(Decode *s, int TYPE){
    RV64MC_Operand op;
}

void mdot(Decode *s, int TYPE){
    
}


