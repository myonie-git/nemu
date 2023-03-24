#include "../local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>
#include <cpu/inst.h>

enum{
  TYPE_V, //VLD & VSD
  TYPE_VS, // VLS VSS
  TYPE_VX, // VLX VSX
  TYPE_N,
};


#define R(i) gpr(i)
#define src1R() do { *src1 = R(rs1); } while (0)
#define src2R() do { *src2 = R(rs2); } while (0)


static void decode_operand_rvv_load(Decode *s, int *nf, int *mew, int *mop, int *vm, int *lumop, int *src1, int *src2, int *width, int *dest, int type){
  uint32_t i = s->isa.inst.val;
  *nf = BITS(i, 31, 29);
  *mew = BITS(i, 28, 28);
  *mop = BITS(i, 27, 26);
  *vm = BITS(i, 25, 25);
  *lumop = BITS(i, 24, 20);
  int rs1 = BITS(i, 19, 15);
  *width = BITS(i, 14, 12);
  *dest = BITS(i, 11, 7);
  switch(type){
    case TYPE_V : break; //get rs1, vd
    case TYPE_VS: break; //get rs1, rs2, vd
    case TYPE_VX: break; //get rs1, vs1, vd
  }

  return ;
}

int decode_exec_rvv_load(Decode *s) {
  //检测是否是rv64v_load_store指令
  int nf = 0, mew = 0, mop = 0, vm = 0, lumop = 0, src2 = 0, src1 = 0, width = 0, dest = 0;


#define INSTPAT_INST(s) ((s)->isa.inst.val)
#define INSTPAT_MATCH(s, name, type, ... /* execute body */ ) { \
  decode_operand_rvv_load(s, &nf, &mew, &mop, &vm, &lumop, &src1, &src2, &width, &dest, concat(TYPE_, type)); \
  __VA_ARGS__ ; \
}
  
  INSTPAT_START(rvv_load);
  INSTPAT("??????? ????? ????? ??? ????? 11011 11", vld    , V, );
  
  INSTPAT_END(rvv_load);

  
  return 0;
}


int decode_exec_rvv_store(Decode *s) {
  //检测是否是rv64v_load_store指令
  return 0;
}

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