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
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <memory/paddr.h>
#include "sdb.h"
#include <utils.h>

int set_watchpoint(char *e);
int delete_watchpoint(int NO);

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();
void list_watchpoint();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  nemu_state.state = NEMU_QUIT;
  return -1;
}

static int cmd_si(char *args){
  char *arg = strtok(NULL, " ");
  if(arg == NULL){
    cpu_exec(1);
  }
    else {
    int n = strtol(arg, NULL, 10);
    printf("si %d\n", n);
    cpu_exec(n);
  }
  return 0;
}

static int cmd_info(char *args){
  char *arg = strtok(NULL, " ");
  if(arg == NULL){
    Log("info r: regs values; info w: watch point state");
  }
  else{
    if(strcmp(arg, "r") == 0){
      isa_reg_display();
    }
    else if(strcmp(arg, "w") == 0){
      list_watchpoint();
    }
  }
  return 0;
}

static int cmd_w(char *args){
  if(args == NULL){
    Log("Usage: w EXPR(for example: w *0x200)");
  }
  else{
    /*TODO: Implement the function of watchpoint*/
    int NO = set_watchpoint(args);
    if(NO != -1) {printf("Watchpoint #%d\n", NO);}
    else {printf("Wrong Expr\n");}
  }
  return 0;
}

static int cmd_x(char *args){
  char *arg1 = strtok(NULL, " ");
  printf("%s", arg1);
  if(arg1 == NULL){
    Log("Usage: x N $EXPR(for example: x 10 $esp)");
  }
  else{
    int n;
    word_t addr;
    int i;
    sscanf(arg1, "%d", &n);
    bool success;
    addr = expr(arg1 + strlen(arg1) + 1, &success);
    if(success){
      for(i = 0; i < n; i++){
        if(i % 4 == 0){
          printf(FMT_WORD ": ", addr);
        }
        printf("0x%08x ", (uint32_t)paddr_read(addr, 4));
        addr += 4;
        if(i % 4 == 3){
          printf("\n");
        }
      }
      printf("\n");
    }
    else { printf("Wrong Expr\n");}
  }
  return 0;
}

static int cmd_p(char *args){
  if(args == NULL){
    Log("Usage: p EXPR(for example: p $a0 + 1)");
  }
  else{
    bool success;
    word_t r = expr(args, &success);
    if(success){
      printf(FMT_WORD "\n", r);
    }
    else{
      printf("Wrong Expr\n");
    }
    return 0;
  }
  return 0;
}

static int cmd_d(char *args){
  char *arg = strtok(NULL, " ");
  if(arg == NULL){
    Log("Usage: d N(for example: d 2)");
  }
  else{
    int NO;
    sscanf(args, "%d", &NO);
    if(delete_watchpoint(NO)){
      printf("Watchpoint #%d delete successfully\n", NO);
    }
    else{
      printf("Watchpoint #%d not found\n", NO);
    }
  }
  return 0;
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Step(usage: si [N]/si)" , cmd_si},
  {"info", "Information(usage: info r: reg values; info w:show watch point state)", cmd_info},
  {"w", "Set watchpoint(usage: w EXPR)", cmd_w},
  {"x", "Scan memory(usage: x N EXPR)", cmd_x},
  {"p", "Evaluate the value of EXPR(usage: p EXPR)", cmd_p},
  {"d", "Delete watchpoint(usage: d N)", cmd_d},
  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
