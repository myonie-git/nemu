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

#define MODE_MCTYPE_WIDTH_8 0
#define MODE_MCTYPE_WIDTH_16 1
#define MODE_MCTYPE_WIDTH_32 2 
#define MODE_MCTYPE_WIDTH_64 3
#define MODE_UNSIGNED_MCTYPE_WIDTH_8 4
#define MODE_UNSIGNED_MCTYPE_WIDTH_16 5
#define MODE_UNSIGNED_MCTYPE_WIDTH_32 6
#define MODE_UNSIGNED_MCTYPE_WIDTH_64 7

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
      switch(cpu.mctype){
        case MODE_MCTYPE_WIDTH_8: V(dest)._64[idx] = (int8_t)V(rs2)._8[idx * 8] > (int8_t)tmp ? V(rs2)._64[idx] : tmp;
        case MODE_MCTYPE_WIDTH_16: V(dest)._64[idx] = (int16_t)V(rs2)._16[idx * 4] > (int16_t)tmp ? V(rs2)._64[idx] : tmp;
        case MODE_MCTYPE_WIDTH_32: V(dest)._64[idx] = (int32_t)V(rs2)._32[idx * 2] > (int32_t)tmp ? V(rs2)._64[idx] : tmp;
        case MODE_MCTYPE_WIDTH_64: V(dest)._64[idx] = (int64_t)V(rs2)._64[idx] > (int64_t)tmp ? V(rs2)._64[idx] : tmp;
        case MODE_UNSIGNED_MCTYPE_WIDTH_8: V(dest)._64[idx] = (uint8_t)V(rs2)._8[idx * 8] > (uint8_t)tmp ? V(rs2)._64[idx] : tmp;
        case MODE_UNSIGNED_MCTYPE_WIDTH_16: V(dest)._64[idx] = (uint16_t)V(rs2)._16[idx * 4] > (uint16_t)tmp ? V(rs2)._64[idx] : tmp;
        case MODE_UNSIGNED_MCTYPE_WIDTH_32: V(dest)._64[idx] = (uint32_t)V(rs2)._32[idx * 2] > (uint32_t)tmp ? V(rs2)._64[idx] : tmp;
        case MODE_UNSIGNED_MCTYPE_WIDTH_64: V(dest)._64[idx] = (uint64_t)V(rs2)._64[idx] > (uint64_t)tmp ? V(rs2)._64[idx] : tmp;     
      }
      //V(dest)._64[idx] = V(rs2)._64[idx] > tmp ? V(rs2)._64[idx] : tmp;
      printf("R(rs1): %ld, rs1: %d, rs2: %d, imm:%ld ",R(rs1),rs1, rs2, imm);
      printf("tmp: %d, V2: %d\n", (uint8_t)tmp, (int8_t)V(rs2)._8[idx * 8]);
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