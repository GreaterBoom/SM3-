/*************************************************************************
 > File Name: sm3_tool.c
 > Description: SM3算法命令行工具（符合作业接口设计要求）
 > 功能：支持字符串输入（-s）和文件输入（-f），输出64位SM3哈希值
 > 使用示例：
 > ./sm3_tool -s "abc" # 计算字符串"abc"的SM3哈希
 > ./sm3_tool -f test.txt # 计算文件test.txt的SM3哈希
 ************************************************************************/
#include "sm3_common.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* 帮助信息 */
static void print_help(const char* prog_name) {
    printf("SM3 密码杂凑算法命令行工具（符合GM/T 0004-2012标准）\n");
    printf("使用方法：%s [选项] [输入]\n", prog_name);
    printf("选项：\n");
    printf("  -s <string> 计算字符串的SM3哈希（支持中英文、特殊字符）\n");
    printf("  -f <file> 计算文件的SM3哈希（支持二进制文件如图片、文档）\n");
    printf("  -h 显示帮助信息\n");
    printf("示例：\n");
    printf("  %s -s \"abc\"\n", prog_name);
    printf("  %s -f test.txt\n", prog_name);
}

int main(int argc, char* argv[]) {
    char* str_input = NULL;
    char* file_input = NULL;
    char output[65]; // 存储64位十六进制哈希值
    int i = 1;

    // 解析命令行参数
    while (i < argc) {
        if (strcmp(argv[i], "-s") == 0 && (i + 1) < argc) {
            str_input = argv[i + 1];
            i += 2;
        } else if (strcmp(argv[i], "-f") == 0 && (i + 1) < argc) {
            file_input = argv[i + 1];
            i += 2;
        } else if (strcmp(argv[i], "-h") == 0) {
            print_help(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "错误：未知参数 %s\n", argv[i]);
            print_help(argv[0]);
            return 1;
        }
    }

    // 校验输入参数（必须指定-s或-f）
    if (str_input == NULL && file_input == NULL) {
        fprintf(stderr, "错误：必须指定输入类型（-s字符串或-f文件）\n");
        print_help(argv[0]);
        return 1;
    }

    // 计算字符串哈希
    if (str_input != NULL) {
        // 字符串转字节流（默认UTF-8编码，兼容中英文、特殊字符）
        sm3_hash((uint8_t*)str_input, strlen(str_input), output);
        printf("SM3 哈希值（字符串）：%s\n", output);
    }

    // 计算文件哈希
    if (file_input != NULL) {
        int ret = sm3_hash_file(file_input, output);
        if (ret == 0) {
            printf("SM3 哈希值（文件）：%s\n", output);
        } else if (ret == 1) {
            fprintf(stderr, "错误：无法打开文件 %s\n", file_input);
            return 1;
        } else if (ret == 2) {
            fprintf(stderr, "错误：读取文件 %s 失败\n", file_input);
            return 1;
        }
    }

    return 0;
}