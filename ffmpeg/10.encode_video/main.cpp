/**
* @projectName   encode_video
* @brief         视频编码，从本地读取YUV数据进行H264编码
* @author        Xu ZhangXin
* @date          2023-6-04
*/

#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/time.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}


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

int64_t get_time()
{
    return  av_gettime_relative() / 1000;  // 换算成毫秒
}

static int encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt,
                  FILE *outfile)
{
    int iRet;

    /* send the frame to the encoder */
    if (frame)
        printf("Send frame %3"PRId64"\n", frame->pts);
    /* 通过查阅代码，使用x264进行编码时，具体缓存帧是在x264源码进行，
     * 不会增加avframe对应buffer的reference*/
    iRet = avcodec_send_frame(enc_ctx, frame);
    if (iRet < 0)
    {
        fprintf(stderr, "Error sending a frame for encoding\n");
        return  -1;
    }

    while (iRet >= 0)
    {
        iRet = avcodec_receive_packet(enc_ctx, pkt);
        if (iRet == AVERROR(EAGAIN) || iRet == AVERROR_EOF) {
            return  0;
        } else if (iRet < 0) {
            fprintf(stderr, "Error encoding audio frame\n");
            return  -1;
        }

        if(pkt->flags & AV_PKT_FLAG_KEY)
            printf("Write packet flags:%d pts:%3"PRId64" dts:%3"PRId64" (size:%5d)\n",
               pkt->flags, pkt->pts, pkt->dts, pkt->size);
        if(!pkt->flags)
            printf("Write packet flags:%d pts:%3"PRId64" dts:%3"PRId64" (size:%5d)\n",
               pkt->flags, pkt->pts, pkt->dts, pkt->size);
        fwrite(pkt->data, 1, pkt->size, outfile);
    }
    return  0;
}

/**
 * @brief 提取测试文件命令：ffmpeg -i test_1280x720.flv -t 5 -r 25 -pix_fmt yuv420p yuv420p_1280x720.yuv
 * @param argc
 * @param argv
 * @return 
 */
int main(int argc, char **argv)
{
    char *inputYuvFile = "yuv420p_1280x720.yuv";
    char *outputH264File = "yuv420p_1280x720.h264";
    const char *codecName = "libx264";

    FILE *infile = NULL;
    FILE *outfile = NULL;

    const AVCodec *codec = NULL;
    AVCodecContext *codecCtx= NULL;
    AVFrame *frame = NULL;
    AVPacket *pkt = NULL;
    int iRet = 0;
    int iFrameIndex = 0;
    /// 查找指定编码器
    codec = avcodec_find_encoder_by_name(codecName);
    if (!codec) {
        fprintf(stderr, "Codec '%s' not found\n", codecName);
        exit(1);
    }

    codecCtx = avcodec_alloc_context3(codec);
    if (!codecCtx) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }


    /// 设置分辨率
    codecCtx->width = 1280;
    codecCtx->height = 720;

    /// 设置time base
    codecCtx->time_base = (AVRational){1, 25};
    codecCtx->framerate = (AVRational){25, 1};

    /// 帧率是25fps，GOP=25，则1s一个I帧
    codecCtx->gop_size = 25;   ///< I帧间隔
    /// B帧间隔。 如果不想包含B帧则设置为0，直播中一般不设置B帧,会造成延迟。因为B帧是双向参考帧，需要得出
    codecCtx->max_b_frames = 2;
    codecCtx->pix_fmt = AV_PIX_FMT_YUV420P; ///< 图像源格式yuv420p

    //
    if (codec->id == AV_CODEC_ID_H264) {
        /// 相关的参数可以参考libx264.c的 AVOption options
        iRet = av_opt_set(codecCtx->priv_data, "preset", "medium", 0);
        if(iRet != 0) {
            printf("av_opt_set preset failed\n");
        }
        iRet = av_opt_set(codecCtx->priv_data, "profile", "main", 0); // 默认是high
        if(iRet != 0) {
            printf("av_opt_set profile failed\n");
        }
        iRet = av_opt_set(codecCtx->priv_data, "tune","zerolatency",0); // 直播是才使用该设置
        if(iRet != 0) {
            printf("av_opt_set tune failed\n");
        }
    }

    /// 设置编码器参数
    codecCtx->bit_rate = 3000000;
//    codecCtx->rc_max_rate = 3000000;
//    codecCtx->rc_min_rate = 3000000;
//    codecCtx->rc_buffer_size = 2000000;
//    codecCtx->thread_count = 4;  // 开了多线程后也会导致帧输出延迟, 需要缓存thread_count帧后再编程。
//    codecCtx->thread_type = FF_THREAD_FRAME; // 并 设置为FF_THREAD_FRAME

    /// 设置GLOBAL_HEDER参数后，I帧的sps、pps、spi信息都存储在extra_data，此时如果存到本地文件，则需要手动将extra_data内容写入到I帧之前
    /// 而如果不设置，本地文件中的，每个I帧都带sps、ppc、spi信息
//    codecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    /// 绑定codec和codecCtx
    iRet = avcodec_open2(codecCtx, codec, NULL);
    if (iRet < 0) {
        printf("Could not open codec");
        exit(1);
    }
    printf("thread_count: %d, thread_type:%d\n", codecCtx->thread_count, codecCtx->thread_type);
    /// 打开输入和输出文件
    infile = fopen(inputYuvFile, "rb");
    if (!infile) {
        fprintf(stderr, "Could not open %s\n", inputYuvFile);
        exit(1);
    }
    outfile = fopen(outputH264File, "wb");
    if (!outfile) {
        fprintf(stderr, "Could not open %s\n", outputH264File);
        exit(1);
    }

    /// 分配pkt和frame
    pkt = av_packet_alloc();
    if (!pkt) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }

    /// 为frame分配buffer
    frame->format = codecCtx->pix_fmt;
    frame->width  = codecCtx->width;
    frame->height = codecCtx->height;
    iRet = av_frame_get_buffer(frame, 0);
    if (iRet < 0) {
        fprintf(stderr, "Could not allocate the video frame data\n");
        exit(1);
    }
    // 计算出每一帧的数据 像素格式 * 宽 * 高
    // 1382400
    int frame_bytes = av_image_get_buffer_size((AVPixelFormat)frame->format, frame->width,
                                               frame->height, 1);
    printf("frame_bytes %d\n", frame_bytes);
    uint8_t *yuv_buf = (uint8_t *)malloc(frame_bytes);
    if(!yuv_buf) {
        printf("yuv_buf malloc failed\n");
        return 1;
    }
    int64_t begin_time = get_time();
    int64_t end_time = begin_time;
    int64_t all_begin_time = get_time();
    int64_t all_end_time = all_begin_time;
    int64_t pts = 0;
    printf("start enode\n");
    for (;;) {
        memset(yuv_buf, 0, frame_bytes);
        size_t read_bytes = fread(yuv_buf, 1, frame_bytes, infile);
        if(read_bytes <= 0) {
            printf("read file finish\n");
            break;
        }
        /// 确保该frame可写, 如果编码器内部保持了内存参考计数（在使用这片内存），则需要重新拷贝一个备份,目的是新写入的数据和编码器保存的数据不能产生冲突
        int frame_is_writable = 1;
        if(av_frame_is_writable(frame) == 0) { // 这里只是用来测试
            printf("the frame can't write, buf:%p\n", frame->buf[0]);
            if(frame->buf && frame->buf[0])        // 打印referenc-counted，必须保证传入的是有效指针
                printf("ref_count1(frame) = %d\n", av_buffer_get_ref_count(frame->buf[0]));
            frame_is_writable = 0;
        }
        iRet = av_frame_make_writable(frame);
        if(frame_is_writable == 0) {  // 这里只是用来测试
            printf("av_frame_make_writable, buf:%p\n", frame->buf[0]);
            if(frame->buf && frame->buf[0])        // 打印referenc-counted，必须保证传入的是有效指针
                printf("ref_count2(frame) = %d\n", av_buffer_get_ref_count(frame->buf[0]));
        }
        if(iRet != 0) {
            printf("av_frame_make_writable failed, iRet = %d\n", iRet);
            break;
        }
        int need_size = av_image_fill_arrays(frame->data, frame->linesize, yuv_buf,
                                             (AVPixelFormat)frame->format,
                                             frame->width, frame->height, 1);
        if(need_size != frame_bytes) {
            printf("av_image_fill_arrays failed, need_size:%d, frame_bytes:%d\n",
                   need_size, frame_bytes);
            break;
        }

        /// yuv无输入封装格式，需要手动计算pts
        /// AV_TIME_BASE 在ffmpeg内部，1s = AV_TIME_BASE个时间刻度，一个刻度占1/AV_TIME_BASE秒
        /// 1s = 25帧
        /// 两帧间的持续时间(us) =  1/25 * AV_TIME_BASE，同时他也代表在ffmpeg内部所占的时间戳刻度
        int64_t calDuration = (double)AV_TIME_BASE / av_q2d(codecCtx->framerate);                          /// av_rescale_q中，其中用的都是倒数去计算

        /// 设置pts,此时pts的单位转化为输入流的时间戳刻度
        /// 由ffmepeg内部的时间戳转化为输入流时间戳     ，输入流时间戳 = ffmpeg内部时间戳* (codecCtx->time_base/AV_TIME_BASE)
        /// 而codecCtx->time_base为AVRational格式，所以需要经过转换，即 ffmpeg内部时间戳 /(av_q2d(codecCtx->time_base) * AV_TIME_BASE)
        frame->pts=(double)(iFrameIndex * calDuration)/(double)(av_q2d(codecCtx->time_base) * AV_TIME_BASE);

        /// 未写输出流，所以暂时不用转换到输出流时间基
		// frame->pts = av_rescale_q_rnd(frame->pts, codecCtx->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));

        begin_time = get_time();
        iRet = encode(codecCtx, frame, pkt, outfile);
        end_time = get_time();
        printf("encode time:%lldms\n", end_time - begin_time);
        if(iRet < 0) {
            printf("encode failed\n");
            break;
        }

        iFrameIndex++;
    }

    /* 冲刷编码器 */
    encode(codecCtx, NULL, pkt, outfile);
    all_end_time = get_time();
    printf("all encode time:%lldms\n", all_end_time - all_begin_time);
    // 关闭文件
    fclose(infile);
    fclose(outfile);

    // 释放内存
    if(yuv_buf) {
        free(yuv_buf);
    }

    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&codecCtx);

    printf("main finish, please enter Enter and exit\n");
    getchar();
    return 0;
}
