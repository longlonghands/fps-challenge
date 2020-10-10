#include "packet.hpp"
#include <string.h>

namespace challenge { namespace media {

   Packet::Ptr Packet::MakeVideoPacket(int64_t newId, uint32_t _capacity /*= DEFAULT_PACKET_CAPACITY*/) {
      return std::make_unique<Packet>(constructor_accessor{}, newId, _capacity, PacketFlags::VideoPacket);
   }

   Packet::Ptr Packet::MakeAudioPacket(int64_t newId, uint32_t _capacity /*= DEFAULT_PACKET_CAPACITY*/) {
      return std::make_unique<Packet>(constructor_accessor{}, newId, _capacity, PacketFlags::AudioPacket);
   }

   Packet::Ptr Packet::CreateFromAVPacket(int64_t newId, const ::AVPacket* pkt, PacketFlags flag) {
      if (pkt == nullptr || pkt->size < 2) {
         return Packet::Ptr{};
      }

      auto packet = std::make_unique<Packet>(constructor_accessor{}, pkt->size + 100, flag);
      packet->Store(pkt->data, pkt->size);
      packet->PTS(pkt->pts);
      packet->StreamId(pkt->stream_index);
      
      packet->AddFlag(flag);
      if (pkt->flags & AV_PKT_FLAG_KEY) {
         packet->AddFlag(PacketFlags::HasKeyFrame);
      }
      if (pkt->flags & AV_PKT_FLAG_CORRUPT) {
         packet->AddFlag(PacketFlags::IsCorrupted);
      }
      return packet;
   }

   Packet::Packet(constructor_accessor accessor, int64_t newId, PacketFlags flag)
       : Packet(accessor, newId, DEFAULT_PACKET_CAPACITY, flag)  // 320KB
   {}

   Packet::Packet(constructor_accessor,int64_t newId, uint32_t _capacity, PacketFlags flag)
       : m_size(0)
       , m_capacity(_capacity)
       , m_data(_capacity)
       , m_id(newId)
       , m_nalUnit(NalUnitTypes::Unknown)
       , m_startCodeLength(0)
       , m_flags(int32_t(flag))
       , m_streamId(0)
       , m_groupId(0)
       , m_pts(0)
       , m_duration(0)
       , m_dts(0) {}

   Packet::~Packet() {}

   void Packet::Add(const uint8_t* _data, uint32_t length) {
      if (m_data.size() < (m_size + length)) {
         m_data.resize(m_size + length);
      }
      memcpy(m_data.data() + m_size, _data, length);
      m_size += length;
      checkData();
   }

   void Packet::Store(const uint8_t* _data, uint32_t length) {
      if (m_data.size() < length) {
         m_data.resize(length, false);
      }
      m_size = length;
      memcpy(m_data.data(), _data, m_size);
      checkData();
   }

   Packet::Ptr Packet::Clone() {
      auto newPkt = std::make_unique<Packet>(constructor_accessor{}, m_capacity,
                                             PacketFlags::VideoPacket);
      newPkt->m_flags = m_flags;
      newPkt->m_startCodeLength = m_startCodeLength;
      newPkt->m_pts = m_pts;
      newPkt->m_dts = m_dts;
      newPkt->m_groupId = m_groupId;
      newPkt->m_streamId = m_streamId;
      newPkt->m_size = m_size;
      newPkt->m_duration = m_duration;
      newPkt->m_data = m_data;
      return std::move(newPkt);
   }

   void Packet::checkData() {
      int32_t tmpFlag = m_flags;
      RemoveAllFlag();
      if (tmpFlag & int32_t(PacketFlags::VideoPacket))
         AddFlag(PacketFlags::VideoPacket);
      else if (tmpFlag & int32_t(PacketFlags::AudioPacket)) {
         AddFlag(PacketFlags::AudioPacket);
         m_startCodeLength = 0;
         m_nalUnit = NalUnitTypes::Unknown;
         return;
      }

      uint8_t* packet = m_data.data();
      m_startCodeLength = 0;
      if (packet[2] == 1 /*24bit start-code*/)
         m_startCodeLength = 3;
      else if (packet[3] == 1 /*32bit start-code*/)
         m_startCodeLength = 4;

      int fragment_type = packet[m_startCodeLength + 0] & 0x1F;
      int nal_type = packet[m_startCodeLength + 1] & 0x1F;
      int start_bit = packet[m_startCodeLength + 1] & 0x80;

      if (fragment_type == 6) {
         m_nalUnit = NalUnitTypes::SEI;
      }
      if (fragment_type == 7)  // 7 is SPS
      {
         m_nalUnit = NalUnitTypes::SPS;

      } else if (fragment_type == 8)  // 8 is PPS
      {
         m_nalUnit = NalUnitTypes::PPS;
      } else if (((fragment_type == 28 || fragment_type == 29) && nal_type == 5 &&
                  start_bit == 128) ||
                 fragment_type == 5) {
         m_nalUnit = NalUnitTypes::I_Frame;
         AddFlag(PacketFlags::HasKeyFrame);
      } else if (fragment_type == 1)  // it's a SLICE (can be P-Frame or B-frame)
      {
         m_nalUnit = NalUnitTypes::P_Frame;
      } else {
         m_nalUnit = NalUnitTypes::Unknown;
         // we can mark packet as PacketFlags::IsCorrupted, but we are not sure so we won't do it.
      }
   }

   void Packet::Clear() {
      m_size = 0;
      m_data.clear();
      m_nalUnit = NalUnitTypes::Unknown;
      m_startCodeLength = 0;
      m_flags = 0;
      m_streamId = 0;
      m_groupId = 0;
      m_pts = 0;
   }

   std::string Packet::NalUnitTypeString() const {
      if (HasFlag(PacketFlags::AudioPacket))
         return "Audio";

      switch (m_nalUnit) {
         case media::NalUnitTypes::Unknown: return "Unknown";
         case media::NalUnitTypes::SEI: return "SEI";
         case media::NalUnitTypes::SPS: return "SPS";
         case media::NalUnitTypes::PPS: return "PPS";
         case media::NalUnitTypes::I_Frame: return "I_Frame";
         case media::NalUnitTypes::P_Frame: return "P_Frame";
         case media::NalUnitTypes::B_Frame: return "B_Frame";
         default: return "Unknown";
      }
   }

   inline std::string Packet::PacketTypeToString(Ptr& pkt) {
      if (!pkt)
         return "Invalid_Packet";
      return pkt->NalUnitTypeString();
   }

}}  // namespace challenge::media
