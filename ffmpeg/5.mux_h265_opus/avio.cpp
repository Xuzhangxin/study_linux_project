#include <stdio.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>

int main() {
    // 初始化FFmpeg库
    av_register_all();

    // 打开输入文件 (H.265流)
    const char* videoInputFile = "input.h265";
    AVFormatContext* videoFormatContext = avformat_alloc_context();
    if (avformat_open_input(&videoFormatContext, videoInputFile, NULL, NULL) < 0) {
        return -1;
    }

    // 打开输入文件 (Opus音频流)
    const char* audioInputFile = "input.opus";
    AVFormatContext* audioFormatContext = avformat_alloc_context();
    if (avformat_open_input(&audioFormatContext, audioInputFile, NULL, NULL) < 0) {
        return -1;
    }

    // 创建内存输出格式上下文
    AVOutputFormat* outputFormat = av_guess_format("mp4", NULL, NULL);
    if (!outputFormat) {
        return -1;
    }

    AVFormatContext* outputFormatContext = avformat_alloc_context();
    outputFormatContext->oformat = outputFormat;

    // 创建内存输出流
    AVIOContext* avio = NULL;
    unsigned char* buffer = NULL;
    size_t buffer_size = 4096;  // 初始缓冲区大小
    buffer = (unsigned char*)malloc(buffer_size);
    if (!buffer) {
        return -1;
    }

    avio = avio_alloc_context(buffer, buffer_size, 1, NULL, NULL, NULL, NULL);
    outputFormatContext->pb = avio;

    // 添加视频和音频流到输出容器（需要设置适当的编解码器和参数）

    // 写容器头
    if (avformat_write_header(outputFormatContext, NULL) < 0) {
        return -1;
    }

    // 初始化数据存储相关变量
    AVPacket packet;

    // 迭代从输入流中读取数据，逐帧，编码，写入内存缓冲区
    while (av_read_frame(videoFormatContext, &packet) >= 0) {
        // 处理视频帧数据，将其写入内存缓冲区
        // 您需要适当的编码和写入操作

        // 如果缓冲区不足，扩展缓冲区的大小
        if (avio->buf_end - avio->buf_ptr < packet.size) {
            size_t new_buffer_size = buffer_size * 2;
            buffer = (unsigned char*)realloc(buffer, new_buffer_size);
            if (!buffer) {
                return -1;
            }

            avio = avio_alloc_context(buffer, new_buffer_size, 1, NULL, NULL, NULL, NULL);
            outputFormatContext->pb = avio;
        }

        avio_write(avio, packet.data, packet.size);
        av_packet_unref(&packet);
    }

    // 写容器尾部
    if (av_write_trailer(outputFormatContext) < 0) {
        return -1;
    }

    // 处理内存中的数据，根据需要

    // 清理资源
    avformat_close_input(&videoFormatContext);
    avformat_close_input(&audioFormatContext);
    avformat_free_context(videoFormatContext);
    avformat_free_context(audioFormatContext);
    avformat_free_context(outputFormatContext);
    av_free(avio);

    free(buffer);

    return 0;
}
