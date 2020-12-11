#ifndef LIB_WEBRTC_AUDIO_TRACK_IMPL_HXX
#define LIB_WEBRTC_AUDIO_TRACK_IMPL_HXX

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
namespace libwebrtc {

class AudioTrackImpl : public RTCAudioTrack,public webrtc::AudioDeviceDataObserver,public webrtc::AudioTrackSinkInterface {
 public:
  AudioTrackImpl(rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track);

  virtual ~AudioTrackImpl();

  virtual const char* kind() const override { return kind_; }

  virtual const char* id() const override { return id_; }

  virtual bool enabled() const override { return rtc_track_->enabled(); }

  virtual bool set_enabled(bool enable) override {
    return rtc_track_->set_enabled(enable);
  }

  
    virtual void AddRenderer(
      RTCAudioRenderer* renderer) override;

  virtual void RemoveRenderer(
        RTCAudioRenderer* renderer) override;
  rtc::scoped_refptr<webrtc::AudioTrackInterface> rtc_track() {
    return rtc_track_;
  }
  void set_rtc_track(rtc::scoped_refptr<webrtc::AudioTrackInterface> rtc_track) {
    
    rtc_track_=rtc_track;
      strncpy(id_, rtc_track_->id().c_str(), sizeof(id_));
  strncpy(kind_, rtc_track_->kind().c_str(), sizeof(kind_));
    rtc_track_->AddSink(this);
  }


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
   virtual void OnData(const void* audio_data,
                      int bits_per_sample,
                      int sample_rate,
                      size_t number_of_channels,
                      size_t number_of_frames,
                      absl::optional<int64_t> absolute_capture_timestamp_ms) override;
 private:
  rtc::scoped_refptr<webrtc::AudioTrackInterface> rtc_track_;
  char id_[kMaxStringLength], kind_[kShortStringLength];
  RTCAudioRenderer* _renderer;
};

} // namespace libwebrtc

#endif  // LIB_WEBRTC_AUDIO_TRACK_IMPL_HXX
