#pragma once
#include <vector>
#include <memory>
#include "media/packet.hpp"
#include "common/circular-buffer.hpp"

namespace challenge { namespace media {
   class AVPacketSource;
   typedef std::shared_ptr<AVPacketSource> AVPacketSourcePtr;
   // typedef std::weak_ptr<PacketSource> PacketSourceRef;

   class PacketSourceSubscriber {
    public:
      explicit PacketSourceSubscriber(int queueSize);

      virtual ~PacketSourceSubscriber();

      virtual std::string ObjectName() = 0;

      std::string ObjectId();

      virtual void NewPacket(Packet::Ptr pkt);

      void Terminate() { m_isInitialized = false; }
      bool IsTerminated() const { return !m_isInitialized; }

      virtual bool Setup(const AVPacketSource* source) = 0;

      virtual Packet::Ptr ReadPacket();

    protected:
      virtual void emptyPacketQueue() { m_packetQueue.clear(); }

      bool m_isInitialized;
      AVPacketSourcePtr m_packetSource;

      common::CircularBuffer<Packet::Ptr> m_packetQueue;
      std::string m_objectId;
   };

   typedef std::shared_ptr<PacketSourceSubscriber> PacketSourceSubscriberPtr;
}}  // namespace challenge::media