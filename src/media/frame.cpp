#include "frame.hpp"
#include <string>
#include <memory>
#include "ffmpeg.h"
extern "C" {
#include <libavutil/imgutils.h>
}

namespace challenge { namespace media {
   uint32_t Frame::GetDataSize(int w, int h, AVPixelFormat pixelFormat) {
      return av_image_get_buffer_size(pixelFormat, w, h, 32);
   }

   uint32_t Frame::GetPlaneSize(int w, int h, AVPixelFormat pixelFormat, uint32_t planeNumber) {
      return av_image_get_linesize(pixelFormat, w, planeNumber) * h;
   }

   std::shared_ptr<Frame> Frame::Create(int w, int h, AVPixelFormat pixelFormat, bool isPlanar) {
      return std::make_shared<Frame>(constructor_access{}, w, h, pixelFormat, isPlanar);
   }

   std::shared_ptr<Frame> Frame::CreateFromAVFrame(AVFrame* avframe) {
      return std::make_shared<Frame>(constructor_access{}, avframe);
   }

   Frame::Frame(constructor_access, int w, int h, AVPixelFormat pixelFormat, bool isPlanar)
       : m_isKeyFrame(false) {
      m_avFrame = av_frame_alloc();
      m_avFrame->width = w;
      m_avFrame->height = h;
      m_avFrame->format = pixelFormat;
      m_dataSize = GetDataSize(w, h, pixelFormat);
      if (!isPlanar) {
         m_avFrame->data[0] = static_cast<uint8_t*>(av_malloc(m_dataSize));
         m_avFrame->linesize[0] = av_image_get_linesize(pixelFormat, w, 0);
         m_avFrame->data[1] = nullptr;
         m_avFrame->linesize[1] = 0;
      } else {
         av_image_alloc(m_avFrame->data, m_avFrame->linesize, w, h, pixelFormat, 32);
      }
   }

   Frame::Frame(constructor_access, AVFrame* avframe) : m_avFrame(avframe), m_isKeyFrame(false) {
      m_dataSize = GetDataSize(avframe->width, avframe->height, (AVPixelFormat)avframe->format);
   }

   Frame::~Frame() {
      if (m_avFrame != nullptr) {
         if (m_avFrame->data[0] != nullptr) {
            av_freep(&m_avFrame->data[0]);
            m_avFrame->data[0] = nullptr;
            m_avFrame->data[1] = nullptr;
            m_avFrame->data[2] = nullptr;
         }
         if (m_avFrame->data[1] != nullptr) {
            av_freep(&m_avFrame->data[1]);
            m_avFrame->data[1] = nullptr;
         }
         if (m_avFrame->data[2] != nullptr) {
            av_freep(&m_avFrame->data[2]);
            m_avFrame->data[2] = nullptr;
         }
         av_frame_free(&m_avFrame);
         m_avFrame = nullptr;
      }
   }

   int Frame::ImageHeight() const {
      if (m_avFrame != nullptr)
         return m_avFrame->height;
      return 0;
   }

   int Frame::ImageWidth() const {
      if (m_avFrame != nullptr)
         return m_avFrame->width;
      return 0;
   }

   AVPixelFormat Frame::PixelFormat() const {
      if (m_avFrame != nullptr)
         return (AVPixelFormat)m_avFrame->format;
      return AV_PIX_FMT_NONE;
   }

   uint8_t* Frame::Data(int plane) const {
      if (m_avFrame != nullptr && plane >= 0 && plane <= 3)
         return m_avFrame->data[plane];
      return nullptr;
   }

   int Frame::Stride(int plane) const {
      if (m_avFrame != nullptr && plane >= 0 && plane <= 3)
         return m_avFrame->linesize[plane];
      return 0;
   }

   void Frame::Stride(int plane, int stride) {
      if (m_avFrame != nullptr && plane >= 0 && plane <= 3)
         m_avFrame->linesize[plane] = stride;
   }

   int Frame::PlaneSize(int plane) const {
      return GetPlaneSize(m_avFrame->width, m_avFrame->height, (AVPixelFormat)m_avFrame->format,
                          plane);
   }

   int Frame::PlanesCount() const {
      int count = 0;
      if (m_avFrame == nullptr)
         return 0;
      if (m_avFrame->data[0] == nullptr)
         return count;
      count++;
      if (m_avFrame->data[1] == nullptr)
         return count;
      count++;
      if (m_avFrame->data[2] == nullptr)
         return count;
      count++;
      if (m_avFrame->data[3] == nullptr)
         return count;
      return count + 1;
   }

   int64_t Frame::PTS() const {
      if (m_avFrame == nullptr) {
         return 0;
      }
      return m_avFrame->pts;
   }

   void Frame::PTS(int64_t value) const {
      if (m_avFrame != nullptr) {
         m_avFrame->pts = value;
      }
   }

}}  // namespace challenge::media
