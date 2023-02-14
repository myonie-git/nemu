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

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char* expr;
  word_t old_expr_val;

  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

WP* new_wp(){
  assert(free_ != NULL);
  WP *p = free_;
  free_ = free_ -> next;
  return p;
}

void free_wp(WP* p){
  free(p->expr);
  p->next = free_;
  free_ = p;
}

int delete_watchpoint(int NO){
  WP *p = head;
  WP *prev = NULL;
  while(p!=NULL){
    if(p->NO == NO){break; };
    prev = p;
    p = p->next;
  }
  if(p == NULL){
    return false;
  }
  if(prev == NULL){//p == head
    head = p->next;
  }
  else{
    prev->next = p->next;
  }
  
  free_wp(p);
  return true;
}

int set_watchpoint(char *e){
  bool success;
  word_t val = expr(e, &success);
  if(!success) return -1;

  WP *p = new_wp();
  p->expr = strdup(e);
  p->old_expr_val = val;
  
  p->next = head;
  head = p;
  return p->NO;
}

/* TODO: Implement the functionality of watchpoint */

int check_watchpoint(){
  WP *p = head;
  while(p!=NULL){
    bool success = false;
    word_t val = expr(p->expr, &success);
    if(!success){
      printf("An error occur on watchpoint #%d\n", p->NO);
      return false; 
    }

    if(val != p->old_expr_val){
      printf("Hit watchpoint #%d : %s, old_val = " FMT_WORD "cur_val = " FMT_WORD "\n", p->NO, p->expr, p->old_expr_val, val);
      p->old_expr_val = val;
      return false;
    }
    p = p->next;
  }
  return true;
}

void list_watchpoint(){
  if(head == NULL){printf("No watchpoints\n"); return;}
  
  WP *p = head;
  while(p != NULL){
    printf("%8d\t%s\t" FMT_WORD "\n", p->NO, p->expr, p->old_expr_val);
    p = p -> next;
  }
return;
}
