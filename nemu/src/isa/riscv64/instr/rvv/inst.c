#include "../local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>
#include <cpu/inst.h>


int decode_exec_rvv_load(Decode *s) {
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