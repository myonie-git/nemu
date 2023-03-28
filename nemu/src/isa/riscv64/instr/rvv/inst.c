#include "../local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>
#include <cpu/inst.h>
#include "vldst.h"

enum{
  TYPE_N,
};


#define R(i) gpr(i)
#define V(i) vgpr(i)

void vgtm(Decode *s){
  uint32_t i = s->isa.inst.val;
  int rs1 = 0, rs2 = 0, funct3 = 0, dest = 0;
  funct3 = BITS(i, 14, 12);
  
  return ;
}