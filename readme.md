# FFMPEG��������
## MP4�ļ�����
1. MP4�ļ���ʾ���ݽṹ��BoxTree
2. ���в����������㷨��  
	1. ����box�������BoxTree* mm_alloc_boxtree()  
	2. ����һ��Box��Box* mm_alloc_box();  
	3. �����ͻ�ȡ����box��Ϣ��int mm_find_stream_info(BoxTree* box_tree, const char* file_path);    
	4. ��������size��type��int mm_find_stream_info_simple(BoxTree* box_tree, const char* file_path);    
	5. ����box������box�����ͷ���box������ж������ֻ����ƥ��ĵ�һ��box��Box* mm_get_first_box_by_name(BoxTree* box_tree, char* box_name);    
	6. ����box��������box��void mm_traverse_box(BoxTree* box_tree);  
	7. �ͷ��������ݣ�void mm_free_boxtree(BoxTree* box_tree);    
3. ע�⣺Ŀǰ����ftype box�н�Ϊ��ϸ�Ľ����⣬����box�������Խ�����������Ϊ�����Ľ���MP4�ļ�����������չ��  

## PNGͼ�����