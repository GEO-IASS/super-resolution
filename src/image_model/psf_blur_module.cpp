#include "image_model/psf_blur_module.h"

#include "image/image_data.h"

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "glog/logging.h"

namespace super_resolution {

PsfBlurModule::PsfBlurModule(const int blur_radius, const double sigma)
    : blur_radius_(blur_radius), sigma_(sigma) {
  CHECK_GE(blur_radius_, 1);
  CHECK_GT(sigma_, 0.0);
  CHECK(blur_radius_ % 2 == 1) << "Blur radius must be an odd number.";
}

void PsfBlurModule::ApplyToImage(
    const ImageData& image_data, const int index) const {

  const cv::Size kernel_size(blur_radius_, blur_radius_);
  int num_image_channels = image_data.GetNumChannels();
  for (int i = 0; i < num_image_channels; ++i) {
    cv::Mat channel_image = image_data.GetChannel(i);
    cv::GaussianBlur(channel_image, channel_image, kernel_size, sigma_);
  }
}

}  // namespace super_resolution