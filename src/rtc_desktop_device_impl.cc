#include "rtc_desktop_device_impl.h" 
#include "src/internal/customizedvideosource.h"
#include "rtc_video_device_impl.h"
namespace libwebrtc {

	DesktopDeviceImpl::DesktopDeviceImpl():Capturer(nullptr)
	{
	}

	// DesktopCaptureOptions DesktopDeviceImpl::CreateOptions()
	// {
	// 	webrtc::DesktopCaptureOptions options = webrtc::DesktopCaptureOptions::CreateDefault();
	// 	options.set_allow_directx_capturer(false);
	// 	return options;
	// } 
    /*×ÀÃæÍ¼Ïñ*/
	scoped_refptr<RTCVideoCapturer> DesktopDeviceImpl::CreateWindowCapturer(std::shared_ptr<LocalDesktopStreamParameters> options,std::unique_ptr<LocalScreenStreamObserver> observer)
	{
        #if defined(WEBRTC_WIN)
        // std::shared_ptr<LocalDesktopStreamParameters> options_ptr(options); 
        webrtc::internal::CustomizedCapturer*  videocapture=webrtc::internal::CustomizedCapturer::Create(options,std::move(observer));
		//Capturer=dynamic_cast<webrtc::internal::BasicDesktopCapturer*>(videocapture);
	    scoped_refptr<RTCVideoCapturerImpl> video_capturer =
        scoped_refptr<RTCVideoCapturerImpl>(
          new RefCountedObject<RTCVideoCapturerImpl>(
              absl::WrapUnique(videocapture)));
         return video_capturer;
        #endif
        return nullptr;
	}

	  bool DesktopDeviceImpl::GetCurrentWindowList(
      std::unordered_map<int, std::string>* window_list) {
		  if(Capturer){
		 return Capturer->GetCurrentWindowList(window_list);
		  }else{
			  return false;
		  }

	  }
    bool DesktopDeviceImpl:: SetCaptureWindow(int window_id) {
           if(Capturer){
		 return Capturer->SetCaptureWindow(window_id);
		  }else{
			  return false;
		  }
	}

	/*Ó¦ÓÃÍ¼Ïñ*/
	// scoped_refptr<RTCVideoCapturer> DesktopDeviceImpl::CreateWindowCapturer(const webrtc::LocalDesktopStreamParameters& options)
	// {
	// 	RTCDesktopCapturer* desktopCapturer = new RTCDesktopCapturer(webrtc::DesktopCapturer::CreateWindowCapturer(options)); 
	// 	scoped_refptr<RTCVideoCapturerImpl> video_capturer = scoped_refptr<RTCVideoCapturerImpl>(new RefCountedObject<RTCVideoCapturerImpl>(
	// 		std::unique_ptr<cricket::VideoCapturer>(desktopCapturer)));
	// 	return video_capturer;

	// }
}