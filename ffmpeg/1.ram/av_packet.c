#include <stdio.h>
#include "av_packet.h"
#include <libavcodec/avcodec.h>

// gcc av_packet.c main.c  -L/home/xzx/ffmpeg_build/lib/  -I/home/xzx/ffmpeg_build/include  -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lswresample -lswscale -lpostproc -lfdk-aac

// new and alloc
int av_packet_test1()
{
    int cnt = 0;

    AVPacket *pack = av_packet_alloc();
    if (pack == NULL) {
        return -1;
    }

    av_init_packet(pack);
    av_new_packet(pack, 1024); ///1.new 时才真正申请内存，才能去get_cnt
    //av_init_packet(pack); ///2.new完若调用init,则改变了buf的指向，造成内存泄漏

    cnt = av_buffer_get_ref_count(pack->buf);
    printf("packet ref cnt:%d\n", cnt);

    /// 3.ref后，引用计数+1.pack2和pack使用同一个buf
    AVPacket *pack2 = av_packet_alloc();
    if (pack2 == NULL) {
        return -1;
    }
    av_packet_ref(pack2, pack);
    cnt = av_buffer_get_ref_count(pack->buf);
    printf("packe1 ref cnt:%d\n", cnt);
    cnt = av_buffer_get_ref_count(pack2->buf);
    printf("packe2 ref cnt:%d\n", cnt);

    av_packet_unref(pack2);
    cnt = av_buffer_get_ref_count(pack->buf);
    printf("after unref packe ref cnt:%d\n", cnt);
    /// unref 完后，buf内容无了，不能再使用
    // cnt = av_buffer_get_ref_count(pack2->buf);
    // printf("after unref packe2 ref cnt:%d\n", cnt);

    /// clone完 相当于alloc+ref，计数+1
    AVPacket *pack3 = av_packet_clone(pack);
    cnt = av_buffer_get_ref_count(pack3->buf);
    printf("after unref pack3 ref cnt:%d\n", cnt);

    av_packet_free(&pack);
    av_packet_free(&pack2);
    av_packet_free(&pack3);

    printf("test successfully\n");
    return 0;
}