#pragma once

// ���������ݿ���ʽ
typedef struct Chunk {
	int data_size;  // 4�ֽڣ������򳤶�
	char* type;  // 4�ֽڣ����ݿ�����
	void* data;  // ����Ϊdata_size�����ݿ��ʵ�����ݡ���ָ��ָ��ͬ���͵�Chunk
	int crc;  // 4�ֽڣ�ѭ��������
} Chunk;

// �ļ�ͷ���ݿ�
typedef struct IHDRChunk {
	int width;  // 4�ֽڣ�ͼ����
	int height;  // 4�ֽڣ�ͼ��߶�
	char bit_depth;  // 1�ֽڣ�ͼ�����
	char color_type;  // 1�ֽڣ���ɫ����
	char compression_method;  // 1�ֽڣ�ѹ������
	char filter_method;  // 1�ֽڣ�����������
	char interlace_method;  // 1�ֽڣ�����ɨ�跽��
} IHDRChunk;

// ��ɫ�����ݿ�
typedef struct PLTEChunk {
	char* rgb;  // 3�ֽڣ��ɰ���1-256����ɫ����Ϣ��red + green + blue
	int size;  // ��ɫ����Ϣ����
} PLTEChunk;

// ͼ�����ݿ飺���ﲻ�����������ԣ�ֱ��ʹ��Chunk
typedef struct IDATChunk {

} IDATChunk;

// �ļ��������ݿ飺������Ϊ�գ��ɲ��øýṹ�壬���ﲻʹ��
typedef struct IENDChunk {

} IENDChunk;

// PNG�ļ��ṹ
typedef struct PNG {
	char* signature;  // 8�ֽ�PNG��ʶ����ȡ����֤��89 50 4E 47 0D 0A 1A 0A
	Chunk* chunk_list;  // PNG�ļ��е��������ݿ�
	int size;  // chunk������
} PNG;