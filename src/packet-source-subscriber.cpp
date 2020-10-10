#include "packet-source-subscriber.hpp"
#include "common/random-string.hpp"
#include "packet-source.hpp"

namespace challenge { namespace media {
   PacketSourceSubscriber::PacketSourceSubscriber(int queueSize)
       : m_isInitialized(true), m_packetQueue(queueSize) {}

   PacketSourceSubscriber::~PacketSourceSubscriber() {
      // if PacketSourceSubscriber destroyed.
      // PacketSource knows and unsubscribe it,
      // so no need here to unsubscribe this from packet source
      m_isInitialized = false;
   }

   std::string PacketSourceSubscriber::ObjectId() {
      if (m_objectId.length() < 10) {
         // we cant call all the time ObjectName() because it's a virtual function
         m_objectId = common::randomString();
         std::string objName = ObjectName();
         m_objectId = objName.append(m_objectId);
      }
      return m_objectId;
   }

   void PacketSourceSubscriber::NewPacket(Packet::Ptr pkt) {
      if (!pkt || !m_isInitialized)
         return;
      m_packetQueue.push_front(pkt);
   }

   Packet::Ptr PacketSourceSubscriber::ReadPacket() {
      Packet::Ptr packet = m_packetQueue.pop_back();
      return std::move(packet);
   }
}}  // namespace challenge::media