#include "video-decoder.hpp"
#include <spdlog/spdlog.h>

namespace challenge { namespace media {
   VideoDecoder::VideoDecoder()
       : m_isReady(false)
       , m_needToStop(false)
       , m_codecContext(nullptr)
       , m_codec(nullptr) {}

   VideoDecoder::~VideoDecoder() {
      Close();
   }

   bool VideoDecoder::Open(FrameCallback callback, AVCodecParameters* codecParams, AVRational time_base) {
      if (codecParams == nullptr) {
         return false;
      }
      if (m_isReady)
         return true;
      
      m_codec = avcodec_find_decoder(codecParams->codec_id);
      if (m_codec == nullptr) {
         spdlog::error("failed to found decoder");
         return false;
      }
      spdlog::info("found decoder");
      m_codecContext = avcodec_alloc_context3(m_codec);
      if (m_codecContext == nullptr) {
         return false;
      }
      if (m_codec->capabilities & AV_CODEC_CAP_TRUNCATED)
         m_codecContext->flags |= AV_CODEC_FLAG_TRUNCATED;  // we do not send complete frames

      m_codecContext->flags |= AV_CODEC_FLAG_LOW_DELAY;

      m_codecContext->width = codecParams->width;
      m_codecContext->height = codecParams->height;
      m_codecContext->time_base = time_base;
      m_codecContext->pix_fmt = (AVPixelFormat)codecParams->format;
      
      avcodec_parameters_to_context(m_codecContext, codecParams);

      if (avcodec_open2(m_codecContext, m_codec, nullptr) < 0) {
         spdlog::error("failed to open decoder");
         return false;
      }
      
      if (m_codecContext->codec_id == AV_CODEC_ID_H264) {
         m_codecContext->flags2 |= AV_CODEC_FLAG2_CHUNKS;
         // avCodecContext->flags2 |= CODEC_FLAG2_SHOW_ALL;
      }
      m_avframe = av_frame_alloc();
      if (m_avframe == nullptr) {
         return false;
      }
      m_avpacket = av_packet_alloc();
      if (!m_avpacket) {
         return false;
      }
      spdlog::info("decoder opened successfully");
      m_frameCallback = callback;
      m_needToStop = false;
      m_isReady = true;
      return m_isReady;
   }

   bool VideoDecoder::Decode(Packet::Ptr& pkt) const {
      if (!m_frameCallback)
         return false;
      if (!m_isReady)
         return false;

      int ret = 0;
      if (pkt) {
         av_init_packet(m_avpacket);
         m_avpacket->data = pkt->Data();
         m_avpacket->size = pkt->Size();
         m_avpacket->duration = pkt->Duration();
         m_avpacket->dts = m_avpacket->pts = pkt->PTS();
      }

      if (pkt) {
         ret = avcodec_send_packet(m_codecContext, m_avpacket);
      } else {
         ret = avcodec_send_packet(m_codecContext, nullptr);
      }
      if (ret == AVERROR(EAGAIN)) {
         while (avcodec_receive_frame(m_codecContext, m_avframe) >= 0) {
            handleDecodedFrame();
         }
         ret = avcodec_send_packet(m_codecContext, m_avpacket);
      } else if (ret < 0) {
         spdlog::error("media::VideoDecoder >> error while decoding");
         return false;
      }
      while (ret >= 0) {
         ret = avcodec_receive_frame(m_codecContext, m_avframe);
         if (ret == AVERROR_EOF) {
            return true;
         } else if (ret == AVERROR(EAGAIN)) {
            return true;
         } else if (ret < 0) {
            spdlog::error("media::VideoDecoder >> error while decoding");
            return false;
         }
         handleDecodedFrame();
      }

      return true;
   }

   void VideoDecoder::handleDecodedFrame() const {
      if (m_avframe->format == AV_PIX_FMT_YUVJ420P)
         m_avframe->format = AV_PIX_FMT_YUV420P;
      else if (m_avframe->format == AV_PIX_FMT_YUVJ422P)
         m_avframe->format = AV_PIX_FMT_YUV422P;
      else if (m_avframe->format == AV_PIX_FMT_YUVJ444P)
         m_avframe->format = AV_PIX_FMT_YUV444P;

      // Media::FramePtr coreFrame = Media::Frame::CreateFromAVFrame(decodedFrame);
      media::FramePtr coreFrame = media::Frame::Create(
            m_avframe->width, m_avframe->height,
            AVPixelFormat(m_avframe->format), true);
      uint8_t* dst[] = {coreFrame->Data(0), coreFrame->Data(1), coreFrame->Data(2), nullptr};

      int dstStride[] = {coreFrame->Stride(0), coreFrame->Stride(1), coreFrame->Stride(2), 0};

      av_image_copy(dst, dstStride, const_cast<const unsigned char**>(m_avframe->data),
                     m_avframe->linesize, AVPixelFormat(m_avframe->format),
                     m_avframe->width, m_avframe->height);

      coreFrame->PTS(m_avframe->pts);

      if (!m_needToStop && m_frameCallback) {
         m_frameCallback(coreFrame->ImageWidth(), coreFrame->ImageHeight(), coreFrame);
      }
   }

   void VideoDecoder::Flush() {
      Packet::Ptr empty;
      Decode(empty);
      if (m_codecContext != nullptr && m_isReady) {
         avcodec_flush_buffers(m_codecContext);
      }
   }

   void VideoDecoder::Close() {
      if (m_isReady) {
         m_needToStop = true;
         if (m_codecContext) {
            int ret = avcodec_close(m_codecContext);
            if (ret < 0 && ret != AVERROR_EOF) {
               char buf[1024];
               av_strerror(ret, buf, sizeof(buf));
               // std::cerr << "Error occurred: " << std::string(buf) << std::endl;
               spdlog::error("Error occurred in LibavDecoder: '{}'", buf);
            }
            av_free(m_codecContext);
            m_codecContext = nullptr;
         }
         m_isReady = false;
      }
   }
}}  // namespace challenge::media
