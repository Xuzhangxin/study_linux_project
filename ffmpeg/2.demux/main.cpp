#include <stdio.h>
extern "C"
{
    #include <libavformat/avformat.h>
}


/**
* @brief 将一个AVRational类型的分数转换为double类型的浮点数
* @param r:r为一个AVRational类型的结构体变量，成员num表示分子，成员den表示分母，r的值即为(double)r.num / (double)r.den。用这种方法表示可以最大程度地避免精度的损失
* @return 如果变量r的分母den为0，则返回0（为了避免除数为0导致程序死掉）;其余情况返回(double)r.num / (double)r.den
*/
static double r2d(AVRational r)
{
    return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}

int main234sxds2()
{
    //需要读取的本地媒体文件相对路径为video1.mp4，这里由于文件video1.mp4就在工程目录下，所以相对路径为video1.mp4
    const char *path = "ande_10s.mp4";
    //const char *path = "audio1.mp3";
    ///av_register_all(); //初始化所有组件，只有调用了该函数，才能使用复用器和编解码器。否则，调用函数avformat_open_input会失败，无法获取媒体文件的信息
    avformat_network_init(); //打开网络流。这里如果只需要读取本地媒体文件，不需要用到网络功能，可以不用加上这一句
    AVDictionary *opts = NULL;

    //AVFormatContext是描述一个媒体文件或媒体流的构成和基本信息的结构体
    AVFormatContext *ic = NULL;

    //媒体打开函数，调用该函数可以获得路径为path的媒体文件的信息，并把这些信息保存到指针ic指向的空间中（调用该函数后会分配一个空间，让指针ic指向该空间）
    int re = avformat_open_input(&ic, path, NULL, &opts);
    if (re != 0)  //如果打开媒体文件失败，打印失败原因。比如，如果上面没有调用函数av_register_all，则会打印“XXX failed!:Invaliddata found when processing input”
    {
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        printf("open %s failed!:%s", path, buf);
    }
    else         //打开媒体文件成功
    {
        printf("打开媒体文件 %s 成功!\n", path);

        //调用该函数可以进一步读取一部分视音频数据并且获得一些相关的信息。
        //调用avformat_open_input之后，我们无法获取到正确和所有的媒体参数，所以还得要调用avformat_find_stream_info进一步的去获取。
        avformat_find_stream_info(ic, NULL);

        //调用avformat_open_input读取到的媒体文件的路径/名字
        printf("媒体文件名称:%s\n", ic->filename);

        //视音频流的个数，如果一个媒体文件既有音频，又有视频，则nb_streams的值为2。如果媒体文件只有音频，则值为1
        printf("视音频流的个数:%d\n", ic->nb_streams);


        //媒体文件的平均码率,单位为bps
        printf("媒体文件的平均码率:%lldbps\n", ic->bit_rate);

        printf("duration:%d\n", ic->duration);
        int tns, thh, tmm, tss;

        //AV_TIME_BASE=1000000,则1s分为AV_TIME_BASE份，时间基为{1,1000000}，一个刻度为1/1000000秒
        //duration的单位为AV_TIME_BASE，duration/AV_TIME_BASE=x秒
        tns = (ic->duration) / AV_TIME_BASE;
        thh = tns / 3600;
        tmm = (tns % 3600) / 60;
        tss = (tns % 60);
        printf("媒体文件总时长：%d时%d分%d秒\n", thh, tmm, tss); //通过上述运算，可以得到媒体文件的总时长

        printf("\n");
        //通过遍历的方式读取媒体文件视频和音频的信息，
        //新版本的FFmpeg新增加了函数av_find_best_stream，也可以取得同样的效果，但这里为了兼容旧版本还是用这种遍历的方式
        for (int i = 0; i < ic->nb_streams; i++)
        {
            AVStream *as = ic->streams[i];
            if (AVMEDIA_TYPE_AUDIO == as->codecpar->codec_type) //如果是音频流，则打印音频的信息
            {
                printf("音频信息:\n");
                printf("index:%d\n", as->index);  //如果一个媒体文件既有音频，又有视频，则音频index的值一般为1。但该值不一定准确，所以还是得通过as->codecpar->codec_type判断是视频还是音频
                printf("音频采样率:%dHz\n", as->codecpar->sample_rate); //音频编解码器的采样率，单位为Hz
                if (AV_SAMPLE_FMT_FLTP == as->codecpar->format)   //音频采样格式
                {
                    printf("音频采样格式:AV_SAMPLE_FMT_FLTP\n");
                }
                else if (AV_SAMPLE_FMT_S16P == as->codecpar->format)
                {
                    printf("音频采样格式:AV_SAMPLE_FMT_S16P\n");
                }
                printf("音频信道数目:%d\n", as->codecpar->channels); //音频信道数目
                if (AV_CODEC_ID_AAC == as->codecpar->codec_id)   //音频压缩编码格式
                {
                    printf("音频压缩编码格式:AAC\n");
                }
                else if (AV_CODEC_ID_MP3 == as->codecpar->codec_id)
                {
                    printf("音频压缩编码格式:MP3\n");
                }
                int DurationAudio = (as->duration) * r2d(as->time_base); //音频总时长，单位为秒。注意如果把单位放大为毫秒或者微妙，音频总时长跟视频总时长不一定相等的
                printf("音频总时长：%d时%d分%d秒\n", DurationAudio / 3600, (DurationAudio % 3600) / 60, (DurationAudio % 60)); //将音频总时长转换为时分秒的格式打印到控制台上
                printf("\n");
            }
            else if (AVMEDIA_TYPE_VIDEO == as->codecpar->codec_type)  //如果是视频流，则打印视频的信息
            {
                printf("视频信息:\n");
                printf("index:%d\n", as->index);   //如果一个媒体文件既有音频，又有视频，则视频index的值一般为0。但该值不一定准确，所以还是得通过as->codecpar->codec_type判断是视频还是音频
                printf("视频帧率:%lffps\n", r2d(as->avg_frame_rate)); //视频帧率,单位为fps，表示每秒出现多少帧
                if (AV_CODEC_ID_MPEG4 == as->codecpar->codec_id) //视频压缩编码格式
                {
                    printf("视频压缩编码格式:MPEG4\n");
                }
                printf("帧宽度:%d 帧高度:%d\n", as->codecpar->width, as->codecpar->height); //视频帧宽度和帧高度
                int DurationVideo = (as->duration) * r2d(as->time_base); //视频总时长，单位为秒。注意如果把单位放大为毫秒或者微妙，音频总时长跟视频总时长不一定相等的
                printf("视频总时长：%d时%d分%d秒\n", DurationVideo / 3600, (DurationVideo % 3600) / 60, (DurationVideo % 60)); //将视频总时长转换为时分秒的格式打印到控制台上
                printf("\n");
            }
        }

        //av_dump_format(ic, 0, path, 0);
    }
    if (ic)
    {
        avformat_close_input(&ic); //关闭一个AVFormatContext,和函数avformat_open_input()成对使用
    }
    avformat_network_deinit();
    getchar(); //加上这一句，防止程序打印完信息就马上退出了
    return 0;
}

int main(int argc, char **argv)
{

// 1. 打开文件
    const char *ifilename = "ande_10s.mp4";
    printf("in_filename = %s\n", ifilename);
    avformat_network_init();

    // AVFormatContext是描述一个媒体文件或媒体流的构成和基本信息的结构体
    AVFormatContext *ifmt_ctx = NULL;           // 输入文件的demux
    // 打开文件，主要是探测协议类型，如果是网络文件则创建网络链接
    int ret = avformat_open_input(&ifmt_ctx, ifilename, NULL, NULL);
    if (ret < 0) {
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof (buf) - 1);
        printf("open %s failed: %s\n", ifilename, buf);
        return -1;
    }

// 2. 读取码流信息
    ret = avformat_find_stream_info(ifmt_ctx, NULL);
    if (ret < 0)  //如果打开媒体文件失败，打印失败原因
    {
        char buf[1024] = { 0 };
        av_strerror(ret, buf, sizeof(buf) - 1);
        printf("avformat_find_stream_info %s failed:%s\n", ifilename, buf);
        avformat_close_input(&ifmt_ctx);
        return -1;
    }


// 3.打印总体信息
    printf("\n==== av_dump_format in_filename:%s ===\n", ifilename);
    av_dump_format(ifmt_ctx, 0, ifilename, 0);
    printf("\n==== av_dump_format finish =======\n\n");
    printf("media name:%s\n", ifmt_ctx->url);
    printf("stream number:%d\n", ifmt_ctx->nb_streams); //  nb_streams媒体流数量
    printf("media average ratio:%lldkbps\n",(int64_t)(ifmt_ctx->bit_rate/1024)); //  媒体文件的码率,单位为bps/1000=Kbps
    // duration: 媒体文件时长，单位微妙
    int total_seconds = (ifmt_ctx->duration) / AV_TIME_BASE;  // 1000us = 1ms, 1000ms = 1秒
    printf("audio duration: %02d:%02d:%02d\n",
           total_seconds / 3600, (total_seconds % 3600) / 60, (total_seconds % 60));
    printf("\n");

// 4.读取码流信息
    // 音频
    int audioindex = av_find_best_stream(ifmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (audioindex < 0) {
        printf("av_find_best_stream %s eror.", av_get_media_type_string(AVMEDIA_TYPE_AUDIO));
        return -1;
    }
    AVStream *audio_stream = ifmt_ctx->streams[audioindex];
    printf("----- Audio info:\n");
    printf("index: %d\n", audio_stream->index); // 序列号
    printf("samplarate: %d Hz\n", audio_stream->codecpar->sample_rate); // 采样率
    printf("sampleformat: %d\n", audio_stream->codecpar->format); // 采样格式 AV_SAMPLE_FMT_FLTP:8
    printf("audio codec: %d\n", audio_stream->codecpar->codec_id); // 编码格式 AV_CODEC_ID_MP3:86017 AV_CODEC_ID_AAC:86018
    if (audio_stream->duration != AV_NOPTS_VALUE) {
        int audio_duration = audio_stream->duration * av_q2d(audio_stream->time_base);
        printf("audio duration: %02d:%02d:%02d\n",
               audio_duration / 3600, (audio_duration % 3600) / 60, (audio_duration % 60));
    }

    // 视频
    int videoindex = av_find_best_stream(ifmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (videoindex < 0) {
        printf("av_find_best_stream %s eror.", av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
        return -1;
    }
    AVStream *video_stream = ifmt_ctx->streams[videoindex];
    printf("----- Video info:\n");
    printf("index: %d\n", video_stream->index); // 序列号
    printf("fps: %lf\n", av_q2d(video_stream->avg_frame_rate)); // 帧率
    printf("width: %d, height:%d \n", video_stream->codecpar->width, video_stream->codecpar->height);
    printf("video codec: %d\n", video_stream->codecpar->codec_id); // 编码格式 AV_CODEC_ID_H264: 27
    if (video_stream->duration != AV_NOPTS_VALUE) {
        int video_duration = video_stream->duration * av_q2d(video_stream->time_base);
        printf("audio duration: %02d:%02d:%02d\n",
               video_duration / 3600, (video_duration % 3600) / 60, (video_duration % 60));
    }

// 5.提取码流
    AVPacket *pkt = av_packet_alloc();
    int pkt_count = 0;
    int print_max_count = 100;
    printf("\n-----av_read_frame start\n");
    while (1)
    {
        ret = av_read_frame(ifmt_ctx, pkt);
        if (ret < 0) {
            printf("av_read_frame end\n");
            break;
        }

        if(pkt_count++ < print_max_count)
        {
            if (pkt->stream_index == audioindex)
            {
                printf("audio pts: %lld\n", pkt->pts);
                printf("audio dts: %lld\n", pkt->dts);
                printf("audio size: %d\n", pkt->size);
                printf("audio pos: %lld\n", pkt->pos);
                printf("audio duration: %lf\n\n",
                       pkt->duration * av_q2d(ifmt_ctx->streams[audioindex]->time_base));
            }
            else if (pkt->stream_index == videoindex)
            {
                printf("video pts: %lld\n", pkt->pts);
                printf("video dts: %lld\n", pkt->dts);
                printf("video size: %d\n", pkt->size);
                printf("video pos: %lld\n", pkt->pos);
                printf("video duration: %lf\n\n",
                       pkt->duration * av_q2d(ifmt_ctx->streams[videoindex]->time_base));
            }
            else
            {
                printf("unknown stream_index:\n", pkt->stream_index);
            }
        }
        av_packet_unref(pkt);
    }

// 6.结束
    if(pkt)
        av_packet_free(&pkt);
    if(ifmt_ctx)
        avformat_close_input(&ifmt_ctx);


    avformat_network_deinit();
    //getchar(); //加上这一句，防止程序打印完信息马上退出
    return 0;
}
