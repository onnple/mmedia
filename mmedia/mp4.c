#pragma warning(disable : 4996)
#include "mp4.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

BoxTree* mm_alloc_boxtree() {
	BoxTree* box_tree = (BoxTree*)malloc(sizeof(BoxTree));
	if (box_tree == NULL) {
		printf("malloc box_tree NULL.\n");
		return NULL;
	}
	box_tree->size = 0;
	Box* root = mm_alloc_box();
	if (root == NULL) {
		printf("alloc root box NULL.\n");
		free(box_tree);
		return NULL;
	}
	root->has_children = 1;
	box_tree->root = root;
	return box_tree;
}

Box* mm_alloc_box() {
	Box* box = (Box*)malloc(sizeof(Box));
	if (box == NULL) {
		printf("alloc box NULL.\n");
		return NULL;
	}

	Header* header = (Header*)malloc(sizeof(Header));
	if (header == NULL) {
		printf("alloc header NULL.\n");
		free(box);
		return NULL;
	}
	header->size = header->largsize = header->version = 0;
	header->type = header->extended_type = header->flags = NULL;
	header->start_position = header->end_position = 0;

	box->header = header;
	box->children = box->has_children = 0;
	box->body = box->child = box->next = NULL;
	return box;
}

/*
1. root != NULL && s >= 0，在文件中从s开始解析
返回：本次解析的最顶层root box，若root != NULL，则直接返回root
解析：仅仅解析size和type，其它均省略
**/
static int mm_resolve_box(FILE *fp, BoxTree* box_tree, Box* root) {
	if (!fp || !root || !box_tree) {
		printf("empty input error.\n");
		return -1;
	}
	if (!root->has_children)
		return 0;

	int box_size = root->header->end_position;
	if (box_size == 0) {
		fseek(fp, 0L, SEEK_END);
		box_size = ftell(fp);
		fseek(fp, 0L, SEEK_SET);
	}

	int p = root->header->start_position;
	if (root->header->type) {
		if (strcmp(root->header->type, "stsd") == 0)
			p += 16;
		else if (strcmp(root->header->type, "avc1") == 0 || strcmp(root->header->type, "mp4a") == 0 ||
			strcmp(root->header->type, "meta") == 0)
			//p += 32;  // 此处尚未确定这三种box头的标准字节长度是多少，暂时忽略掉
			return -1;
		else
			p += 8;
	}
	// 获取根box
	while (p < box_size) {
		fseek(fp, p, SEEK_SET);
		Box* root_box = mm_alloc_box();
		if (!root_box) {
			printf("alloc box error.\n");
			return -1;
		}

		// 1. size
		int size = 0;
		fread(&size, 4, 1, fp);
		size = from_hex4(size);

		// 2. type
		char* type = new_str(4);
		fread(type, 4, 1, fp);

		root_box->header->size = size;
		root_box->header->type = type;
		root_box->header->start_position = p;

		if (strcmp(type, "moov") == 0 || strcmp(type, "trak") == 0 || strcmp(type, "udta") == 0 ||
			strcmp(type, "meta") == 0 || strcmp(type, "ilst") == 0 || strcmp(type, "edts") == 0 ||
			strcmp(type, "mdia") == 0 || strcmp(type, "minf") == 0 || strcmp(type, "dinf") == 0 ||
			strcmp(type, "stbl") == 0 || strcmp(type, "stsd") == 0 || strcmp(type, "avc1") == 0 ||
			strcmp(type, "mp4a") == 0) {
			root_box->has_children = 1;
		}

		Box* root_child = root->child;
		if (!root_child)
			root->child = root_box;
		else {
			Box* temp = root_child;
			while (temp) {
				if (!temp->next) {
					temp->next = root_box;
					break;
				}
				temp = temp->next;
			}
		}
		root->children++;
		box_tree->size++;
		p += size;
		root_box->header->end_position = p;
		mm_resolve_box(fp, box_tree, root_box, p);
	}
	
	return 1;
}

int mm_find_stream_info(BoxTree* box_tree, const char* file_path) {
	FILE* fp = fopen(file_path, "rb");
	if (!fp) {
		printf("read file error.\n");
		return -1;
	}

	fseek(fp, 0L, SEEK_END);
	int f_size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	int p = 0;
	// 获取根box
	while (p < f_size) {
		Box* root_box = mm_alloc_box();
		if (!root_box) {
			printf("alloc box error.\n");
			return -1;
		}

		// 1. size
		int size = 0;
		fread(&size, 4, 1, fp);
		size = from_hex4(size);

		// 2. type
		char* type = new_str(4);
		fread(type, 4, 1, fp);

		root_box->header->size = size;
		root_box->header->type = type;
		root_box->header->start_position = p;

		if (strcmp(type, "moov") == 0) {
			root_box->has_children = 1;
		}

		if (strcmp(type, "ftyp") == 0) {
			Body* body = (Body*)malloc(sizeof(Body));
			if (body) {
				body->major_brand = 0;
				body->minor_version = 0;
				body->compatible_brands_len = 0;
				body->compatible_brands = NULL;

				char* major_brand = new_str(4);
				fread(major_brand, 4, 1, fp);
				body->major_brand = major_brand;
				int minor_version = 0;
				fread(&minor_version, 4, 1, fp);
				body->minor_version = from_hex4(minor_version);

				while (ftell(fp) - p < size) {
					char* brand = new_str(4);
					fread(brand, 4, 1, fp);
					if (!body->compatible_brands) {
						body->compatible_brands = (char**)malloc(sizeof(char*));
						if (!body->compatible_brands)
							continue;
						body->compatible_brands[0] = brand;
						body->compatible_brands_len = 1;
						continue;
					}
					else {
						char** brands_tmp = (char**)realloc(body->compatible_brands, (body->compatible_brands_len + 1) * sizeof(char*));
						if (!brands_tmp)
							continue;
						body->compatible_brands = brands_tmp;
						body->compatible_brands[body->compatible_brands_len] = brand;
						body->compatible_brands_len++;
					}
				}

				root_box->body = body;
			}
		}

		Box* root_child = box_tree->root->child;
		if(!root_child) 
			box_tree->root->child = root_box;
		else {
			Box* temp = root_child;
			while (temp) {
				if (!temp->next) {
					temp->next = root_box;
					break;
				}
				temp = temp->next;
			}
		}
		box_tree->root->children++;
		box_tree->size++;
		p += size;
		root_box->header->end_position = p;
		fseek(fp, p, SEEK_SET);
	}

	// 指定一个root box进行进一步解析, 下面不再解析更多字段细节, 只解析头部
	Box* moov_box = mm_get_first_box_by_name(box_tree, "moov");
	mm_resolve_box(fp, box_tree, moov_box);
	
	fclose(fp);
	return 1;
}

int mm_find_stream_info_simple(BoxTree* box_tree, const char* file_path) {
	FILE* fp = fopen(file_path, "rb");
	if (!fp) {
		printf("read file error.\n");
		return -1;
	}
	mm_resolve_box(fp, box_tree, box_tree->root);
	fclose(fp);
	return 1;
}

static Box* _get_first_box_by_name(Box* root, const char* name) {
	if (!root)
		return NULL;
	Box* child = root->child;
	if (child) {
		Box* next = child;
		while (next) {
			if (strcmp(name, next->header->type) == 0)
				return next;
			Box* target = _get_first_box_by_name(next, name);
			if (target) return target;
			next = next->next;
		}
	}
	return NULL;
}

// 根据box名称查找box
Box* mm_get_first_box_by_name(BoxTree* box_tree, char* box_name) {
	if (!box_tree || box_tree->size == 0 || !box_tree->root || !box_name) {
		printf("empty input error.\n");
		return;
	}
	Box* root = box_tree->root;
	return _get_first_box_by_name(root, box_name);
}

static void _traverse_box(Box* root) {
	if (!root)
		return;
	Box* child = root->child;
	if (child) {
		Box* next = child;
		while (next) {
			if (next->header->type)					printf("Box Type:				%s\n", next->header->type);
			if (next->header->size)					printf("Box Size:				%d\n", next->header->size);
													printf("Start Position:				%d\n", next->header->start_position);
													printf("End Position:				%d\n", next->header->end_position);
			if (next->header->largsize)				printf("Box Size:				%d\n", next->header->largsize);
			if (next->header->extended_type)		printf("Box Extended Type:		%s\n", next->header->extended_type);
			if (next->header->version)				printf("Box Version:			%d\n", next->header->version);
			if (next->header->flags)				printf("Box Flags:				%s\n", next->header->flags);

			if (next->body && next->body->major_brand >= 0)		printf("Major Brand:				%s\n", next->body->major_brand);
			if (next->body && next->body->minor_version >= 0)		printf("Minor Version:				%d\n", next->body->minor_version);
			for (int i = 0; next->body && i < next->body->compatible_brands_len; i++) {
													printf("compatible_brands.%d:			%s\n", i, next->body->compatible_brands[i]);
			}

			printf("\n");
			_traverse_box(next);
			next = next->next;
		}
	}
}

// DFS
void mm_traverse_box(BoxTree* box_tree) {
	if (!box_tree || box_tree->size == 0 || !box_tree->root) {
		printf("empty box tree\n");
		return;
	}
	Box* root = box_tree->root;
	_traverse_box(root);
}

static void _free_box(Box* box) {
	Header* header = box->header;
	Body* body = box->body;
	if (header) {
		if (header->type) free(header->type);
		if (header->extended_type) free(header->extended_type);
		if (header->flags) free(header->flags);
		free(header);
	}
	if (body) {
		if (body->major_brand) free(body->major_brand);
		if (body->compatible_brands) {
			for (int i = 0; i < body->compatible_brands_len; i++) {
				free(body->compatible_brands[i]);
			}
			free(body->compatible_brands);
		}
		free(body);
	}
	free(box);
}

static void _free_boxtree(Box* root) {
	if (!root)
		return;
	Box* child = root->child;
	if (child) {
		Box* next = child;
		while (next) {
			Box* temp = next->next;
			_traverse_box(next);
			_free_box(next);
			next = temp;
		}
	}
}

// 释放所有malloc分配的内存空间
void mm_free_boxtree(BoxTree* box_tree) {
	if (!box_tree || box_tree->size == 0 || !box_tree->root) {
		free(box_tree);
		return;
	}
	Box* root = box_tree->root;
	_free_boxtree(root);
	_free_box(root);
	free(box_tree);
}