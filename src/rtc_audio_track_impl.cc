#include "rtc_audio_track_impl.h"

namespace libwebrtc {

AudioTrackImpl::AudioTrackImpl(
    rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track)
    : rtc_track_(audio_track),_renderer(nullptr) {
  RTC_LOG(INFO) << __FUNCTION__ << ": ctor ";
  if(rtc_track_){
  strncpy(id_, rtc_track_->id().c_str(), sizeof(id_));
  strncpy(kind_, rtc_track_->kind().c_str(), sizeof(kind_));
  
  rtc_track_->AddSink(this);
  }
}

AudioTrackImpl::~AudioTrackImpl() {
   if(rtc_track_)
  rtc_track_->RemoveSink(this);
  RTC_LOG(INFO) << __FUNCTION__ << ": dtor ";
}

// AudioTrackSinkInterface implementation.
void AudioTrackImpl:: OnData(const void* audio_data,
                      int bits_per_sample,
                      int sample_rate,
                      size_t number_of_channels,
                      size_t number_of_frames,
                      absl::optional<int64_t> absolute_capture_timestamp_ms){
   if(_renderer){
                   _renderer->OnData( audio_data,
             static_cast<int>( bits_per_sample),
              static_cast<int>(sample_rate),
              static_cast<int>(number_of_channels),
              static_cast<int>(number_of_frames));
                }
                      }
  void AudioTrackImpl::OnRenderData(const void* audio_samples,
                             const size_t num_samples,
                             const size_t bytes_per_sample,
                             const size_t num_channels,
                             const uint32_t samples_per_sec){
            //     if(_renderer){
            //        _renderer->OnData( audio_samples,
            //  static_cast<int>( bytes_per_sample)*8,
            //   static_cast<int>(samples_per_sec),
            //   static_cast<int>(num_channels),
            //   static_cast<int>(num_samples));
            //     }

              }

    void AudioTrackImpl::OnCaptureData(const void* audio_samples,
                             const size_t nSamples,
                             const size_t nBytesPerSample,
                             const size_t nChannels,
                             const uint32_t samples_per_sec){
                if(_renderer){
                   _renderer->OnData( audio_samples,
             static_cast<int>( nSamples)*8,
              static_cast<int>(nBytesPerSample),
              static_cast<int>(nChannels),
              static_cast<int>(samples_per_sec));
                }

              }

void  AudioTrackImpl::AddRenderer(
      RTCAudioRenderer* renderer) {
        
  _renderer=renderer;
}

void AudioTrackImpl::RemoveRenderer(
        RTCAudioRenderer* renderer) {
   _renderer=nullptr;
}

} // namespace libwebrtc
