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
#define src1R() do { *src1 = R(rs1); } while (0)
#define src2R() do { *src2 = R(rs2); } while (0)
/*
vld*

{reg: [
  {bits: 7, name: 0x7, attr: 'VL* unit-stride'}, - 0x07
  {bits: 5, name: 'vd', attr: 'destination of load', type: 2}, 
  {bits: 3, name: 'width'},
  {bits: 5, name: 'rs1', attr: 'base address', type: 4},
  {bits: 5, name: 'lumop'},
  {bits: 1, name: 'vm'},
  {bits: 2, name: 'mop'},
  {bits: 1, name: 'mew'},
  {bits: 3, name: 'nf'},
]}
*/

void vgtm(Decode *s){
  uint32_t i = s->isa.inst.val;
  int rs1 = 0, rs2 = 0, funct3 = 0, dest = 0;
  
  return ;
}