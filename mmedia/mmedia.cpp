#include <stdio.h>

extern "C" {
#include "mp4.h"
}

static void test_mp4() {
    const char* file_path = "F:/迅雷下载/Gangs.of.London.S02E01.1080p.WEBRip.x265-RARBG.chs.eng.mp4";

    BoxTree* box_tree = mm_alloc_boxtree();
    if (!box_tree) {
        printf("box_tree error.");
        return;
    }
    if (mm_find_stream_info(box_tree, file_path) <= 0) {
        printf("find stream info error.");
        return;
    }
    printf("Tree Size:                              %d\n", box_tree->size);
    printf("Root Children:                          %d\n", box_tree->root->children);
    printf("Root Address:                           0x%p\n\n", box_tree->root);
    mm_traverse_box(box_tree);
    mm_free_boxtree(box_tree);
}

int main() {
    test_mp4();
    return 0;
}
