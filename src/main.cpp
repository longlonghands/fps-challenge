#include <spdlog/spdlog.h>
#include "spdlog/sinks/basic_file_sink.h"
#include "fmt/fmt.hpp"
#include <iostream>

#include "media/ffmpeg.h"

extern "C" {
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/log.h>
#include <libavutil/base64.h>
#include <libavutil/imgutils.h>
}

#include "packet-source.hpp"
#include "frame-coutner.hpp"

int main(int argc, char* argv[]) {
   if (argc<2) {
      spdlog::info("no media url provided");   
      return 0;
   }
   
   spdlog::set_level(spdlog::level::trace);  // Set global log level to debug
   // change log pattern
   spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");

   challenge::media::FFmpegInitializer::Init();
   std::string url = argv[1];
   try {
      auto frameCounter = std::make_shared<challenge::media::FrameCounter>(2000);
      challenge::media::AVPacketSource pktsource;
      
      pktsource.Subscribe(frameCounter);
      pktsource.Start(url);
      frameCounter->Start();

      std::string input;
      while (input != "quit") {
         std::cin >> input;
      }
      pktsource.Unsubscribe(frameCounter);
      frameCounter->Stop();
      pktsource.Stop();
   } catch (std::exception& ex) {
      spdlog::error(ex.what());
      return 1;
   }

   return 0;
}