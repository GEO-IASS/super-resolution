#include "image_model/downsampling_module.h"
#include "image_model/image_model.h"
#include "image_model/psf_blur_module.h"

#include "opencv2/core/core.hpp"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

const cv::Mat kSmallTestImage = (cv::Mat_<double>(4, 6)
    << 1, 2, 3, 4, 5, 6,
       7, 8, 9, 0, 1, 2,
       9, 7, 5, 4, 2, 1,
       2, 4, 6, 8, 0, 1);
const cv::Size kSmallTestImageSize = cv::Size(6, 4);  // 24 pixels total

// Returns true if the two given matrices contain identical values.
// Source:
//   http://stackoverflow.com/questions/9905093/how-to-check-whether-two-matrixes-are-identical-in-opencv  NOLINT
// TODO: this may need to do float comparisons.
bool AreMatricesEqual(const cv::Mat& mat1, const cv::Mat& mat2) {
  if (mat1.empty() && mat2.empty()) {
    return true;
  }
  if (mat1.cols != mat2.cols ||
      mat1.rows != mat2.rows ||
      mat1.dims != mat2.dims) {
    return false;
  }

  cv::Mat diff;
  cv::compare(mat1, mat2, diff, cv::CMP_NE);
  return cv::countNonZero(diff) == 0;
}

TEST(ImageModel, AdditiveNoiseModule) {
  // TODO: implement
}

TEST(ImageModel, DownsamplingModule) {
  super_resolution::DownsamplingModule downsampling_module(2);
  const cv::Mat downsampling_matrix =
      downsampling_module.GetOperatorMatrix(kSmallTestImageSize, 0);

  // 24 pixels in high-res input, 6 (= 24 / 2*2) pixels in downsampled output.
  const cv::Mat expected_matrix = (cv::Mat_<double>(6, 24) <<
      1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0);

  EXPECT_TRUE(AreMatricesEqual(downsampling_matrix, expected_matrix));

  // Vectorize the test image and compare to the expected outcome.
  const cv::Mat test_image = kSmallTestImage.reshape(1, 24);
  const cv::Mat expected_downsampled_image = (cv::Mat_<double>(6, 1)
      << 1, 3, 5,
         9, 5, 2);
  EXPECT_TRUE(AreMatricesEqual(
      downsampling_matrix * test_image, expected_downsampled_image));
}

TEST(ImageModel, MotionModule) {
  // TODO: implement
}

TEST(ImageModel, PsfBlurModule) {
  super_resolution::PsfBlurModule blur_module(3, 1);
  blur_module.GetOperatorMatrix(kSmallTestImageSize, 0);
  // TODO: implement
}

TEST(ImageModel, TestGetOperatorMatrix) {
  // TODO: implement
}
