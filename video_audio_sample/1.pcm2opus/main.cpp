#include "audio_codec_entity.h"

namespace maix {

    TestCodeC::TestCodeC()
    {
        m_bDecodeInit = mxfalse;
        m_pOpusDecoder = NULL;
    }

    TestCodeC::~TestCodeC()
    {
        
    }

    mxbool TestCodeC::initOpusDecode()
    {
        std::unique_lock<std::mutex> lck(m_decodeMutex);

        logPrint(MX_LOG_INFOR, "opus_creat\n");
        int iErr = 0;
        m_pOpusDecoder = opus_decoder_create(MX_DECODE_RATE, MX_DECODE_CHANNEL, &iErr);
        if ((iErr != OPUS_OK) || (m_pOpusDecoder == NULL))
        {
            logPrint(MX_LOG_INFOR, "opus decoder create failed:%s", opus_strerror(iErr));
            return mxfalse;
        }
        m_bDecodeInit = mxtrue;

        return mxtrue;
    }

    mxbool TestCodeC::unInitOpusDecode()
    {
        std::unique_lock<std::mutex> lck(m_decodeMutex);
        logPrint(MX_LOG_INFOR, "opus_destroy\n");
        opus_decoder_destroy(m_pOpusDecoder);

        m_bDecodeInit = mxfalse;

        return mxtrue; 
    }

    int TestCodeC::opusDecode(uint8_t * pInData, int inLen, uint8_t *pOutData, int inEncFinalRange)
    {
        std::unique_lock<std::mutex> lck(m_decodeMutex);
        int resultLen = -1;
        if(!m_bDecodeInit)
        {
            logPrint(MX_LOG_ERROR, "opus_decode: not create\n");
            return resultLen;
        }

        unsigned int decFinalRange;
        short *out = (short*)malloc(MX_DECODE_MAX_FRAME_SIZE * MX_DECODE_CHANNEL * sizeof(short));
        opus_int32 output_samples = opus_decode(m_pOpusDecoder, pInData, inLen, out, MX_DECODE_MAX_FRAME_SIZE, 0);
        if (output_samples>0)
        {
            int i;
            for(i = 0; i < (output_samples) * MX_DECODE_CHANNEL; i++)
            {
                short s;
                s = out[i];
                pOutData[2 * i] = s&0xFF;
                pOutData[2 * i + 1] = (s >> 8) & 0xFF;
            }
            resultLen = output_samples * sizeof(short) * MX_DECODE_CHANNEL;
        } else {
            logPrint(MX_LOG_ERROR, "error decoding frame %d \n",output_samples);
        }
        opus_decoder_ctl(m_pOpusDecoder, OPUS_GET_FINAL_RANGE(&decFinalRange));
        /* compare final range encoder rng values of encoder and decoder */
        if( inEncFinalRange !=0  && decFinalRange != inEncFinalRange) {
    //        resultLen = -1;
            logPrint(MX_LOG_ERROR, "Error: Range coder state mismatch inEncFinalRange %d  decFinalRange %d", inEncFinalRange, decFinalRange);
        }

        if (out) 
        {
            free(out);
            out = NULL;
        }

        return resultLen;
    }

    mxbool TestCodeC::initOpusEncode()
    {
        std::unique_lock<std::mutex> lck(m_encodeMutex);

        int iErr = 0;

        m_pOpusEncoder = opus_encoder_create(MX_DECODE_RATE, MX_DECODE_CHANNEL, OPUS_APPLICATION_VOIP, &iErr);
        if ((iErr != OPUS_OK) || (m_pOpusEncoder == NULL))
        {
            logPrint(MX_LOG_INFOR, "opus encoder create failed:%s", opus_strerror(iErr));
            return mxfalse;
        }

        opus_encoder_ctl(m_pOpusEncoder, OPUS_SET_VBR(0));//0:CBR, 1:VBR
        opus_encoder_ctl(m_pOpusEncoder, OPUS_SET_VBR_CONSTRAINT(true));
        opus_encoder_ctl(m_pOpusEncoder, OPUS_SET_BITRATE(MX_DECODE_RATE * sizeof(short) * MX_DECODE_CHANNEL));
        opus_encoder_ctl(m_pOpusEncoder, OPUS_SET_COMPLEXITY(8));//8    0~10
        opus_encoder_ctl(m_pOpusEncoder, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE));
        opus_encoder_ctl(m_pOpusEncoder, OPUS_SET_LSB_DEPTH(16));
        opus_encoder_ctl(m_pOpusEncoder, OPUS_SET_DTX(0));
        opus_encoder_ctl(m_pOpusEncoder, OPUS_SET_INBAND_FEC(0));

        m_bEncodeInit = mxtrue;
        logPrint(MX_LOG_DEBUG, "opus init encode success:%p\n", m_pOpusEncoder);
        return mxtrue;
    }

    mxbool TestCodeC::unInitOpusEncode()
    {
        std::unique_lock<std::mutex> lck(m_encodeMutex);

        logPrint(MX_LOG_INFOR, "opus encode destroy\n");
        opus_encoder_destroy(m_pOpusEncoder);

        m_bEncodeInit = mxfalse;
    }

    int TestCodeC::opusEncode(opus_int16 * pInData, int inLen, uint8_t *pOutData, int iOutLen)
    {
        std::unique_lock<std::mutex> lck(m_encodeMutex);
        if(!m_bEncodeInit)
        {
            logPrint(MX_LOG_ERROR, "opus encode faied, encoder is not inited\n");
            return -1;
        }

        int iLen = opus_encode(m_pOpusEncoder, pInData, inLen, pOutData, iOutLen);
        if ((iLen < 0) || (iLen >  iOutLen))
        {
            logPrint(MX_LOG_ERROR, "opus encode failed, len is %d", iLen);
            return -1;
        }

        return iLen;
    }

}

void CAudioManageChannel::pcmToOpus()
    {
		int enc_final_range = 0;
		int opusLen = 0;
        int sampleCount = 16000; /// 采样率
        int channelCount = 1; /// 通道数
        int sampleWidth = 16; ///采样宽度
        int packetSize = 1280; /// 一帧音频包大小
        unsigned char pcm_data[packetSize] = {0};
		unsigned char opus_to_pcm_data[packetSize] = {0};
        unsigned char opus_data[packetSize] = {0};
        opus_int16 pcm_int16_data[packetSize] = {0};

        FILE *pcm_file = fopen("./pcm16bit_16ksample.pcm", "r");
        FILE *opus_file = fopen("./pcm16bit_16ksample.opus", "w+");
		FILE *out_pcm_file = fopen("./out.pcm", "w+");
        if (pcm_file  == NULL || opus_file == NULL)
        {
            logPrint(MX_LOG_DEBUG, "pcmToOpus open file error");
            return ;
        } 
        int n = 0;

        while (!feof(pcm_file)) {
            n = fread(pcm_data, 1, packetSize, pcm_file);
			for (int i = 0; i < n; i++) {
				pcm_int16_data[i] = pcm_data[2*i + 1] << 8 | pcm_data[2*i];
			}

            // unsigned char int_field[4];
            // int_to_char(n, int_field);
            // if (fwrite(int_field, 1, 4, opus_file) != 4) {
            //    logPrint(MX_LOG_DEBUG, "Error writing.\n");
            //    return ;
            // }
            // int_to_char(m_objAudioCodec->getFinalRange(), int_field);
            // if (fwrite(int_field, 1, 4, opus_file) != 4) {
            //    logPrint(MX_LOG_DEBUG, "Error writing11.\n");
            //    return ;
            // }
            // if (fwrite(data[toggle], 1, len[toggle], fout) != (unsigned)len[toggle]) {
            //    fprintf(stderr, "Error writing.\n");
            //    goto failure;
            // }

            int len = m_objAudioCodec->opusEncode((opus_int16 *)pcm_int16_data, n/2, opus_data, packetSize);
            logPrint(MX_LOG_DEBUG, "pcmToOpus read len:%d, encode len:%d", n, len);
            int write_len = fwrite(opus_data, 1, len, opus_file);
            logPrint(MX_LOG_DEBUG, "pcmToOpus write len:%d", write_len);

			int opus_to_pcm_len = m_objAudioCodec->opusDecode(opus_data, len, opus_to_pcm_data, 0);
            int out_pcm_write_len = fwrite(opus_to_pcm_data, 1, opus_to_pcm_len, out_pcm_file);
            logPrint(MX_LOG_DEBUG, "pcmToOpus out_pcm_write_len:%d, opus_to_pcm_len:%d", out_pcm_write_len, opus_to_pcm_len);
        }
        logPrint(MX_LOG_DEBUG, "pcmToOpus wirte done");
        fclose(pcm_file);
		fclose(out_pcm_file);
        fclose(opus_file);
    }