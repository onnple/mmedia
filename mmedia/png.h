#pragma once

// 基本的数据块形式
typedef struct Chunk {
	int data_size;  // 4字节，数据域长度
	char* type;  // 4字节，数据块类型
	void* data;  // 长度为data_size，数据块的实际数据。该指针指向不同类型的Chunk
	int crc;  // 4字节，循环冗余码
} Chunk;

// 文件头数据块
typedef struct IHDRChunk {
	int width;  // 4字节，图像宽度
	int height;  // 4字节，图像高度
	char bit_depth;  // 1字节，图像深度
	char color_type;  // 1字节，颜色类型
	char compression_method;  // 1字节，压缩方法
	char filter_method;  // 1字节，过滤器方法
	char interlace_method;  // 1字节，隔行扫描方法
} IHDRChunk;

// 调色板数据块
typedef struct PLTEChunk {
	char* rgb;  // 3字节，可包含1-256个调色板信息，red + green + blue
	int size;  // 调色板信息个数
} PLTEChunk;

// 图像数据块：这里不做解析，忽略，直接使用Chunk
typedef struct IDATChunk {
	char data;
} IDATChunk;

// 文件结束数据块：数据域为空，可不用该结构体，这里不使用
typedef struct IENDChunk {
	char empty;
} IENDChunk;

// PNG文件结构
typedef struct PNG {
	char is_png;  // 8字节PNG标识，读取和验证：89 50 4E 47 0D 0A 1A 0A
	Chunk** chunk_list;  // PNG文件中的所有数据块
	int size;  // chunk的数量
} PNG;

PNG* png_alloc();  // 创建一个PNG结构体
Chunk* png_alloc_chunk();  // 创建一个Chunk数据块结构体
IHDRChunk* png_alloc_ihdr_chunk();  // 创建一个IHDRChunk
PLTEChunk* png_alloc_plte_chunk();  // 创建一个PLTEChunk

int png_find_chunk_info(PNG* png, const char* file_path);  // 解析所有Chunk
void png_traverse_chunk(PNG* png);  // 遍历chunk
void png_free_png(PNG* png);  // 释放所有内容