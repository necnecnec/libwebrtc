#ifndef LIB_WEBRTC_RTC_AUDIO_RENDERER_HXX
#define LIB_WEBRTC_RTC_AUDIO_RENDERER_HXX

#include "rtc_types.h"

namespace libwebrtc {


class RTCAudioRenderer {
 public:
  virtual ~RTCAudioRenderer() {}

  virtual void OnData(const void* audio_data,
              int bits_per_sample,
              int sample_rate,
              int number_of_channels,
              int number_of_frames) = 0;
};

} // namespace libwebrtc

#endif //LIB_WEBRTC_RTC_AUDIO_RENDERER_HXX