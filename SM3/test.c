/*************************************************************************
 > File Name: test.c
 > Description: SM3算法测试文件
 > 测试内容：验证"abc"的标准SM3哈希值是否正确
 ************************************************************************/
#include "sm3_common.h"
#include <stdio.h>
#include <string.h>

int main() {
    char output[65];
    const char* test_str = "abc";
    
    // 计算"abc"的SM3哈希值
    sm3_hash((uint8_t*)test_str, strlen(test_str), output);
    
    printf("测试字符串: \"%s\"\n", test_str);
    printf("SM3哈希值: %s\n", output);
    
    // 标准"abc"的SM3哈希值（来自GM/T 0004-2012标准）
    const char* standard_hash = "66c7f0f462eeedd9d1f2d46bdc10e4e24167c4875cf2f7a2297da02b8f4ba8e0";
    
    if (strcmp(output, standard_hash) == 0) {
        printf("\033[32m测试通过！哈希值符合标准。\033[0m\n");
        return 0;
    } else {
        printf("\033[31m测试失败！\033[0m\n");
        printf("标准哈希值: %s\n", standard_hash);
        return 1;
    }
}