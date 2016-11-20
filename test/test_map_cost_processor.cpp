#include <memory>
#include <vector>

#include "image/image_data.h"
#include "solvers/map_cost_processor.h"
#include "solvers/regularizer.h"

#include "opencv2/core/core.hpp"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using testing::ContainerEq;
using testing::Each;
using testing::ElementsAre;
using testing::Return;
using testing::SizeIs;

class MockRegularizer : public super_resolution::Regularizer {
 public:
  // Handle super constructor, since we don't need the image_size_ field.
  MockRegularizer() : super_resolution::Regularizer(cv::Size(0, 0)) {}

  MOCK_CONST_METHOD1(
      ComputeResiduals, std::vector<double>(const double* image_data));
};

// Verifies that the correct data term residuals are returned for an image.
TEST(MapCostProcessor, ComputeDataTermResiduals) {
  const cv::Size image_size(3, 3);
  const cv::Mat lr_channel_1 = (cv::Mat_<double>(3, 3)
      << 0.5, 0.5, 0.5,
         0.5, 0.5, 0.5,
         0.5, 0.5, 0.5);
  const cv::Mat lr_channel_2 = (cv::Mat_<double>(3, 3)
      << 1.0,  0.5,  0.0,
         0.25, 0.5,  0.75,
         1.0,  0.0,  1.0);
  const cv::Mat lr_channel_3 = (cv::Mat_<double>(3, 3)
      << 1.0, 1.0, 1.0,
         1.0, 1.0, 1.0,
         1.0, 1.0, 1.0);

  super_resolution::ImageData lr_image_data_1;
  lr_image_data_1.AddChannel(lr_channel_1);
  lr_image_data_1.AddChannel(lr_channel_2);
  super_resolution::ImageData lr_image_data_2(lr_channel_3);
  const std::vector<super_resolution::ImageData> low_res_images = {
    lr_image_data_1,  // 2 channels
    lr_image_data_2   // 1 channel
  };

  super_resolution::ImageModel empty_image_model;
  std::unique_ptr<super_resolution::Regularizer> regularizer(
      new MockRegularizer());
  super_resolution::MapCostProcessor map_cost_processor(
      low_res_images,
      empty_image_model,
      image_size,
      std::move(regularizer),
      0.0);  // TODO: test with regularizer applied (lambda > 0).

  const double hr_pixel_values[9] = {
    0.5, 0.5, 0.5,
    0.5, 0.5, 0.5,
    0.5, 0.5, 0.5
  };

  // (Image 1, Channel 1) and hr pixels are identical, so expect all zeros.
  std::vector<double> residuals_channel_1 =
      map_cost_processor.ComputeDataTermResiduals(0, 0, hr_pixel_values);
  EXPECT_THAT(residuals_channel_1, SizeIs(9));
  EXPECT_THAT(residuals_channel_1, Each(0));

  // (Image 1, Channel 2) residuals should be different at each pixel.
  std::vector<double> residuals_channel_2 =
      map_cost_processor.ComputeDataTermResiduals(0, 1, hr_pixel_values);
  EXPECT_THAT(residuals_channel_2, ElementsAre(
      -0.5,  0.0,  0.5,
       0.25, 0.0, -0.25,
      -0.5,  0.5, -0.5));

  // (Image 2, channel 1) ("channel_3") should all be -0.5.
  std::vector<double> residuals_channel_3 =
      map_cost_processor.ComputeDataTermResiduals(1, 0, hr_pixel_values);
  EXPECT_THAT(residuals_channel_3, SizeIs(9));
  EXPECT_THAT(residuals_channel_3, Each(-0.5));

  // TODO: Mock the ImageModel and make sure the residuals are computed
  // correctly if the HR image is degraded first.
}

// Verifies that the correct regularization residuals are returned for an
// image.  This test does not cover regularization operators; instead, it tests
// the MapCostProcessor with a mock Regularizer.
TEST(MapCostProcessor, ComputeRegularizationResiduals) {
  std::unique_ptr<MockRegularizer> mock_regularizer(new MockRegularizer());
  const double image_data[5] = {1, 2, 3, 4, 5};
  const std::vector<double> residuals = {1, 2, 3, 4, 5};
  EXPECT_CALL(*mock_regularizer, ComputeResiduals(image_data))
      .WillOnce(Return(residuals));

  // TODO: this should weight the residuals appropriately. Test it once that is
  // implemented.
  std::vector<super_resolution::ImageData> empty_image_vector;
  super_resolution::ImageModel empty_image_model;
  super_resolution::MapCostProcessor map_cost_processor(
      empty_image_vector,
      empty_image_model,
      cv::Size(0, 0),
      std::move(mock_regularizer),
      0.0);  // TODO: test with this regularization parameter.
  
  const std::vector<double> expected_residuals = {1, 2, 3, 4, 5};
  const std::vector<double> returned_residuals =
      map_cost_processor.ComputeRegularizationResiduals(image_data);
  EXPECT_THAT(returned_residuals, ContainerEq(expected_residuals));
}
