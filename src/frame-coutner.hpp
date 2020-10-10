#pragma once

#include "packet-source.hpp"
#include "media/video-decoder.hpp"
#include "common/circular-buffer.hpp"
#include "packet-source-subscriber.hpp"

namespace challenge { namespace media {

   class FrameCounter : public media::PacketSourceSubscriber {
    public:
      FrameCounter(int durationMS);
      ~FrameCounter();

      virtual std::string ObjectName() override {
         return "FrameCounter";
      }

      virtual bool Setup(const AVPacketSource* source) override;

      void Start();
      void Stop();

    private:
      void readLoop();
      void frameCallback(FramePtr frame);

      bool m_isStarted;
      bool m_needToStop;
      int64_t m_lastFrameTime;
      int64_t m_frameCounts;
      int m_currentDuration;
      int m_targetDuration;
      double m_fps;
      VideoDecoder m_decoder;
      AVRational m_streamBaseTime;
      common::CircularBuffer<int> m_durations;
      common::async::Thread m_readThrd;
   };

}}  // namespace challenge::media