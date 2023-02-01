#include <stdio.h>
#include <libavutil/log.h>
#include <libavformat/avio.h>
#include <libavformat/avformat.h>
//gcc main.c  -L/home/xzx/ffmpeg_build/lib/  -I/home/xzx/ffmpeg_build/include  -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lswresample -lswscale -lpostproc -lfdk-aac
#define ADTS_HEADER_LEN  7;
const int sampling_frequencies[] = {
    96000,  // 0x0
    88200,  // 0x1
    64000,  // 0x2
    48000,  // 0x3
    44100,  // 0x4
    32000,  // 0x5
    24000,  // 0x6
    22050,  // 0x7
    16000,  // 0x8
    12000,  // 0x9
    11025,  // 0xa
    8000   // 0xb
    // 0xc d e f是保留的
};

int add_adts_header(char * const p_adts_header, const int data_length,
                const int profile, const int samplerate,
                const int channels);

int main(int argc, char **argv)
{
    av_log_set_level(AV_LOG_DEBUG);
    char *in_filename = "./believe.mp4";
    char *out_filename = "./believe.aac";

    //1.打开输入mp4文件和输出文件
    AVFormatContext *ifmt_ctx = NULL;
    int ret = avformat_open_input(&ifmt_ctx, in_filename, NULL, NULL);
    if (ret < 0) {
        av_log(NULL, AV_LOG_DEBUG, "open input:%s failed", in_filename);
        return ret;
    }

    FILE *outfile = fopen(out_filename, "wb");
    if (outfile == NULL) {
        av_log(NULL, AV_LOG_DEBUG, "open output:%s failed", out_filename);
        return ret;
    }

    //2.获取音视频文件信息
    ret = avformat_find_stream_info(ifmt_ctx, NULL);
    if (ret < 0) {
        av_log(NULL, AV_LOG_DEBUG, "find stream info failed:%s", in_filename);
        return ret;
    }

    //转存并打印媒体信息
    av_dump_format(ifmt_ctx, 0, in_filename, 0);

    int audio_idx = av_find_best_stream(ifmt_ctx, AVMEDIA_TYPE_AUDIO,
                        -1, -1,
                        NULL, 0);

    /// 3.找到音频帧并添加adts头，让其成为ADTS格式aac数据
    AVPacket ipkt;
    while(av_read_frame(ifmt_ctx, &ipkt) >= 0) {
        /// 当前是音频帧
        if (ipkt.stream_index == audio_idx) {
            char header_buf[7] = {0};
            add_adts_header(header_buf, ipkt.size, 
                            ifmt_ctx->streams[audio_idx]->codecpar->profile,
                            ifmt_ctx->streams[audio_idx]->codecpar->sample_rate,
                            ifmt_ctx->streams[audio_idx]->codecpar->channels
            );
            fwrite(header_buf, 1, sizeof(header_buf), outfile);
            fwrite(ipkt.data, 1, ipkt.size, outfile);
            // fwrite(ipkt.buf->buffer.data, 1, sizeof(ipkt.buf->buffer.data),)
            av_packet_unref(&ipkt); //这里使用的是栈内存，并没有alloc，所以不要free。
        }
    }



    avformat_close_input(&ifmt_ctx);
    fclose(outfile);
    return 0;
}

int add_adts_header(char * const p_adts_header, const int data_length,
                const int profile, const int samplerate,
                const int channels)
{

    int sampling_frequency_index = 3; // 默认使用48000hz
    int adtsLen = data_length + 7;

    int frequencies_size = sizeof(sampling_frequencies) / sizeof(sampling_frequencies[0]);
    int i = 0;
    for(i = 0; i < frequencies_size; i++)
    {
        if(sampling_frequencies[i] == samplerate)
        {
            sampling_frequency_index = i;
            break;
        }
    }
    if(i >= frequencies_size)
    {
        printf("unsupport samplerate:%d\n", samplerate);
        return -1;
    }

    p_adts_header[0] = 0xff;         //syncword:0xfff                          高8bits
    p_adts_header[1] = 0xf0;         //syncword:0xfff                          低4bits
    p_adts_header[1] |= (0 << 3);    //MPEG Version:0 for MPEG-4,1 for MPEG-2  1bit
    p_adts_header[1] |= (0 << 1);    //Layer:0                                 2bits
    p_adts_header[1] |= 1;           //protection absent:1                     1bit

    p_adts_header[2] = (profile)<<6;            //profile:profile               2bits
    p_adts_header[2] |= (sampling_frequency_index & 0x0f)<<2; //sampling frequency index:sampling_frequency_index  4bits
    p_adts_header[2] |= (0 << 1);             //private bit:0                   1bit
    p_adts_header[2] |= (channels & 0x04)>>2; //channel configuration:channels  高1bit

    p_adts_header[3] = (channels & 0x03)<<6; //channel configuration:channels 低2bits
    p_adts_header[3] |= (0 << 5);               //original：0                1bit
    p_adts_header[3] |= (0 << 4);               //home：0                    1bit
    p_adts_header[3] |= (0 << 3);               //copyright id bit：0        1bit
    p_adts_header[3] |= (0 << 2);               //copyright id start：0      1bit
    p_adts_header[3] |= ((adtsLen & 0x1800) >> 11);           //frame length：value   高2bits

    p_adts_header[4] = (uint8_t)((adtsLen & 0x7f8) >> 3);     //frame length:value    中间8bits
    p_adts_header[5] = (uint8_t)((adtsLen & 0x7) << 5);       //frame length:value    低3bits
    p_adts_header[5] |= 0x1f;                                 //buffer fullness:0x7ff 高5bits
    p_adts_header[6] = 0xfc;      //‭11111100‬       //buffer fullness:0x7ff 低6bits
    // number_of_raw_data_blocks_in_frame：
    //    表示ADTS帧中有number_of_raw_data_blocks_in_frame + 1个AAC原始帧。

    return 0;
}