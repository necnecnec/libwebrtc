#ifndef LIB_WEBRTC_AUDIO_MIXER_IMPL_HXX
#define LIB_WEBRTC_AUDIO_MIXER_IMPL_HXX

#include "rtc_audio_mixer.h"

#include "modules/audio_mixer/audio_mixer_impl.h"

#include <string.h>

#include <limits>
#include <memory>
#include <string>
#include <utility>

#include "api/audio/audio_mixer.h"
#include "modules/audio_mixer/default_output_rate_calculator.h"
// #include "rtc_base/critical_section.h"
#include "rtc_base/logging.h"
#include "rtc_base/deprecated/recursive_critical_section.h"

#include "common_audio/resampler/include/push_resampler.h"

#include "rtc_base/bind.h"
#include "rtc_base/deprecated/recursive_critical_section.h"
#include "rtc_base/thread.h"

namespace libwebrtc {
class AudioMixerSource;
class RTCAudioMixerImpl : public RTCAudioMixer {
 public:
  RTCAudioMixerImpl();

  virtual ~RTCAudioMixerImpl();

  rtc::scoped_refptr<webrtc::AudioMixerImpl> rtc_audio_source() {
    return rtc_audio_mixer_;
  }
     virtual void AddSource(
      scoped_refptr<RTCAudioTrack> track) override;

  virtual void RemoveSource(
       scoped_refptr<RTCAudioTrack> renderer) override;
  virtual char* Mix(
        int bits_per_sample,
        int sample_rate,
        int number_of_channels,
        int number_of_frame) override;

  void  CaptureMixFrame();
 private:
 class MixCaptureThread;  // Forward declaration, defined in .cc.
  rtc::scoped_refptr<webrtc::AudioMixerImpl> rtc_audio_mixer_;
  std::map<scoped_refptr<RTCAudioTrack>,AudioMixerSource*> track_map_;
  webrtc::AudioFrame audioFrame;
  webrtc::AudioFrame mixFrame;
  webrtc::PushResampler<int16_t> mix_resampler_;
  std::unique_ptr<MixCaptureThread> mixCaptureThread_;
  bool have_new_frame = false;
};

} // namespace libwebrtc

#endif  // LIB_WEBRTC_AUDIO_SOURCE_IMPL_HXX