// Emmanuel Butsana, 27 May 2025, manipulations.cpp
// File containing core image manipulations implemented

#include "manipulations.hpp"

/** rotateImageNoCrop **/
cv::Mat rotateImageNoCrop(const cv::Mat &im, double deg) {
  if (im.empty()) return cv::Mat();

  cv::Point2f centre(im.cols / 2.0f, im.rows / 2.0f);
  cv::Mat rot = cv::getRotationMatrix2D(centre, deg, 1.0);
  cv::Rect2f bbox =
      cv::RotatedRect(cv::Point2f(), im.size(), deg).boundingRect2f();
  rot.at<double>(0, 2) += bbox.width / 2.0f - im.cols / 2.0f;
  rot.at<double>(1, 2) += bbox.height / 2.0f - im.rows / 2.0f;
  cv::Mat res;
  cv::warpAffine(im, res, rot, bbox.size());
  return res;
}

/** rotateImageCrop **/
cv::Mat rotateImageCrop(const cv::Mat &im, double deg) {
  if (im.empty()) return cv::Mat();

  auto rotatedRectWithMaxArea = [deg](double width,
                                      double height) -> std::pair<int, int> {
    // Compute relevant angles
    double rad = deg * (PI / 180.0);
    double sin_a = std::abs(std::sin(rad));
    double cos_a = std::abs(std::cos(rad));

    // Determine long and short side
    bool width_longer = width >= height;
    double long_side = width_longer ? width : height;
    double short_side = width_longer ? height : width;

    // Compute rotated bounding box with max area
    double wr, hr;
    if (short_side <= 2. * sin_a * cos_a * long_side ||
        std::abs(sin_a - cos_a) < 1e-10) {
      double x = 0.5 * short_side;
      if (width_longer) {
        wr = x / sin_a;
        hr = x / cos_a;
      } else {
        wr = x / cos_a;
        hr = x / sin_a;
      }
    } else {
      double cos_2a = cos_a * cos_a - sin_a * sin_a;
      wr = (width * cos_a - height * sin_a) / cos_2a;
      hr = (height * cos_a - width * sin_a) / cos_2a;
    }
    return {static_cast<int>(wr), static_cast<int>(hr)};
  };

  // Get rotation matrix and rotate image
  cv::Point2f centre(im.cols / 2.0f, im.rows / 2.0f);
  cv::Mat rot = cv::getRotationMatrix2D(centre, deg, 1.0);
  cv::Mat res;
  cv::warpAffine(im, res, rot, cv::Size(im.cols, im.rows));
  auto [crop_w, crop_h] = rotatedRectWithMaxArea(im.cols, im.rows);
  int x = (res.cols - crop_w) / 2;
  int y = (res.rows - crop_h) / 2;
  cv::Rect crop(x, y, crop_w, crop_h);

  return res(crop);
}

/** rotateImage **/
cv::Mat rotateImage(const cv::Mat &im, double deg) {
  if (im.empty()) return cv::Mat();

  cv::Point2f centre(im.cols / 2.0f, im.rows / 2.0f);
  cv::Mat rot = cv::getRotationMatrix2D(centre, deg, 1.0);
  cv::Mat res;
  cv::warpAffine(im, res, rot, cv::Size(im.cols, im.rows));
  return res;
}

/** reflectImageHorizontal **/
int reflectImageHorizontal(cv::Mat &im) {
  if (im.empty()) return -1;
  cv::flip(im, im, 1);
  return 0;
}

/** reflectImageVertical **/
int reflectImageVertical(cv::Mat &im) {
  if (im.empty()) return -1;
  cv::flip(im, im, 0);
  return 0;
}

/** histogramEqualization **/
int histogramEqualization(cv::Mat &im) {
  if (im.empty() || im.channels() != 3) return -1;

  // Convert, split, equalize, and merge
  cv::Mat ycrcb;
  cv::cvtColor(im, ycrcb, cv::COLOR_BGR2YCrCb);
  std::vector<cv::Mat> channels;
  cv::split(ycrcb, channels);
  cv::equalizeHist(channels[0], channels[0]);
  cv::merge(channels, ycrcb);
  cv::cvtColor(ycrcb, im, cv::COLOR_YCrCb2BGR);
  return 0;
}

/** whiteBalancing **/
int whiteBalance(cv::Mat &im) {
  if (im.empty() || im.channels() != 3) return -1;

  // Split channels, find averages
  std::vector<cv::Mat> channels;
  cv::split(im, channels);
  double mean_b = cv::mean(channels[0])[0];
  double mean_g = cv::mean(channels[1])[0];
  double mean_r = cv::mean(channels[2])[0];
  double gray = (mean_b + mean_g + mean_r) / 3.0;

  // Find and apply scale factors, merge
  channels[0] *= gray / mean_b;
  channels[1] *= gray / mean_g;
  channels[2] *= gray / mean_r;
  cv::merge(channels, im);
  return 0;
}

/** adjustBrightness **/
int adjustBrightness(cv::Mat &im, double val) {
  if (im.empty() || im.channels() != 3) return -1;
  im.convertTo(im, im.type(), 1.0, val);
  return 0;
}

/** adjustContrast **/
int adjustContrast(cv::Mat &im, double val) {
  if (im.empty() || im.channels() != 3) return -1;
  im.convertTo(im, im.type(), val, 0);
  return 0;
}

/** adjustSaturation **/
int adjustSaturation(cv::Mat &im, double val) {
  if (im.empty() || im.channels() != 3) return -1;

  // Convert, split channels, scale, clamp, merge
  cv::Mat hsv;
  cv::cvtColor(im, hsv, cv::COLOR_BGR2HSV);
  std::vector<cv::Mat> channels;
  cv::split(hsv, channels);
  channels[1].convertTo(channels[1], channels[1].type(), val);
  cv::threshold(channels[1], channels[1], 255, 255, cv::THRESH_TRUNC);
  cv::merge(channels, hsv);
  cv::cvtColor(hsv, im, cv::COLOR_HSV2BGR);
  return 0;
}

/** adjustHue **/
int adjustHue(cv::Mat &im, int val) {
  if (im.empty() || im.channels() != 3) return -1;

  // Convert to HSV, split channels
  cv::Mat hsv;
  cv::cvtColor(im, hsv, cv::COLOR_BGR2HSV);
  std::vector<cv::Mat> channels;
  cv::split(hsv, channels);

  // Add delta to hue (%180)
  for (int y = 0; y < channels[0].rows; ++y) {
    uchar *row = channels[0].ptr<uchar>(y);
    for (int x = 0; x < channels[0].cols; ++x) {
      row[x] = (row[x] + val + 180) % 180;
    }
  }

  // Merge and convete to BGR
  cv::merge(channels, hsv);
  cv::cvtColor(hsv, im, cv::COLOR_HSV2BGR);
  return 0;
}

/** injectNoise **/
int injectNoise(cv::Mat &im, double mean, double stdev) {
  if (im.empty() || im.channels() != 3) return -1;

  // Generate noise image
  cv::Mat noise(im.size(), im.type());
  cv::RNG rand(cv::getTickCount());
  rand.fill(noise, cv::RNG::NORMAL, mean, stdev);

  // Add and clamp to image
  cv::Mat temp;
  cv::add(im, noise, temp, cv::noArray(), im.type());
  cv::threshold(temp, temp, 255, 255, cv::THRESH_TRUNC);
  cv::threshold(temp, temp, 0, 0, cv::THRESH_TOZERO);
  temp.copyTo(im);
  return 0;
}

/** blurImage **/
int blurImage(cv::Mat &im, int ksize) {
  if (im.empty() || ksize <= 1) return -1;
  if (ksize % 2 != 0) ++ksize;

  cv::blur(im, im, cv::Size(ksize, ksize));
  return 0;
}

/** sharpenImage **/
int sharpenImage(cv::Mat &im) {
  if (im.empty()) return -1;

  const cv::Mat kernel =
      (cv::Mat_<float>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
  cv::Mat temp;
  cv::filter2D(im, temp, im.depth(), kernel);
  temp.copyTo(im);
  return 0;
}
