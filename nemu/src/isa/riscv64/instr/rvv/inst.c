#include "../local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>
#include <cpu/inst.h>
#include "inst.h"

#define OPIVV 0
#define OPFVV 1
#define OPMVV 2
#define OPIVI 3
#define OPIVX 4
#define OPFVF 5
#define OPMVX 6


enum{
  TYPE_N,
};


#define R(i) gpr(i)
#define V(i) vgpr(i)

void vgtm(Decode *s){
  uint32_t i = s->isa.inst.val;
  int rs1 = 0, rs2 = 0, funct3 = 0, dest = 0, vm = 0;
  funct3 = BITS(i, 14, 12);
  rs1 = BITS(i, 19, 15);
  rs2 = BITS(i, 24, 20);
  dest = BITS(i, 11, 7);
  vm = BITS(i, 25, 25);
  uint64_t mask = 0;
  uint64_t imm = 0;

  switch(vm){
    case true: mask = cpu.p; break;
    case false: mask = -1; break;
  }
  
  switch(funct3){
    case OPIVV: break;
    case OPIVX: imm = R(rs1); break;
    case OPIVI: imm = SEXT(BITS(i, 19, 15), 5);  break;
    default: assert(0); break;
  }

  for(int idx = cpu.vstart; idx  < cpu.vl; idx++){
    if(!vm || BITS(mask, idx, idx)){
      uint64_t tmp = (funct3 == OPIVV) ? V(rs1)._64[idx] : imm;
      V(dest)._64[idx] = V(rs2)._64[idx] > tmp ? V(rs2)._64[idx] : tmp;
    }
  }

  /*TODO: 对不同类型的值如何设置？ 在vld，vst阶段就进行类型导入*/
  return ;
}

void guv(Decode *s){
  //提供OPIVX以及OPIVI，OPIVV
  //TODO: vs2的空间设定
  uint32_t i = s->isa.inst.val;
  int rs1 = 0, funct3 = 0, dest = 0, vm = 0;
  funct3 = BITS(i, 14, 12);
  rs1 = BITS(i, 19, 15);
  //rs2 = BITS(i, 24, 20);
  dest = BITS(i, 11, 7);
  vm = BITS(i, 25, 25);
  uint64_t mask = 0;

  switch(vm){
    case true: mask = cpu.p; break;
    case false: mask = -1; break;
  }

  for(int idx = cpu.vstart; idx  < cpu.vl; idx++){
    if(!vm || BITS(mask, idx, idx)){
      switch(funct3){
        case OPIVV: V(dest)._64[idx] = V(rs1)._64[idx];break;
        case OPIVX: V(dest)._64[idx] = R(rs1); break;
        case OPIVI: V(dest)._64[idx] = SEXT(BITS(i, 19, 15), 5);  break;
        default: assert(0); break;
      }
    }
  }

  return;
}