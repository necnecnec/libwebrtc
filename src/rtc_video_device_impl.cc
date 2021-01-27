#include "rtc_video_device_impl.h"

#include "modules/video_capture/video_capture_factory.h"

namespace libwebrtc {

RTCVideoDeviceImpl::RTCVideoDeviceImpl()
    : device_info_(webrtc::VideoCaptureFactory::CreateDeviceInfo()) {}

uint32_t RTCVideoDeviceImpl::NumberOfDevices() {
  if (!device_info_) {
    return 0;
  }
  return device_info_->NumberOfDevices();
}

int32_t RTCVideoDeviceImpl::GetDeviceName(
    uint32_t deviceNumber,
    char* deviceNameUTF8,
    uint32_t deviceNameLength,
    char* deviceUniqueIdUTF8,
    uint32_t deviceUniqueIdUTF8Length,
    char* productUniqueIdUTF8 /*= 0*/,
    uint32_t productUniqueIdUTF8Length /*= 0*/) {
  if (!device_info_) {
    return -1;
  }

  if (device_info_->GetDeviceName(deviceNumber, deviceNameUTF8,
                                  deviceNameLength, deviceUniqueIdUTF8,
                                  deviceUniqueIdUTF8Length) != -1) {
    return 0;
  }
  return 0;
}

scoped_refptr<RTCVideoCapturer> RTCVideoDeviceImpl::Create(const char* name,
                                                           uint32_t index,int32_t width,int32_t height,int32_t fps) {
  scoped_refptr<RTCVideoCapturerImpl> video_capturer =
      scoped_refptr<RTCVideoCapturerImpl>(
          new RefCountedObject<RTCVideoCapturerImpl>(absl::WrapUnique(
              webrtc::internal::VcmCapturer::Create(width, height, fps, index))));
  return video_capturer;
}

int32_t RTCVideoDeviceImpl::NumberOfCapabilities(
    const char* deviceUniqueIdUTF8) {
  if (!device_info_) {
    return -1;
  }

  return device_info_->NumberOfCapabilities(deviceUniqueIdUTF8);
}
int32_t RTCVideoDeviceImpl::GetCapability(const char* deviceUniqueIdUTF8,
                                          const uint32_t deviceCapabilityNumber,
                                          int32_t& width,
                                          int32_t& height,
                                          int32_t& fps) {
  if (!device_info_) {
    return -1;
  }
  webrtc::VideoCaptureCapability capability;
  device_info_->GetCapability(deviceUniqueIdUTF8, deviceCapabilityNumber,
                              capability);
  width = capability.width;
  height = capability.height;
  fps = capability.maxFPS;
  return 0;
}
}  // namespace libwebrtc
