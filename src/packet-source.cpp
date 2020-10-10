#include "packet-source.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>

namespace challenge { namespace media {
   AVPacketSource::AVPacketSource() : m_isStarted(false), m_needToStop(false), m_lastPktId(0) {}

   bool AVPacketSource::Start(std::string url) {
      m_uri = url;
      m_isStarted = false;

      try {
         m_fmtCtx = avformat_alloc_context();
         if (avformat_open_input(&m_fmtCtx, m_uri.c_str(), NULL, NULL) < 0) {
            throw FFmpegException("cannot open url input using libav");
         }
         spdlog::info("avformat_open_input successfully");

         /* retrieve stream information */
         if (avformat_find_stream_info(m_fmtCtx, NULL) < 0) {
            throw FFmpegException("Could not find stream information");
         }
         video_stream_idx = av_find_best_stream(m_fmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
         if (video_stream_idx >= 0) {
            spdlog::info("found video stream");
         }
         audio_stream_idx = av_find_best_stream(m_fmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
         if (audio_stream_idx >= 0) {
            spdlog::info("found audio stream");
         }
         for (auto& sub : m_PacketSubscribers) {
            if (sub) {
               sub->Setup(this);
            }
         }

         m_needToStop = false;

         m_readThrd.start([&]() { readLoop(); });
      } catch (std::exception& ex) {
         spdlog::error(ex.what());
         avCleanUp();
         return false;
      }
      return true;
   }

   void AVPacketSource::Stop() {
      m_needToStop = true;
   }

   const AVStream* AVPacketSource::VideoStream() const {
      if (m_fmtCtx == nullptr || video_stream_idx < 0) {
         return nullptr;
      }
      return m_fmtCtx->streams[video_stream_idx];
   }

   const AVStream* AVPacketSource::AudioStream() const {
      if (m_fmtCtx == nullptr || audio_stream_idx < 0) {
         return nullptr;
      }
      return m_fmtCtx->streams[audio_stream_idx];
   }

   void AVPacketSource::avCleanUp() {
      if (m_fmtCtx != nullptr) {
         avformat_close_input(&m_fmtCtx);
         m_fmtCtx = nullptr;
      }

      video_stream_idx = -1;
      audio_stream_idx = -1;
   }

   void AVPacketSource::readLoop() {
      m_isStarted = true;
      spdlog::info("reading packets started");
      /* read frames from the stream */
      while (!m_needToStop && av_read_frame(m_fmtCtx, &pkt) >= 0) {
         Packet::Ptr packet;
         int64_t pts = 0;
         if (pkt.stream_index == video_stream_idx) {
            packet = Packet::CreateFromAVPacket(++m_lastPktId, &pkt, PacketFlags::VideoPacket);
            packet->StreamId(video_stream_idx);
            packet->Duration(pkt.duration);
         } else if (pkt.stream_index == audio_stream_idx) {
            // ignoreing audio packet for now
         }
         if (packet != nullptr) {
            packet->PTS(pkt.pts);
            packet->DTS(packet->PTS());

            publishToAll(std::move(packet));
         }
         av_packet_unref(&pkt);
      }
      m_isStarted = false;
      // m_needToStop = false;
   }

   void AVPacketSource::Subscribe(PacketSourceSubscriberPtr subscriber) {
      if (m_needToStop)
         return;
      if (!subscriber)
         return;

      std::unique_lock<std::mutex> lock(m_mtx);
      removeTerminatedPacketSource();
      auto res = std::find_if(
          m_PacketSubscribers.begin(), m_PacketSubscribers.end(),
          [&](auto elemntPtr) { return subscriber->ObjectId() == elemntPtr->ObjectId(); });
      if (res == m_PacketSubscribers.end()) {
         m_PacketSubscribers.push_back(subscriber);
      }
      onSubscribed(subscriber);
   }

   void AVPacketSource::Unsubscribe(PacketSourceSubscriberPtr subscriber) {
      if (m_needToStop)
         return;
      if (!subscriber)
         return;

      std::unique_lock<std::mutex> lock(m_mtx);
      auto res = std::find_if(
          m_PacketSubscribers.begin(), m_PacketSubscribers.end(),
          [&](auto elementPtr) { return subscriber->ObjectId() == elementPtr->ObjectId(); });
      if (res != m_PacketSubscribers.end()) {
         m_PacketSubscribers.erase(res);
      }
      removeTerminatedPacketSource();
      onUnsubscribed(subscriber);
   }

   void AVPacketSource::publishToAll(Packet::Ptr pkt) {
      if (m_needToStop || !m_isStarted)
         return;
      std::unique_lock<std::mutex> lock(m_mtx);
      removeTerminatedPacketSource();
      if (m_PacketSubscribers.size() == 1) {
         auto& subscriberPtr = m_PacketSubscribers.front();
         if (subscriberPtr && !subscriberPtr->IsTerminated()) {
            subscriberPtr->NewPacket(std::move(pkt));
         } else {
            m_PacketSubscribers.clear();
         }
      } else {
         for (auto& subscriberPtr : m_PacketSubscribers) {
            if (m_needToStop)
               return;
            if (subscriberPtr && !subscriberPtr->IsTerminated()) {
               auto clonedPkt = pkt->Clone();
               subscriberPtr->NewPacket(std::move(clonedPkt));
            } else {
               // remove
            }
         }
      }
   }

   void AVPacketSource::removeTerminatedPacketSource() {
      auto iter = m_PacketSubscribers.begin();
      while (iter != m_PacketSubscribers.end()) {
         if ((*iter)->IsTerminated()) {
            m_PacketSubscribers.erase(iter++);
         } else {
            ++iter;
         }
      }
   }
}}  // namespace challenge::media