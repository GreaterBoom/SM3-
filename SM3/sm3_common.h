/*************************************************************************
 > File Name: sm3_common.h
 > Description: SM3算法公共定义头文件（符合GM/T 0004-2012标准）
 > 包含：宏定义、上下文结构体、函数原型声明，供其他模块引用
 ************************************************************************/
#ifndef SM3_COMMON_H
#define SM3_COMMON_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* 32位大端字节序操作宏（SM3标准要求） */
#ifndef GET_ULONG_BE
#define GET_ULONG_BE(n, b, i)       \
{                                   \
    (n) = ( (uint32_t)(b)[(i) ] << 24 )        \
        | ( (uint32_t)(b)[(i) + 1] << 16 )     \
        | ( (uint32_t)(b)[(i) + 2] << 8 )      \
        | ( (uint32_t)(b)[(i) + 3] );          \
}
#endif

#ifndef PUT_ULONG_BE
#define PUT_ULONG_BE(n, b, i)       \
{                                   \
    (b)[(i) ] = (uint8_t)( (n) >> 24 );        \
    (b)[(i) + 1] = (uint8_t)( (n) >> 16 );     \
    (b)[(i) + 2] = (uint8_t)( (n) >> 8 );      \
    (b)[(i) + 3] = (uint8_t)( (n) );           \
}
#endif

/* SM3核心函数宏（符合GM/T 0004-2012标准） */
#define FF1(X, Y, Z) ((X) ^ (Y) ^ (Z))               // 1-16轮布尔函数
#define FF2(X, Y, Z) (((X) & (Y)) | ((X) & (Z)) | ((Y) & (Z)))   // 17-64轮布尔函数
#define GG1(X, Y, Z) ((X) ^ (Y) ^ (Z))               // 1-16轮布尔函数
#define GG2(X, Y, Z) (((X) & (Y)) | ((~(X)) & (Z)))  // 17-64轮布尔函数
#define ROTL(X, n) (((X) << (n)) | ((X) >> (32 - (n))))   // 32位循环左移n位
#define P0(X) ((X) ^ ROTL((X), 9) ^ ROTL((X), 17))    // 置换函数P0
#define P1(X) ((X) ^ ROTL((X), 15) ^ ROTL((X), 23))   // 置换函数P1

/* SM3上下文结构体（存储算法运行中间状态） */
typedef struct {
    uint32_t state[8];     // 压缩函数中间状态（A~H）
    uint32_t total[2];     // 已处理数据总长度（单位：字节），total[0]低32位，total[1]高32位
    uint8_t buffer[64];    // 未填满的512bit分组缓冲区
    uint8_t ipad[64];      // HMAC内填充缓冲区（0x36）
    uint8_t opad[64];      // HMAC外填充缓冲区（0x5C）
} sm3_context;

/* 轮常量定义（1-16轮：0x79CC4519；17-64轮：0x7A879D8A） */
static const uint32_t SM3_T[64] = {
    0x79CC4519, 0x79CC4519, 0x79CC4519, 0x79CC4519,
    0x79CC4519, 0x79CC4519, 0x79CC4519, 0x79CC4519,
    0x79CC4519, 0x79CC4519, 0x79CC4519, 0x79CC4519,
    0x79CC4519, 0x79CC4519, 0x79CC4519, 0x79CC4519,
    0x7A879D8A, 0x7A879D8A, 0x7A879D8A, 0x7A879D8A,
    0x7A879D8A, 0x7A879D8A, 0x7A879D8A, 0x7A879D8A,
    0x7A879D8A, 0x7A879D8A, 0x7A879D8A, 0x7A879D8A,
    0x7A879D8A, 0x7A879D8A, 0x7A879D8A, 0x7A879D8A,
    0x7A879D8A, 0x7A879D8A, 0x7A879D8A, 0x7A879D8A,
    0x7A879D8A, 0x7A879D8A, 0x7A879D8A, 0x7A879D8A,
    0x7A879D8A, 0x7A879D8A, 0x7A879D8A, 0x7A879D8A,
    0x7A879D8A, 0x7A879D8A, 0x7A879D8A, 0x7A879D8A,
    0x7A879D8A, 0x7A879D8A, 0x7A879D8A, 0x7A879D8A,
    0x7A879D8A, 0x7A879D8A, 0x7A879D8A, 0x7A879D8A,
    0x7A879D8A, 0x7A879D8A, 0x7A879D8A, 0x7A879D8A,
    0x7A879D8A, 0x7A879D8A, 0x7A879D8A, 0x7A879D8A
};

/* 模块1：消息填充模块函数声明 */
// 功能：对输入数据进行SM3标准填充，返回填充后的完整数据（需调用free释放）
// 参数：input-原始输入数据，input_len-原始数据长度（字节），output_len-输出填充后数据长度（字节）
uint8_t* sm3_pad(const uint8_t* input, size_t input_len, size_t* output_len);

/* 模块2：分组处理模块函数声明 */
// 功能：将填充后的数据拆分为512bit分组，并对单个分组进行扩展（生成W和W'数组）
// 参数：ctx-SM3上下文，pad_data-填充后的数据，pad_len-填充后数据长度（字节）
void sm3_group_process(sm3_context* ctx, const uint8_t* pad_data, size_t pad_len);

// 功能：对单个512bit分组进行扩展，生成68个32bit字W[0-67]和64个32bit字W'[0-63]
// 参数：block-512bit分组数据（64字节），W-输出W数组（需提前分配68个uint32_t空间），W1-输出W'数组（需提前分配64个uint32_t空间）
void sm3_group_expand(const uint8_t* block, uint32_t* W, uint32_t* W1);

/* 模块3：压缩函数模块函数声明 */
// 功能：初始化SM3上下文（设置初始IV）
void sm3_compress_init(sm3_context* ctx);

// 功能：对单个512bit分组执行压缩函数（64轮迭代）
// 参数：ctx-SM3上下文，block-512bit分组数据（64字节）
void sm3_compress_block(sm3_context* ctx, const uint8_t* block);

/* 模块4：接口设计模块函数声明 */
// 功能：计算内存数据的SM3哈希值（输出64位十六进制字符串）
// 参数：input-输入数据，input_len-数据长度（字节），output-输出缓冲区（至少65字节，含'\0'）
void sm3_hash(const uint8_t* input, size_t input_len, char* output);

// 功能：计算文件的SM3哈希值（输出64位十六进制字符串）
// 参数：file_path-文件路径，output-输出缓冲区（至少65字节，含'\0'），返回值-0成功，1文件打开失败，2读取失败
int sm3_hash_file(const char* file_path, char* output);

// 功能：计算HMAC-SM3认证码（输出64位十六进制字符串）
// 参数：key-密钥，key_len-密钥长度，input-输入数据，input_len-数据长度，output-输出缓冲区（至少65字节）
void sm3_hmac(const uint8_t* key, size_t key_len, const uint8_t* input, size_t input_len, char* output);

#endif // SM3_COMMON_H