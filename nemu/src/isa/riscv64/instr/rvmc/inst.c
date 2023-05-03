#include "../local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>
#include <cpu/inst.h>
#include "decode.h"
#include "inst.h"

#define MODE_MCTYPE_WIDTH_8 0
#define MODE_MCTYPE_WIDTH_16 1
#define MODE_MCTYPE_WIDTH_32 2 
#define MODE_MCTYPE_WIDTH_64 3
#define MODE_UNSIGNED_MCTYPE_WIDTH_8 4
#define MODE_UNSIGNED_MCTYPE_WIDTH_16 5
#define MODE_UNSIGNED_MCTYPE_WIDTH_32 6
#define MODE_UNSIGNED_MCTYPE_WIDTH_64 7

#define OPICC 0
#define OPFCC 1
#define OPICV 2
#define OPICX 3
#define OPICI 4
#define OPFCV 5
#define OPFCF 6
#define OPSET 7


#define R(i) gpr(i)
#define V(i) vgpr(i)
#define MX(i) mgpr_x(i)
#define MY(i) mgpr_y(i)


#define srcC(name, src) do {name.x = mgpr_x(src); name.y = mgpr_y(src); } while(0)
#define srcV(name, src) do {name = src; } while(0)
#define srcR(name, src) do {name = R(src); } while(0)
#define srcI(name, src) do {name = src; } while(0) //TODO, 可以进一步扩展imm

typedef struct {
    int vdest, vsrc2;
    int vm;
    int vm_col;
    word_t rs2, imm;
    MC_POSITION_REG m0, m1;
} RV64MC_Operand;

enum{
    TYPE_V,
    TYPE_VSET,
    TYPE_N,
};

static void decode_operand_rvmc(Decode *s, RV64MC_Operand* op, int type){
    uint32_t i = s->isa.inst.val;
        
    int src1 = BITS(i, 19, 15);
    int src2 = BITS(i, 24, 20);

    srcC(op->m0, src1);
    srcV(op->vdest, BITS(i, 11, 7));
    
    op->vm_col = BITS(i, 26, 26);
    op->vm = BITS(i, 25, 25);
    switch(type){
        case OPICC: srcC(op->m1, src2)   ; break;
        case OPFCC: assert(0)            ; break;
        case OPICV: srcV(op->vsrc2, src2); break;
        case OPICX: srcR(op->rs2, src2)  ; break;
        case OPICI: srcI(op->imm, src2)  ; break;
        case OPFCV: assert(0)            ; break;
        case OPFCF: assert(0)            ; break;
        default:    assert(0)            ; break;
    }
}


void madd(Decode *s, int TYPE){
    RV64MC_Operand op;
    decode_operand_rvmc(s, &op, TYPE);
    uint64_t mask = 0, mask_col = 0;

    switch(op.vm){
        case true: mask = cpu.p; break;
        case false: mask = 0xffffffffffffffff; break;
    }

    switch(op.vm_col){
        case true: mask_col = cpu.colp; break;
        case false: mask_col = 0xffffffffffffffff; break;
    }

    if(op.m0.x != op.m1.x) {assert(0); }
    if(op.m0.x + cpu.mvl.xlen >= MCLEN) assert(0);
    if(cpu.mvl.ylen > 1) assert(0);
    
    for(int idx = op.m0.x; idx < cpu.mvl.xlen + op.m0.x; idx++){
        if(!op.vm_col || BITS(mask_col, idx, idx)){
            /*TODO: 这里好像可以删掉 !op.vm_col*/
            switch(cpu.mctype){
                case MODE_MCTYPE_WIDTH_8: V(op.vdest)._64[idx] = (int8_t)(cpu.mc[op.m0.x + idx][op.m0.y] + cpu.mc[op.m1.x + idx][op.m1.y]); break;
                case MODE_MCTYPE_WIDTH_16: V(op.vdest)._64[idx] = (int16_t)(cpu.mc[op.m0.x + idx][op.m0.y] + cpu.mc[op.m1.x + idx][op.m1.y]); break;
                case MODE_MCTYPE_WIDTH_32: V(op.vdest)._64[idx] = (int32_t)(cpu.mc[op.m0.x + idx][op.m0.y] + cpu.mc[op.m1.x + idx][op.m1.y]); break;
                case MODE_MCTYPE_WIDTH_64: V(op.vdest)._64[idx] = (int64_t)(cpu.mc[op.m0.x + idx][op.m0.y] + cpu.mc[op.m1.x + idx][op.m1.y]); break;
                case MODE_UNSIGNED_MCTYPE_WIDTH_8: V(op.vdest)._64[idx] = (uint8_t)(cpu.mc[op.m0.x + idx][op.m0.y] + cpu.mc[op.m1.x + idx][op.m1.y]); break;
                case MODE_UNSIGNED_MCTYPE_WIDTH_16: V(op.vdest)._64[idx] = (uint16_t)(cpu.mc[op.m0.x + idx][op.m0.y] + cpu.mc[op.m1.x + idx][op.m1.y]); break;
                case MODE_UNSIGNED_MCTYPE_WIDTH_32: V(op.vdest)._64[idx] = (uint32_t)(cpu.mc[op.m0.x + idx][op.m0.y] + cpu.mc[op.m1.x + idx][op.m1.y]); break;
                case MODE_UNSIGNED_MCTYPE_WIDTH_64: V(op.vdest)._64[idx] = (uint64_t)(cpu.mc[op.m0.x + idx][op.m0.y] + cpu.mc[op.m1.x + idx][op.m1.y]); break;        
            }
        }
    }
}

void msub(Decode *s, int TYPE){
    RV64MC_Operand op;
    decode_operand_rvmc(s, &op, TYPE);
    uint64_t mask = 0, mask_col = 0;

    switch(op.vm){
        case true: mask = cpu.p; break;
        case false: mask = 0xffffffffffffffff; break;
    }

    switch(op.vm_col){
        case true: mask_col = cpu.colp; break;
        case false: mask_col = 0xffffffffffffffff; break;
    }

    if(op.m0.x != op.m1.x) {assert(0); }
    if(op.m0.x + cpu.mvl.xlen > MCLEN) assert(0);
    if(cpu.mvl.ylen > 1) assert(0);
    
    for(int idx = op.m0.x; idx < cpu.mvl.xlen + op.m0.x; idx++){
        if(!op.vm_col || BITS(mask_col, idx, idx)){
            switch(cpu.mctype){
                case MODE_MCTYPE_WIDTH_8: V(op.vdest)._64[idx] = (int8_t)(cpu.mc[op.m0.x + idx][op.m0.y] - cpu.mc[op.m1.x + idx][op.m1.y]); break;
                case MODE_MCTYPE_WIDTH_16: V(op.vdest)._64[idx] = (int16_t)(cpu.mc[op.m0.x + idx][op.m0.y] - cpu.mc[op.m1.x + idx][op.m1.y]); break;
                case MODE_MCTYPE_WIDTH_32: V(op.vdest)._64[idx] = (int32_t)(cpu.mc[op.m0.x + idx][op.m0.y] - cpu.mc[op.m1.x + idx][op.m1.y]); break;
                case MODE_MCTYPE_WIDTH_64: V(op.vdest)._64[idx] = (int64_t)(cpu.mc[op.m0.x + idx][op.m0.y] - cpu.mc[op.m1.x + idx][op.m1.y]); break;
                case MODE_UNSIGNED_MCTYPE_WIDTH_8: V(op.vdest)._64[idx] = (uint8_t)(cpu.mc[op.m0.x + idx][op.m0.y] - cpu.mc[op.m1.x + idx][op.m1.y]); break;
                case MODE_UNSIGNED_MCTYPE_WIDTH_16: V(op.vdest)._64[idx] = (uint16_t)(cpu.mc[op.m0.x + idx][op.m0.y] - cpu.mc[op.m1.x + idx][op.m1.y]); break;
                case MODE_UNSIGNED_MCTYPE_WIDTH_32: V(op.vdest)._64[idx] = (uint32_t)(cpu.mc[op.m0.x + idx][op.m0.y] - cpu.mc[op.m1.x + idx][op.m1.y]); break;
                case MODE_UNSIGNED_MCTYPE_WIDTH_64: V(op.vdest)._64[idx] = (uint64_t)(cpu.mc[op.m0.x + idx][op.m0.y] - cpu.mc[op.m1.x + idx][op.m1.y]); break;
            
            }
        }
    }
}

void mdot(Decode *s, int TYPE){
    RV64MC_Operand op;
    decode_operand_rvmc(s, &op, TYPE);

    uint64_t mask = 0, mask_col = 0;
    switch(op.vm){
        case true: mask = cpu.p; break;
        case false: mask = 0xffffffffffffffff; break;
    }

    switch(op.vm_col){
        case true: mask_col = cpu.colp; break;
        case false: mask_col = 0xffffffffffffffff; break;
    }

    if(op.m0.x + cpu.mvl.xlen > MCLEN) assert(0);
    if(op.m0.y + cpu.mvl.ylen > MCLEN) assert(0);
    
    uint64_t tmp = 0;

    for(int idx = op.m0.x; idx < cpu.mvl.xlen + op.m0.x; idx++){
        if(!op.vm_col || BITS(mask_col, idx, idx)){
            V(op.vdest)._64[idx] = 0;
            for(int idy = op.m0.y; idy < cpu.mvl.ylen + op.m0.y; idy++){
                if(!op.vm || BITS(mask, idy, idy));
                switch(TYPE){
                    case OPICV: tmp = V(op.vsrc2)._64[idx]; break;
                    case OPICX: tmp = op.rs2              ; break;
                    case OPICI: tmp = op.imm              ; break;
                }
                switch(cpu.mctype){
                    case MODE_MCTYPE_WIDTH_8:  V(op.vdest)._64[idx] += (int8_t)((int8_t)cpu.mc[idx][idy] * (int8_t)tmp); break;
                    case MODE_MCTYPE_WIDTH_16: V(op.vdest)._64[idx] += (int16_t)((int16_t)cpu.mc[idx][idy] * (int16_t)tmp); break;
                    case MODE_MCTYPE_WIDTH_32: V(op.vdest)._64[idx] += (int32_t)((int32_t)cpu.mc[idx][idy] * (int32_t)tmp); break;
                    case MODE_MCTYPE_WIDTH_64: V(op.vdest)._64[idx] += (int64_t)((int64_t)cpu.mc[idx][idy] * (int64_t)tmp); break;
                    case MODE_UNSIGNED_MCTYPE_WIDTH_8:  V(op.vdest)._64[idx] += (uint8_t)((uint8_t)cpu.mc[idx][idy] * (uint8_t)tmp); break;
                    case MODE_UNSIGNED_MCTYPE_WIDTH_16: V(op.vdest)._64[idx] += (uint16_t)((uint16_t)cpu.mc[idx][idy] * (uint16_t)tmp); break;
                    case MODE_UNSIGNED_MCTYPE_WIDTH_32: V(op.vdest)._64[idx] += (uint32_t)((uint32_t)cpu.mc[idx][idy] * (uint32_t)tmp); break;
                    case MODE_UNSIGNED_MCTYPE_WIDTH_64: V(op.vdest)._64[idx] += (uint64_t)((uint64_t)cpu.mc[idx][idy] * (uint64_t)tmp); break;
                }
                printf("idx: %d, mc: %d, tmp: %d, res: %d\n",idx, cpu.mc[idx][idy], tmp, V(op.vdest)._64[idx]);
            }
            assert(0);
        }
    }
}


void mld(Decode* s, int TYPE){
    //printf("mld\n");
    //assert(0);
    RV64MC_Operand op;
    decode_operand_rvmc(s, &op, TYPE);
    uint64_t mask = 0;
    switch(op.vm){
        case true: mask = cpu.p; break;
        case false: mask = 0xffffffffffffffff; break;
    }

    if(op.m0.x + cpu.mvl.xlen > MCLEN) assert(0);
    if(op.m0.y + cpu.mvl.ylen > MCLEN) assert(0);
    if(cpu.mvl.xlen > 1) assert(0);

    uint64_t tmp = 0;

    for(int idy = op.m0.y; idy < cpu.mvl.ylen + op.m0.y; idy++){
        if(!op.vm || BITS(mask, idy, idy)){
            switch(TYPE){
                case OPICV: tmp = V(op.vsrc2)._64[idy]; break;
                case OPICX: tmp = op.rs2              ; break;
                case OPICI: tmp = op.imm              ; break;
            }
            switch(cpu.mctype){
                case MODE_MCTYPE_WIDTH_8: cpu.mc[op.m0.x][idy] = (word_t)((int8_t)tmp) + R(op.vdest); break;
                case MODE_MCTYPE_WIDTH_16: cpu.mc[op.m0.x][idy] = (word_t)((int16_t)tmp) + R(op.vdest); break;
                case MODE_MCTYPE_WIDTH_32: cpu.mc[op.m0.x][idy] = (word_t)((int32_t)tmp) + R(op.vdest); break;
                case MODE_MCTYPE_WIDTH_64: cpu.mc[op.m0.x][idy] = (word_t)((int64_t)tmp) + R(op.vdest); break;
                case MODE_UNSIGNED_MCTYPE_WIDTH_8: cpu.mc[op.m0.x][idy] = (word_t)((uint8_t)tmp) + R(op.vdest); break;
                case MODE_UNSIGNED_MCTYPE_WIDTH_16: cpu.mc[op.m0.x][idy] = (word_t)((uint16_t)tmp) + R(op.vdest); break;
                case MODE_UNSIGNED_MCTYPE_WIDTH_32: cpu.mc[op.m0.x][idy] = (word_t)((uint32_t)tmp) + R(op.vdest); break;
                case MODE_UNSIGNED_MCTYPE_WIDTH_64: cpu.mc[op.m0.x][idy] = (word_t)((uint64_t)tmp) + R(op.vdest); break;
            
            }
        }
        printf("cpu.mc[%ld][%d]: %d\n", op.m0.x, idy, (int8_t)cpu.mc[op.m0.x][idy]);
    }



}

void mewmul(Decode *s, int TYPE){
    RV64MC_Operand op;
    decode_operand_rvmc(s, &op, TYPE);
    uint64_t mask = 0, mask_col = 0; 
    switch(op.vm){
        case true: mask = cpu.p; break;
        case false: mask = 0xffffffffffffffff; break;
    }
    switch(op.vm_col){
        case true: mask_col = cpu.colp; break;
        case false: mask_col = 0xffffffffffffffff; break;
    }
    
    if(op.m0.x + cpu.mvl.xlen > MCLEN) assert(0);
    if(op.m0.y + cpu.mvl.ylen > MCLEN) assert(0);
    if(cpu.mvl.ylen > 1) assert(0);
    uint64_t tmp = 0;
    switch(TYPE){
        case OPICV: tmp = V(op.vsrc2)._64[op.m0.y]; break;
        case OPICX: tmp = op.rs2              ; break;
        case OPICI: tmp = op.imm              ; break;
    }
    for(int idx = op.m0.x; idx < op.m0.x + cpu.mvl.xlen; idx++){
        if(!op.vm_col || BITS(mask_col, idx, idx)){
            switch(cpu.mctype){
                case MODE_MCTYPE_WIDTH_8 : V(op.vdest)._64[idx] = (int8_t)((int8_t)tmp * (int8_t)cpu.mc[idx][op.m0.y]); break;
                case MODE_MCTYPE_WIDTH_16: V(op.vdest)._64[idx] = (int16_t)((int16_t)tmp * (int16_t)cpu.mc[idx][op.m0.y]); break;
                case MODE_MCTYPE_WIDTH_32: V(op.vdest)._64[idx] = (int32_t)((int32_t)tmp * (int32_t)cpu.mc[idx][op.m0.y]); break;
                case MODE_MCTYPE_WIDTH_64: V(op.vdest)._64[idx] = (int64_t)((int64_t)tmp * (int64_t)cpu.mc[idx][op.m0.y]); break;
                case MODE_UNSIGNED_MCTYPE_WIDTH_8 : V(op.vdest)._64[idx] = (uint8_t)((uint8_t)tmp * (uint8_t)cpu.mc[idx][op.m0.y]); break;
                case MODE_UNSIGNED_MCTYPE_WIDTH_16: V(op.vdest)._64[idx] = (uint16_t)((uint16_t)tmp * (uint16_t)cpu.mc[idx][op.m0.y]); break;
                case MODE_UNSIGNED_MCTYPE_WIDTH_32: V(op.vdest)._64[idx] = (uint32_t)((uint32_t)tmp * (uint32_t)cpu.mc[idx][op.m0.y]); break;
                case MODE_UNSIGNED_MCTYPE_WIDTH_64: V(op.vdest)._64[idx] = (uint64_t)((uint64_t)tmp * (uint64_t)cpu.mc[idx][op.m0.y]); break;
            }
        }
    }
}