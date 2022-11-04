#pragma warning(disable : 4996)
#include "png.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 创建一个PNG结构体
PNG* png_alloc() {
	PNG* png = malloc(sizeof(PNG));
	if (!png) {
		printf("alloc mem for png error.\n");
		return NULL;
	}
	png->is_png = 0;
	png->chunk_list = NULL;
	png->size = 0;
	return png;
}

// 创建一个Chunk数据块结构体
Chunk* png_alloc_chunk() {
	Chunk* chunk = malloc(sizeof(Chunk));
	if (!chunk) {
		printf("alloc mem for chunk error.\n");
		return NULL;
	}
	chunk->data_size = chunk->crc = 0;
	chunk->type = chunk->data = NULL;
	return chunk;
}

// 创建一个IHDRChunk
IHDRChunk* png_alloc_ihdr_chunk() {
	IHDRChunk* ic = malloc(sizeof(IHDRChunk));
	if (!ic) {
		printf("alloc mem for IHDRChunk error.\n");
		return NULL;
	}
	ic->width = ic->height = 0;
	ic->bit_depth = ic->color_type = ic->compression_method = ic->filter_method = ic->interlace_method = 0;
	return ic;
}

// 创建一个PLTEChunk
PLTEChunk* png_alloc_plte_chunk() {
	PLTEChunk* pe = malloc(sizeof(PLTEChunk));
	if (!pe) {
		printf("alloc mem for PLTEChunk error.\n");
		return NULL;
	}
	pe->size = 0;
	pe->rgb = NULL;
	return pe;
}

// 解析所有Chunk
int png_find_chunk_info(PNG* png, const char* file_path) {
	if (!png || !file_path)
		return -1;
	FILE* fp = fopen(file_path, "rb");
	if (!fp) {
		printf("read png file error.\n");
		return -1;
	}

	// 文件大小
	fseek(fp, 0L, SEEK_END);
	int file_size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	char is_png = 1;
	for (int i = 0; i < 8; i++) {
		char byte = 0;
		fread(&byte, 1, 1, fp);
		if (i == 0 || byte == 0x89)
			is_png = is_png && 1;
		else if (i == 1 || byte == 0x50)
			is_png = is_png && 1;
		else if (i == 2 || byte == 0x4E)
			is_png = is_png && 1;
		else if (i == 3 || byte == 0x47)
			is_png = is_png && 1;
		else if (i == 4 || byte == 0x0D)
			is_png = is_png && 1;
		else if (i == 5 || byte == 0x0A)
			is_png = is_png && 1;
		else if (i == 6 || byte == 0x1A)
			is_png = is_png && 1;
		else if (i == 7 || byte == 0x0A)
			is_png = is_png && 1;
		else
			is_png = is_png && 0;
	}
	if (!is_png) {
		printf("not a valid png file.\n");
		return -1;
	}
	png->is_png = 1;

	int current_position = 0;
	while(current_position < file_size) {
		Chunk* ck = png_alloc_chunk();
		if (!ck)
			continue;

		int size;
		fread(&size, 4, 1, fp);
		size = from_hex4(size);

		char* type = new_str(4);
		fread(type, 4, 1, fp);

		if (strcmp(type, "IHDR") == 0) {
			IHDRChunk* ic = png_alloc_ihdr_chunk();
			if (!ic) {
				free(ck);
				continue;
			 }
			int width, height;
			char bit_depth, color_type, compression_method, filter_method, interlace_method;
			fread(&width, 4, 1, fp);
			fread(&height, 4, 1, fp);
			fread(&bit_depth, 1, 1, fp);
			fread(&color_type, 1, 1, fp);
			fread(&compression_method, 1, 1, fp);
			fread(&filter_method, 1, 1, fp);
			fread(&interlace_method, 1, 1, fp);
			width = from_hex4(width);
			height = from_hex4(height);
			ic->width = width;
			ic->height = height;
			ic->bit_depth = bit_depth;
			ic->color_type = color_type;
			ic->compression_method = compression_method;
			ic->filter_method = filter_method;
			ic->interlace_method = interlace_method;
			ck->data = ic;
		}
		else if (strcmp(type, "PLTE") == 0) {
			PLTEChunk* pe = png_alloc_plte_chunk();
			if (!pe) {
				free(ck);
				continue;
			}
			pe->rgb = (char*)malloc(size * sizeof(char));
			int sum = 0;
			while (sum != size) {
				char r, g, b;
				fread(&r, 1, 1, fp);
				fread(&g, 1, 1, fp);
				fread(&b, 1, 1, fp);
				pe->rgb[sum] = r;
				pe->rgb[sum + 1] = g;
				pe->rgb[sum +2 ] = b;
				sum += 3;
				pe->size++;
			}
		} 
		else {
			fseek(fp, size, SEEK_CUR);
		}

		int crc;
		fread(&crc, 4, 1, fp);
		crc = from_hex4(crc);
		
		ck->data_size = size;
		ck->type = type;
		ck->crc = crc;

		if (png->size == 0 && !png->chunk_list) {
			png->chunk_list = malloc(sizeof(Chunk*));
			if (png->chunk_list) {
				png->chunk_list[0] = ck;
				png->size++;
			}
		}
		else {
			Chunk** list = realloc(png->chunk_list, (png->size + 1) * sizeof(Chunk*));
			if (list) {
				list[png->size] = ck;
				png->size++;
				png->chunk_list = list;
			}
		}
		
		current_position = ftell(fp);
	}
	
	fclose(fp);
	return 1;
}

// 遍历chunk
void png_traverse_chunk(PNG* png) {
	if (!png || png->size < 1 || !png->is_png || !png->chunk_list)
		return;
	for (int i = 0; i < png->size; i++) {
		Chunk* ck = png->chunk_list[i];
		printf("Index:				chunk.%d\n", i);
		printf("Type:				%s\n", ck->type);
		printf("Data Size:			%d\n", ck->data_size);
		printf("CRC:				%x\n", ck->crc);
		if (strcmp(ck->type, "IHDR") == 0 && ck->data) {
			IHDRChunk* ic = (IHDRChunk*)ck->data;
			printf("Width:				%d\n", ic->width);
			printf("Height:				%d\n", ic->height);
			printf("Bit Depth:			%d\n", ic->bit_depth);
			printf("Color Type:			%d\n", ic->color_type);
			printf("Compression Method:		%d\n", ic->compression_method);
			printf("Filter Method:			%d\n", ic->filter_method);
			printf("Interlace Method:		%d\n", ic->interlace_method);
		}
		else if (strcmp(ck->type, "PLTE") == 0 && ck->data) {
			PLTEChunk* pe = (PLTEChunk*)ck->data;
			if (pe->size > 0 || pe->rgb) {
				for (int i = 0; i < pe->size; i++) {
					printf("R-G-B:				[%d %d %d]\n", (int)(*(pe->rgb)), (int)(*(pe->rgb + 1)), (int)(*(pe->rgb + 2)));
				}
				printf("\n");
			}
		}
		printf("\n");
	}
}

// 释放所有内容
void png_free_png(PNG* png) {
	if (!png || png->size < 1 || !png->is_png || !png->chunk_list)
		return;
	for (int i = 0; i < png->size; i++) {
		Chunk* ck = png->chunk_list[i];
		if (ck->type)
			free(ck->type);
		if (ck->data)
			free(ck->data);
		free(ck);
	}
	free(png->chunk_list);
	free(png);
}
