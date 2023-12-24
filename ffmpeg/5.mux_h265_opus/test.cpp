
#include <iostream>
#include <string>
#include <cstdlib>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

// void extractStreams(const std::string& input, const std::string& video_output, const std::string& audio_output) 
// {
//     av_register_all();

//     AVFormatContext* inputFormatCtx = nullptr;
//     if (avformat_open_input(&inputFormatCtx, input.c_str(), nullptr, nullptr) < 0) {
//         std::cerr << "Could not open input file" << std::endl;
//         exit(1);
//     }

//     if (avformat_find_stream_info(inputFormatCtx, nullptr) < 0) {
//         std::cerr << "Could not find stream info" << std::endl;
//         exit(1);
//     }

//     int video_stream_index = -1;
//     int audio_stream_index = -1;

//     for (unsigned int i = 0; i < inputFormatCtx->nb_streams; ++i) {
//         if (inputFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
//             video_stream_index = i;
//         } else if (inputFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
//             audio_stream_index = i;
//         }
//     }

//     if (video_stream_index == -1 || audio_stream_index == -1) {
//         std::cerr << "Could not find required streams" << std::endl;
//         exit(1);
//     }

//     AVFormatContext* videoOutputFormatCtx = nullptr;
//     AVFormatContext* audioOutputFormatCtx = nullptr;

//     avformat_alloc_output_context2(&videoOutputFormatCtx, nullptr, nullptr, video_output.c_str());
//     avformat_alloc_output_context2(&audioOutputFormatCtx, nullptr, nullptr, audio_output.c_str());

//     if (!videoOutputFormatCtx || !audioOutputFormatCtx) {
//         std::cerr << "Could not allocate output format contexts" << std::endl;
//         exit(1);
//     }

//     std::cerr << "begin................ 000000000 " << std::endl;
//     av_dump_format(inputFormatCtx, 0, input.c_str(), 0); /// 显示一些信息
// 	av_dump_format(videoOutputFormatCtx, 0, video_output.c_str(), 1); /// 显示一些信息
//     av_dump_format(audioOutputFormatCtx, 0, audio_output.c_str(), 1);
//     std::cerr << "end................ 000000000 " << std::endl;
//     exit(1);

//     AVStream* videoOutputStream = avformat_new_stream(videoOutputFormatCtx, nullptr);
//     AVStream* audioOutputStream = avformat_new_stream(audioOutputFormatCtx, nullptr);

//     if (!videoOutputStream || !audioOutputStream) {
//         std::cerr << "Could not allocate output streams" << std::endl;
//         exit(1);
//     }

//     avcodec_parameters_copy(videoOutputStream->codecpar, inputFormatCtx->streams[video_stream_index]->codecpar);
//     avcodec_parameters_copy(audioOutputStream->codecpar, inputFormatCtx->streams[audio_stream_index]->codecpar);

//     if (!(videoOutputFormatCtx->oformat->flags & AVFMT_NOFILE)) {
//         if (avio_open(&videoOutputFormatCtx->pb, video_output.c_str(), AVIO_FLAG_WRITE) < 0) {
//             std::cerr << "Could not open video output file" << std::endl;
//             exit(1);
//         }
//     }

//     if (!(audioOutputFormatCtx->oformat->flags & AVFMT_NOFILE)) {
//         if (avio_open(&audioOutputFormatCtx->pb, audio_output.c_str(), AVIO_FLAG_WRITE) <0)  {
//              std::cerr << "Could not open audio output file" << std::endl; exit(1); 
//         } 
//     }
    
//     if (avformat_write_header(videoOutputFormatCtx, nullptr) < 0) {
//         std::cerr << "Could not write headers video" << std::endl; 
//         exit(1); 
//     }
    
//     int ret = 0;
//     if ( (ret = avformat_write_header(audioOutputFormatCtx, nullptr)) < 0) {
//         std::cerr << "Could not write headers audio" << ret << std::endl; 
//         exit(1); 
//     }
    
//     AVPacket packet; 
//     while (av_read_frame(inputFormatCtx, &packet) >= 0) {
//         if (packet.stream_index == video_stream_index) {
//             packet.stream_index = 0; 
//             av_interleaved_write_frame(videoOutputFormatCtx, &packet); 
//         } else if (packet.stream_index == audio_stream_index) {
//             packet.stream_index = 0; 
//             av_interleaved_write_frame(audioOutputFormatCtx, &packet); 
//         } 
//         av_packet_unref(&packet);
//      }
    
//     av_write_trailer(videoOutputFormatCtx); 
//     av_write_trailer(audioOutputFormatCtx);
    
//     avformat_close_input(&inputFormatCtx); 
//     avformat_free_context(inputFormatCtx);
    
//     if (!(videoOutputFormatCtx->oformat->flags & AVFMT_NOFILE)) {
//         avio_closep(&videoOutputFormatCtx->pb); 
//     } 
//     avformat_free_context(videoOutputFormatCtx);
    
//     if (!(audioOutputFormatCtx->oformat->flags & AVFMT_NOFILE)) {
//         avio_closep(&audioOutputFormatCtx->pb); 
//     } 
//     avformat_free_context(audioOutputFormatCtx); 
// }
    
long long int duration = 0;
void remux(const std::string& video_input, const std::string& audio_input, const std::string& output) { 
    av_register_all();

    AVFormatContext* videoInputFormatCtx = nullptr;
    AVFormatContext* audioInputFormatCtx = nullptr;

    if (avformat_open_input(&videoInputFormatCtx, video_input.c_str(), nullptr, nullptr) < 0 ||
        avformat_open_input(&audioInputFormatCtx, audio_input.c_str(), nullptr, nullptr) < 0) {
        std::cerr << "Could not open input files" << std::endl;
        exit(1);
    }

    if (avformat_find_stream_info(videoInputFormatCtx, nullptr) < 0 ||
        avformat_find_stream_info(audioInputFormatCtx, nullptr) < 0) {
        std::cerr << "Could not find stream info" << std::endl;
        exit(1);
    }

    AVFormatContext* outputFormatCtx = nullptr;
    avformat_alloc_output_context2(&outputFormatCtx, nullptr, nullptr, output.c_str());
    if (!outputFormatCtx) {
        std::cerr << "Could not allocate output format context" << std::endl;
        exit(1);
    }

    AVStream* videoOutputStream = avformat_new_stream(outputFormatCtx, nullptr);
    AVStream* audioOutputStream = avformat_new_stream(outputFormatCtx, nullptr);

    if (!videoOutputStream || !audioOutputStream) {
        std::cerr << "Could not allocate output streams" << std::endl;
        exit(1);
    }

    avcodec_parameters_copy(videoOutputStream->codecpar, videoInputFormatCtx->streams[0]->codecpar);
    avcodec_parameters_copy(audioOutputStream->codecpar, audioInputFormatCtx->streams[0]->codecpar);

    if (!(outputFormatCtx->oformat->flags & AVFMT_NOFILE)) {
        if (avio_open(&outputFormatCtx->pb, output.c_str(), AVIO_FLAG_WRITE) < 0) {
            std::cerr << "Could not open output file" << std::endl;
            exit(1);
        }
    }

    std::cerr << "begin................ outputFormatCtx " << std::endl;
	av_dump_format(outputFormatCtx, 0, output.c_str(), 1); /// 显示一些信息
    std::cerr << "begin................ audioInputFormatCtx" << std::endl;
    av_dump_format(audioInputFormatCtx, 0, audio_input.c_str(), 0);
    std::cerr << "begin................ videoInputFormatCtx" << std::endl;
    av_dump_format(videoInputFormatCtx, 0, video_input.c_str(), 0);
    std::cerr << "begin................ end" << std::endl;
    // exit(1);

    int ret = 0;
    if ((ret = avformat_write_header(outputFormatCtx, nullptr)) < 0) {
        std::cerr << "Could not write header " << ret << std::endl;
        exit(1);
    }

    AVPacket packet;
    /// 写失败的原因

    int frame_index = 0;
    while (1) {
        int ret = 0;
        if ( (ret = av_read_frame(videoInputFormatCtx, &packet)) >= 0) {
            // 如果该帧没有pts，需要补上
            if (packet.pts == AV_NOPTS_VALUE) {
                AVRational timebase = videoInputFormatCtx->streams[0]->time_base;
                // 两帧之间的持续时间
                int64_t calc_duration = (double)AV_TIME_BASE / av_q2d(videoInputFormatCtx->streams[0]->r_frame_rate);
                
                packet.pts = (double)(frame_index * calc_duration) / (double)(av_q2d(timebase)*AV_TIME_BASE);
                packet.dts = packet.pts;
                packet.duration = (double)calc_duration / (double)(av_q2d(timebase)*AV_TIME_BASE);
                printf("packet duration:%lld\n", packet.duration);
                frame_index++;
            }

            // 更新PTS/DTS
            // 将 packet 中的各时间值从输入流封装格式时间基转换到输出流封装格式时间基
            // av_packet_rescale_ts(&packet, videoInputFormatCtx->streams[0]->time_base, videoOutputStream->time_base);

            // packet.pts = av_rescale_q_rnd(packet.pts, videoInputFormatCtx->streams[0]->time_base,
            //         videoOutputStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
            // packet.dts = av_rescale_q_rnd(packet.dts, videoInputFormatCtx->streams[0]->time_base,
            //         videoOutputStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
            packet.duration = av_rescale_q(packet.dts, videoInputFormatCtx->streams[0]->time_base, videoOutputStream->time_base);
            packet.pos = -1;
            packet.stream_index = 0;

            ret = av_interleaved_write_frame(outputFormatCtx, &packet);
            av_packet_unref(&packet);   ///TODO  unref是否需要
            std::cout << "ret is " << ret << " parcket->dts:" << packet.dts << " parcket->pts:" << packet.pts  << std::endl;
        } else {
            break;
        }

        if (av_read_frame(audioInputFormatCtx, &packet) >= 0) {
            // 如果该帧没有pts，需要补上
            if (packet.pts == AV_NOPTS_VALUE) {
                AVRational timebase = audioInputFormatCtx->streams[0]->time_base;
                // 两帧之间的持续时间
                int64_t calc_duration = (double)AV_TIME_BASE / av_q2d(audioInputFormatCtx->streams[0]->r_frame_rate);
                
                packet.pts = (double)(frame_index * calc_duration) / (double)(av_q2d(timebase)*AV_TIME_BASE);
                packet.dts = packet.pts;
                packet.duration = (double)calc_duration / (double)(av_q2d(timebase)*AV_TIME_BASE);
                frame_index++;
            } 

            // 更新PTS/DTS
            // packet.pts = av_rescale_q_rnd(packet.pts, audioInputFormatCtx->streams[0]->time_base,
            //         audioOutputStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
            // packet.dts = av_rescale_q_rnd(packet.dts, audioInputFormatCtx->streams[0]->time_base,
            //         audioOutputStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
            // packet.duration = av_rescale_q(packet.dts, audioInputFormatCtx->streams[0]->time_base, audioOutputStream->time_base);
            // 将 packet 中的各时间值从输入流封装格式时间基转换到输出流封装格式时间基
            av_packet_rescale_ts(&packet, audioInputFormatCtx->streams[0]->time_base, audioOutputStream->time_base);
            packet.pos = -1;
            packet.stream_index = 1;

            ret = av_interleaved_write_frame(outputFormatCtx, &packet);
            av_packet_unref(&packet);

            std::cout << "ret is " << ret << " parcket->dts:" << packet.dts << " parcket->pts:" << packet.pts  << std::endl;
        } else {
            break;
        }
    }

    // while(1) {


    // }

    av_write_trailer(outputFormatCtx);

    int64_t duration = outputFormatCtx->duration;  
  
    // 将时长转换为秒并打印输出  
    double durationInSeconds = (double)duration / 1000.0;  
    printf("Video duration: %.2f seconds\n", durationInSeconds);

    av_dump_format(outputFormatCtx, 0, output.c_str(), 1); /// 显示一些信息

  int duration2 = static_cast<double>(outputFormatCtx->duration) / AV_TIME_BASE;
  printf("\nduration11:%d\n", outputFormatCtx->duration);

    avformat_close_input(&videoInputFormatCtx);
    avformat_close_input(&audioInputFormatCtx);
   
    avformat_free_context(videoInputFormatCtx);
    avformat_free_context(audioInputFormatCtx);

    if (!(outputFormatCtx->oformat->flags & AVFMT_NOFILE)) {
        avio_closep(&outputFormatCtx->pb);
    }
     avformat_close_input(&outputFormatCtx);
    // avformat_free_context(outputFormatCtx);


   

    
    // AVFormatContext* format_ctx = nullptr;
    // ret=avformat_open_input(&format_ctx, output.c_str(), nullptr, nullptr);
    // if(ret < 0)
    // {
    //     std::cout << "avformat_open_input error\n" << output.c_str() << std::endl;
    // }
    // avformat_find_stream_info(format_ctx, nullptr);
 

}

#define IF_JUST_REMUX

int main(int argc, char** argv) 
{
#ifndef IF_JUST_REMUX
    std::string input = "h265_opus.mp4";
    std::string video_output = "output_h265_v.h265";
    std::string audio_output = "output_opus_a.opus";
    std::string output = "new_h265_opus.mp4";

    extractStreams(input, video_output, audio_output);
#else

    std::string input = "h265_opus.mp4";
    std::string video_output = "input_h265.hevc";
    std::string audio_output = "input_opus.opus";
    std::string output = "new_h265_opus.mp4";
#endif
    remux(video_output, audio_output, output);

    return 0;
}