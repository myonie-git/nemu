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

int opset_decode_exec(Decode *s){
#define INSTPAT_INST(s) ((s)->isa.inst.val)
#define INSTPAT_MATCH(s, name, type, ... /* execute body */ ) { \
    __VA_ARGS__; \
}
    INSTPAT_START();

    INSTPAT("0000?? ? ????? ????? 111 ????? 00010 11", mvl   , VSET, );
    INSTPAT("0001?? ? ????? ????? 111 ????? 00010 11", mgpr  , VSET, );
    INSTPAT("0010?? ? ????? ????? 111 ????? 00010 11", p     , VSET, );
    INSTPAT("0011?? ? ????? ????? 111 ????? 00010 11", vstart, VSET, );
    INSTPAT("0100?? ? ????? ????? 111 ????? 00010 11", vlen  , VSET, );




    INSTPAT_END();
    return 0;
}