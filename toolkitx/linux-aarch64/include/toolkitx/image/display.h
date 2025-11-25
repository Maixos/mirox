#pragma once

#include <opencv2/opencv.hpp>

namespace toolkitx::image {
    cv::Mat concat_images(const cv::Mat &img1, const cv::Mat &img2);
}
