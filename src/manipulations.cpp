/**
 * @file manipulations.cpp
 * @brief Implementation of core image augmentation operations used in augmento.
 * @author Emmanuel Butsana
 * @date Initial release: May 27, 2025
 *
 * This file implements the image transformation and filtering functions
 * declared in manipulations.hpp. These include operations for cropping,
 * resizing, rotating, brightness and contrast adjustment, blurring, sharpening,
 * and more.
 */

#include "../include/manipulations.hpp"

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

/** resizeImage **/
cv::Mat resizeImage(const cv::Mat &im, int width, int height) {
  if (im.empty()) return cv::Mat();

  cv::Mat res;
  cv::resize(im, res, cv::Size(width, height));
  return res;
}

/** resizeImage **/
cv::Mat resizeImage(const cv::Mat &im, double scale) {
  if (im.empty()) return cv::Mat();

  cv::Mat res;
  cv::resize(im, res, cv::Size(), scale, scale, cv::INTER_LINEAR);
  return res;
}

/** cropImage **/
cv::Mat cropImage(const cv::Mat &im, int x, int y, int width, int height) {
  if (im.empty()) return cv::Mat();

  // Crop dimensions are larger than image dimensions
  if (width > im.cols || im.rows < height) {
    throw std::invalid_argument(
        "cropImage: crop size (" + std::to_string(width) + "x" +
        std::to_string(height) + ") exceeds input image dimensions (" +
        std::to_string(im.cols) + "x" + std::to_string(im.rows) + ").");
  }

  // Start point is not in image
  if (x < 0 || y < 0 || x > im.cols || y > im.rows) {
    throw std::invalid_argument("cropImage: start point of (" +
                                std::to_string(x) + "," + std::to_string(y) +
                                ") is outside the bounds of the image.");
  }

  // Crop area is bigger than normal area
  if (x + width >= im.cols || y + height >= im.rows) {
    throw std::invalid_argument(
        "cropImage: Attempting to crop area larger than image of (" +
        std::to_string(im.cols) + "," + std::to_string(im.rows) + ").");
  }

  cv::Rect roi(x, y, width, height);
  cv::Mat crop = im(roi).clone();
  return crop;
}

/** randomCrop **/
cv::Mat randomCrop(const cv::Mat &im, int width, int height) {
  if (im.empty()) return cv::Mat();

  // Crop size exceeds image size
  if (width > im.cols || height > im.rows) {
    throw std::invalid_argument(
        "randomCrop: crop size (" + std::to_string(width) + "x" +
        std::to_string(height) + ") exceeds the image dimensions (" +
        std::to_string(im.cols) + "x" + std::to_string(im.rows) + ").");
  }

  // Randomly set starting point (x,y)
  std::random_device rand;
  std::mt19937 gen(rand());
  std::uniform_int_distribution<int> xdist(0, im.rows);
  std::uniform_int_distribution<int> ydist(0, im.cols);
  int x = xdist(gen);
  int y = ydist(gen);

  cv::Rect roi(x, y, width, height);
  cv::Mat crop = im(roi).clone();
  return crop;
}

/** affineTransform **/
cv::Mat affineTransform(const cv::Mat &im, const cv::Mat &matrix) {
  if (im.empty()) return cv::Mat();
  if (matrix.empty()) return im;

  cv::Mat warp;
  cv::warpAffine(im, warp, matrix, im.size());
  return warp;
}

/** colorJitter **/
int colorJitter(cv::Mat &im, double brightness, double contrast,
                double saturation, int hue) {
  if (im.empty() || im.channels() != 3) return -1;

  std::random_device rand;
  std::mt19937 gen(rand());

  // Random distributions
  std::uniform_real_distribution<double> dist_b(-brightness, brightness);
  std::uniform_real_distribution<double> dist_c(1.0 - contrast, 1.0 + contrast);
  std::uniform_real_distribution<double> dist_s(1.0 - saturation,
                                                1.0 + saturation);
  std::uniform_int_distribution<int> dist_h(-hue, hue);

  // Apply brightness
  double bshift = dist_b(gen);
  im.convertTo(im, im.type(), 1.0, bshift);

  // Apply contrast
  double cscale = dist_c(gen);
  im.convertTo(im, im.type(), cscale, 0);

  // Convert to HSV for saturation and hue adjustment
  cv::Mat hsv;
  cv::cvtColor(im, hsv, cv::COLOR_BGR2HSV);
  std::vector<cv::Mat> hsv_channels;
  cv::split(hsv, hsv_channels);

  // Saturation adjustment
  double sscale = dist_s(gen);
  hsv_channels[1].convertTo(hsv_channels[1], hsv_channels[1].type(), sscale);
  cv::threshold(hsv_channels[1], hsv_channels[1], 255, 255, cv::THRESH_TRUNC);

  // Hue adjustment
  int hshift = dist_h(gen);
  for (int y = 0; y < hsv_channels[0].rows; ++y) {
    uchar *row = hsv_channels[0].ptr<uchar>(y);
    for (int x = 0; x < hsv_channels[0].cols; ++x) {
      row[x] = (row[x] + hshift + 180) % 180;
    }
  }

  // Merge and convert back to BGR
  cv::merge(hsv_channels, hsv);
  cv::cvtColor(hsv, im, cv::COLOR_HSV2BGR);

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

/* toGrayscale */
int toGrayscale(cv::Mat &im) {
  if (im.empty() || im.channels() != 3) return -1;

  cv::Mat gray;
  cv::cvtColor(im, gray, cv::COLOR_BGR2GRAY);
  im = gray;
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

/** randomErase **/
int randomErase(cv::Mat &im, int min_h, int max_h, int min_w, int max_w) {
  if (im.empty() || min_h > max_h || min_w > max_w) return -1;

  std::random_device rand;
  std::mt19937 gen(rand());
  std::uniform_int_distribution<int> h_dist(min_h, max_h);
  std::uniform_int_distribution<int> w_dist(min_w, max_w);

  int erase_h = h_dist(gen);
  int erase_w = w_dist(gen);

  if (erase_h > im.rows || erase_w > im.cols) return -1;

  std::uniform_int_distribution<int> y_dist(0, im.rows - erase_h);
  std::uniform_int_distribution<int> x_dist(0, im.cols - erase_w);

  int x = x_dist(gen);
  int y = y_dist(gen);

  cv::Rect erase_rect(x, y, erase_w, erase_h);
  im(erase_rect).setTo(cv::Scalar::all(0));
  return 0;
}
