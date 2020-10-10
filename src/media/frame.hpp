#pragma once

#include <stdint.h>
#include <memory>
#include "common/random-string.hpp"
#include "ffmpeg.h"

namespace challenge { namespace media {
   class Frame {
      struct constructor_access {};

    public:
      static uint32_t GetDataSize(int w, int h, AVPixelFormat pixelFormat);
      static uint32_t GetPlaneSize(int w, int h, AVPixelFormat pixelFormat, uint32_t planeNumber);

      static std::shared_ptr<Frame> Create(int w, int h, AVPixelFormat pixelFormat, bool isPlanar);
      static std::shared_ptr<Frame> CreateFromAVFrame(AVFrame *avframe);

      Frame(constructor_access access, int w, int h, AVPixelFormat pixelFormat, bool isPlanar);
      Frame(constructor_access access, AVFrame *avframe);
      ~Frame();

      int ImageHeight() const;

      int ImageWidth() const;

      AVPixelFormat PixelFormat() const;

      bool IsKey() const {
         return m_isKeyFrame;
      }

      void MarkAsKeyFrame() {
         m_isKeyFrame = true;
      }

      uint8_t *Data(int plane = 0) const;

      int Stride(int plane) const;

      void Stride(int plane, int stride);

      int PlaneSize(int plane) const;

      int PlanesCount() const;

      int64_t PTS() const;

      void PTS(int64_t value) const;

      uint32_t DataSize() const {
         return m_dataSize;
      }

    private:

      Frame(const Frame &frame);
      Frame &operator=(const Frame &frame);

      AVFrame *m_avFrame;
      bool m_isKeyFrame;
      uint32_t m_dataSize;

   };

   typedef std::shared_ptr<Frame> FramePtr;

}}  // namespace challenge::media