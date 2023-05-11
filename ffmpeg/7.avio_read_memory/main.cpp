
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/file.h>
}

#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>

#define my_printf(fmt, ...) myprintf(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

void myprintf(const char *file, int line, const char *__restrict fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    time_t curr_time;
    struct tm* curr_tm;
    char time_str[21];

    curr_time = time(NULL);
    curr_tm = localtime(&curr_time);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", curr_tm);

    struct timeval tv;
    gettimeofday(&tv, NULL);
    long ms = tv.tv_usec / 1000;

    printf("[%s.%03ld] %s:%d: ", time_str, ms, file, line);
    vprintf(fmt, args);
    printf("\n");
    fflush(stdout);

    va_end(args);
}

//自定义缓冲区
struct buffer_data {
    uint8_t *ptr;
    size_t size; ///< size left in the buffer
};

//读取数据（回调函数）
static int read_packet(void *opaque, uint8_t *buf, int buf_size)
{
    struct buffer_data *bd = (struct buffer_data *)opaque;
    buf_size = FFMIN(buf_size, bd->size);

    if (!buf_size)
        return AVERROR_EOF;
    printf("ptr:%p size:%d\n", bd->ptr, bd->size);

    /// 灵活应用[内存buf]：读取的是内存，将内容拷贝到ffmpeg的缓冲区
    memcpy(buf, bd->ptr, buf_size);
    bd->ptr  += buf_size;
    bd->size -= buf_size;

    return buf_size;
}

int main(int argc, char *argv[])
{
    AVFormatContext *fmt_ctx = NULL;
    AVIOContext *avio_ctx = NULL;
    uint8_t *buffer = NULL, *avio_ctx_buffer = NULL;
    size_t buffer_size, avio_ctx_buffer_size = 4096;
    char *input_filename = NULL;
    int ret = 0;
    struct buffer_data bd = { 0 };

    input_filename = "../filesource/believe.mp4";

    ///1.将文件映射到内存buffer中，并用结构体记录
    ret = av_file_map(input_filename, &buffer, &buffer_size, 0, NULL);
    if (ret < 0)
        goto end;
    bd.ptr  = buffer;
    bd.size = buffer_size;

    /// 创建对象：AVFormatContext
    if (!(fmt_ctx = avformat_alloc_context())) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    /// 分配内存
    avio_ctx_buffer = (uint8_t *)av_malloc(avio_ctx_buffer_size);
    if (!avio_ctx_buffer) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    /// 2.创建AVIOContext对象：其中read_packet为数据读取的回调，avio_ctx_buffer为ffmpeg内部使用的内存
    avio_ctx = avio_alloc_context(
                avio_ctx_buffer, avio_ctx_buffer_size,
                                  0,
                &bd,
                &read_packet,
                NULL,
                NULL);
    if (!avio_ctx) {
        ret = AVERROR(ENOMEM);
        goto end;
    }
    fmt_ctx->pb = avio_ctx; ///< 3.AVIO 的context赋值到fAVFormatContext的pb成员
    fmt_ctx->flags = AVFMT_FLAG_CUSTOM_IO;
    printf("avio_alloc_context,ok\n");

    /// 4.打开输入流
    ret = avformat_open_input(&fmt_ctx, NULL, NULL, NULL);
    if (ret < 0) {
        fprintf(stderr, "Could not open input\n");
        goto end;
    }
    printf("avformat_open_input,ok\n");

    ///5. 查找流信息
    ret = avformat_find_stream_info(fmt_ctx, NULL);
    if (ret < 0) {
        fprintf(stderr, "Could not find stream information\n");
        goto end;
    }
    printf("avformat_find_stream_info,ok\n");
    printf("******nb_streams=%d\n",fmt_ctx->nb_streams);

    av_dump_format(fmt_ctx, 0, input_filename, 0);

end:
    avformat_close_input(&fmt_ctx);

    if (avio_ctx)
        av_freep(&avio_ctx->buffer);
    avio_context_free(&avio_ctx);

    /// 内存映射文件：解绑定
    av_file_unmap(buffer, buffer_size);

    if (ret < 0) {
        fprintf(stderr, "Error occurred: %d\n", ret);
        return 1;
    }

    return 0;
}