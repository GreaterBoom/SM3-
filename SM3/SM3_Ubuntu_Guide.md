# SM3算法在Linux Ubuntu中的运行与OpenSSL对比指南

## 一、环境准备

### 1. 安装必要工具
在Ubuntu终端中执行以下命令安装编译工具和OpenSSL：

```bash
# 更新软件包列表
sudo apt update

# 安装编译工具链
sudo apt install build-essential

# 安装OpenSSL（用于对比测试）
sudo apt install openssl
```

### 2. 确认OpenSSL版本
```bash
openssl version
```
确保OpenSSL版本支持SM3算法（通常OpenSSL 1.1.1及以上版本支持）。

## 二、编译SM3项目

### 1. 复制项目文件
将SM3项目的所有源文件复制到Ubuntu系统中（可以使用`scp`、U盘等方式）。

### 2. 编译项目
在项目目录中执行：

```bash
# 使用Makefile编译
make
```

编译成功后，目录中会生成`sm3_tool`可执行文件。

## 三、运行SM3工具

### 1. 计算字符串的SM3哈希
```bash
./sm3_tool -s "abc"
```

### 2. 计算文件的SM3哈希
```bash
# 首先创建一个测试文件
echo "test content" > test.txt

# 计算文件哈希
./sm3_tool -f test.txt
```

## 四、使用OpenSSL计算SM3哈希

### 1. 计算字符串的SM3哈希
```bash
# 方式1：直接输入字符串
echo -n "abc" | openssl dgst -sm3

# 方式2：使用echo命令（注意-n参数避免添加换行符）
openssl dgst -sm3 <<< "abc"
```

### 2. 计算文件的SM3哈希
```bash
openssl dgst -sm3 test.txt
```

## 五、结果对比

### 1. 字符串"abc"的SM3哈希对比
- **SM3工具输出**：
  ```
  SM3 哈希值（字符串）：66c7f0f462eeedd9d1f2d46bdc10e4e24167c4875cf2f7a2297da02b8f4ba8e0
  ```

- **OpenSSL输出**：
  ```
  (stdin)= 66c7f0f462eeedd9d1f2d46bdc10e4e24167c4875cf2f7a2297da02b8f4ba8e0
  ```

### 2. 文件哈希对比
确保测试文件内容完全相同，然后对比两者的输出结果。

## 六、批量测试与验证

### 1. 创建测试脚本
```bash
#!/bin/bash

# 测试字符串列表
test_strings=("" "a" "abc" "abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcd")

echo "=== SM3算法对比测试 ==="
echo ""

for str in "${test_strings[@]}"; do
    echo "测试字符串: '$str'"
    
    # 使用SM3工具计算
    sm3_result=$(./sm3_tool -s "$str" 2>&1 | grep "SM3 哈希值" | awk '{print $4}')
    
    # 使用OpenSSL计算
    openssl_result=$(echo -n "$str" | openssl dgst -sm3 | awk '{print $2}')
    
    echo "SM3工具: $sm3_result"
    echo "OpenSSL: $openssl_result"
    
    # 对比结果
    if [ "$sm3_result" == "$openssl_result" ]; then
        echo "✓ 结果一致"
    else
        echo "✗ 结果不一致"
    fi
    
    echo ""
done
```

### 2. 运行测试脚本
```bash
chmod +x test_sm3.sh
./test_sm3.sh
```

## 七、常见问题排查

### 1. 编译错误
- 确保已安装`build-essential`包
- 检查Makefile中的编译器设置是否正确

### 2. 结果不一致
- 检查字符串输入时是否包含换行符（使用`-n`参数避免）
- 确保文件内容完全相同
- 验证OpenSSL版本是否支持SM3

### 3. OpenSSL不支持SM3
如果出现`unknown option '-sm3'`错误，说明OpenSSL版本过低，需要升级：

```bash
# 添加OpenSSL 1.1.1源
sudo apt install software-properties-common
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt update
sudo apt install openssl
```

## 八、性能对比（可选）

### 1. 测试SM3工具性能
```bash
time for i in {1..1000}; do ./sm3_tool -s "test string for performance comparison" > /dev/null; done
```

### 2. 测试OpenSSL性能
```bash
time for i in {1..1000}; do echo -n "test string for performance comparison" | openssl dgst -sm3 > /dev/null; done
```

## 九、总结

通过以上步骤，您可以：
1. 在Ubuntu环境中成功编译和运行SM3项目
2. 使用OpenSSL计算SM3哈希值
3. 对比两种实现的结果一致性
4. 进行批量测试和性能对比

SM3算法是中国商用密码标准，与OpenSSL的实现结果一致，证明了我们的SM3实现的正确性。