#include "../local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>
#include <cpu/inst.h>
#include "decode.h"
#include "opset.h"

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

/*
提供以下七种funct3编码，最后一种保留
OPICC: 000
OPFCC: 001
OPICV: 010
OPICX: 011
OPICI: 100
OPFCV: 101
OPFCF: 110
Others: 用于设置相关寄存器
*/

#define R(i) gpr(i)
#define V(i) vgpr(i)

enum {
    TYPE_V,
    TYPE_VSET,
    TYPE_N,
};



int rvmc_decode_exec(Decode *s){

#define INSTPAT_INST(s) ((s)->isa.inst.val)
#define INSTPAT_MATCH(s, name, type, ... /* execute body */ ) { \
  /*decode_operand(s, &dest, &src1, &src2, &imm, concat(TYPE_, type)); */ \
  __VA_ARGS__ ; \
}

    //printf("rvmc\n");
    INSTPAT_START();
    /*OPICC and OPICF*/

    INSTPAT("00000? ? ????? ????? 000 ????? 00010 11", madd   , V, );
    INSTPAT("00000? ? ????? ????? 001 ????? 00010 11", madd   , V, );

    INSTPAT("00001? ? ????? ????? 000 ????? 00010 11", msub   , V, );
    INSTPAT("00001? ? ????? ????? 001 ????? 00010 11", msub   , V, );

    /*OPICV、OPICVI、OPICX*/

    INSTPAT("000000 ? ????? ????? 010 ????? 00010 11", mdot   , V, );
    INSTPAT("000000 ? ????? ????? 011 ????? 00010 11", mdot   , V, );
    INSTPAT("000000 ? ????? ????? 100 ????? 00010 11", mdot   , V, );

    INSTPAT("000001 ? ????? ????? 010 ????? 00010 11", mewmul   , V, );
    INSTPAT("000001 ? ????? ????? 011 ????? 00010 11", mewmul   , V, );
    INSTPAT("000001 ? ????? ????? 100 ????? 00010 11", mewmul   , V, );

    INSTPAT("000010 ? ????? ????? 010 ????? 00010 11", mld   , V, );
    INSTPAT("000010 ? ????? ????? 011 ????? 00010 11", mld   , V, );
    INSTPAT("000010 ? ????? ????? 100 ????? 00010 11", mld   , V, );

    /*OPSET*/
    
    INSTPAT("?????? ? ????? ????? 111 ????? 00010 11", mvl   , VSET, opset_decode_exec(s));

    INSTPAT("??????? ????? ????? ??? ????? ????? ??", inv    , N, INV(s->pc));
    INSTPAT_END();

    return 0;

}
//仍然保留rv64v的基本指令集架构

