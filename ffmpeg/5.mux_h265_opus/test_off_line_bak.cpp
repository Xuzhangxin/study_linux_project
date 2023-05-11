#include "off_line_event_proc.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include "local_storage_module.h"
#include "log_mx.h"

#define OUTPUT_HEVC_FILE	"output_hevc.hevc"
#define OUTPUT_OPUS_FILE	"output_opus.opus"

namespace maix {
	void RecordTimeOutProc(COffLineEventProc* module)
	{
		std::cout << "record time out " << std::endl;
		if (module)
		{
			module->stopRecord();
		}
	}

	COffLineEventProc::COffLineEventProc(CModule * module)
		: m_module(module)
		, m_bRecordStart(mxfalse)
		, m_bAddVideoTrack(mxfalse)
		, m_bAddAudioTrack(mxfalse)
	{
	}

	COffLineEventProc::~COffLineEventProc()
	{
	}

	mxbool COffLineEventProc::init(int iRecordDuration)
	{
		if (!m_objPacketQueue.init(60, 1000))
			return mxfalse;

		m_iRecordDuration = iRecordDuration;
		return mxtrue;
	}

	mxbool COffLineEventProc::unInit()
	{
		return mxbool();
	}

	bool COffLineEventProc::pushFrameData(
		std::shared_ptr<CMediaFramePacket>& packet)
	{
		return  m_objPacketQueue.push(packet);
	}

	void COffLineEventProc::popFrameData(
		std::shared_ptr<CMediaFramePacket>& packet)
	{
		m_objPacketQueue.pop(packet);
	}

	void COffLineEventProc::run()
	{
		while (1)
		{
			std::shared_ptr<CMediaFramePacket> packet = NULL;
			popFrameData(packet);

			if (!packet)
			{
#ifdef	WIN32
				Sleep(500);
#else
				usleep(500*1000);
#endif
				continue;
			}

			if (m_bRecordStart)
			{
				int iPacketHeaderLen = sizeof(T_MediaFramePacketHeader);
				T_MediaFramePacketHeader tMediaFramePacketHeader;
				unsigned char *pcData = packet->getFrameData() + iPacketHeaderLen;

				if (packet->getPacketType() == E_P_VIDEO_H264)
				{
					std::cout << "video: " <<
						packet->getFrameDataLen() << std::endl;
					
					//writeMP4VideoFrame(packet->getFrameData(),
					//	packet->getFrameDataLen
					writeMP4VideoFrame(pcData,
						packet->getFrameDataLen() - iPacketHeaderLen);
				}
					
				if (packet->getPacketType() == E_P_AUDIO_AAC)
				{
					std::cout << "audio: " <<
						packet->getFrameDataLen() << std::endl;

					writeMP4AudioAACFrame(pcData,
						packet->getFrameDataLen() - iPacketHeaderLen);
				}

				if (packet->getPacketType() == E_P_VIDEO_H265)
				{
					std::cout << "video h265: " <<
						packet->getFrameDataLen() << std::endl;
					
					//writeMP4VideoFrame(packet->getFrameData(),
					//	packet->getFrameDataLen
					// writeMP4VideoFrameV2(pcData,
					// 	packet->getFrameDataLen() - iPacketHeaderLen);
#ifdef OUTPUT_HEVC_FILE
					writeTESTVideoH265(pcData,
						packet->getFrameDataLen() - iPacketHeaderLen);
#endif
					writeMP4VideoH265(pcData,
						packet->getFrameDataLen() - iPacketHeaderLen);
				}

				if (packet->getPacketType() == E_P_AUDIO_OPUS)
				{
					std::cout << "audio opus: " <<
						packet->getFrameDataLen() << std::endl;

					writeMP4AudioFrameV2(pcData,
						packet->getFrameDataLen() - iPacketHeaderLen);

					writeTESTAudioOpus(pcData,
						packet->getFrameDataLen() - iPacketHeaderLen);
				}
			}
			

		}
		
	}

	mxbool COffLineEventProc::startRecord()
	{
		if (m_bRecordStart == mxtrue)
			return mxtrue;

		m_timerRecord.StartTimerOnce(m_iRecordDuration * 1000,	
			std::bind(RecordTimeOutProc, this));

		m_pMP4FileHandle = MP4Create("sunyang.mp4", 0);

		m_bRecordStart = mxtrue;
		return mxtrue;
	}

	mxbool COffLineEventProc::stopRecord()
	{
		if (m_bRecordStart == mxfalse)
			return mxtrue;

		m_bRecordStart = mxfalse;

		CLocalStorageModule *objLocalStorageModule =
			(CLocalStorageModule *)m_module;

		objLocalStorageModule->stopRecord();

		MP4Close(m_pMP4FileHandle, 0);
		m_bAddVideoTrack = mxfalse;
		m_bAddAudioTrack = mxfalse;
		return mxtrue;
	}

	int COffLineEventProc::writeMP4VideoFrame(
		unsigned char* data, int iLen)
	{
		if (data == NULL && iLen < 4)
			return 0;

		int iRealLen = 0;
		unsigned char* pRealData = data;

		for (int i = 0; i < (iLen - 4); i++)
		{
			if (0x00 == data[i] &&
				0x00 == data[i + 1] &&
				0x01 == data[i + 2])
			{
				for (int j = i + 3; j < (iLen - 4); j++)
				{
					if (0x00 == data[j] &&
						0x00 == data[j + 1] &&
						0x01 == data[j + 2])
					{
						iRealLen = j - i;
						break;
					}
					else if (0x00 == data[j] &&
						0x00 == data[j + 1] &&
						0x00 == data[j + 2] &&
						0x01 == data[j + 3])
					{
						iRealLen = j - i;
						break;
					}
					else
					{
						iRealLen = iLen - i;
					}
				}
				unsigned char naluType =
					pRealData[i + 3] & H264_NALU_TYPE_MASK;
				switch (naluType)
				{
				case 0x07:
				{
					//std::cout << "sps" << std::endl;
				}
				break;
				case 0x08:
				{
					//std::cout << "pps" << std::endl;
				}
				break;
				default:
				{
					//std::cout << "frame" << std::endl;
				}
				break;
				}

				i = i + iRealLen;
			}
			else if (0x00 == data[i] &&
				0x00 == data[i + 1] &&
				0x00 == data[i + 2] &&
				0x01 == data[i + 3])
			{
				for (int j = i + 4; j < (iLen - 4); j++)
				{
					if (0x00 == data[j] &&
						0x00 == data[j + 1] &&
						0x01 == data[j + 2])
					{
						iRealLen = j - i;
						break;
					}
					else if (0x00 == data[j] &&
						0x00 == data[j + 1] &&
						0x00 == data[j + 2] &&
						0x01 == data[j + 3])
					{
						iRealLen = j - i;
						break;
					}
					else
					{
						iRealLen = iLen - i;
					}
				}

				unsigned char naluType =
					pRealData[i + 4] & H264_NALU_TYPE_MASK;
				switch (naluType)
				{
				case 0x07:
				{
					std::cout << "sps" << std::endl;
					writeMP4VideoSPS(&pRealData[i + 4], iRealLen - 4);
				}
				break;
				case 0x08:
				{
					std::cout << "pps" << std::endl;
					writeMP4VideoPPS(&pRealData[i + 4], iRealLen - 4);
				}
				break;
				default:
				{
					std::cout << "frame" << std::endl;
					pRealData[i] = ((iRealLen - 4) >> 24) & 0xFF;
					pRealData[i + 1] = ((iRealLen - 4) >> 16) & 0xFF;
					pRealData[i + 2] = ((iRealLen - 4) >> 8) & 0xFF;
					pRealData[i + 3] = (iRealLen - 4) & 0xFF;
					writeMP4VideoH264(&pRealData[i], iRealLen);
				}
				break;
				}
				i = i + iRealLen - 1;
			}
		}
	}

	int COffLineEventProc::writeMP4VideoSPS(unsigned char * data, int iLen)
	{
		if (!m_bAddVideoTrack)
		{
			m_MP4VideoID = MP4AddH264VideoTrack(m_pMP4FileHandle,
				timeScale, timeScale / 30, 1280, 720,
				data[1], //sps[1] AVCProfileIndication
				data[2], //sps[2] profile_compat
				data[3], //sps[3] AVCLevelIndication
				3);

			if (m_MP4VideoID == MP4_INVALID_TRACK_ID)
			{
				return -1;
			}

			MP4SetVideoProfileLevel(m_pMP4FileHandle, 0x7F);
			m_bAddVideoTrack = mxtrue;
		}

		MP4AddH264SequenceParameterSet(m_pMP4FileHandle,
			m_MP4VideoID, data, iLen);

		return 0;
	}

	int COffLineEventProc::writeMP4VideoPPS(unsigned char * data, int iLen)
	{
		if (m_bAddVideoTrack)
		{
			MP4AddH264PictureParameterSet(m_pMP4FileHandle,
				m_MP4VideoID, data, iLen);
		}
	
		return 0;
	}

	int COffLineEventProc::writeMP4VideoH264(unsigned char * data, int iLen)
	{
		if (m_bAddVideoTrack)
		{
			MP4WriteSample(m_pMP4FileHandle, m_MP4VideoID,
				data,
				iLen,
				MP4_INVALID_DURATION, 0, 1);
			
		}
		return 0;
	}

	int COffLineEventProc::writeMP4AudioAACFrame(unsigned char * data, int iLen)
	{
		unsigned int iMetaLen = ((data[1] & 0x01) == 1?7:9);
		int H2 = data[3] & 0x03;
		int M8 = data[4];
		int L3 = (int(data[5] & 0xE0)) >> 5;

		int iTotalLen = (H2 << 9) | (M8 << 3) | L3;
		if (iTotalLen != iLen)
		{
			std::cout << "iLen error" << std::endl;
		}

		if (!m_bAddAudioTrack)
		{
			const int iProfileOffset = 2;
			const unsigned char ucProfileMask = 0xC0;
			const unsigned char ucSampleRateMask = 0x3C;
			const unsigned int aiSampleRates[] = {
				96000,
				88200,
				64000,
				48000,
				44100,
				32000,
				24000,
				22050,
				16000
			};

			unsigned char ucProfile = ((data[iProfileOffset] & ucProfileMask) >> 6) + 1;
			unsigned char ucSampleRate = (data[iProfileOffset] & ucSampleRateMask) >> 2;
			unsigned char ucChannel = (data[iProfileOffset] & 0x1) << 2 | ((data[iProfileOffset+1] & 0xc0) >> 6);

			m_MP4AudioID = MP4AddAudioTrack(m_pMP4FileHandle,
				aiSampleRates[ucSampleRate], 2048, 
				MP4_MPEG4_AAC_LC_AUDIO_TYPE);

			if (m_MP4AudioID == MP4_INVALID_TRACK_ID)
			{
				std::cout << "add audio track failed" << std::endl;
				return -2;
			}

			MP4SetAudioProfileLevel(m_pMP4FileHandle, ucProfile);
			unsigned char ucEsConfig[2] = { 0 };
			ucEsConfig[0] = (ucProfile << 3) | (ucSampleRate >> 1);
			ucEsConfig[1] = ((ucSampleRate & 0x01) << 7) | (ucChannel << 3);

			if (!MP4SetTrackESConfiguration(m_pMP4FileHandle,
				m_MP4AudioID, ucEsConfig, sizeof(ucEsConfig)))
			{
				if (!MP4SetTrackESConfiguration(m_pMP4FileHandle,
					m_MP4AudioID, ucEsConfig, sizeof(ucEsConfig)))
				{
					std::cout << "fail to es config" << std::endl;
					return -4;
				}
			}
			
			m_bAddAudioTrack = mxtrue;
		}

		MP4WriteSample(m_pMP4FileHandle, m_MP4AudioID,
			data + iMetaLen,
			iLen - iMetaLen,
			MP4_INVALID_DURATION, 0, 1);

		return 0;
	}

	mxbool COffLineEventProc::isH265KeyFrame(unsigned char* pdata, int iLen)
	{
		if (pdata == NULL && iLen < 4)
			return 0;

		int iRealLen = 0;
		unsigned char* pRealData = pdata;

		for (int i = 0; i < (iLen - 4); i++)
		{
			if (0x00 == pdata[i] &&
				0x00 == pdata[i + 1] &&
				0x01 == pdata[i + 2])
			{
				for (int j = i + 3; j < (iLen - 4); j++)
				{
					if (0x00 == pdata[j] &&
						0x00 == pdata[j + 1] &&
						0x01 == pdata[j + 2])
					{
						iRealLen = j - i;
						break;
					}
					else if (0x00 == pdata[j] &&
						0x00 == pdata[j + 1] &&
						0x00 == pdata[j + 2] &&
						0x01 == pdata[j + 3])
					{
						iRealLen = j - i;
						break;
					}
					else
					{
						iRealLen = iLen - i;
					}
				}
				unsigned char naluType =
					((pRealData[i + 3] & H265_NALU_TYPE_MASK) >> 1);
				switch (naluType)
				{
				case H265_NALU_TYPE_SPS:
				{
					return mxtrue;
				}
				break;
				case H265_NALU_TYPE_PPS:
				{
					return mxtrue;
				}
				break;
				case H265_NALU_TYPE_IDR:
				{
					return mxtrue;
				}
				break;
				default:
				{
				}
				break;
				}
				i = i + iRealLen;
			}
			else if (0x00 == pdata[i] &&
				0x00 == pdata[i + 1] &&
				0x00 == pdata[i + 2] &&
				0x01 == pdata[i + 3])
			{
				for (int j = i + 4; j < (iLen - 4); j++)
				{
					if (0x00 == pdata[j] &&
						0x00 == pdata[j + 1] &&
						0x01 == pdata[j + 2])
					{
						iRealLen = j - i;
						break;
					}
					else if (0x00 == pdata[j] &&
						0x00 == pdata[j + 1] &&
						0x00 == pdata[j + 2] &&
						0x01 == pdata[j + 3])
					{
						iRealLen = j - i;
						break;
					}
					else
					{
						iRealLen = iLen - i;
					}
				}

				unsigned char naluType =
					((pRealData[i + 4] & H265_NALU_TYPE_MASK) >> 1);
				switch (naluType)
				{
				case H265_NALU_TYPE_SPS:
				{
					return mxtrue;
				}
				break;
				case H265_NALU_TYPE_PPS:
				{
					return mxtrue;
				}
				break;
				case H265_NALU_TYPE_IDR:
				{
					return mxtrue;
				}
				break;
				default:
				{

				}
				break;
				}
				i = i + iRealLen - 1;
			}
		}
		return mxfalse;
	}

	mxbool COffLineEventProc::writeMP4VideoH265(unsigned char* pdata, int iLen)
	{
		AVStream *stream = NULL;
		stream = m_mp4FormatCtx->streams[0];
		if (NULL == stream)
		{
			logPrint(MX_LOG_ERROR, "m_iVideoIndex:%d stream is null", m_iVideoIndex);
			return mxfalse;
		}

		AVPacket packet;
		av_init_packet(&packet);
		/// 帧类型为sps、pps或idr，则代表是IDR帧
		if (mxtrue == isH265KeyFrame(pdata, iLen)) 
		{
			packet.flags |= AV_PKT_FLAG_KEY; ///< 关键帧
		}
		else
		{
			packet.flags = 0;
		}
		packet.stream_index = stream->index;
		packet.data = pdata;
		packet.size = iLen;
		if (mxtrue == m_bWaitIDR)
		{
			if (0 == (packet.flags & AV_PKT_FLAG_KEY))
			{
				return mxtrue; 
			}
			m_bWaitIDR = mxfalse;
		}

		logPrint(MX_LOG_DEBUG, "write mp4 video wait IDR packet success, nalu type:%d", packet.flags);

		/// 计算时间戳
		if (packet.pts == AV_NOPTS_VALUE) {
			AVRational timebase = m_inputVideoCtx->time_base;
			// 两帧之间的持续时间
			int64_t calc_duration = (double)AV_TIME_BASE / m_fps;   //   m_fps
			
			packet.pts = (double)(m_frameVideoIndex * calc_duration) / (double)(av_q2d(timebase)*AV_TIME_BASE);
			packet.dts = packet.pts;
			packet.duration = (double)calc_duration / (double)(av_q2d(timebase)*AV_TIME_BASE);
			m_frameVideoIndex++;
		}

		/// 换算时间戳
		av_packet_rescale_ts(&packet, m_inputVideoCtx->time_base, stream->time_base);
		packet.pos = -1;
		packet.stream_index = m_iVideoIndex;

		logPrint(MX_LOG_DEBUG, "write mp4 video prepare to write frame");

		if (av_interleaved_write_frame(m_mp4FormatCtx, &packet) < 0) {
			logPrint(MX_LOG_WARN, "write mp4 video frame failed");
		}

		av_packet_unref(&packet);
		logPrint(MX_LOG_DEBUG, "write mp4 video end to write frame");

		if (m_frameVideoIndex > 60)
		{
			logPrint(MX_LOG_DEBUG, "done done done done done done done done done done done done");
			writeMP4DeInit();
		}


		return mxtrue;
	}


	mxbool COffLineEventProc::writeTESTVideoH265(unsigned char* pdata, int iLen)
	{
		/// 这里不区分I帧，外面通过解析NALU单元，已经区分出I、P、B、SPS、PPS       TODO  写sps信息
		AVStream *stream = NULL;
		stream = m_h265videoInputStream;
		if (NULL == stream)
		{
			logPrint(MX_LOG_ERROR, "m_iVideoIndex:%d stream is null", m_iVideoIndex);
			return mxfalse;
		}

		AVPacket packet;
		av_init_packet(&packet);
		if (mxtrue == isH265KeyFrame(pdata, iLen))
		{
			packet.flags |= AV_PKT_FLAG_KEY; ///< 关键帧
		}
		else
		{
			packet.flags = 0;
		}
		packet.stream_index = stream->index;
		packet.data = pdata;
		packet.size = iLen;
		if (mxtrue == m_bWaitIDR)
		{
			if (0 == (packet.flags & AV_PKT_FLAG_KEY))
			{
				return mxtrue; 
			}
			m_bWaitIDR = mxfalse;
		}

		logPrint(MX_LOG_DEBUG, "write frame h265 video wait IDR packet success, nalu flags:%d", packet.flags);

		/// 计算时间戳
		if (packet.pts == AV_NOPTS_VALUE) {
			AVRational timebase = m_h265inputVideoCtx->time_base;
			// 两帧之间的持续时间
			int64_t calc_duration = (double)AV_TIME_BASE / m_fps;
			
			packet.pts = (double)(m_h265VideoIndex * calc_duration) / (double)(av_q2d(timebase)*AV_TIME_BASE);
			packet.dts = packet.pts;
			packet.duration = (double)calc_duration / (double)(av_q2d(timebase)*AV_TIME_BASE);
			m_h265VideoIndex++;
		}

		/// 换算时间戳
		av_packet_rescale_ts(&packet, m_h265inputVideoCtx->time_base, stream->time_base);
		packet.pos = -1;
		packet.stream_index = m_iVideoIndex;

		logPrint(MX_LOG_DEBUG, "write mp4 video prepare to write frame");


#ifdef OUTPUT_HEVC_FILE
		if (av_interleaved_write_frame(m_h265FormatCtx, &packet) < 0) {
			logPrint(MX_LOG_WARN, "write h265 video frame failed");
		}
#endif 

		av_packet_unref(&packet);
		logPrint(MX_LOG_DEBUG, "write mp4 video end to write frame");

		if (m_h265VideoIndex > 60)
		{
			logPrint(MX_LOG_DEBUG, "done done done done done done done done done done done done");
			writeMP4DeInit();
		}


		return mxtrue;
	}

	mxbool COffLineEventProc::writeTESTAudioOpus(unsigned char* pdata, int iLen)
	{
		AVStream *stream = NULL;
		stream = m_opusaudioInputStream;
		if (NULL == stream)
		{
			logPrint(MX_LOG_ERROR, "m_iVideoIndex:%d stream is null", m_iAudioIndex);
			return mxfalse;
		}

		AVPacket packet;
		av_init_packet(&packet);
		packet.flags = 0 ;
		packet.data = pdata;
		packet.size = iLen;
		if (mxtrue == m_bWaitIDR)
		{
			logPrint(MX_LOG_WARN, "write mp4 audio wait IDR packet");
			return mxtrue; 
		}

		/// 计算时间戳
		if (packet.pts == AV_NOPTS_VALUE) {
			AVRational timebase = m_opusinputAudioCtx->time_base;
			// 两帧之间的持续时间
			int64_t calc_duration = (double)AV_TIME_BASE / m_opusinputAudioCtx->sample_rate;
			
			packet.pts = (double)(m_opusAudioIndex * calc_duration) / (double)(av_q2d(timebase)*AV_TIME_BASE);
			packet.dts = packet.pts;
			packet.duration = (double)calc_duration / (double)(av_q2d(timebase)*AV_TIME_BASE);
			m_opusAudioIndex++;
		}

		logPrint(MX_LOG_WARN, "write opus audio......r_frame_rate:%d, time_base1:%lf, time_base2:%lf", stream->r_frame_rate, av_q2d(stream->time_base), av_q2d(m_opusinputAudioCtx->time_base));
		logPrint(MX_LOG_WARN, "write opus audio......r_frame_rate:%d, time_base1:%d, time_base2:%d", stream->r_frame_rate, stream->time_base.den, stream->time_base.num);

		/// 换算时间戳
		av_packet_rescale_ts(&packet, m_opusinputAudioCtx->time_base, stream->time_base);

		packet.pos = -1;
		packet.stream_index = stream->index;


		if (av_interleaved_write_frame(m_opusFormatCtx, &packet) < 0) {
			logPrint(MX_LOG_WARN, "write mp4 audio frame failed");
		}
		av_packet_unref(&packet);

		return mxtrue;
	}

	mxbool COffLineEventProc::writeMP4AudioFrameV2(unsigned char* pdata, int iLen)
	{
		AVStream *stream = NULL;
		stream = m_mp4FormatCtx->streams[m_iAudioIndex];
		if (NULL == stream)
		{
			logPrint(MX_LOG_ERROR, "m_iVideoIndex:%d stream is null", m_iAudioIndex);
			return mxfalse;
		}

		AVPacket packet;
		av_init_packet(&packet);
		packet.flags = 0;
		packet.stream_index = stream->index;
		packet.data = pdata;
		packet.size = iLen;
		if (mxtrue == m_bWaitIDR)
		{
			logPrint(MX_LOG_WARN, "write mp4 audio wait IDR packet");
			return mxtrue; 
		}

		/// 计算时间戳
		if (packet.pts == AV_NOPTS_VALUE) {
			AVRational timebase = m_inputAudioCtx->time_base;
			// 两帧之间的持续时间
			int64_t calc_duration = (double)AV_TIME_BASE /m_opusinputAudioCtx->sample_rate  ; //    
			
			packet.pts = (double)(m_frameAudioIndex * calc_duration) / (double)(av_q2d(timebase)*AV_TIME_BASE);
			packet.dts = packet.pts;
			packet.duration = (double)calc_duration / (double)(av_q2d(timebase)*AV_TIME_BASE);
			m_frameAudioIndex++;
		}

		/// 换算时间戳
		av_packet_rescale_ts(&packet, m_inputAudioCtx->time_base, stream->time_base);
		packet.pos = -1;
		packet.stream_index = m_iAudioIndex;


		if (av_interleaved_write_frame(m_mp4FormatCtx, &packet) < 0) {
			logPrint(MX_LOG_WARN, "write mp4 audio frame failed");
		}
		av_packet_unref(&packet);

		return mxtrue;
	}

	mxbool COffLineEventProc::writeMP4Init(const std::string& mp4FileName)
	{
		int ret = 0;

		m_bWaitIDR = mxtrue;
		m_iVideoIndex = -1;
		m_iAudioIndex = -1;
		m_frameAudioIndex = 0;	
		m_frameVideoIndex = 0;
		m_fps = 15;
		m_iBitrate = 1500;
		m_inputAudioCtx = NULL;
		m_inputVideoCtx = NULL;


		av_register_all();
		m_mp4FormatCtx = NULL;
		avformat_alloc_output_context2(&m_mp4FormatCtx, NULL, NULL, mp4FileName.c_str());
		if (!m_mp4FormatCtx) 
		{
			logPrint(MX_LOG_ERROR, "Could not allocate output format context");
			return mxfalse;
		}

		m_videoInputStream = avformat_new_stream(m_mp4FormatCtx, NULL);
		m_audioInputStream = avformat_new_stream(m_mp4FormatCtx, NULL);
		AVCodec *videoCodec = avcodec_find_decoder(AV_CODEC_ID_HEVC);
		if (NULL == videoCodec)
		{
			logPrint(MX_LOG_ERROR, "could not find video decoder for '%s' \n", avcodec_get_name(AV_CODEC_ID_HEVC));
			return mxfalse;
		}

		AVCodec *audioCodec = avcodec_find_decoder(AV_CODEC_ID_OPUS);
		if (NULL == audioCodec)
		{
			logPrint(MX_LOG_ERROR, "could not find audio decoder for '%s' \n", avcodec_get_name(AV_CODEC_ID_OPUS));
			return mxfalse;
		}

		m_inputVideoCtx = avcodec_alloc_context3(videoCodec);
		m_inputAudioCtx = avcodec_alloc_context3(audioCodec);

		m_inputVideoCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

		m_inputVideoCtx->codec_id = AV_CODEC_ID_HEVC;
		m_inputVideoCtx->bit_rate =  1500*1000;
		m_inputVideoCtx->width = 2560;
		m_inputVideoCtx->height = 1440;
		m_inputVideoCtx->pix_fmt  = AV_PIX_FMT_YUV420P;
		m_inputVideoCtx->time_base = AVRational{1, m_fps};

		m_inputAudioCtx->codec_id = AV_CODEC_ID_OPUS;
		m_inputAudioCtx->sample_fmt = AV_SAMPLE_FMT_FLTP;
		m_inputAudioCtx->sample_rate = 16000;
		m_inputAudioCtx->channels = 1;
		m_inputAudioCtx->channel_layout = AV_CH_LAYOUT_MONO;
		m_inputAudioCtx->time_base = AVRational{ 1, m_inputAudioCtx->sample_rate };

		avcodec_parameters_from_context(m_videoInputStream->codecpar, m_inputVideoCtx);
		avcodec_parameters_from_context(m_audioInputStream->codecpar, m_inputAudioCtx);

        uint8_t cOpusHeader[19] = {
            0x4f,0x70,0x75,0x73,0x48,0x65,0x61,0x64,  ///< opus header
            0x01,                                     ///< version
            0x01,                                     ///< channel                
            0x68,0x00,                                ///< preskip
            0x80,0x3e,0x00,0x00,                       ///< sample_rate = 160000
            0x00,0x00,0x00                              ///< Output Gain,Channel Mapping Family
        };
        m_audioInputStream->codecpar->extradata  = (uint8_t*)av_malloc(sizeof(cOpusHeader));
        memcpy(m_audioInputStream->codecpar->extradata, cOpusHeader, sizeof(cOpusHeader));
        m_audioInputStream->codecpar->extradata_size = sizeof(cOpusHeader);

		if (!(m_mp4FormatCtx->oformat->flags & AVFMT_NOFILE)) {
			if (avio_open(&m_mp4FormatCtx->pb, mp4FileName.c_str(), AVIO_FLAG_WRITE) < 0) {
				logPrint(MX_LOG_ERROR, "Could not not open output file");
				return mxfalse;
			}
		}

		logPrint(MX_LOG_DEBUG, "begin to print msg outputFormatCtx333............");
		av_dump_format(m_mp4FormatCtx, 0, mp4FileName.c_str(), 1); /// 显示一些信息
		logPrint(MX_LOG_DEBUG, "end to print msg MX_LOG_DEBUG............");

		ret = avformat_write_header(m_mp4FormatCtx, nullptr);
		if (ret < 0)
		{
			logPrint(MX_LOG_ERROR, "could not write mp4 header");
			return mxfalse;
		}

		m_iVideoIndex = av_find_best_stream(m_mp4FormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
		if(-1 == m_iVideoIndex)
		{
			logPrint(MX_LOG_ERROR, "could not find best video stream");
			avformat_close_input(&m_mp4FormatCtx);
			return mxfalse;
		}

		m_iAudioIndex = av_find_best_stream(m_mp4FormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
		if (-1 == m_iAudioIndex)
		{
			logPrint(MX_LOG_ERROR, "could not find best audio stream");
			avformat_close_input(&m_mp4FormatCtx);
			return mxfalse;
		}
		logPrint(MX_LOG_DEBUG, "mp4 write init success, videoIndex: %d, audioIndex:%d", m_iVideoIndex, m_iAudioIndex);


#ifdef OUTPUT_HEVC_FILE
		m_h265FormatCtx = NULL;
		avformat_alloc_output_context2(&m_h265FormatCtx, NULL, NULL, OUTPUT_HEVC_FILE);
		if (!m_h265FormatCtx) 
		{
			logPrint(MX_LOG_ERROR, "Could not allocate hevc output format context");
			return mxfalse;
		}
		m_h265videoInputStream = avformat_new_stream(m_h265FormatCtx, NULL);
		AVCodec *h265videoCodec = avcodec_find_decoder(AV_CODEC_ID_HEVC);
		if (NULL == h265videoCodec)
		{
			logPrint(MX_LOG_ERROR, "could not find video decoder for '%s' \n", avcodec_get_name(AV_CODEC_ID_HEVC));
			return mxfalse;
		}
		m_h265inputVideoCtx = avcodec_alloc_context3(h265videoCodec);
		m_h265inputVideoCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
		///TODO  output这边应该添加信息
		m_h265inputVideoCtx->codec_id = AV_CODEC_ID_HEVC;
		m_h265inputVideoCtx->bit_rate =  1500*1000;
		m_h265inputVideoCtx->width = 2560;
		m_h265inputVideoCtx->height = 1440;
		// videoInputStream->codecpar->gop_size = m_fps;
		m_h265inputVideoCtx->pix_fmt  = AV_PIX_FMT_YUV420P;
		m_h265inputVideoCtx->time_base = AVRational{1, m_fps};

		avcodec_parameters_from_context(m_h265videoInputStream->codecpar, m_h265inputVideoCtx);

		logPrint(MX_LOG_DEBUG, "m_h265videoInputStream->codecpar->extradata_size:%d", m_h265videoInputStream->codecpar->extradata_size);

		if (!(m_h265FormatCtx->oformat->flags & AVFMT_NOFILE)) {
			if (avio_open(&m_h265FormatCtx->pb, OUTPUT_HEVC_FILE, AVIO_FLAG_WRITE) < 0) {
				logPrint(MX_LOG_ERROR, "Could not not open output file");
				return mxfalse;
			}
		}

		logPrint(MX_LOG_DEBUG, "begin to print msg outputFormatCtx333............");
		av_dump_format(m_h265FormatCtx, 0, OUTPUT_HEVC_FILE, 1); /// 显示一些信息
		logPrint(MX_LOG_DEBUG, "end to print msg MX_LOG_DEBUG............");

		ret = avformat_write_header(m_h265FormatCtx, nullptr);
		if (ret < 0)
		{
			logPrint(MX_LOG_ERROR, "could not write mp4 header");
			return mxfalse;
		}
#endif



#ifdef OUTPUT_OPUS_FILE
		m_opusFormatCtx = NULL;
		avformat_alloc_output_context2(&m_opusFormatCtx, NULL, NULL, OUTPUT_OPUS_FILE);
		if (!m_opusFormatCtx) 
		{
			logPrint(MX_LOG_ERROR, "Could not allocate opus output format context");
			return mxfalse;
		}
		m_opusaudioInputStream = avformat_new_stream(m_opusFormatCtx, NULL);
		AVCodec *opusAudioCodec = avcodec_find_decoder(AV_CODEC_ID_OPUS);
		if (NULL == opusAudioCodec)
		{
			logPrint(MX_LOG_ERROR, "could not find audio decoder for '%s' \n", avcodec_get_name(AV_CODEC_ID_OPUS));
			return mxfalse;
		}
		m_opusinputAudioCtx = avcodec_alloc_context3(opusAudioCodec);
		m_opusinputAudioCtx->codec_id = AV_CODEC_ID_OPUS;
		m_opusinputAudioCtx->sample_fmt = AV_SAMPLE_FMT_FLTP;
		// m_inputAudioCtx->bit_rate = 20000;
		m_opusinputAudioCtx->sample_rate = 16000;
		m_opusinputAudioCtx->channels = 1;
		m_opusinputAudioCtx->channel_layout = AV_CH_LAYOUT_MONO;
		// m_opusinputAudioCtx->time_base.den = m_opusinputAudioCtx->sample_rate;
		// m_opusinputAudioCtx->time_base.num =  1;
		m_opusinputAudioCtx->time_base = AVRational{ 1, m_inputAudioCtx->sample_rate };
		// AVRational{ 1, m_opusinputAudioCtx->sample_rate };
		avcodec_parameters_from_context(m_opusaudioInputStream->codecpar, m_opusinputAudioCtx);

        // uint8_t cOpusHeader[19] = {
        //     0x4f,0x70,0x75,0x73,0x48,0x65,0x61,0x64,  ///< opus header
        //     0x01,                                     ///< version
        //     0x01,                                     ///< channel                
        //     0x68,0x00,                                ///< preskip
        //     0x80,0x3e,0x00,0x00,                       ///< sample_rate = 160000
        //     0x00,0x00,0x00                              ///< Output Gain,Channel Mapping Family
        // };
        m_opusaudioInputStream->codecpar->extradata  = (uint8_t*)av_malloc(19);
        memcpy(m_opusaudioInputStream->codecpar->extradata, cOpusHeader, sizeof(cOpusHeader));
        m_opusaudioInputStream->codecpar->extradata_size = sizeof(cOpusHeader);
		m_opusaudioInputStream->time_base = m_opusinputAudioCtx->time_base ;

		if (!(m_opusFormatCtx->oformat->flags & AVFMT_NOFILE)) {
			if (avio_open(&m_opusFormatCtx->pb, OUTPUT_OPUS_FILE, AVIO_FLAG_WRITE) < 0) {
				logPrint(MX_LOG_ERROR, "Could not not open output file");
				return mxfalse;
			}
		}

		logPrint(MX_LOG_DEBUG, "begin to print msg outputFormatCtx333............");
		av_dump_format(m_opusFormatCtx, 0, OUTPUT_OPUS_FILE, 1); /// 显示一些信息
		logPrint(MX_LOG_DEBUG, "end to print msg MX_LOG_DEBUG............");

		ret = avformat_write_header(m_opusFormatCtx, nullptr);
		if (ret < 0)
		{
			logPrint(MX_LOG_ERROR, "could not write opus header");
			return mxfalse;
		}
#endif

		return mxtrue;		
	}

	mxbool COffLineEventProc::writeMP4DeInit()
	{
		logPrint(MX_LOG_DEBUG, "mp4 deinit begin free");
		if (m_mp4FormatCtx)
		{
			av_write_trailer(m_mp4FormatCtx);
			logPrint(MX_LOG_DEBUG, "mp4 deinit begin free111");
			if (!(m_mp4FormatCtx->oformat->flags & AVFMT_NOFILE)) 
			{
				avio_closep(&m_mp4FormatCtx->pb);
			}
			logPrint(MX_LOG_DEBUG, "mp4 deinit begin free222");
			avformat_free_context(m_mp4FormatCtx);
			m_mp4FormatCtx = NULL;
		}

#ifdef OUTPUT_HEVC_FILE
		if (m_h265FormatCtx)
		{
			av_write_trailer(m_h265FormatCtx);
			logPrint(MX_LOG_DEBUG, "h265 deinit begin free111");
			if (!(m_h265FormatCtx->oformat->flags & AVFMT_NOFILE)) 
			{
				avio_closep(&m_h265FormatCtx->pb);
			}
			logPrint(MX_LOG_DEBUG, "h265 deinit begin free222");
			avformat_free_context(m_h265FormatCtx);
			m_h265FormatCtx = NULL;
		}
#endif

#ifdef OUTPUT_OPUS_FILE
		if (m_opusFormatCtx)
		{
			av_write_trailer(m_opusFormatCtx);
			logPrint(MX_LOG_DEBUG, "opus deinit begin free111");
			if (!(m_opusFormatCtx->oformat->flags & AVFMT_NOFILE)) 
			{
				avio_closep(&m_opusFormatCtx->pb);
			}
			logPrint(MX_LOG_DEBUG, "opus deinit begin free222");
			avformat_free_context(m_opusFormatCtx);
			m_opusFormatCtx = NULL;
		}
#endif

		if (m_inputAudioCtx)
		{
			avcodec_free_context(&m_inputAudioCtx);
		}
		logPrint(MX_LOG_DEBUG, "mp4 deinit begin free333");
		if (m_inputVideoCtx)
		{
			avcodec_free_context(&m_inputAudioCtx);
		}
		logPrint(MX_LOG_DEBUG, "mp4 deinit begin free444");

		return mxtrue;
	}
}
