/**
 * @file operation.cpp
 * @brief Implementation of concrete augmentation operations for augmento
 * @author Emmanuel Butsana
 * @date Initial release: July 20, 2025
 *
 * Implements geometric, color, noise, and filtering transformations for use in
 * a data augmentation pipeline.
 */

#include "../include/operation.hpp"

/** ---------------- RotateImage ---------------- **/
RotateImage::RotateImage(double min_angle, double max_angle, size_t rot_type)
    : min_angle_(min_angle), max_angle_(max_angle), rot_type_(rot_type) {
  if (min_angle_ > max_angle_) {
    std::ostringstream oss;
    oss << "RotateImage: min angle (" << min_angle_
        << ") cannot be greater than max angle (" << max_angle_ << ")";
    throw std::invalid_argument(oss.str());
  }
}

void RotateImage::apply(Image& img, std::mt19937& rng) const {
  std::uniform_real_distribution<double> angleDist(min_angle_, max_angle_);
  double angle = angleDist(rng);

  if (rot_type_ == 0) {
    img.setData(rotateImageNoCrop(img.getData(), angle));
    img.logOperation("RotateImage (no crop): " + std::to_string(angle));
  } else if (rot_type_ == 1) {
    img.setData(rotateImageCrop(img.getData(), angle));
    img.logOperation("RotateImage (crop): " + std::to_string(angle));
  } else if (rot_type_ == 2) {
    img.setData(rotateImage(img.getData(), angle));
    img.logOperation("RotateImage (fill-in): " + std::to_string(angle));
  } else {
    std::ostringstream oss;
    oss << "RotateImage: Invalid rotation type (" << rot_type_ << ")";
    throw std::invalid_argument(oss.str());
  }
}

std::string RotateImage::name() const {
  return "RotateImage: Rotates image with crop, no crop, or fill-in";
}

/** ---------------- ReflectImage ---------------- **/
ReflectImage::ReflectImage() = default;

void ReflectImage::apply(Image& img, std::mt19937& rng) const {
  std::uniform_int_distribution<int> axisDist(0, 1);
  int axis = axisDist(rng);

  if (axis == 0) {
    if (reflectImageVertical(img.getData()) == -1) {
      std::ostringstream oss;
      oss << "ReflectImage: could not perform vertical flip on image '"
          << img.getName() << "'";
      throw std::runtime_error(oss.str());
    }
    img.logOperation("ReflectImage: Vertical");
  } else {
    if (reflectImageHorizontal(img.getData()) == -1) {
      std::ostringstream oss;
      oss << "ReflectImage: could not perform horizontal flip on image '"
          << img.getName() << "'";
      throw std::runtime_error(oss.str());
    }
    img.logOperation("ReflectImage: Horizontal");
  }
}

std::string ReflectImage::name() const {
  return "ReflectImage: Reflects image along horizontal or vertical axis";
}

/** ---------------- ResizeImage ---------------- **/
ResizeImage::ResizeImage(double min_scale, double max_scale)
    : min_scale_(min_scale),
      max_scale_(max_scale),
      min_w_(-1),
      max_w_(-1),
      min_h_(-1),
      max_h_(-1) {
  if (min_scale_ > max_scale_) {
    std::ostringstream oss;
    oss << "ResizeImage: min scale (" << min_scale_
        << ") cannot be greater than max scale (" << max_scale_ << ")";
    throw std::invalid_argument(oss.str());
  }
}

ResizeImage::ResizeImage(int min_w, int max_w, int min_h, int max_h)
    : min_w_(min_w),
      max_w_(max_w),
      min_h_(min_h),
      max_h_(max_h),
      min_scale_(-1),
      max_scale_(-1) {
  if (min_w_ > max_w_) {
    std::ostringstream oss;
    oss << "ResizeImage: min width (" << min_w_
        << ") cannot be greater than max width (" << max_w_ << ")";
    throw std::invalid_argument(oss.str());
  }
  if (min_h_ > max_h_) {
    std::ostringstream oss;
    oss << "ResizeImage: min height (" << min_h_
        << ") cannot be greater than max height (" << max_h_ << ")";
    throw std::invalid_argument(oss.str());
  }
}

void ResizeImage::apply(Image& img, std::mt19937& rng) const {
  if (min_w_ == -1) {
    std::uniform_real_distribution<double> scaleDist(min_scale_, max_scale_);
    double scale = scaleDist(rng);
    img.setData(resizeImage(img.getData(), scale));
    img.logOperation("ResizeImage (scale): " + std::to_string(scale));
  } else if (min_scale_ == -1) {
    std::uniform_int_distribution<int> wDist(min_w_, max_w_);
    std::uniform_int_distribution<int> hDist(min_h_, max_h_);
    int w = wDist(rng);
    int h = hDist(rng);
    img.setData(resizeImage(img.getData(), w, h));
    img.logOperation("ResizeImage (absolute): " + std::to_string(w) + "x" +
                     std::to_string(h));
  } else {
    throw std::logic_error(
        "ResizeImage: Invalid configuration, neither scale nor absolute "
        "dimensions are provided.");
  }
}

std::string ResizeImage::name() const {
  return "ResizeImage: Resizes input image by scale or absolute dimensions";
}

/** ---------------- CropImage ---------------- **/
CropImage::CropImage(int width, int height)
    : w_(width), h_(height), x_(-1), y_(-1) {
  if (width < 0 || height < 0) {
    std::ostringstream oss;
    oss << "CropImage: cannot crop by negative dimensions: width=" << width
        << ", height=" << height;
    throw std::invalid_argument(oss.str());
  }
}

CropImage::CropImage(int x, int y, int width, int height)
    : x_(x), y_(y), w_(width), h_(height) {
  if (width < 0 || height < 0 || x < 0 || y < 0) {
    std::ostringstream oss;
    oss << "CropImage: all parameters must be non-negative. Received x=" << x
        << ", y=" << y << ", width=" << width << ", height=" << height;
    throw std::invalid_argument(oss.str());
  }
}

void CropImage::apply(Image& img, std::mt19937& rng) const {
  if (x_ == -1) {
    img.setData(randomCrop(img.getData(), w_, h_));
    img.logOperation("CropImage (random): " + std::to_string(w_) + "x" +
                     std::to_string(h_));
  } else {
    img.setData(cropImage(img.getData(), x_, y_, w_, h_));
    img.logOperation("CropImage (fixed): (" + std::to_string(x_) + "," +
                     std::to_string(y_) + ") " + std::to_string(w_) + "x" +
                     std::to_string(h_));
  }
}

std::string CropImage::name() const {
  return "CropImage: Crops image either randomly or deterministically";
}

/** ---------------- AffineTransform ---------------- **/
AffineTransform::AffineTransform(std::mt19937& rng) {
  std::uniform_real_distribution<double> dist(-2, 2);
  cv::Mat matrix(2, 3, CV_64F);
  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 3; ++j) matrix.at<double>(i, j) = dist(rng);
  matrix_ = matrix.clone();
}

AffineTransform::AffineTransform(const cv::Mat& matrix) {
  // Validate matrix size and type
  if (matrix.rows != 2 || matrix.cols != 3) {
    std::ostringstream oss;
    oss << "AffineTransform: matrix must be of size 2x3, but received "
        << matrix.rows << "x" << matrix.cols;
    throw std::invalid_argument(oss.str());
  }
  if (matrix.type() != CV_64F) {
    std::ostringstream oss;
    oss << "AffineTransform: matrix must have type CV_64F (double precision), "
           "but received type "
        << matrix.type();
    throw std::invalid_argument(oss.str());
  }
  matrix_ = matrix.clone();
}

void AffineTransform::apply(Image& img, std::mt19937& /*rng*/) const {
  img.setData(affineTransform(img.getData(), matrix_));
  img.logOperation("AffineTransform");
}

std::string AffineTransform::name() const {
  return "AffineTransform: Applies affine transform to image";
}

/** ---------------- ColorJitter ---------------- **/
ColorJitter::ColorJitter(double brightness_range, double contrast_range,
                         double saturation_range, int hue_range)
    : brightness_range_(brightness_range),
      contrast_range_(contrast_range),
      saturation_range_(saturation_range),
      hue_range_(hue_range) {
  if (brightness_range < 0 || contrast_range < 0 || saturation_range < 0 ||
      hue_range < 0) {
    std::ostringstream oss;
    oss << "ColorJitter: all arguments must be non-negative. "
        << "Received brightness_range=" << brightness_range
        << ", contrast_range=" << contrast_range
        << ", saturation_range=" << saturation_range
        << ", hue_range=" << hue_range;
    throw std::invalid_argument(oss.str());
  }
}

void ColorJitter::apply(Image& img, std::mt19937& /*rng*/) const {
  colorJitter(img.getData(), brightness_range_, contrast_range_,
              saturation_range_, hue_range_);
  img.logOperation("ColorJitter: " + std::to_string(brightness_range_) + " " +
                   std::to_string(contrast_range_) + " " +
                   std::to_string(saturation_range_) + " " +
                   std::to_string(hue_range_));
}

std::string ColorJitter::name() const {
  return "ColorJitter: Applies brightness/contrast/saturation/hue jitter";
}

/** ---------------- HistogramEqualization ---------------- **/
HistogramEqualization::HistogramEqualization() = default;

void HistogramEqualization::apply(Image& img, std::mt19937& /*rng*/) const {
  histogramEqualization(img.getData());
  img.logOperation("HistogramEqualization");
}

std::string HistogramEqualization::name() const {
  return "HistogramEqualization: Applies histogram equalization";
}

/** ---------------- WhiteBalance ---------------- **/
WhiteBalance::WhiteBalance() = default;

void WhiteBalance::apply(Image& img, std::mt19937& /*rng*/) const {
  whiteBalance(img.getData());
  img.logOperation("WhiteBalance");
}

std::string WhiteBalance::name() const {
  return "WhiteBalance: Applies white balance correction";
}

/** ---------------- ToGrayscale ---------------- **/
ToGrayscale::ToGrayscale() = default;

void ToGrayscale::apply(Image& img, std::mt19937& /*rng*/) const {
  toGrayscale(img.getData());
  img.logOperation("ToGrayscale");
}

std::string ToGrayscale::name() const {
  return "ToGrayscale: Converts image to grayscale";
}

/** ---------------- AdjustBrightness ---------------- **/
AdjustBrightness::AdjustBrightness(double min_val, double max_val)
    : min_val_(min_val), max_val_(max_val) {
  if (min_val_ > max_val_) {
    std::ostringstream oss;
    oss << "AdjustBrightness: Minimum value (" << min_val_
        << ") cannot be greater than maximum value (" << max_val_ << ")";
    throw std::invalid_argument(oss.str());
  }
}

void AdjustBrightness::apply(Image& img, std::mt19937& rng) const {
  std::uniform_real_distribution<double> bDist(min_val_, max_val_);
  double brightness = bDist(rng);
  adjustBrightness(img.getData(), brightness);
  img.logOperation("AdjustBrightness: " + std::to_string(brightness));
}

std::string AdjustBrightness::name() const {
  return "AdjustBrightness: Randomly adjusts brightness";
}

/** ---------------- AdjustContrast ---------------- **/
AdjustContrast::AdjustContrast(double min_val, double max_val)
    : min_val_(min_val), max_val_(max_val) {
  if (min_val_ > max_val_) {
    std::ostringstream oss;
    oss << "AdjustContrast: Minimum value (" << min_val_
        << ") cannot be greater than maximum value (" << max_val_ << ")";
    throw std::invalid_argument(oss.str());
  }
}

void AdjustContrast::apply(Image& img, std::mt19937& rng) const {
  std::uniform_real_distribution<double> cDist(min_val_, max_val_);
  double contrast = cDist(rng);
  adjustContrast(img.getData(), contrast);
  img.logOperation("AdjustContrast: " + std::to_string(contrast));
}

std::string AdjustContrast::name() const {
  return "AdjustContrast: Randomly adjusts contrast";
}

/** ---------------- AdjustSaturation ---------------- **/
AdjustSaturation::AdjustSaturation(double min_val, double max_val)
    : min_val_(min_val), max_val_(max_val) {
  if (min_val_ > max_val_) {
    std::ostringstream oss;
    oss << "AdjustSaturation: Minimum value (" << min_val_
        << ") cannot be greater than maximum value (" << max_val_ << ")";
    throw std::invalid_argument(oss.str());
  }
}

void AdjustSaturation::apply(Image& img, std::mt19937& rng) const {
  std::uniform_real_distribution<double> sDist(min_val_, max_val_);
  double saturation = sDist(rng);
  adjustSaturation(img.getData(), saturation);
  img.logOperation("AdjustSaturation: " + std::to_string(saturation));
}

std::string AdjustSaturation::name() const {
  return "AdjustSaturation: Randomly adjusts saturation";
}

/** ---------------- AdjustHue ---------------- **/
AdjustHue::AdjustHue(int min_val, int max_val)
    : min_val_(min_val), max_val_(max_val) {
  if (min_val_ > max_val_) {
    std::ostringstream oss;
    oss << "AdjustHue: Minimum value (" << min_val_
        << ") cannot be greater than maximum value (" << max_val_ << ")";
    throw std::invalid_argument(oss.str());
  }
}

void AdjustHue::apply(Image& img, std::mt19937& rng) const {
  std::uniform_int_distribution<int> hDist(min_val_, max_val_);
  int hue = hDist(rng);
  adjustHue(img.getData(), hue);
  img.logOperation("AdjustHue: " + std::to_string(hue));
}

std::string AdjustHue::name() const {
  return "AdjustHuue: Randomly adjusts image hue";
}

/** ---------------- InjectNoise ---------------- **/
InjectNoise::InjectNoise()
    : mean_min_(-10.0), mean_max_(10.0), stdev_min_(0.0), stdev_max_(20.0) {}

InjectNoise::InjectNoise(double mean_min, double mean_max, double stdev_min,
                         double stdev_max)
    : mean_min_(mean_min),
      mean_max_(mean_max),
      stdev_min_(stdev_min),
      stdev_max_(stdev_max) {}

void InjectNoise::apply(Image& img, std::mt19937& rng) const {
  std::uniform_real_distribution<double> mDist(mean_min_, mean_max_);
  std::uniform_real_distribution<double> sDist(stdev_min_, stdev_max_);
  double mean = mDist(rng);
  double stdev = sDist(rng);
  injectNoise(img.getData(), mean, stdev);
  img.logOperation("InjectNoise: μ=" + std::to_string(mean) +
                   ", σ=" + std::to_string(stdev));
}

std::string InjectNoise::name() const {
  return "InjectNoise: Adds Gaussian noise to image";
}

/** ---------------- BlurImage ---------------- **/
BlurImage::BlurImage() : min_k_(3), max_k_(9) {}
BlurImage::BlurImage(int min_k, int max_k) : min_k_(min_k), max_k_(max_k) {}

void BlurImage::apply(Image& img, std::mt19937& rng) const {
  std::uniform_int_distribution<int> kDist(min_k_, max_k_);
  int k = kDist(rng);
  if (k % 2 == 0) k += 1;  // Ensure odd kernel size
  blurImage(img.getData(), k);
  img.logOperation("BlurImage: k=" + std::to_string(k));
}

std::string BlurImage::name() const {
  return "BlurImage: Applies blur using square averaging kernel";
}

/** ---------------- SharpenImage ---------------- **/
SharpenImage::SharpenImage() = default;

void SharpenImage::apply(Image& img, std::mt19937& /*rng*/) const {
  sharpenImage(img.getData());
  img.logOperation("SharpenImage");
}

std::string SharpenImage::name() const {
  return "SharpenImage: Sharpens image using Laplacian enhancement";
}

/** ---------------- RandomErase ---------------- **/
RandomErase::RandomErase() : min_h_(1), max_h_(10), min_w_(1), max_w_(10) {}

RandomErase::RandomErase(int min_h, int max_h, int min_w, int max_w)
    : min_h_(min_h), max_h_(max_h), min_w_(min_w), max_w_(max_w) {
  if (min_h_ > max_h_) {
    std::ostringstream oss;
    oss << "RandomErase: min height (" << min_h_
        << ") cannot be greater than max height (" << max_h_ << ")";
    throw std::invalid_argument(oss.str());
  }
  if (min_w_ > max_w_) {
    std::ostringstream oss;
    oss << "RandomErase: min width (" << min_w_
        << ") cannot be greater than max width (" << max_w_ << ")";
    throw std::invalid_argument(oss.str());
  }
  if (max_w_ < 0 || max_h_ < 0) {
    std::ostringstream oss;
    oss << "RandomErase: width and height parameters cannot be negative. "
           "Received max_w="
        << max_w_ << ", max_h=" << max_h_;
    throw std::invalid_argument(oss.str());
  }
}

void RandomErase::apply(Image& img, std::mt19937& /*rng*/) const {
  randomErase(img.getData(), min_h_, max_h_, min_w_, max_w_);
  img.logOperation("RandomErase: h=[" + std::to_string(min_h_) + "," +
                   std::to_string(max_h_) + "], w=[" + std::to_string(min_w_) +
                   "," + std::to_string(max_w_) + "]");
}

std::string RandomErase::name() const {
  return "RandomErase: Randomly erases rectangular region within image";
}
