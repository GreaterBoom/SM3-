# Makefile for SM3 Algorithm Tool
# 编译说明：适用于Linux/macOS环境，Windows可使用MinGW-w64编译

CC = gcc
CFLAGS = -Wall -O2 -std=c99  # 开启警告、优化，兼容C99标准
TARGET = sm3_tool
OBJS = sm3_pad.o sm3_group.o sm3_compress.o sm3_api.o sm3_tool.o

# 链接生成可执行文件
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# 编译各模块目标文件
sm3_pad.o: sm3_pad.c sm3_common.h
	$(CC) $(CFLAGS) -c sm3_pad.c -o sm3_pad.o

sm3_group.o: sm3_group.c sm3_common.h
	$(CC) $(CFLAGS) -c sm3_group.c -o sm3_group.o

sm3_compress.o: sm3_compress.c sm3_common.h
	$(CC) $(CFLAGS) -c sm3_compress.c -o sm3_compress.o

sm3_api.o: sm3_api.c sm3_common.h
	$(CC) $(CFLAGS) -c sm3_api.c -o sm3_api.o

sm3_tool.o: sm3_tool.c sm3_common.h
	$(CC) $(CFLAGS) -c sm3_tool.c -o sm3_tool.o

# 清理目标文件和可执行文件
clean:
	rm -f $(OBJS) $(TARGET)