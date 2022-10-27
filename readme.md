# FFMPEG 开发基础
## MP4解析
1. MP4文件表示数据结构：BoxTree
2. 所有操作函数或算法：  
BoxTree* mm_alloc_boxtree();  // 创建box多叉树  

Box* mm_alloc_box();  // 创建box  

int mm_find_stream_info(BoxTree* box_tree, const char* file_path);  // 解析和获取所有box信息  

int mm_find_stream_info_simple(BoxTree* box_tree, const char* file_path);  // 仅仅解析size和type  

Box* mm_get_first_box_by_name(BoxTree* box_tree, char* box_name);  // 根据box的类型返回box，如果有多个，则只返回匹配的第一个box  

void mm_traverse_box(BoxTree* box_tree);  // 遍历box树的所有box  

void mm_free_boxtree(BoxTree* box_tree);  // 释放所有内容  


3. 注意：目前除了ftype box有较为详细的解析外，其它box仅作粗略解析，如果想较为完整的解析MP4文件，请自行定义。  
