#include "../local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>
#include <cpu/inst.h>
#include "decode.h"
#include "opset.h"
#include "inst.h"

#define MODE_MCTYPE_WIDTH_8 0
#define MODE_MCTYPE_WIDTH_16 1
#define MODE_MCTYPE_WIDTH_32 2
#define MODE_MCTYPE_WIDTH_64 3
#define MODE_UNSIGNED_MCTYPE_WIDTH_8 4
#define MODE_UNSIGNED_MCTYPE_WIDTH_16 5
#define MODE_UNSIGNED_MCTYPE_WIDTH_32 6
#define MODE_UNSIGNED_MCTYPE_WIDTH_64 7

#define OPICC 0
#define OPFCC 1
#define OPICV 2
#define OPICX 3
#define OPICI 4
#define OPFCV 5
#define OPFCF 6
#define OPSET 7

#define INT_MODE 0
#define REG_MODE 1

#define R(i) gpr(i)
#define MX(i) mgpr_x(i)
#define MY(i) mgpr_y(i)

static void decode_operand_opset(Decode *s, uint64_t *tmp1, uint64_t* tmp2, int *dest){
    uint32_t i = s->isa.inst.val;
    *dest = BITS(i, 11, 7);
    int src1 = BITS(i, 19, 15);
    int src2 = BITS(i, 24, 20);
    switch(BITS(i, 27, 27)){
        case INT_MODE: *tmp1 = src1; break;
        case REG_MODE: *tmp1 = R(src1); break;
        default: assert(0); break;
    }
    switch(BITS(i, 26, 26)){
        case INT_MODE: *tmp2 = src2; break;
        case REG_MODE: *tmp2 = R(src2); break;
        default: assert(0); break;
    }
    return ;
}

int opset_decode_exec(Decode *s){
    word_t tmp1 = 0, tmp2 = 0;
    int dest = 0;
#define INSTPAT_INST(s) ((s)->isa.inst.val)
#define INSTPAT_MATCH(s, name, type, ... /* execute body */ ) { \
    decode_operand_opset(s, &tmp1, &tmp2, &dest);  \
    __VA_ARGS__; \
}
    INSTPAT_START();
    INSTPAT("0000?? ? ????? ????? 111 ????? 00010 11", mvl   , VSET, cpu.mvl.xlen = tmp1; cpu.mvl.ylen = tmp2; printf("mvl.xlen is set to: %d, mvl.ylen is set to: %d\n", tmp1, tmp2););
    INSTPAT("0001?? ? ????? ????? 111 ????? 00010 11", mgpr  , VSET, mgpr_x(dest) = tmp1; mgpr_y(dest) = tmp2; printf("mgpr[%d] x is set t0: %d, y is set to: %d\n", tmp1, tmp2););
    INSTPAT("0010?? ? ????? ????? 111 ????? 00010 11", p     , VSET, cpu.p = tmp1; printf("p is set to: %lx\n", tmp1););
    INSTPAT("0011?? ? ????? ????? 111 ????? 00010 11", vstart, VSET, cpu.vstart = tmp1; printf("vstart is set to: %lx\n", tmp1););
    INSTPAT("0100?? ? ????? ????? 111 ????? 00010 11", vlen  , VSET, cpu.vl = tmp1; printf("vl is set to: %lx\n", tmp1););
    INSTPAT("0101?? ? ????? ????? 111 ????? 00010 11", mctype, VSET, cpu.mctype = tmp1; printf("mctype is set to %d\n", tmp1););
    INSTPAT("0110?? ? ????? ????? 111 ????? 00010 11", colp  , VSET, cpu.colp = tmp1; printf("colp is set to %lx\n", tmp1););
    INSTPAT_END();
    return 0;
}