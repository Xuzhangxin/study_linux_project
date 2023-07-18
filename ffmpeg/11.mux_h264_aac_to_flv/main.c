/**
* @projectName   mux_h264_aac_to_flv
* @brief         h264+aac封装为flv
* @author        Xu ZhangXin
* @date          2023-6-18
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>


#define STREAM_DURATION   5.0  //流时长 单位秒
#define STREAM_FRAME_RATE 25 /* 25 images/s */
#define STREAM_PIX_FMT    AV_PIX_FMT_YUV420P /* default pix_fmt */

#define SCALE_FLAGS SWS_BICUBIC     // scale flag

// 封装单个输出AVStream
typedef struct OutputStream
{
    AVStream *stream;               // 代表一个stream, 1路audio或1路video都代表独立的steam
    AVCodecContext *codecCtx;        // 编码器上下文

    /* pts of the next frame that will be generated */
    int64_t next_pts;
    int samples_count;  // 音频的采样数量累计

    AVFrame *frame;     // 重采样后的frame，  视频叫scale
    AVFrame *tmp_frame; // 重采样前

    float t, tincr, tincr2; // 这几个参数用来生成PCM和YUV用的

    struct SwsContext *sws_ctx;     // 图像scale
    struct SwrContext *swr_ctx;     // 音频重采样
} OutputStream;

static void logPacket(const AVFormatContext *fmt_ctx, const AVPacket *pkt)
{
    AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;

    printf("pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
           av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
           av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
           av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
           pkt->stream_index);
}

static int write_frame(AVFormatContext *fmt_ctx, const AVRational *time_base,
                       AVStream *stream, AVPacket *pkt)
{
    // 将packet的timestamp由codec to stream timebase pts_before = -1024
    av_packet_rescale_ts(pkt, *time_base, stream->time_base);
    pkt->stream_index = stream->index;  // pts_before * 1/44100 = pts_after *1/1000
    // pts_after = pts_before * 1/44100 * 1000 = -1024 * 1/44100 * 1000 = -23
    /* Write the compressed frame to the media file. */
    logPacket(fmt_ctx, pkt);
    return av_interleaved_write_frame(fmt_ctx, pkt);
}

/// 设置输出流属性
static void add_stream(OutputStream *tOutputStream, AVFormatContext *outputFmtCtx,
                       AVCodec **codec,
                       enum AVCodecID codec_id)
{
    AVCodecContext *codecCtx;
    int i;

    /// 根据codec_id找到编码器
    *codec = avcodec_find_encoder(codec_id);
    if (!(*codec))
    {
        fprintf(stderr, "Could not find encoder for '%s'\n",
                avcodec_get_name(codec_id));
        exit(1);
    }
    /// 新建码流,绑定到outputFmtCtx上
    tOutputStream->stream = avformat_new_stream(outputFmtCtx, NULL); 
    if (!tOutputStream->stream)
    {
        fprintf(stderr, "Could not allocate stream\n");
        exit(1);
    }

    tOutputStream->stream->id = outputFmtCtx->nb_streams - 1;
    codecCtx = avcodec_alloc_context3(*codec);
    if (!codecCtx)
    {
        fprintf(stderr, "Could not alloc an encoding context\n");
        exit(1);
    }
    tOutputStream->codecCtx = codecCtx;
    // 初始化编码器参数
    switch ((*codec)->type)
    {
    case AVMEDIA_TYPE_AUDIO:
        codecCtx->codec_id = codec_id;
        codecCtx->sample_fmt  = (*codec)->sample_fmts ?    // 采样格式
                    (*codec)->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
        codecCtx->bit_rate    = 64000;     // 码率
        codecCtx->sample_rate = 44100;     // 采样率
        if ((*codec)->supported_samplerates)
        {
            codecCtx->sample_rate = (*codec)->supported_samplerates[0];
            for (i = 0; (*codec)->supported_samplerates[i]; i++)
            {
                if ((*codec)->supported_samplerates[i] == 44100)
                    codecCtx->sample_rate = 44100;
            }
        }
        codecCtx->channel_layout = AV_CH_LAYOUT_STEREO;
        codecCtx->channels        = av_get_channel_layout_nb_channels(codecCtx->channel_layout);
        if ((*codec)->channel_layouts)
        {
            codecCtx->channel_layout = (*codec)->channel_layouts[0];
            for (i = 0; (*codec)->channel_layouts[i]; i++) {
                if ((*codec)->channel_layouts[i] == AV_CH_LAYOUT_STEREO)
                    codecCtx->channel_layout = AV_CH_LAYOUT_STEREO;
            }
        }
        codecCtx->channels        = av_get_channel_layout_nb_channels(codecCtx->channel_layout);
        // 设置timebase, 使用采样率
        tOutputStream->stream->time_base = (AVRational){ 1, codecCtx->sample_rate };
        break;

    case AVMEDIA_TYPE_VIDEO:
        codecCtx->codec_id = codec_id;
        codecCtx->bit_rate = 400000;
        /* Resolution must be a multiple of two. */
        codecCtx->width    = 352;      // 分辨率
        codecCtx->height   = 288;
        codecCtx->max_b_frames = 1;
        tOutputStream->stream->time_base = (AVRational){ 1, STREAM_FRAME_RATE };  // 时基
        codecCtx->time_base       = tOutputStream->stream->time_base;    // 为什么这里需要设置
        codecCtx->gop_size      = STREAM_FRAME_RATE; //
        codecCtx->pix_fmt       = STREAM_PIX_FMT;
        break;

    default:
        break;
    }

    if (outputFmtCtx->oformat->flags & AVFMT_GLOBALHEADER)
        codecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;    //
}

/**************************************************************/
/* audio output */

static AVFrame *alloc_audio_frame(enum AVSampleFormat sample_fmt,
                                  uint64_t channel_layout,
                                  int sample_rate, int nb_samples)
{
    AVFrame *frame = av_frame_alloc();
    int ret;

    if (!frame)
    {
        fprintf(stderr, "Error allocating an audio frame\n");
        exit(1);
    }

    frame->format = sample_fmt;
    frame->channel_layout = channel_layout;
    frame->sample_rate = sample_rate;
    frame->nb_samples = nb_samples;

    if (nb_samples)
    {
        ret = av_frame_get_buffer(frame, 0);
        if (ret < 0)
        {
            fprintf(stderr, "Error allocating an audio buffer\n");
            exit(1);
        }
    }

    return frame;
}

static void open_audio(AVFormatContext *outputFmtCtx, AVCodec *codec, OutputStream *tOutputStream, AVDictionary *opt_arg)
{
    AVCodecContext *codecCtx;
    int nb_samples;
    int ret;
    AVDictionary *opt = NULL;

    codecCtx = tOutputStream->codecCtx;

    /* open it */
    av_dict_copy(&opt, opt_arg, 0);
    // 1. 关联编码器 会设置codec_ctx->time_base
    ret = avcodec_open2(codecCtx, codec, &opt);
    av_dict_free(&opt);
    if (ret < 0)
    {
        fprintf(stderr, "Could not open audio codec: \n");
        exit(1);
    }

    /* init signal generator */
    // 2. 初始化产生PCM的参数
    tOutputStream->t     = 0;
    tOutputStream->tincr = 2 * M_PI * 110.0 / codecCtx->sample_rate;
    /* increment frequency by 110 Hz per second */
    tOutputStream->tincr2 = 2 * M_PI * 110.0 / codecCtx->sample_rate / codecCtx->sample_rate;

    // 每次需要的samples
//    if (codecCtx->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
//        nb_samples = 10000; // 支持可变FRAME size的编码器极少，直接注释掉
//    else
    nb_samples = codecCtx->frame_size;

    // signal generator -> PCM -> tOutputStream->tmp_frame -> swr_convert重采样 -> tOutputStream->frame -> 编码器
    // 分配送给编码器的帧, 并申请对应的buffer
    tOutputStream->frame     = alloc_audio_frame(codecCtx->sample_fmt, codecCtx->channel_layout,
                                       codecCtx->sample_rate, nb_samples);
    // 分配送给信号生成PCM的帧, 并申请对应的buffer
    tOutputStream->tmp_frame = alloc_audio_frame(AV_SAMPLE_FMT_S16, codecCtx->channel_layout,
                                       codecCtx->sample_rate, nb_samples);

    /* copy the stream parameters to the muxer */
    ret = avcodec_parameters_from_context(tOutputStream->stream->codecpar, codecCtx);
    if (ret < 0)
    {
        fprintf(stderr, "Could not copy the stream parameters\n");
        exit(1);
    }

    /* create resampler context 创建重采样器 */
    tOutputStream->swr_ctx = swr_alloc();
    if (!tOutputStream->swr_ctx)
    {
        fprintf(stderr, "Could not allocate resampler context\n");
        exit(1);
    }

    /* set options */
    av_opt_set_int       (tOutputStream->swr_ctx, "in_channel_count",   codecCtx->channels,       0);
    av_opt_set_int       (tOutputStream->swr_ctx, "in_sample_rate",     codecCtx->sample_rate,    0);
    av_opt_set_sample_fmt(tOutputStream->swr_ctx, "in_sample_fmt",      AV_SAMPLE_FMT_S16, 0);
    av_opt_set_int       (tOutputStream->swr_ctx, "out_channel_count",  codecCtx->channels,       0);
    av_opt_set_int       (tOutputStream->swr_ctx, "out_sample_rate",    codecCtx->sample_rate,    0);
    av_opt_set_sample_fmt(tOutputStream->swr_ctx, "out_sample_fmt",     codecCtx->sample_fmt,     0);

    /* initialize the resampling context */
    if ((ret = swr_init(tOutputStream->swr_ctx)) < 0)
    {
        fprintf(stderr, "Failed to initialize the resampling context\n");
        exit(1);
    }
}

/* Prepare a 16 bit dummy audio frame of 'frame_size' samples and
 * 'nb_channels' channels. */
static AVFrame *get_audio_frame(OutputStream *tOutputStream)
{
    AVFrame *frame = tOutputStream->tmp_frame;
    int j, i, v;
    int16_t *q = (int16_t*)frame->data[0];

    /* check if we want to generate more frames */
    // 44100 * {1, 44100} = 1  -》44100*5 * {1, 44100} = 5
    // 5 *{1,1} = 5
    if (av_compare_ts(tOutputStream->next_pts, tOutputStream->codecCtx->time_base,
                      STREAM_DURATION, (AVRational){ 1, 1 }) >= 0)
        return NULL;

    for (j = 0; j <frame->nb_samples; j++)
    {
        v = (int)(sin(tOutputStream->t) * 10000);
        for (i = 0; i < tOutputStream->codecCtx->channels; i++)
            *q++ = v;
        tOutputStream->t     += tOutputStream->tincr;
        tOutputStream->tincr += tOutputStream->tincr2;
    }

    frame->pts = tOutputStream->next_pts; // 使用samples作为计数 设置pts 0, nb_samples(1024) 2048
    tOutputStream->next_pts  += frame->nb_samples;    // 音频PTS使用采样samples叠加

    return frame;
}

/*
 * encode one audio frame and send it to the muxer
 * return 1 when encoding is finished, 0 otherwise
 */
static int write_audio_frame(AVFormatContext *outputFmtCtx, OutputStream *tOutputStream)
{
    AVCodecContext *codecCtx;
    AVPacket pkt = { 0 }; // data and size must be 0;
    AVFrame *frame;
    int ret;
    int got_packet;
    int dst_nb_samples;

    av_init_packet(&pkt);
    codecCtx = tOutputStream->codecCtx;

    frame = get_audio_frame(tOutputStream);

    if (frame)
    {
        /* convert samples from native format to destination codec format, using the resampler */
        /* compute destination number of samples */
        dst_nb_samples = av_rescale_rnd(swr_get_delay(tOutputStream->swr_ctx, codecCtx->sample_rate) + frame->nb_samples,
                                        codecCtx->sample_rate, codecCtx->sample_rate, AV_ROUND_UP);
        av_assert0(dst_nb_samples == frame->nb_samples);

        /* when we pass a frame to the encoder, it may keep a reference to it
         * internally;
         * make sure we do not overwrite it here
         */
        ret = av_frame_make_writable(tOutputStream->frame);
        if (ret < 0)
            exit(1);

        /* convert to destination format */
        ret = swr_convert(tOutputStream->swr_ctx,
                          tOutputStream->frame->data, dst_nb_samples,
                          (const uint8_t **)frame->data, frame->nb_samples);
        if (ret < 0)
        {
            fprintf(stderr, "Error while converting\n");
            exit(1);
        }
        frame = tOutputStream->frame;
        // 转换time_base
        frame->pts = av_rescale_q(tOutputStream->samples_count, (AVRational){1, codecCtx->sample_rate},
                                  codecCtx->time_base);
        tOutputStream->samples_count += dst_nb_samples;
    }

    ret = avcodec_encode_audio2(codecCtx, &pkt, frame, &got_packet);
    if (ret < 0)
    {
        fprintf(stderr, "Error encoding audio frame:\n");
        exit(1);
    }

    if (got_packet)
    {
        ret = write_frame(outputFmtCtx, &codecCtx->time_base, tOutputStream->stream, &pkt);
        if (ret < 0)
        {
            fprintf(stderr, "Error while writing audio frame\n");
            exit(1);
        }
    }
    // frame == NULL 读取不到frame（比如读完了5秒的frame）; got_packet == 0 没有帧了
    return (frame || got_packet) ? 0 : 1;
}

/// 申请buffer
static AVFrame *alloc_picture(enum AVPixelFormat pix_fmt, int width, int height)
{
    AVFrame *picture;
    int ret;

    picture = av_frame_alloc();
    if (!picture)
        return NULL;

    picture->format = pix_fmt;
    picture->width  = width;
    picture->height = height;

    /* allocate the buffers for the frame data */
    ret = av_frame_get_buffer(picture, 32);
    if (ret < 0)
    {
        fprintf(stderr, "Could not allocate frame data.\n");
        exit(1);
    }

    return picture;
}

static void open_video(AVFormatContext *outputFmtCtx, AVCodec *codec, OutputStream *tOutputStream, AVDictionary *opt_arg)
{
    int ret;
    AVCodecContext *codecCtx = tOutputStream->codecCtx;
    AVDictionary *opt = NULL;

    av_dict_copy(&opt, opt_arg, 0);

    /// 关联编码器
    ret = avcodec_open2(codecCtx, codec, &opt);
    av_dict_free(&opt);
    if (ret < 0)
    {
        fprintf(stderr, "Could not open video codec\n");
        exit(1);
    }
    // 2. 分配帧buffer
    /* allocate and init a re-usable frame */
    tOutputStream->frame = alloc_picture(codecCtx->pix_fmt, codecCtx->width, codecCtx->height);
    if (!tOutputStream->frame)
    {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }

    /* If the output format is not YUV420P, then a temporary YUV420P
     * picture is needed too. It is then converted to the required
     * output format. */
    tOutputStream->tmp_frame = NULL;
    if (codecCtx->pix_fmt != AV_PIX_FMT_YUV420P)
    {
        // 编码器格式需要的数据不是 AV_PIX_FMT_YUV420P才需要 调用图像scale
        tOutputStream->tmp_frame = alloc_picture(AV_PIX_FMT_YUV420P, codecCtx->width, codecCtx->height);
        if (!tOutputStream->tmp_frame)
        {
            fprintf(stderr, "Could not allocate temporary picture\n");
            exit(1);
        }
    }

    /* copy the stream parameters to the muxer */
    ret = avcodec_parameters_from_context(tOutputStream->stream->codecpar, codecCtx);
    if (ret < 0)
    {
        fprintf(stderr, "Could not copy the stream parameters\n");
        exit(1);
    }
}

/* Prepare a dummy image. */
static void fill_yuv_image(AVFrame *pict, int frame_index,
                           int width, int height)
{
    int x, y, i;

    i = frame_index;

    /* Y */
    for (y = 0; y < height; y++)
        for (x = 0; x < width; x++)
            pict->data[0][y * pict->linesize[0] + x] = x + y + i * 3;

    /* Cb and Cr */
    for (y = 0; y < height / 2; y++)
    {
        for (x = 0; x < width / 2; x++) {
            pict->data[1][y * pict->linesize[1] + x] = 128 + y + i * 2;
            pict->data[2][y * pict->linesize[2] + x] = 64 + x + i * 5;
        }
    }
}

static AVFrame *get_video_frame(OutputStream *tOutputStream)
{
    AVCodecContext *codecCtx = tOutputStream->codecCtx;

    /* check if we want to generate more frames */
    // 我们测试时只产生STREAM_DURATION(这里是5.0秒)的视频数据
    if (av_compare_ts(tOutputStream->next_pts, codecCtx->time_base,
                      STREAM_DURATION, (AVRational){ 1, 1 }) >= 0)
        return NULL;

    /* when we pass a frame to the encoder, it may keep a reference to it
     * internally; make sure we do not overwrite it here */
    if (av_frame_make_writable(tOutputStream->frame) < 0)
        exit(1);

    if (codecCtx->pix_fmt != AV_PIX_FMT_YUV420P)
    {
        /* as we only generate a YUV420P picture, we must convert it
         * to the codec pixel format if needed */
        if (!tOutputStream->sws_ctx)
        {
            tOutputStream->sws_ctx = sws_getContext(codecCtx->width, codecCtx->height,
                                          AV_PIX_FMT_YUV420P,
                                          codecCtx->width, codecCtx->height,
                                          codecCtx->pix_fmt,
                                          SCALE_FLAGS, NULL, NULL, NULL);
            if (!tOutputStream->sws_ctx) {
                fprintf(stderr
                        ,
                        "Could not initialize the conversion context\n");
                exit(1);
            }
        }
        fill_yuv_image(tOutputStream->tmp_frame, tOutputStream->next_pts, codecCtx->width, codecCtx->height);
        sws_scale(tOutputStream->sws_ctx, (const uint8_t * const *) tOutputStream->tmp_frame->data,
                  tOutputStream->tmp_frame->linesize, 0, codecCtx->height, tOutputStream->frame->data,
                  tOutputStream->frame->linesize);
    } else {
        fill_yuv_image(tOutputStream->frame, tOutputStream->next_pts, codecCtx->width, codecCtx->height);
    }

    tOutputStream->frame->pts = tOutputStream->next_pts++;  // 为什么+1? 单位是 1/25 = 40ms
    // 0  1 2  -> 0 40ms 80ms
    return tOutputStream->frame;
}

/*
 * encode one video frame and send it to the muxer
 * return 1 when encoding is finished, 0 otherwise
 */
static int write_video_frame(AVFormatContext *outputFmtCtx, OutputStream *tOutputStream)
{
    int ret;
    AVCodecContext *codecCtx;
    AVFrame *frame;
    int got_packet = 0;
    AVPacket pkt = { 0 };

    codecCtx = tOutputStream->codecCtx;

    frame = get_video_frame(tOutputStream);

    av_init_packet(&pkt);

    /* encode the image */
    ret = avcodec_encode_video2(codecCtx, &pkt, frame, &got_packet);
    if (ret < 0)
    {
        fprintf(stderr, "Error encoding video frame\n");
        exit(1);
    }

    if (got_packet)
    {
        ret = write_frame(outputFmtCtx, &codecCtx->time_base, tOutputStream->stream, &pkt);
    }
    else
    {
        ret = 0;
    }

    if (ret < 0)
    {
        fprintf(stderr, "Error while writing video frame\n");
        exit(1);
    }

    // 这里之所以有两个判断条件
    // frame非NULL: 表示还在产生YUV数据帧
    // got_packet为1: 编码器还有缓存的帧
    return (frame || got_packet) ? 0 : 1;
}

static void close_stream(AVFormatContext *outputFmtCtx, OutputStream *tOutputStream)
{
    avcodec_free_context(&tOutputStream->codecCtx);
    av_frame_free(&tOutputStream->frame);
    av_frame_free(&tOutputStream->tmp_frame);
    sws_freeContext(tOutputStream->sws_ctx);
    swr_free(&tOutputStream->swr_ctx);
}

/**************************************************************/
/* media file output */

int main(int argc, char **argv)
{
    OutputStream video_st = { 0 }; /// 封装视频编码相关
    OutputStream audio_st = { 0 }; /// 封装音频编码相关
    const char *filename = "output.flv";

    AVOutputFormat *outputFmt;    /// 输出文件格式
    AVFormatContext *outputFmtCtx;
    AVCodec *audio_codec, *video_codec;
    int ret;
    int have_video = 0, have_audio = 0;
    int encode_video = 0, encode_audio = 0;
    AVDictionary *opt = NULL;
    int i;

    /// 根据文件名绑定合适的AVFormatContext
    avformat_alloc_output_context2(&outputFmtCtx, NULL, NULL, filename);

    outputFmt = outputFmtCtx->oformat;

    outputFmt->video_codec = AV_CODEC_ID_H264;    // 指定视频编码器
    outputFmt->audio_codec = AV_CODEC_ID_AAC;     // 指定音频编码器

    /// 根据编码格式，设置视频和音频流
    if (outputFmt->video_codec != AV_CODEC_ID_NONE)
    {
        add_stream(&video_st, outputFmtCtx, &video_codec, outputFmt->video_codec);
        have_video = 1;
        encode_video = 1;
    }
    if (outputFmt->audio_codec != AV_CODEC_ID_NONE)
    {
        add_stream(&audio_st, outputFmtCtx, &audio_codec, outputFmt->audio_codec);
        have_audio = 1;
        encode_audio = 1;
    }

    /// 打开编码器，申请内存等
    if (have_video)
        open_video(outputFmtCtx, video_codec, &video_st, opt);

    if (have_audio)
        open_audio(outputFmtCtx, audio_codec, &audio_st, opt);

    av_dump_format(outputFmtCtx, 0, filename, 1);

    /* open the output file, if needed */
    if (!(outputFmt->flags & AVFMT_NOFILE))
    {
        // 打开对应的输出文件，没有则创建
        ret = avio_open(&outputFmtCtx->pb, filename, AVIO_FLAG_WRITE);
        if (ret < 0)
        {
            fprintf(stderr, "Could not open '%s'\n", filename);
            return 1;
        }
    }
    // audio AVstream->base_time = 1/44100, video AVstream->base_time = 1/25
    /// 最终调用道flv_write_header，另外写头部时，会将对应封装格式的TIME_BASE写入
    ret = avformat_write_header(outputFmtCtx, &opt);// base_time audio = 1/1000 video = 1/1000
    if (ret < 0)
    {
        fprintf(stderr, "Error occurred when opening output file:\n");
        return 1;
    }

    while (encode_video || encode_audio)
    {
        /* select the stream to encode */
        if (encode_video &&         // video_st.next_pts值 <= audio_st.next_pts时
                (!encode_audio || av_compare_ts(video_st.next_pts, video_st.codecCtx->time_base,
                                                audio_st.next_pts, audio_st.codecCtx->time_base) <= 0)) {
            printf("\nwrite_video_frame\n");
            encode_video = !write_video_frame(outputFmtCtx, &video_st);
        }
        else
        {
            printf("\nwrite_audio_frame\n");
            encode_audio = !write_audio_frame(outputFmtCtx, &audio_st);
        }
    }

    /* Write the trailer, if any. The trailer must be written before you
     * close the CodecContexts open when you wrote the header; otherwise
     * av_write_trailer() may try to use memory that was freed on
     * av_codec_close(). */
    av_write_trailer(outputFmtCtx);

    /* Close each codec. */
    if (have_video)
        close_stream(outputFmtCtx, &video_st);
    if (have_audio)
        close_stream(outputFmtCtx, &audio_st);

    if (!(outputFmt->flags & AVFMT_NOFILE))
        /* Close the output file. */
        avio_closep(&outputFmtCtx->pb);

    /* free the stream */
    avformat_free_context(outputFmtCtx);

    return 0;
}
