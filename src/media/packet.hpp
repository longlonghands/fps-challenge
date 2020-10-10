#pragma once

#include <stdint.h>
#include <memory>
#include <vector>
#include <deque>
#include "ffmpeg.h"

namespace challenge { namespace media {
   enum class PacketFlags : uint32_t {
      VideoPacket = 0x0001,
      AudioPacket = 0x0002,
      HasKeyFrame = 0x0004,
      IsCorrupted = 0x0008
   };

   enum class NalUnitTypes { Unknown, SEI, SPS, PPS, I_Frame, P_Frame, B_Frame };


   class Packet {
    private:
      struct constructor_accessor {};

    public:
      typedef std::unique_ptr<Packet> Ptr;

      static const uint32_t DEFAULT_PACKET_CAPACITY = 1024 * 240;  // 240KB

      static Ptr MakeVideoPacket(int64_t newId, uint32_t _capacity = DEFAULT_PACKET_CAPACITY);

      static Ptr MakeAudioPacket(int64_t newId, uint32_t _capacity = DEFAULT_PACKET_CAPACITY);

      static Ptr CreateFromAVPacket(int64_t newId, const ::AVPacket* pkt, PacketFlags flag);

      static std::string PacketTypeToString(Ptr &pkt);

      Packet(constructor_accessor,int64_t newId, PacketFlags flag);

      Packet(constructor_accessor,int64_t newId, uint32_t _capacity, PacketFlags flag);

      ~Packet();

      void Add(const uint8_t *_data, uint32_t length);

      // this function will copy the data to its buffer, so you can release your data allocated
      // memory if you want
      void Store(const uint8_t *_data, uint32_t length);

      uint8_t *Data() {
         return m_data.data();
      }

      int Size() const {
         return m_size;
      }

      int64_t PTS() const {
         return m_pts;
      }
      void PTS(int64_t value) {
         m_pts = value;
         if (HasFlag(PacketFlags::AudioPacket)) {
            m_dts = value;
         }
      }

      int64_t DTS() const {
         return m_dts;
      }
      void DTS(int64_t value) {
         m_dts = value;
      }

      int64_t Id() const {
         return m_id;
      }

      int64_t GroupId() const {
         return m_groupId;
      }
      void GroupId(int64_t value) {
         m_groupId = value;
      }

      int64_t StreamId() const {
         return m_streamId;
      }
      void StreamId(int64_t value) {
         m_streamId = value;
      }

      void AddFlag(PacketFlags flag) {
         m_flags |= int32_t(flag);
      }

      void RemoveFlag(PacketFlags flag) {
         m_flags &= (!int32_t(flag));
      }

      bool HasFlag(PacketFlags flag) const {
         return (m_flags & int32_t(flag)) != 0;
      }

      void RemoveAllFlag() {
         m_flags = 0;
      }

      bool IsKey() const {
         return HasFlag(PacketFlags::HasKeyFrame);
      }

      bool isValid() const {
         return !(m_size > 1) && !HasFlag(PacketFlags::IsCorrupted) &&
                m_pts >= 0;  //! data.isEmpty()?
      }

      bool HasStartCode() const {
         return m_startCodeLength > 0;
      }

      int StartCodeLength() const {
         return m_startCodeLength;
      }

      NalUnitTypes NalUnitType() const {
         return m_nalUnit;
      }

      void Duration(int value) {
         m_duration = value;
      }

      int Duration() const {
         return m_duration;
      }

      std::string MetaData() const {
         return m_metaData;
      }

      void MetaData(std::string value) {
         m_metaData = value;
      }

      Ptr Clone();

      void Clear();

      std::string NalUnitTypeString() const;

    private:
      /* after start code which can be 3 bytes of 4 bytes start code:
       * int fragment_type = Data[0] & 0x1F;
       * int nal_type = Data[1] & 0x1F;
       * int start_bit = Data[1] & 0x80;
       * int end_bit = Data[1] & 0x40;
       *
       * fragment_type == 28 or 29 means data represents the video frame fragment
       * 28 is 'Fragmented Unit A(FU-A)' and 29 is FU-B
       *
       */
      void checkData();

      uint32_t m_size;
      uint32_t m_capacity;
      std::vector<uint8_t> m_data;

      NalUnitTypes m_nalUnit;
      int m_startCodeLength;

      int32_t m_flags;
      int64_t m_id;
      int64_t m_streamId;
      int64_t m_groupId;
      int64_t m_pts;
      int64_t m_dts;
      int m_duration;
      std::string m_metaData;
   };

   typedef std::deque<Packet::Ptr> PacketList;

}}  // namespace challenge::media
