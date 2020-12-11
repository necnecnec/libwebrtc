#ifndef LIB_WEBRTC_RTC_AUDIO_TRACK_HXX
#define LIB_WEBRTC_RTC_AUDIO_TRACK_HXX

#include "rtc_types.h"
#include "rtc_media_track.h"
#include "rtc_audio_renderer.h"

namespace libwebrtc {

class RTCAudioTrack : public RTCMediaTrack {
  public:
    virtual void AddRenderer(
      RTCAudioRenderer* renderer) = 0;

  virtual void RemoveRenderer(
        RTCAudioRenderer* renderer) = 0;
 protected:
  virtual ~RTCAudioTrack() {}
};

} // namespace libwebrtc

#endif //LIB_WEBRTC_RTC_AUDIO_TRACK_HXX
