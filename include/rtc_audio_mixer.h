#ifndef LIB_WEBRTC_RTC_AUDIO_MIXER_HXX
#define LIB_WEBRTC_RTC_AUDIO_MIXER_HXX

#include "rtc_types.h"
#include "rtc_audio_track.h"
#include "rtc_audio_renderer.h"

namespace libwebrtc {

class RTCAudioMixer: public RefCountInterface {
  public:
    virtual void AddSource(
      scoped_refptr<RTCAudioTrack> track) = 0;

  virtual void RemoveSource(
       scoped_refptr<RTCAudioTrack> renderer) = 0;
  virtual char* Mix(
        int bits_per_sample,
        int sample_rate,
        int number_of_channels,
        int number_of_frame)=0;
 protected:
  virtual ~RTCAudioMixer() {}
};

} // namespace libwebrtc

#endif //LIB_WEBRTC_RTC_AUDIO_MIXER_HXX
