#include "rtc_audio_mixer_impl.h"
#include "rtc_audio_renderer.h"
#include "audio/remix_resample.h"


using namespace webrtc;
namespace libwebrtc {

class AudioMixerSource : public AudioMixer::Source,public RTCAudioRenderer {
 public:
  explicit AudioMixerSource():crt_sec_(new rtc::RecursiveCriticalSection())
       {}

   void OnData(const void* audio_data,
        int bits_per_sample,
        int sample_rate,
        int number_of_channels,
        int number_of_frames) override{
            rtc::CritScope cs(crt_sec_.get());
            audioFrame_.UpdateFrame(0,(const int16_t*)audio_data,sample_rate/100,sample_rate,AudioFrame::kNormalSpeech, AudioFrame::kVadUnknown,number_of_channels);
            have_new_frame=true;
        }
  AudioFrameInfo GetAudioFrameWithInfo(int target_rate_hz,
                                       AudioFrame* frame) override {


   rtc::CritScope cs(crt_sec_.get());
   if(!have_new_frame) return AudioFrameInfo::kMuted;
    frame->sample_rate_hz_ = target_rate_hz;
         frame->samples_per_channel_ = audioFrame_.samples_per_channel();
     frame->num_channels_ =audioFrame_.num_channels();
  //   frame->sample_rate_hz_ = target_rate_hz;
   voe::RemixAndResample(audioFrame_,&capture_resampler_,frame);
    
    // frame->UpdateFrame(0,audioFrame_.mutable_data(),audioFrame_.samples_per_channel(),audioFrame_.sample_rate_hz_,audioFrame_.speech_type_,audioFrame_.vad_activity_,audioFrame_.num_channels_);
    // RTC_CHECK_EQ(target_rate_hz, audioFrame_.samples_per_channel());


    have_new_frame=false;
    return  AudioFrameInfo::kNormal;
  }

  int Ssrc() const override { return 0; }

  int PreferredSampleRate() const override { return audioFrame_.samples_per_channel(); }

  



 private:
  AudioFrame audioFrame_;
  bool have_new_frame = false;
 std::unique_ptr<rtc::RecursiveCriticalSection> crt_sec_;
  PushResampler<int16_t> capture_resampler_;
};

///////////////////////////////////////////////////////////////////////
// Definition of private class BasicScreenCaptureThread that periodically
// generates frames.
///////////////////////////////////////////////////////////////////////
class RTCAudioMixerImpl::MixCaptureThread
    : public rtc::Thread,
      public rtc::MessageHandler {
 public:
  explicit MixCaptureThread(RTCAudioMixerImpl* capturer)
      : rtc::Thread(rtc::SocketServer::CreateDefault()),
        rtc::MessageHandler(false),
        capturer_(capturer),
        finished_(false) {
    waiting_time_ms_ = 10;  // webrtc capture audio every 10ms
  }

  virtual ~MixCaptureThread() { Stop(); }
  // Override virtual method of parent Thread. Context: Worker Thread.
  virtual void Run() {
    // Read the first frame and start the message pump. The pump runs until
    // Stop() is called externally or Quit() is called by OnMessage().
    if (capturer_) {
      capturer_->CaptureMixFrame();
      rtc::Thread::Current()->Post(RTC_FROM_HERE, this);
      rtc::Thread::Current()->ProcessMessages(kForever);
    }
    rtc::CritScope cs(&crit_);
    finished_ = true;
  }
  // Override virtual method of parent MessageHandler. Context: Worker Thread.
  virtual void OnMessage(rtc::Message* /*pmsg*/) {
    if (capturer_) {
      capturer_->CaptureMixFrame();
      rtc::Thread::Current()->PostDelayed(RTC_FROM_HERE, waiting_time_ms_,
                                          this);
    } else {
      rtc::Thread::Current()->Quit();
    }
  }
  // Check if Run() is finished.
  bool Finished() const {
    rtc::CritScope cs(&crit_);
    return finished_;
  }
 private:
  RTCAudioMixerImpl* capturer_;
  mutable rtc::RecursiveCriticalSection crit_;
  bool finished_;
  int waiting_time_ms_;
  RTC_DISALLOW_COPY_AND_ASSIGN(MixCaptureThread);
};

RTCAudioMixerImpl::RTCAudioMixerImpl()
    {
       rtc_audio_mixer_= AudioMixerImpl::Create();
      audioFrame.mutable_data();
     mixCaptureThread_.reset(new MixCaptureThread(this));
  bool ret = mixCaptureThread_->Start();
  if (!ret) {
    RTC_LOG(LS_ERROR) << "Mix audio capture thread failed to start";
  }
  RTC_LOG(INFO) << __FUNCTION__ << ": ctor ";
}

RTCAudioMixerImpl::~RTCAudioMixerImpl() {
  if (mixCaptureThread_) {
    mixCaptureThread_->Quit();
    mixCaptureThread_.reset();
  }
  RTC_LOG(INFO) << __FUNCTION__ << ": dtor ";
}

void RTCAudioMixerImpl::AddSource(
      scoped_refptr<RTCAudioTrack> track){
          auto  audioMixerSource= new AudioMixerSource();
  track->AddRenderer(audioMixerSource);
  rtc_audio_mixer_->AddSource(audioMixerSource);
  track_map_[track]=audioMixerSource;
}

void RTCAudioMixerImpl::RemoveSource(
       scoped_refptr<RTCAudioTrack> track)  {
   track->RemoveRenderer(track_map_[track]);
   rtc_audio_mixer_->RemoveSource(track_map_[track]);
   
track_map_.erase(track);

}

char*  RTCAudioMixerImpl::Mix(
        int bits_per_sample,
        int sample_rate,
        int number_of_channels,
        int number_of_frame) {

            
 audioFrame.num_channels_=number_of_channels;
      audioFrame.sample_rate_hz_=sample_rate;
      audioFrame.samples_per_channel_=number_of_frame;
 // rtc_audio_mixer_->Mix(number_of_channels,&mixFrame);
  if(!have_new_frame ) return nullptr;
  voe::RemixAndResample(mixFrame,&mix_resampler_,&audioFrame);
  have_new_frame=false;
  return (char *)audioFrame.data();
}

void  RTCAudioMixerImpl::CaptureMixFrame(){
  
  rtc_audio_mixer_->Mix(2,&mixFrame);
have_new_frame=true;

}

} // namespace libwebrtc
