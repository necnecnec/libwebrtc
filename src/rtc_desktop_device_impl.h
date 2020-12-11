
#ifndef LIB_WEBRTC_DESKTOP_DEVICE_IMPL_HXX
#define LIB_WEBRTC_DESKTOP_DEVICE_IMPL_HXX
#include "rtc_desktop_device.h"  
#include "src/internal/desktopcapturer.h"  
#include "modules/desktop_capture/desktop_capturer.h"
#include "modules/desktop_capture/desktop_frame.h"
#include "modules/desktop_capture/desktop_capture_options.h"

#include <memory>
//Íâ²¿µ÷ÓÃ
	//std::unique_ptr<cricket::VideoCapturer> video_device = std::unique_ptr<cricket::VideoCapturer>(new DesktopCapturerDeviceImpl());

namespace libwebrtc
{
	class DesktopDeviceImpl :public RTCDesktopDevice {
	public:
		DesktopDeviceImpl();

		// scoped_refptr<RTCVideoCapturer> CreateScreenCapturer(const LocalDesktopStreamParameters& options)  override;

		scoped_refptr<RTCVideoCapturer> CreateWindowCapturer(std::shared_ptr<LocalDesktopStreamParameters> options,std::unique_ptr<LocalScreenStreamObserver> observer)  override;
		  bool GetCurrentWindowList(
      std::unordered_map<int, std::string>* window_list) override;
    bool SetCaptureWindow(int window_id) override;
		//DesktopCaptureOptions CreateOptions() override;
	private:
	   webrtc::internal::BasicDesktopCapturer* Capturer;
	};
}

#endif 