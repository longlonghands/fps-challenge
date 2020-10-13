#include "frame-coutner.hpp"
#include "packet-source.hpp"
#include <spdlog/spdlog.h>

namespace challenge { namespace media {
   
   FrameCounter::FrameCounter(int duration)
      : PacketSourceSubscriber(50)
      , m_durations(15)
      , m_targetDuration(duration)
      , m_fps(0.0)
      , m_frameCounts(-1)
      , m_lastFrameTime(0)
      , m_currentDuration(0)
      , m_isStarted(false) {}

   FrameCounter::~FrameCounter() {}

   bool FrameCounter::Setup(const AVPacketSource* source) {
      auto videoStream = source->VideoStream();
      if (videoStream == nullptr) {
         return false;
      }
      m_streamBaseTime = videoStream->time_base;
      spdlog::info("setupping frame counter");
      return m_decoder.Open([this](int width, int height, FramePtr frame) { this->frameCallback(frame); },
                     videoStream->codecpar, m_streamBaseTime);
   }

   void FrameCounter::Start() {
      if (m_isStarted) {
         return;
      }
      if (!m_decoder.IsInitiated()) {
         spdlog::info("initializing the decoder failed");
         return;
      }
      m_needToStop = false;
      m_readThrd.start([&]() { readLoop(); });
   }

   void FrameCounter::Stop() {
      m_needToStop = true;
      m_readThrd.join();
      m_decoder.Close();
      m_isStarted = false;
   }

   void FrameCounter::readLoop() {
      m_isStarted = true;
      spdlog::info("fps counter started");
      while (!m_needToStop) {
         auto pkt = ReadPacket();
         if (!pkt) {
            common::async::sleep(10);
            continue;
         }
         // spdlog::info("pkt pts is {}", pkt->PTS());
         m_decoder.Decode(pkt);
      }
      m_isStarted = false;
      m_needToStop = false;
   }

   void FrameCounter::frameCallback(FramePtr frame) {
      m_frameCounts++;
      AVRational perSecond = AVRational{1, 1000};
      auto frameTime = av_rescale_q_rnd(frame->PTS(), m_streamBaseTime, perSecond, AV_ROUND_NEAR_INF);
      int duration = int(frameTime - m_lastFrameTime);
      m_lastFrameTime = frameTime;
      m_currentDuration += duration;
      // spdlog::info("frame pts is {}", frame->PTS());
      // spdlog::info("frame duration is {}", duration);
      if (m_currentDuration>m_targetDuration) {
         m_fps = m_frameCounts / (m_targetDuration/1000);
         spdlog::info("frame-rate is {}", m_fps);
         m_currentDuration = 0;
         m_frameCounts = 0;
      }
      // m_durations.push_front(duration);
      // m_fps = 1000/(double)m_durations.mean();
      
   }
}}  // namespace challenge::media