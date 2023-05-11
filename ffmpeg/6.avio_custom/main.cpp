
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

int read_func(void* ptr, uint8_t* buf, int buf_size)
{
    FILE* fp = (FILE*)ptr;
    size_t size = fread(buf, 1, buf_size, fp);
    int ret = size;
    my_printf("Read Bytes:%d, buf_size:%d", size, buf_size);
    return ret;

}


int64_t seek_func(void *opaque, int64_t offset, int whence)
{
    int64_t ret;
    FILE *fp = (FILE*)opaque;
    if (whence == AVSEEK_SIZE) {
        return -1;
    }

    fseek(fp, offset, whence);

    return ftell(fp);

}


int main(int argc, char *argv[]){
    AVInputFormat *inputFormat = NULL;

    unsigned char *buffer  = (unsigned char *)av_mallocz(32 * 1024); ///< 参考vlc  使用32k的buffer
    if (NULL == buffer)
    {
        my_printf("Error malloc format");
        return -1;
    }

    FILE *fp = fopen("../filesource/believe.mp4", "rw");
    if (NULL == fp)
    {
        my_printf("Error fopen");
        return -1;
    }
    AVIOContext *ioCtx = avio_alloc_context(buffer, 32 * 1024, 0, fp, read_func, NULL, seek_func);
    if (NULL == ioCtx)
    {
        my_printf("Error malloc ioCtx");
        return -1;
    }

    AVFormatContext *formatCtx = avformat_alloc_context();
    if (NULL == formatCtx)
    {
        my_printf("Error malloc formatCtx");
        return -1;
    }
    formatCtx->pb = ioCtx;
    formatCtx->flags = AVFMT_FLAG_CUSTOM_IO;
    int ret = avformat_open_input(&formatCtx, "", inputFormat, NULL);
    if (ret < 0)
    {
        my_printf("Error opening input format");
        return -1;
    }

    ret = avformat_find_stream_info(formatCtx, NULL);
    if (ret < 0)
    {
        my_printf("Error opening find_stream_info");
        return -1;
    }
    my_printf("find stream info success, nb_streams:%d", formatCtx->nb_streams);

EXIT:
    avformat_close_input(&formatCtx); ///< 内存会由close input释放
    return 0;
}
