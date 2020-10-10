#pragma once

#include <string>
#include <memory>
#include <inttypes.h>
#define __STDC_CONSTANT_MACROS

extern "C" {
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/log.h>
#include <libavutil/base64.h>
#include <libavdevice/avdevice.h>
#include <libavutil/imgutils.h>
}

#include <sstream>

namespace challenge { namespace media {
   class FFmpegInitializer {
      static std::unique_ptr<FFmpegInitializer> instance;
      FFmpegInitializer();

    public:
      ~FFmpegInitializer();
      static void Init();
   };

   class FFmpegException: public std::exception {
      public:
      FFmpegException(int error, std::string message = "") {
         size_t bufferSize = 100;
         char buffer[bufferSize];
         memset(buffer, 0, bufferSize);
         av_strerror(error, buffer, bufferSize);
         std::stringstream strBuilder;
         strBuilder << message << "(error: '" << buffer << "')";
         msg = strBuilder.str();
      }

      FFmpegException(std::string message) {
         msg = message;
      }

      virtual const char* what() const noexcept override {
         return msg.c_str();
      }

    private:
      std::string msg;

   };
}}  // namespace challenge::media