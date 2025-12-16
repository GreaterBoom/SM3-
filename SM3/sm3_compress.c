/*************************************************************************
 > File Name: sm3_compress.c
 > Description: SM3算法压缩函数模块（符合GM/T 0004-2012标准）
 > 核心功能：初始IV设置、64轮迭代计算、状态更新
 ************************************************************************/
#include "sm3_common.h"

/* 
 * 函数名：sm3_compress_init
 * 功能：初始化SM3上下文，设置标准初始链接变量IV（GM/T 0004-2012规定）
 * IV值（十六进制）：7380166f, 4914b2b9, 172442d7, da8a0600, a96f30bc, 163138aa, e38dee4d, b0fb0e4e
 */
void sm3_compress_init(sm3_context* ctx) {
    // 初始化状态变量（IV）
    ctx->state[0] = 0x7380166F;
    ctx->state[1] = 0x4914B2B9;
    ctx->state[2] = 0x172442D7;
    ctx->state[3] = 0xDA8A0600;
    ctx->state[4] = 0xA96F30BC;
    ctx->state[5] = 0x163138AA;
    ctx->state[6] = 0xE38DEE4D;
    ctx->state[7] = 0xB0FB0E4E;

    // 初始化已处理数据长度为0
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    // 清空缓冲区
    memset(ctx->buffer, 0, sizeof(ctx->buffer));
}

/* 
 * 函数名：sm3_compress_block
 * 功能：对单个512bit分组执行压缩函数，完成64轮迭代并更新上下文状态
 * 参数：ctx-SM3上下文，block-512bit分组数据（64字节）
 * 注意：变量更新为并行操作，需使用原始值计算，避免覆盖
 */
void sm3_compress_block(sm3_context* ctx, const uint8_t* block) {
    uint32_t A, B, C, D, E, F, G, H;
    uint32_t SS1, SS2, TT1, TT2;
    uint32_t W[68], W1[64];
    int j;

    // 步骤1：分组扩展（生成W和W'数组）
    sm3_group_expand(block, W, W1);

    // 步骤2：初始化8个中间变量（从上下文状态读取）
    A = ctx->state[0];
    B = ctx->state[1];
    C = ctx->state[2];
    D = ctx->state[3];
    E = ctx->state[4];
    F = ctx->state[5];
    G = ctx->state[6];
    H = ctx->state[7];

    // 步骤3：第1-16轮迭代（使用FF1、GG1函数）
    for (j = 0; j < 16; j++) {
        SS1 = ROTL(ROTL(A, 12) + E + ROTL(SM3_T[j], j), 7);
        SS2 = SS1 ^ ROTL(A, 12);
        TT1 = FF1(A, B, C) + D + SS2 + W1[j];
        TT2 = GG1(E, F, G) + H + SS1 + W[j];
        
        // 并行更新中间变量（必须按此顺序，避免覆盖原始值）
        D = C;
        C = ROTL(B, 9);
        B = A;
        A = TT1;
        H = G;
        G = ROTL(F, 19);
        F = E;
        E = P0(TT2);
    }

    // 步骤4：第17-64轮迭代（使用FF2、GG2函数）
    for (j = 16; j < 64; j++) {
        SS1 = ROTL(ROTL(A, 12) + E + ROTL(SM3_T[j], j), 7);
        SS2 = SS1 ^ ROTL(A, 12);
        TT1 = FF2(A, B, C) + D + SS2 + W1[j];
        TT2 = GG2(E, F, G) + H + SS1 + W[j];
        
        // 并行更新中间变量
        D = C;
        C = ROTL(B, 9);
        B = A;
        A = TT1;
        H = G;
        G = ROTL(F, 19);
        F = E;
        E = P0(TT2);
    }

    // 步骤5：更新上下文状态（中间变量与原状态异或）
    ctx->state[0] ^= A;
    ctx->state[1] ^= B;
    ctx->state[2] ^= C;
    ctx->state[3] ^= D;
    ctx->state[4] ^= E;
    ctx->state[5] ^= F;
    ctx->state[6] ^= G;
    ctx->state[7] ^= H;
}