#pragma once

typedef struct Header {
    int size;  // box大小，4字节
    const char* type;  // box类型，4字节
    long largsize;  // size==1, box大小，8字节
    const char* extended_type;  // type=uuid, 扩展类型，16字节
    char version; // 版本，1字节
    const char* flags;  // 标记，3字节
    int start_position;  // 当前box在文件中的起始位置
    int end_position;  // 当前box在文件中的结束位置
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
    int children;  // 子box的数量
    struct Box* child;  // 第一个子box
    struct Box* next;  // 同级的box
    int has_children;
} Box;

typedef struct BoxTree {
    int size;  // box数量
    Box* root;
} BoxTree;

BoxTree* mm_alloc_boxtree();  // 创建box多叉树
Box* mm_alloc_box();  // 创建box
int mm_find_stream_info(BoxTree* box_tree, const char* file_path);  // 解析和获取所有box信息
int mm_find_stream_info_simple(BoxTree* box_tree, const char* file_path);  // 仅仅解析size和type
Box* mm_get_first_box_by_name(BoxTree* box_tree, char* box_name);  // 根据box的类型返回box，如果有多个，则只返回匹配的第一个box
void mm_traverse_box(BoxTree* box_tree);  // 遍历box树的所有box
void mm_free_boxtree(BoxTree* box_tree);  // 释放所有内容