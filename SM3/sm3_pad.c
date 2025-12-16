/*************************************************************************
 > File Name: sm3_pad.c
 > Description: SM3算法消息填充模块（符合GM/T 0004-2012标准）
 > 核心功能：实现SM3填充规则，支持空消息、448mod512、0mod512等特殊场景
 ************************************************************************/
#include "sm3_common.h"

/* 
 * 函数名：sm3_pad
 * 功能：对原始输入数据执行SM3标准填充
 * 填充规则：
 * 1. 设原始数据长度为l（bit），先补1个"1"bit，再补k个"0"bit，使l+1+k ≡ 448 mod 512
 * 2. 最后补64bit的l（大端序存储）
 * 特殊场景覆盖：空消息（l=0）、l≡448mod512、l≡0mod512
 * 参数：input-原始输入数据（字节），input_len-原始数据长度（字节），output_len-填充后数据长度（字节）
 * 返回值：填充后的完整数据（堆内存，需调用free释放）
 */
uint8_t* sm3_pad(const uint8_t* input, size_t input_len, size_t* output_len) {
    size_t l_bits = input_len * 8;      // 原始数据长度（bit）
    size_t pad_zero_bits;               // 需要补充的"0"bit数量
    size_t total_bits;                  // 填充后总长度（bit），需是512的整数倍

    // 步骤1：计算需要补充的"0"bit数量k
    if ((l_bits + 1) % 512 <= 448) {
        pad_zero_bits = 448 - (l_bits + 1) % 512;
    } else {
        pad_zero_bits = 512 + 448 - (l_bits + 1) % 512;
    }

    // 步骤2：计算填充后总长度（bit）= 原始长度 + 1（"1"bit） + k（"0"bit） + 64（长度字段）
    total_bits = l_bits + 1 + pad_zero_bits + 64;
    *output_len = total_bits / 8;       // 转换为字节数

    // 步骤3：分配填充后数据的内存
    uint8_t* pad_data = (uint8_t*)malloc(*output_len);
    if (pad_data == NULL) {
        *output_len = 0;
        return NULL;
    }
    memset(pad_data, 0, *output_len);

    // 步骤4：复制原始数据到填充缓冲区
    if (input_len > 0 && input != NULL) {
        memcpy(pad_data, input, input_len);
    }

    // 步骤5：补1个"1"bit（对应字节的最高位，其余位为0）
    pad_data[input_len] |= 0x80;

    // 步骤6：补64bit原始长度（大端序存储）
    uint8_t len_bytes[8];
    PUT_ULONG_BE((uint32_t)(l_bits >> 32), len_bytes, 0);   // 高32位
    PUT_ULONG_BE((uint32_t)(l_bits & 0xFFFFFFFF), len_bytes, 4); // 低32位
    memcpy(pad_data + (*output_len - 8), len_bytes, 8);     // 写入长度字段

    return pad_data;
}