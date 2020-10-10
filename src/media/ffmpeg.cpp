#include "ffmpeg.h"
#include <fstream>

static void ErrorCallback(void *ptr, int level, const char *fmt, va_list vl) {
   char line[1024];
   int printPrefix = 0;
   av_log_format_line(ptr, level, fmt, vl, line, 1024, &printPrefix);
   printf("%s\n", line);
   std::ofstream file("libav-log.txt", std::ios_base::app);
   file << line << std::endl;
   file.flush();
}

namespace challenge { namespace media {
   std::unique_ptr<FFmpegInitializer> FFmpegInitializer::instance;

   FFmpegInitializer::FFmpegInitializer() {}

   FFmpegInitializer::~FFmpegInitializer() {}

   void FFmpegInitializer::Init() {
      if (!instance) {
         instance = std::unique_ptr<FFmpegInitializer>(new FFmpegInitializer());
         // av_register_all();

         // avcodec_register_all();
         // avdevice_register_all();
         // av_register_hwaccel(AVHWAccel);
         avformat_network_init();
         // av_log_set_level(AV_LOG_TRACE);
         // av_log_set_callback(ErrorCallback);
      }
   }
}}  // namespace challenge::media
