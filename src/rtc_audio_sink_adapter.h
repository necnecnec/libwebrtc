#ifndef LIB_WEBRTC_AUDIO_SINK_IMPL_HXX
#define LIB_WEBRTC_AUDIO_SINK_IMPL_HXX

#include "rtc_audio_track.h"

#include "api/media_stream_interface.h"
#include "api/peer_connection_interface.h"
#include "api/peer_connection_proxy.h"
#include "common_audio/resampler/include/push_resampler.h"
#include "common_audio/vad/include/webrtc_vad.h"
#include "media/engine/webrtc_video_engine.h"
#include "media/engine/webrtc_voice_engine.h"
#include "pc/media_session.h"
// #include "rtc_base/critical_section.h"
#include "rtc_base/logging.h"

#include "modules/audio_device/include/audio_device_data_observer.h"
#include "rtc_audio_track_impl.h"

namespace libwebrtc {

class AudioSinkAdapterImpl :public webrtc::AudioDeviceDataObserver {
 public:
  AudioSinkAdapterImpl(rtc::scoped_refptr<AudioTrackImpl> rtc_track);

  
 virtual void SetSink(
      rtc::scoped_refptr<AudioTrackImpl> rtc_track);


 protected:
// AudioDeviceDataObserver implementation.
   virtual void OnCaptureData(const void* audio_samples,
                             const size_t num_samples,
                             const size_t bytes_per_sample,
                             const size_t num_channels,
                             const uint32_t samples_per_sec) override;

  virtual void OnRenderData(const void* audio_samples,
                            const size_t num_samples,
                            const size_t bytes_per_sample,
                            const size_t num_channels,
                            const uint32_t samples_per_sec)override;

 private:
  rtc::scoped_refptr<AudioTrackImpl> rtc_track_;

};

} // namespace libwebrtc

#endif  // LIB_WEBRTC_AUDIO_SINK_IMPL_HXX
