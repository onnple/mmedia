#include <stdio.h>

extern "C" {
#include "mp4.h"
}

// 测试MP4文件解析
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

// 测试当前CPU是大端还是小端
static void test_cpu_endian() {
    short int x = 0x1122;;
    char x1 = *((char*)&x), x2 = *((char*)&x + 1);
    printf("x1 = 0x%x, x2 = 0x%x\n", x1, x2);
    if (x1 == 0x11)
        printf("big-endian\n");
    else if (x1 == 0x22)
        printf("little-endian\n");
    else {
        printf("unknown\n");
    }
}

// 颜色值和位处理：提取颜色分量和逆序颜色值
static void test_color() {
    // 储存的颜色为bgra
    int color = 0x783251ff;
    int alpha = color & 0x000000ff;
    int red = (color & 0x0000ff00) >> 8;
    int green = (color & 0x00ff0000) >> 16;
    int blue = (color & 0xff000000) >> 24;
    int argb = (alpha << 24) | (red << 16) | (green << 8) | blue;
    printf("a = 0x%x, r = 0x%x, g = 0x%x, b = 0x%x\n", alpha, red, green, blue);
    printf("argb = 0x%x\n", argb);
}

// 测试PNG图像解析
static void test_png() {
    
}

int main() {
    //test_mp4();
    //test_cpu_endian();
    test_color();
    test_png();
    return 0;
}
