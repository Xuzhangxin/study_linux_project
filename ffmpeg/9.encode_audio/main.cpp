/**
* @projectName   encode_audio
* @brief         视频编码，从本地读取PCM数据进行AAC编码
* @author        Xu ZhangXin
* @date          2023-6-04
*/
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}

#define INPUT_FILE "48000_2_f32le.pcm"
#define OUTPUT_FILE "output.aac"

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

void f32le_convert_to_fltp(float *f32le, float *fltp, int nb_samples) 
{
    /// packet to planner
    /// packet[0] = L R L R L R

    /// planner[0] = L L L L L
    /// planner[1] = R R R R R 
    float *fltp_l = fltp;   // 左通道
    float *fltp_r = fltp + nb_samples;   // 右通道
    for(int i = 0; i < nb_samples; i++) 
    {
        fltp_l[i] = f32le[i*2];
        fltp_r[i] = f32le[i*2+1];
    }
}

static void get_adts_header(AVCodecContext *codecContext, uint8_t *adtsHeader, int aacLength)
{
    uint8_t freq_idx = 0;    //0: 96000 Hz  3: 48000 Hz 4: 44100 Hz
    switch (codecContext->sample_rate) 
    {
        case 96000: freq_idx = 0; break;
        case 88200: freq_idx = 1; break;
        case 64000: freq_idx = 2; break;
        case 48000: freq_idx = 3; break;
        case 44100: freq_idx = 4; break;
        case 32000: freq_idx = 5; break;
        case 24000: freq_idx = 6; break;
        case 22050: freq_idx = 7; break;
        case 16000: freq_idx = 8; break;
        case 12000: freq_idx = 9; break;
        case 11025: freq_idx = 10; break;
        case 8000: freq_idx = 11; break;
        case 7350: freq_idx = 12; break;
        default: freq_idx = 4; break;
    }
    uint8_t chanCfg = codecContext->channels;
    uint32_t frame_length = aacLength + 7;
    adtsHeader[0] = 0xFF;
    adtsHeader[1] = 0xF1;
    adtsHeader[2] = ((codecContext->profile) << 6) + (freq_idx << 2) + (chanCfg >> 2);
    adtsHeader[3] = (((chanCfg & 3) << 6) + (frame_length  >> 11));
    adtsHeader[4] = ((frame_length & 0x7FF) >> 3);
    adtsHeader[5] = (((frame_length & 7) << 5) + 0x1F);
    adtsHeader[6] = 0xFC;
}

int encode(AVCodecContext *codecContext, AVFrame *frame, AVPacket *packet, FILE *fileOutputAAC)
{
    int ret = 0;
    ret = avcodec_send_frame(codecContext, frame);
    if (ret < 0) 
    {
        my_printf("avcodec_send_frame ret:%d failed", ret);
    }

    while (ret >= 0)
    {
        ret = avcodec_receive_packet(codecContext, packet);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) 
        {
            return 0;
        }
        else if (ret < 0) 
        {
            my_printf("avcodec_receive_packet ret:%d failed", ret);
            return -1;
        }

        if((codecContext->flags & AV_CODEC_FLAG_GLOBAL_HEADER)) {
            unsigned char adtsHeader[7] = {0};
            get_adts_header(codecContext, adtsHeader, packet->size);
            ret = fwrite(adtsHeader, 1, 7, fileOutputAAC);
            if(ret != 7) {
                my_printf("write aac_header failed");
                return -1;
            }
        }

        ret = fwrite(packet->data, 1, packet->size, fileOutputAAC);
        if (ret != packet->size)
        {
            my_printf("fwrite ret:%d packet->size:%d failed", ret, packet->size);
            return -1;
        }
        // av_packet_unref(packet);  avcodec_receive_packet第一个调用的就是 av_packet_unref
    }

    return -1;
}


bool checkSampleFmtVaild(AVCodecContext *codecContext, AVSampleFormat sampleFmt)
{
    for (int i = 0; i != AV_SAMPLE_FMT_NONE; i++)
    {
        if (sampleFmt == codecContext->codec->sample_fmts[i])
        {
            return true;
        }
    }

    return false;
}

bool checkSampleRateVaild(AVCodecContext *codecContext, int sampleRate)
{
    for (int i = codecContext->codec->supported_samplerates[0]; i != 0; i++)
    {
        if (sampleRate == codecContext->codec->supported_samplerates[i])
        {
            return true;
        }
    }

    return false;
}

bool checkChannelLayouttVaild(AVCodecContext *codecContext, int channelLayout)
{
    for (int i = codecContext->codec->channel_layouts[0]; i != 0; i++)
    {
        if (channelLayout == codecContext->codec->channel_layouts[i])
        {
            return true;
        }
    }

    return false;
}

/// pcm编码为aac
int main(int argc, char **argv)
{
    int ret = 0;
    /// 1.设置Codec上下文
    AVCodecID codecId = AV_CODEC_ID_AAC;
    AVCodec *codec = avcodec_find_encoder(codecId);
    if (NULL == codec)
    {
        my_printf("codec is null");
        return -1;
    }

    AVCodecContext *codecContext = avcodec_alloc_context3(codec);
    if (NULL == codecContext)
    {
        my_printf("codecContext is null");
        return -1;
    }

    codecContext->codec_id = codecId;
    codecContext->sample_rate = 48000;
    codecContext->channel_layout = AV_CH_LAYOUT_STEREO; 
    codecContext->channels = av_get_channel_layout_nb_channels(codecContext->channel_layout);
    codecContext->bit_rate = 128*1024;
    codecContext->profile = FF_PROFILE_AAC_LOW; ///< 音质
    codecContext->sample_fmt = AV_SAMPLE_FMT_FLTP; // float planner

    /// 2.检查编码参数合法性
    if (!checkSampleFmtVaild(codecContext, codecContext->sample_fmt))
    {
        my_printf("sample fmt:%d is not vailed", codecContext->sample_fmt);
        return -1;
    }
    if (!checkSampleRateVaild(codecContext, codecContext->sample_rate))
    {
        my_printf("sample rate:%d is not vailed", codecContext->sample_rate);
        return -1;
    }
    if (!checkSampleRateVaild(codecContext, codecContext->channel_layout))
    {
        my_printf("channel layout:%d is not vailed", codecContext->channel_layout);
        return -1;
    }

    my_printf("Audio encode config");
    my_printf("bit_rate:%ldkbps", codecContext->bit_rate/1024);
    my_printf("sample_rate:%d", codecContext->sample_rate);
    my_printf("sample_fmt:%s", av_get_sample_fmt_name(codecContext->sample_fmt));
    my_printf("channels:%d", codecContext->channels);

    codecContext->flags = AV_CODEC_FLAG_GLOBAL_HEADER; /// ffmpeg默认的aac是不带adts，而fdk_aac默认带adts，所以一般都强制指定adts
    /// 3.打开编码器
    if (avcodec_open2(codecContext, codec, NULL) < 0)
    {
        my_printf("open codec failed");
        return -1;
    }

    // frame_size是在avcodec_open2后进行关联
    my_printf("frame_size:%d\n", codecContext->frame_size);  /// 每帧数据对应的采样数

    /// 4.打开输入文件和输出文件，申请缓冲区buffer，填写frame属性，分配frame的内存等
    FILE *fileInputPCM = fopen(INPUT_FILE, "rb");
    if (NULL == fileInputPCM)
    {
        my_printf("inputfile:%s open failed", INPUT_FILE);
        return -1;
    }

    FILE *fileOutputAAC = fopen(OUTPUT_FILE, "wb");
    if (!fileOutputAAC)
    {
        my_printf("inputfile:%s open failed", fileOutputAAC);
        return -1;
    }

    AVPacket *packet = av_packet_alloc();
    if (!packet)
    {
        my_printf("packet alloc failed");
        return -1;
    }

    AVFrame *frame = av_frame_alloc();
    if (!frame)
    {
        my_printf("frame alloc failed");
        return -1;
    }

    /// pcm每一帧的数据量 =  采样点数量 * 采样字节数 * 通道数
    frame->format = codecContext->sample_fmt;
    frame->nb_samples = codecContext->frame_size;
    frame->channel_layout = codecContext->channel_layout;
    frame->channels = codecContext->channels;
    /// 会根据属性去分配buffer内存
    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0) 
    {
        my_printf("av_frame_get_buffer failed");
        return -1;
    }

    /// 采样字节数
    int sampleBytes = av_get_bytes_per_sample(codecContext->sample_fmt);
    int frameBytes = frame->nb_samples * sampleBytes * frame->channels;
    my_printf("frameBytes:%d", frameBytes);

    uint8_t *pcmBuf = (uint8_t *)malloc(frameBytes);
    if (!pcmBuf)
    {
        my_printf("pcmBuf malloc failed");
        return -1;
    }

    uint8_t *pcmTmpBuf = (uint8_t *)malloc(frameBytes);
    if (!pcmTmpBuf)
    {
        my_printf("pcmTmpBuf malloc failed");
        return -1;
    }
    

    while (1)
    {
        memset(pcmBuf, 0, frameBytes);
        ret  = fread(pcmBuf, 1, frameBytes, fileInputPCM);
        if (ret <= 0)
        {
            fclose(fileInputPCM);
            my_printf("fread ret:%d done", ret);
            return -1;
        }
        my_printf("len = %d", ret);
        ret = av_frame_make_writable(frame);
        if (ret < 0) 
        {
            my_printf("av_frame_make_writable ret:%d failed", ret);
        }


        f32le_convert_to_fltp((float *)pcmBuf, (float *)pcmTmpBuf, frame->nb_samples);
        ret = av_samples_fill_arrays(frame->data, frame->linesize, pcmTmpBuf,
                            frame->channels, frame->nb_samples, (AVSampleFormat)frame->format, 0);
        if (ret < 0) 
        {
            my_printf("av_samples_fill_arrays ret:%d failed", ret);
        }     

        ret = encode(codecContext, frame, packet, fileOutputAAC);   
        if (ret < 0) 
        {
            my_printf("encode ret:%d failed", ret);
            break;
        }     
    }

    encode(codecContext, NULL, packet, fileOutputAAC);

    fclose(fileInputPCM);
    fclose(fileOutputAAC);

    av_packet_unref(packet);
    av_frame_unref(frame);
    avcodec_free_context(&codecContext);

    return 0;
}


