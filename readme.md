# FFMPEG ��������
## MP4����
1. MP4�ļ���ʾ���ݽṹ��BoxTree
2. ���в����������㷨��  
BoxTree* mm_alloc_boxtree();  // ����box�����  

Box* mm_alloc_box();  // ����box  

int mm_find_stream_info(BoxTree* box_tree, const char* file_path);  // �����ͻ�ȡ����box��Ϣ  

int mm_find_stream_info_simple(BoxTree* box_tree, const char* file_path);  // ��������size��type  

Box* mm_get_first_box_by_name(BoxTree* box_tree, char* box_name);  // ����box�����ͷ���box������ж������ֻ����ƥ��ĵ�һ��box  

void mm_traverse_box(BoxTree* box_tree);  // ����box��������box  

void mm_free_boxtree(BoxTree* box_tree);  // �ͷ���������  


3. ע�⣺Ŀǰ����ftype box�н�Ϊ��ϸ�Ľ����⣬����box�������Խ�����������Ϊ�����Ľ���MP4�ļ��������ж��塣  
