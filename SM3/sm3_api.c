/*************************************************************************
 > File Name: sm3_api.c
 > Description: SM3算法接口设计模块（符合GM/T 0004-2012标准）
 > 核心功能：提供内存数据哈希、文件哈希、HMAC-SM3接口，输出标准64位十六进制字符串
 ************************************************************************/
#include "sm3_common.h"
#include <stdio.h>

/* 
 * 函数名：sm3_hash
 * 功能：计算内存数据的SM3哈希值，输出64位十六进制字符串（UTF-8编码）
 * 参数：input-输入数据（字节），input_len-数据长度（字节），output-输出缓冲区（至少65字节，含'\0'终止符）
 * 流程：填充→分组处理→压缩→结果转换
 */
void sm3_hash(const uint8_t* input, size_t input_len, char* output) {
    sm3_context ctx;
    uint8_t* pad_data = NULL;
    size_t pad_len = 0;
    char hex_table[] = "0123456789abcdef"; // 十六进制字符表

    // 步骤1：初始化压缩函数上下文
    sm3_compress_init(&ctx);

    // 步骤2：对输入数据进行填充
    pad_data = sm3_pad(input, input_len, &pad_len);
    if (pad_data == NULL || pad_len == 0) {
        memset(output, 0, 65);
        return;
    }

    // 步骤3：分组处理与压缩
    sm3_group_process(&ctx, pad_data, pad_len);

    // 步骤4：将最终状态转换为64位十六进制字符串
    for (int i = 0; i < 8; i++) {
        // 每个32bit状态变量转换为8位十六进制字符
        output[i*8 + 0] = hex_table[(ctx.state[i] >> 28) & 0x0F];
        output[i*8 + 1] = hex_table[(ctx.state[i] >> 24) & 0x0F];
        output[i*8 + 2] = hex_table[(ctx.state[i] >> 20) & 0x0F];
        output[i*8 + 3] = hex_table[(ctx.state[i] >> 16) & 0x0F];
        output[i*8 + 4] = hex_table[(ctx.state[i] >> 12) & 0x0F];
        output[i*8 + 5] = hex_table[(ctx.state[i] >> 8) & 0x0F];
        output[i*8 + 6] = hex_table[(ctx.state[i] >> 4) & 0x0F];
        output[i*8 + 7] = hex_table[(ctx.state[i] >> 0) & 0x0F];
    }
    output[64] = '\0'; // 字符串终止符

    // 步骤5：释放填充数据内存
    free(pad_data);
}

/* 
 * 函数名：sm3_hash_file
 * 功能：计算文件内容的SM3哈希值，输出64位十六进制字符串
 * 参数：file_path-文件路径，output-输出缓冲区（至少65字节）
 * 返回值：0-成功，1-文件打开失败，2-文件读取失败
 */
int sm3_hash_file(const char* file_path, char* output) {
    FILE* fp = NULL;
    uint8_t buf[4096]; // 文件读取缓冲区（4KB）
    size_t read_len = 0;
    sm3_context ctx;
    uint8_t* file_data = NULL;
    size_t file_len = 0;
    uint8_t* pad_data = NULL;
    size_t pad_len = 0;

    // 步骤1：打开文件（二进制只读模式）
    fp = fopen(file_path, "rb");
    if (fp == NULL) {
        return 1; // 文件打开失败
    }

    // 步骤2：读取文件内容到内存
    while ((read_len = fread(buf, 1, sizeof(buf), fp)) > 0) {
        // 扩容并复制数据
        file_data = (uint8_t*)realloc(file_data, file_len + read_len);
        if (file_data == NULL) {
            fclose(fp);
            return 2;
        }
        memcpy(file_data + file_len, buf, read_len);
        file_len += read_len;

        // 检查读取错误
        if (ferror(fp)) {
            free(file_data);
            fclose(fp);
            return 2;
        }
    }

    // 步骤3：关闭文件
    fclose(fp);

    // 步骤4：初始化上下文并处理数据
    sm3_compress_init(&ctx);
    pad_data = sm3_pad(file_data, file_len, &pad_len);
    if (pad_data != NULL && pad_len > 0) {
        sm3_group_process(&ctx, pad_data, pad_len);
        free(pad_data);
    }

    // 步骤5：转换结果为十六进制字符串
    // 直接将最终状态转换为十六进制字符串
    char hex_table[] = "0123456789abcdef";
    for (int i = 0; i < 8; i++) {
        output[i*8 + 0] = hex_table[(ctx.state[i] >> 28) & 0x0F];
        output[i*8 + 1] = hex_table[(ctx.state[i] >> 24) & 0x0F];
        output[i*8 + 2] = hex_table[(ctx.state[i] >> 20) & 0x0F];
        output[i*8 + 3] = hex_table[(ctx.state[i] >> 16) & 0x0F];
        output[i*8 + 4] = hex_table[(ctx.state[i] >> 12) & 0x0F];
        output[i*8 + 5] = hex_table[(ctx.state[i] >> 8) & 0x0F];
        output[i*8 + 6] = hex_table[(ctx.state[i] >> 4) & 0x0F];
        output[i*8 + 7] = hex_table[(ctx.state[i] >> 0) & 0x0F];
    }
    output[64] = '\0';

    // 步骤6：释放内存
    free(file_data);
    return 0;
}

/* 
 * 函数名：sm3_hmac
 * 功能：计算HMAC-SM3认证码（基于SM3的密钥哈希消息认证码）
 * 参数：key-密钥，key_len-密钥长度，input-输入数据，input_len-数据长度，output-输出缓冲区（至少65字节）
 * 流程：密钥处理→内填充→哈希→外填充→二次哈希
 */
void sm3_hmac(const uint8_t* key, size_t key_len, const uint8_t* input, size_t input_len, char* output) {
    sm3_context ctx;
    uint8_t key_hash[32]; // 密钥哈希结果（密钥过长时使用）
    uint8_t inner_data[64 + input_len]; // 内填充+输入数据
    uint8_t outer_data[64 + 32]; // 外填充+内哈希结果
    char inner_hash[65]; // 内哈希结果

    // 步骤1：处理长密钥（长度>64字节时哈希压缩）
    if (key_len > 64) {
        sm3_hash(key, key_len, (char*)key_hash);
        key = key_hash;
        key_len = 32;
    }

    // 步骤2：初始化内填充（ipad = 0x36 ^ key）
    memset(ctx.ipad, 0x36, 64);
    for (size_t i = 0; i < key_len; i++) {
        ctx.ipad[i] ^= key[i];
    }

    // 步骤3：计算内填充 + 输入数据的哈希
    memcpy(inner_data, ctx.ipad, 64);
    memcpy(inner_data + 64, input, input_len);
    sm3_hash(inner_data, 64 + input_len, inner_hash);

    // 步骤4：初始化外填充（opad = 0x5C ^ key）
    memset(ctx.opad, 0x5C, 64);
    for (size_t i = 0; i < key_len; i++) {
        ctx.opad[i] ^= key[i];
    }

    // 步骤5：计算外填充 + 内哈希的哈希（最终HMAC）
    memcpy(outer_data, ctx.opad, 64);
    for (size_t i = 0; i < 32; i++) {
        outer_data[64 + i] = (inner_hash[i*2] >= 'a' ? inner_hash[i*2] - 'a' + 10 : inner_hash[i*2] - '0') << 4 | \
                           (inner_hash[i*2 + 1] >= 'a' ? inner_hash[i*2 + 1] - 'a' + 10 : inner_hash[i*2 + 1] - '0');
    }
    sm3_hash(outer_data, 64 + 32, output);
}