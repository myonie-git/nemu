/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#ifndef __ISA_RISCV64_H__
#define __ISA_RISCV64_H__

#include <common.h>
#include "../instr/rvv/vreg.h"

typedef struct{
  uint64_t x; //列编号
  uint64_t y; //行编号
} MC_POSITION_REG;

typedef struct{
  uint64_t xlen;
  uint64_t ylen;
} MC_LENGTH_REG;


typedef struct {
  word_t mc[MCLEN][MCLEN];
  word_t gpr[32];
  vaddr_t pc;
  //32*64
  union {
    //uint128_t _128[2048/128];
    uint64_t _64[MCLEN];
    uint32_t _32[MCLEN * 2];
    uint16_t _16[MCLEN * 4];
    uint8_t _8[MCLEN * 8];
    //TODO(): 这里结构还需要更改，具体更改方式取决于union是如何操作的，以及架构是如何定义的
    //TODO(): Float type
  } vgpr[32];

  MC_POSITION_REG mgpr[32]; // 四个忆阻器位置寄存器 // 先按照32个来弄
  MC_LENGTH_REG mvl; //忆阻器长度寄存器
  uint64_t vl; 
  uint64_t vstart; 
  uint64_t p; // 掩码寄存器
  uint64_t mctype; // 元素类型寄存器

} riscv64_CPU_state;


// decode
typedef struct {
  union {
    uint32_t val;
  } inst;
} riscv64_ISADecodeInfo;

#define isa_mmu_check(vaddr, len, type) (MMU_DIRECT)

#endif
