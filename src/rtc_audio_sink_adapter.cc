#include "rtc_audio_sink_adapter.h"

namespace libwebrtc {

AudioSinkAdapterImpl:: AudioSinkAdapterImpl(
  rtc::scoped_refptr<AudioTrackImpl> rtc_track): rtc_track_(rtc_track) {
  RTC_LOG(INFO) << __FUNCTION__ << ": ctor ";
}




                      
void AudioSinkAdapterImpl::OnRenderData(const void* audio_samples,
                             const size_t num_samples,
                             const size_t bytes_per_sample,
                             const size_t num_channels,
                             const uint32_t samples_per_sec){
            //     if(rtc_track_){
            //        rtc_track_->OnRenderData( audio_samples,
            //  static_cast<int>( bytes_per_sample),
            //   static_cast<int>(samples_per_sec),
            //   static_cast<int>(num_channels),
            //   static_cast<int>(num_samples));
            //     }

              }

    void AudioSinkAdapterImpl::OnCaptureData(const void* audio_samples,
                             const size_t num_samples,
                             const size_t bytes_per_sample,
                             const size_t num_channels,
                             const uint32_t samples_per_sec){
                if(rtc_track_){
                   rtc_track_->OnCaptureData( audio_samples,
             static_cast<int>( bytes_per_sample),
              static_cast<int>(samples_per_sec),
              static_cast<int>(num_channels),
              static_cast<int>(num_samples));
                }

              }

void  AudioSinkAdapterImpl::SetSink(
      rtc::scoped_refptr<AudioTrackImpl> rtc_track){
        
  rtc_track_=rtc_track;
}



} // namespace libwebrtc
