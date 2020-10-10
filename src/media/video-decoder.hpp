#pragma once


#include "media/frame.hpp"
#include "media/packet.hpp"
#include "media/ffmpeg.h"
#include <functional>

namespace challenge { namespace media {
   typedef std::function<void(int width, int height, FramePtr frame)> FrameCallback;

   class VideoDecoder  {
    public:
      VideoDecoder();
      ~VideoDecoder();

      bool Open(FrameCallback callback, AVCodecParameters* codecParams, AVRational time_base);

      bool Decode(Packet::Ptr& pkt) const;

      void Flush();

      void Close();

      bool IsInitiated() const {
         return m_isReady;
      }

    private:
      VideoDecoder(const VideoDecoder&);
      VideoDecoder& operator=(const VideoDecoder&);

      void handleDecodedFrame() const;

      bool m_isReady;
      bool m_needToStop;
      FrameCallback m_frameCallback;
      AVCodecContext* m_codecContext;
      AVCodec* m_codec;
      AVFrame* m_avframe;
      AVPacket* m_avpacket;
   };
}}  // namespace challenge::media
