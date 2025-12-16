/*************************************************************************
 > File Name: sm3_group.c
 > Description: SM3算法分组处理模块（符合GM/T 0004-2012标准）
 > 核心功能：分组拆分（512bit/组）、分组扩展（生成W和W'数组）
 ************************************************************************/
#include "sm3_common.h"
#include <assert.h>

/* 
 * 函数名：sm3_group_expand
 * 功能：对单个512bit分组进行扩展，生成68个32bit字W[0-67]和64个32bit字W'[0-63]
 * 参数：block-512bit分组数据（64字节），W-输出W数组（需提前分配68个uint32_t空间），W1-输出W'数组（需提前分配64个uint32_t空间）
 */
void sm3_group_expand(const uint8_t* block, uint32_t* W, uint32_t* W1) {
    int j;

    // 步骤1：生成W[0-15]（直接从分组中按大端序提取32bit字）
    for (j = 0; j < 16; j++) {
        GET_ULONG_BE(W[j], block, j * 4);
    }

    // 步骤2：生成W[16-67]（按SM3扩展公式计算）
    for (j = 16; j < 68; j++) {
        W[j] = P1(W[j-16] ^ W[j-9] ^ ROTL(W[j-3], 15)) ^ ROTL(W[j-13], 7) ^ W[j-6];
    }

    // 步骤3：生成W'[0-63]（W'[j] = W[j] ^ W[j+4]）
    for (j = 0; j < 64; j++) {
        W1[j] = W[j] ^ W[j + 4];
    }
}

/* 
 * 函数名：sm3_group_process
 * 功能：将填充后的数据拆分为512bit分组，并逐个分组调用压缩函数
 * 参数：ctx-SM3上下文，pad_data-填充后的数据，pad_len-填充后数据长度（字节）
 * 断言：填充后数据长度必须是64的整数倍（否则填充逻辑错误）
 */
void sm3_group_process(sm3_context* ctx, const uint8_t* pad_data, size_t pad_len) {
    // 断言：填充后数据长度必须是512bit（64字节）的整数倍，否则填充逻辑异常
    assert(pad_len % 64 == 0 && "Pad data length must be multiple of 64 bytes");

    size_t block_num = pad_len / 64;        // 分组数量

    // 逐个处理每个512bit分组
    for (size_t i = 0; i < block_num; i++) {
        const uint8_t* block = pad_data + i * 64;    // 当前分组数据
        sm3_compress_block(ctx, block);              // 调用压缩函数处理分组
    }
}