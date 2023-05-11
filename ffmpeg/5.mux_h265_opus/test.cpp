
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
    int64_t cur_video_pts = 0, cur_audio_pts = 0;

    while (1) {
        int ret = 0;

        AVFormatContext* usingFormatCtx = nullptr;
        AVStream *out_stream = NULL;
        int out_index = -1;
        AVStream* usingOutputStream = nullptr;
        
        ///TODO 双码流，解决音视频同步问题，应该判断，如果应该写音频了，就写，没轮到则。
        /// 或者应该是放在队列里的，一阵阵取。。。
        //比较时间戳，判断当前应该写什么帧
        if (av_compare_ts(cur_video_pts, videoInputFormatCtx->streams[0]->time_base, \
                    cur_audio_pts, audioInputFormatCtx->streams[0]->time_base) < 0) {
            usingFormatCtx = videoInputFormatCtx;
            usingOutputStream = videoOutputStream;
            out_index = 0;
        } else {
            usingFormatCtx = audioInputFormatCtx;
            usingOutputStream = audioOutputStream;
            out_index = 1;
        }

        if ( (ret = av_read_frame(usingFormatCtx, &packet)) >= 0) {
            // 如果该帧没有pts，需要补上
            if (packet.pts == AV_NOPTS_VALUE) {
                AVRational timebase = usingFormatCtx->streams[0]->time_base;
                // 两帧之间的持续时间
                double calc_duration = (double)AV_TIME_BASE / av_q2d(usingFormatCtx->streams[0]->r_frame_rate);
                
                packet.pts = (double)(frame_index * calc_duration) / (double)(av_q2d(timebase)*AV_TIME_BASE);
                packet.dts = packet.pts;
                packet.duration = (double)calc_duration / (double)(av_q2d(timebase)*AV_TIME_BASE);
                frame_index++;
            }

            if (out_index == 0) cur_video_pts = packet.pts;
            else cur_audio_pts = packet.pts;

            // 更新PTS/DTS
            // 将 packet 中的各时间值从输入流封装格式时间基转换到输出流封装格式时间基
            av_packet_rescale_ts(&packet, usingFormatCtx->streams[0]->time_base, usingOutputStream->time_base);
            packet.pos = -1;
            packet.stream_index = out_index;


            ret = av_interleaved_write_frame(outputFormatCtx, &packet);
            av_packet_unref(&packet);
            std::cout << "ret is " << ret << " parcket->dts:" << packet.dts << " parcket->pts:" << packet.pts  << std::endl;
        } else {
            break;
        }
    }

    av_write_trailer(outputFormatCtx);

    avformat_close_input(&videoInputFormatCtx);
    avformat_close_input(&audioInputFormatCtx);
    avformat_free_context(videoInputFormatCtx);
    avformat_free_context(audioInputFormatCtx);
    if (!(outputFormatCtx->oformat->flags & AVFMT_NOFILE)) {
        avio_closep(&outputFormatCtx->pb);
    }
    avformat_free_context(outputFormatCtx);
}


#define OUTPUT_OPUS_FILE "output_opus.opus"
void test()
{
    AVFormatContext* videoInputFormatCtx = nullptr;
    AVFormatContext* audioInputFormatCtx = nullptr;

    if (avformat_open_input(&audioInputFormatCtx, "input_opus.opus", nullptr, nullptr) < 0) {
        std::cerr << "Could not open input files" << std::endl;
        exit(1);
    }
// OpusHead\001\001h   19
    if (avformat_find_stream_info(audioInputFormatCtx, nullptr) < 0) {
        std::cerr << "Could not find stream info" << std::endl;
        exit(1);
    }

    	AVFormatContext *m_opusFormatCtx = NULL;
		avformat_alloc_output_context2(&m_opusFormatCtx, NULL, NULL, OUTPUT_OPUS_FILE);
		if (!m_opusFormatCtx) 
		{
			// logPrint(MX_LOG_ERROR, "Could not allocate opus output format context");
			return ;
		}
		AVStream *m_opusaudioInputStream = avformat_new_stream(m_opusFormatCtx, NULL);
		AVCodec *opusAudioCodec = avcodec_find_decoder(AV_CODEC_ID_OPUS);
		if (NULL == opusAudioCodec)
		{
			// logPrint(MX_LOG_ERROR, "could not find audio decoder for '%s' \n", avcodec_get_name(AV_CODEC_ID_OPUS));
			return ;
		}
		AVCodecContext *m_opusinputAudioCtx = avcodec_alloc_context3(opusAudioCodec);
		m_opusinputAudioCtx->codec_id = AV_CODEC_ID_OPUS;
		m_opusinputAudioCtx->sample_fmt = AV_SAMPLE_FMT_FLTP;
		m_opusinputAudioCtx->bit_rate = 20000;
		m_opusinputAudioCtx->sample_rate = 16000;
		m_opusinputAudioCtx->channels = 1;
		m_opusinputAudioCtx->channel_layout = AV_CH_LAYOUT_MONO;
		m_opusinputAudioCtx->time_base.den = m_opusinputAudioCtx->sample_rate;
		m_opusinputAudioCtx->time_base.num =  1;


		// AVRational{ 1, m_opusinputAudioCtx->sample_rate };
		avcodec_parameters_from_context(m_opusaudioInputStream->codecpar, m_opusinputAudioCtx);

        // avcodec_parameters_copy(m_opusaudioInputStream->codecpar, audioInputFormatCtx->streams[0]->codecpar);
        uint8_t cOpusHeader[19] = {
            0x4f,0x70,0x75,0x73,0x48,0x65,0x61,0x64,  ///< opus header
            0x01,                                     ///< version
            0x01,                                     ///< channel                
            0x68,0x00,                                ///< preskip
            0x80,0x3e,0x00,0x00,                       ///< sample_rate = 160000
            0x00,0x00,0x00                              ///< Output Gain,Channel Mapping Family
        };
        m_opusaudioInputStream->codecpar->extradata  = (uint8_t*)av_malloc(sizeof(cOpusHeader));
        memcpy(m_opusaudioInputStream->codecpar->extradata, cOpusHeader, sizeof(cOpusHeader));
        m_opusaudioInputStream->codecpar->extradata_size = sizeof(cOpusHeader);

		if (!(m_opusFormatCtx->oformat->flags & AVFMT_NOFILE)) {
			if (avio_open(&m_opusFormatCtx->pb, OUTPUT_OPUS_FILE, AVIO_FLAG_WRITE) < 0) {
				return ;
			}
		}

		av_dump_format(m_opusFormatCtx, 0, OUTPUT_OPUS_FILE, 1); /// 显示一些信息


		int ret = avformat_write_header(m_opusFormatCtx, nullptr);
		if (ret < 0)
		{
			return ;
		}

        printf("success\n");
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
    // test();
    return 0;
}