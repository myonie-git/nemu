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

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_NUM, TK_NEQ, TK_AND, TK_OR, TK_REG, TK_NEG, TK_PTR

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {"0x[0-9a-fA-F]{1,16}", TK_NUM}, //hex
  {"[0-9]{1,10}", TK_NUM}, //dec
  {"\\$[a-z0-9]{1,31}", TK_REG},
  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"-", '-'},           // sub
  {"\\*", '*'},         // multi
  {"/", '/'},
  {"%", '%'},
  {"==", TK_EQ},        // equal
  {"!=", TK_NEQ},       // not equal
  {"&&", TK_AND},
  {"\\|\\|", TK_OR},
  {"!", '!'},
  {"\\(",'('},
  {"\\)", ')'}
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE: break;
          case TK_NUM:
          case TK_REG: //change into num
            sprintf(tokens[nr_token].str, "%.*s", substr_len, substr_start);
            //assert(substr_len < 32); //stack overflow
            printf("%s\n",tokens[nr_token].str);
          default: 
            tokens[nr_token].type = rules[i].token_type;
            nr_token++; 
            break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}


static int op_priority(int op){
  switch(op){
    case '!': case TK_NEG: case TK_PTR: return 2;
    case '*': case '/': case '%': return 3;
    case '+': case '-': return 4;
    case TK_EQ: case TK_NEQ: return 7;
    case TK_AND: return 11;
    case TK_OR: return 12;
    default: assert(0);
  }
}


static int op_priority_cmp(int op1, int op2){
  return op_priority(op1) - op_priority(op2);
}



static int find_dominated_op(int p, int q, bool *success){
  int brackets_num = 0;
  int dominated_op_position = -1;
  for(int i = p; i <= q; i++){
    switch(tokens[i].type){
      case TK_REG: case TK_NUM: break;
      case '(':
        brackets_num ++;
        break;
      case ')':
        brackets_num --;
        if(brackets_num < 0){
            *success = false;
            return 0;
        }
        break;
      default:
        if(brackets_num == 0){
          //traverse the search to find the highest priority
          if(dominated_op_position == -1){
            dominated_op_position = i;
          }
          else if(op_priority_cmp(tokens[dominated_op_position].type, tokens[i].type) < 0)
          {
            dominated_op_position = i;
          }
          else if(op_priority_cmp(tokens[dominated_op_position].type, tokens[i].type) == 0 && tokens[i].type !='!' && tokens[i].type != TK_NEG && tokens[i].type != TK_PTR){
            //from left to right
            dominated_op_position = i;
          }
        }
        break;
    }
  }
  if(dominated_op_position != -1) *success = true;
  return dominated_op_position;
}



word_t eval(int p, int q, bool *success){
  if(p > q){
    *success = false;
    return 0;
  }
  else if(p == q){
    //single token 
    word_t val;
    switch(tokens[p].type){
      case TK_REG: 
        val = isa_reg_str2val(tokens[p].str + 1, success);
        if(!*success){return 0;}
        break;
      case TK_NUM: 
        val = strtoul(tokens[p].str, NULL, 0);
        break;
      default: assert(0);
    }
    *success = true;
    return val;
  }
  else{
    //multi token
    if(tokens[p].type == '(' && tokens[q].type == ')'){
      return eval(p+1, q-1, success);
    } 
    else{
      //find op
      int dominated_op_position = find_dominated_op(p, q, success);
      if(!*success){return 0;}

      int op_type = tokens[dominated_op_position].type;
      if(op_type == '!' || op_type == TK_NEG || op_type == TK_PTR){
        assert(dominated_op_position != p);
        word_t val = eval(dominated_op_position + 1, q, success);
        if(!*success){return 0;}
        switch(op_type){
          case '!': return !(val);
          case TK_NEG: return -(val);
          case TK_PTR: TODO(); return 0;
        }
     }
      
    }
  }
  return 0;
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    //assert(0);
    *success = false;
    return 0;
  }
  
  /* TODO: Insert codes to evaluate the expression. */
  for(int i = 0; i<nr_token; i++){
    //detect the inv('-') and pointer('*') token
      if(i == 0){
        if(tokens[i].type == '-'){
          tokens[i].type = TK_NEG;
        }
        else if(tokens[i].type == '*'){
          tokens[i].type = TK_PTR;
        }
      }
      else{
        int pre_token = tokens[i-1].type;
        if(tokens[i].type == '-'){
          if(pre_token != ')' && pre_token != TK_NUM && pre_token != TK_REG){
            tokens[i].type = TK_NEG;
          }
        } 
        else if(tokens[i].type == '*'){
          if(pre_token != ')' && pre_token != TK_NUM && pre_token != TK_REG){
            tokens[i].type = TK_PTR;
          }
        }
      }
  }
  return eval(0, nr_token-1, success);
  
}
