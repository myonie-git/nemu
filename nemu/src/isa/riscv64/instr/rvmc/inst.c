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

enum{
    TYPE_V,
    TYPE_VSET,
    TYPE_N,
};

void madd(Decode *s, int TYPE){
    
}

