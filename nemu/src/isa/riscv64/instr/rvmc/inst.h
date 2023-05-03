#include <common.h>
#ifndef __RVMC_INST_H__
#define __RVMC_INST_H__

void mld(Decode* s, int TYPE);
void madd(Decode *s, int TYPE);
void msub(Decode *s, int TYPE);
void mdot(Decode *s, int TYPE);
void mld(Decode* s, int TYPE);
void mewmul(Decode *s, int TYPE);

#endif
