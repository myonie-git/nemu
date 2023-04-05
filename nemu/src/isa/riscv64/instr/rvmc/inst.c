#include "../local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>
#include <cpu/inst.h>
#include "decode.h"

#define MODE_MCTYPE_WISTH_8 0
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

typedef struct {
    int src1, src2, dest;
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
    op->src1 = BITS(i, 19, 15);
    op->src2 = BITS(i, 24, 20);
    op->dest = BITS(i, 11, 7);
    switch(type){
        case OPICC: break;
        case OPFCC: break;
        case OPICV: break;
        case OPICX: break;
        case OPICI: break;
        case OPFCV: break;
        case OPFCF: break;
        default: assert(0); break;
    }
}


void madd(Decode *s, int TYPE){
    RV64MC_Operand op;
    decode_operand_rvmc(s, &op, TYPE);

}

void msub(Decode *s, int TYPE){
    RV64MC_Operand op;
}

void mdot(Decode *s, int TYPE){
    
}


