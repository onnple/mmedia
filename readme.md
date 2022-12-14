# FFMPEG开发基础
## MP4文件解析
1. MP4文件表示数据结构：BoxTree
2. 所有操作函数或算法：  
	1. 创建box多叉树：BoxTree* mm_alloc_boxtree()  
	2. 创建一个Box：Box* mm_alloc_box();  
	3. 解析和获取所有box信息：int mm_find_stream_info(BoxTree* box_tree, const char* file_path);    
	4. 仅仅解析size和type：int mm_find_stream_info_simple(BoxTree* box_tree, const char* file_path);    
	5. 查找box，根据box的类型返回box，如果有多个，则只返回匹配的第一个box：Box* mm_get_first_box_by_name(BoxTree* box_tree, char* box_name);    
	6. 遍历box树的所有box：void mm_traverse_box(BoxTree* box_tree);  
	7. 释放所有内容：void mm_free_boxtree(BoxTree* box_tree);    
3. 注意：目前除了ftype box有较为详细的解析外，其它box仅作粗略解析，如果想较为完整的解析MP4文件，请自行扩展。  

## PNG图像解析
1. PNG文件表示数据结构：PNG
2. 所有操作函数或算法：   
	1. 创建一个PNG结构体：PNG* png_alloc();
	2. 创建一个Chunk数据块结构体：Chunk* png_alloc_chunk();
	3. 创建一个IHDRChunk：IHDRChunk* png_alloc_ihdr_chunk();
	4. 创建一个PLTEChunk：PLTEChunk* png_alloc_plte_chunk();
	5. 解析所有Chunk：int png_find_chunk_info(PNG* png, const char* file_path); 
	6. 遍历chunk：void png_traverse_chunk(PNG* png);
	7. 释放所有内容：void png_free_png(PNG* png);  
3. 注意：目前仅仅简单解析数据块的基本内容，如需更详细的解析，可参考PNG文件规范自行重构代码。