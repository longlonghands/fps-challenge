#pragma once
#include "media/ffmpeg.h"
#include "common/thread.hpp"
#include "packet-source-subscriber.hpp"
#include <mutex>
#include <list>

namespace challenge { namespace media {
   class AVPacketSource {
    public:
      AVPacketSource();
      virtual ~AVPacketSource() {}

      virtual bool Start(std::string uri);
      virtual void Stop();

      std::string Uri() const {
         return m_uri;
      }

      virtual bool IsStarted() const {
         return m_isStarted;
      }

      const AVStream* VideoStream() const;
      const AVStream* AudioStream() const;

      void Subscribe(PacketSourceSubscriberPtr subscriber);
      void Unsubscribe(PacketSourceSubscriberPtr subscriber);

    protected:
      virtual void publishToAll(Packet::Ptr pkt);
      virtual void onSubscribed(PacketSourceSubscriberPtr) {}
      virtual void onUnsubscribed(PacketSourceSubscriberPtr) {}

      void removeTerminatedPacketSource();

      std::list<PacketSourceSubscriberPtr> m_PacketSubscribers;

      std::string m_uri;
      bool m_isStarted;
      bool m_needToStop;

   private:
      void readLoop();
      void avCleanUp();

      std::mutex m_mtx;

      AVFormatContext* m_fmtCtx = nullptr;
      int video_stream_idx = -1, audio_stream_idx = -1;
      AVPacket pkt;
      int64_t m_lastPktId;
      common::async::Thread m_readThrd;
   };
}}