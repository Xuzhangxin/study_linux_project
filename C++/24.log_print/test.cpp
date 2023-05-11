
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/timeb.h>
#include <string>
typedef enum
{
MX_LOG_ERROR,
MX_LOG_WARN,
MX_LOG_INFOR,
MX_LOG_DEBUG,
MX_LOG_TRACE,
}E_MX_LOG_LEVEL;


	typedef enum
	{
		E_P_NULL,
		E_P_VIDEO_YUV,
		E_P_VIDEO_H264,
		E_P_VIDEO_H265,
		E_P_AUDIO_PCM,
		E_P_AUDIO_AAC,
		E_P_AUDIO_OPUS,
	}E_P_TYPE;

    
	std::string getLogTime(void)
	{
		std::string str;
		char    szTime[120];
		struct  tm      *ptm;
		struct  timeb   stTimeb;
		ftime(&stTimeb);
		ptm = localtime(&stTimeb.time);
		sprintf(szTime, "%d %02d-%02d %02d:%02d:%02d.%03d",
			ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, stTimeb.millitm);
		str = szTime;

		return str;
	}

void logPrint(E_MX_LOG_LEVEL eLevel, const char *format, ...)
{
    va_list arg;
    va_start(arg, format);
    char buf[1024] = { 0 };
    vsnprintf(buf, sizeof(buf), format, arg);
    va_end(arg);

    printf("%s [%s] %s\n", getLogTime().c_str(), 
        "local_storage_app", buf);
    fflush(stdout);
}


int main(int argc, char* argv[])
{
    int64_t  len = 105555555555;
    std::string m_strName = "audio_channel_1_server";
    logPrint(MX_LOG_DEBUG, "video after: %s, type:%d, send frame len: %d, timestamp:%ld",
    	m_strName.c_str(), E_P_AUDIO_OPUS, 10235, len);


    logPrint(MX_LOG_DEBUG, "video after timestamp:..........:%ld", len);

    return 0;
}