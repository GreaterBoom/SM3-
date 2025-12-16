# SM3密码杂凑算法实现

本项目实现了符合GM/T 0004-2012标准的SM3密码杂凑算法，采用模块化设计，支持内存数据哈希、文件哈希和HMAC-SM3功能。

## 项目结构

    SM3/
    ├── sm3_common.h     # 公共定义头文件
    ├── sm3_pad.c        # 消息填充模块
    ├── sm3_group.c      # 分组处理模块
    ├── sm3_compress.c   # 压缩函数模块
    ├── sm3_api.c        # 接口设计模块
    ├── sm3_tool.c       # 命令行工具封装
    ├── test.c           # 测试程序
    ├── Makefile         # Linux/Mac编译配置
    ├── compile.bat      # Windows编译脚本
    └── README.md        # 项目说明

## 功能特性

1. **基础哈希功能**
  
  * 计算内存数据的SM3哈希值
  * 计算文件的SM3哈希值
  * 支持任意长度的输入数据
2. **HMAC-SM3认证码**
  
  * 基于SM3的密钥哈希消息认证码
  * 支持任意长度的密钥
3. **命令行工具**
  
  * 支持字符串输入：`sm3_tool -s "abc"`
  * 支持文件输入：`sm3_tool -f test.txt`

## 编译方法

### Linux/Mac环境

    # 使用Makefile编译
    make
    
    # 或者直接编译
    gcc -Wall -O2 -std=c99 -o sm3_tool sm3_pad.c sm3_group.c sm3_compress.c sm3_api.c sm3_tool.c
    gcc -Wall -O2 -std=c99 -o test sm3_pad.c sm3_group.c sm3_compress.c sm3_api.c test.c

### Windows环境

    # 使用Visual Studio编译器（需先安装Visual Studio）
    compile.bat

## 使用示例

### 命令行工具

    # 计算字符串"abc"的SM3哈希
    ./sm3_tool -s "abc"
    
    # 计算文件test.txt的SM3哈希
    ./sm3_tool -f test.txt

### 编程接口

    #include "sm3_common.h"
    
    // 计算内存数据的SM3哈希
    char output[65];
    const char* data = "hello world";
    sm3_hash((uint8_t*)data, strlen(data), output);
    printf("SM3哈希值: %s\n", output);
    
    // 计算文件的SM3哈希
    int ret = sm3_hash_file("test.txt", output);
    if (ret == 0) {
        printf("文件SM3哈希值: %s\n", output);
    }
    
    // 计算HMAC-SM3
    const uint8_t key[] = "secret_key";
    size_t key_len = strlen("secret_key");
    sm3_hmac(key, key_len, (uint8_t*)data, strlen(data), output);
    printf("HMAC-SM3值: %s\n", output);

## 测试验证

运行测试程序验证"abc"的标准SM3哈希值：

    ./test

标准"abc"的SM3哈希值：

    66c7f0f462eeedd9d1f2d46bdc10e4e24167c4875cf2f7a2297da02b8f4ba8e0

## 注意事项

1. **编译器要求**
  
  * C99兼容的编译器
  * 支持标准C库
2. **内存管理**
  
  * 消息填充模块会动态分配内存，需确保程序有足够的内存空间
  * 文件哈希功能会将文件内容加载到内存，大文件可能需要大量内存
3. **错误处理**
  
  * 函数参数检查已实现，避免空指针和无效输入
  * 文件操作包含错误检查和返回值

## 算法标准

* 标准：GM/T 0004-2012《SM3密码杂凑算法》
* 分组长度：512位
* 输出长度：256位（64个十六进制字符）
* 迭代轮数：64轮

## 许可证

本项目仅供学习和研究使用，遵循国家密码管理局相关规定。
