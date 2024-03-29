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

#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

const char *vregs[] = {
  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", 
  "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15",
  "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23",
  "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31", 
};

void isa_reg_display() {
  //display rv32 regs
  for(int i = 0; i<32; i++){
    printf("%4s: " FMT_WORD " ", regs[i], cpu.gpr[i]);
    if (i % 4 == 3) {
      printf("\n");
    }
  }
}

word_t isa_reg_str2val(const char *s, bool *success) {
  //int i = 0;
  
  *success = true;
  for(int i = 0; i < 32; i++){
    if(strcmp(regs[i], s) == 0){
      return cpu.gpr[i];
    }
  }

  if(strcmp("pc", s) == 0) {return cpu.pc;}

  *success = false;
  return 0;
}
