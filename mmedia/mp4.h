#pragma once

typedef struct Header {
    int size;  // box��С��4�ֽ�
    const char* type;  // box���ͣ�4�ֽ�
    long largsize;  // size==1, box��С��8�ֽ�
    const char* extended_type;  // type=uuid, ��չ���ͣ�16�ֽ�
    char version; // �汾��1�ֽ�
    const char* flags;  // ��ǣ�3�ֽ�
    int start_position;  // ��ǰbox���ļ��е���ʼλ��
    int end_position;  // ��ǰbox���ļ��еĽ���λ��
} Header;

typedef struct Body {
    const char* major_brand;
    int minor_version;
    char** compatible_brands;
    int compatible_brands_len;
} Body;

/*
1. ftyp/free/mdat/moov/trak/udta/general box
2. mvhd/tkhd
**/

typedef struct Box {
    Header* header;
    Body* body;
    int children;  // ��box������
    struct Box* child;  // ��һ����box
    struct Box* next;  // ͬ����box
    int has_children;
} Box;

typedef struct BoxTree {
    int size;  // box����
    Box* root;
} BoxTree;

BoxTree* mm_alloc_boxtree();  // ����box�����
Box* mm_alloc_box();  // ����box
int mm_find_stream_info(BoxTree* box_tree, const char* file_path);  // �����ͻ�ȡ����box��Ϣ
int mm_find_stream_info_simple(BoxTree* box_tree, const char* file_path);  // ��������size��type
Box* mm_get_first_box_by_name(BoxTree* box_tree, char* box_name);  // ����box�����ͷ���box������ж������ֻ����ƥ��ĵ�һ��box
void mm_traverse_box(BoxTree* box_tree);  // ����box��������box
void mm_free_boxtree(BoxTree* box_tree);  // �ͷ���������