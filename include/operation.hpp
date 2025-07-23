/**
 * @file operation.hpp
 * @brief Abstract base class and concrete augmentation operations for augmento.
 * @author Emmanuel Butsana
 * @date Initial release: July 18, 2025
 *
 * Defines the Operation interface and a registry of geometric, color, noise,
 * and filtering transformations for use in a data augmentation pipeline.
 */

#pragma once

#include <memory>
#include <opencv2/core.hpp>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>

#include "image.hpp"
#include "manipulations.hpp"

/**
 * @class Operation
 * @brief Abstract base class for all image augmentation operations.
 */
class Operation {
 public:
  /// Virtual destructor.
  virtual ~Operation() = default;

  /**
   * @brief Apply the operation to the given image using provided RNG.
   * @param img Image object to modify.
   * @param rng Random number generator.
   */
  virtual void apply(Image& img, std::mt19937& rng) const = 0;

  /**
   * @brief Get human-readable name of the operation.
   * @return Name of the operation.
   */
  virtual std::string name() const = 0;
};

/// @brief Type alias for a shared pointer to an Operation.
using OperationPtr = std::shared_ptr<Operation>;

// ==========================================================
// Geometric Operations
// ==========================================================

/**
 * @class RotateImage
 * @brief Applies a random rotation to an image (with optional cropping
 * behavior).
 */
class RotateImage : public Operation {
 public:
  RotateImage(double min_angle, double max_angle, size_t rot_type);
  void apply(Image& img, std::mt19937& rng) const override;
  std::string name() const override;

 private:
  double min_angle_, max_angle_;
  size_t rot_type_;  ///< 0: no crop, 1: crop, 2: clip
};

/**
 * @class ReflectImage
 * @brief Randomly reflects the image horizontally or vertically.
 */
class ReflectImage : public Operation {
 public:
  ReflectImage();
  void apply(Image& img, std::mt19937& rng) const override;
  std::string name() const override;
};

/**
 * @class ResizeImage
 * @brief Resizes an image based on scale or absolute dimensions.
 */
class ResizeImage : public Operation {
 public:
  ResizeImage(double min_scale, double max_scale);
  ResizeImage(int min_w, int max_w, int min_h, int max_h);
  void apply(Image& img, std::mt19937& rng) const override;
  std::string name() const override;

 private:
  bool use_scale_;
  double min_scale_, max_scale_;
  int min_w_, max_w_, min_h_, max_h_;
};

/**
 * @class CropImage
 * @brief Crops an image using random or fixed parameters.
 */
class CropImage : public Operation {
 public:
  CropImage(int width, int height);                ///< Random crop
  CropImage(int x, int y, int width, int height);  ///< Fixed crop
  void apply(Image& img, std::mt19937& rng) const override;
  std::string name() const override;

 private:
  int x_, y_, w_, h_;
};

/**
 * @class AffineTransform
 * @brief Applies a fixed affine transformation to an image.
 */
class AffineTransform : public Operation {
 public:
  AffineTransform(std::mt19937& rng);
  AffineTransform(const cv::Mat& matrix);
  void apply(Image& img, std::mt19937& rng) const override;
  std::string name() const override;

 private:
  cv::Mat matrix_;
};

// ==========================================================
// Color & Intensity Operations
// ==========================================================

/**
 * @class ColorJitter
 * @brief Randomly alters brightness, contrast, saturation, and hue.
 */
class ColorJitter : public Operation {
 public:
  ColorJitter(double brightness_range, double contrast_range,
              double saturation_range, int hue_range);
  void apply(Image& img, std::mt19937& rng) const override;
  std::string name() const override;

 private:
  double brightness_range_, contrast_range_, saturation_range_;
  int hue_range_;
};

/**
 * @class HistogramEqualization
 * @brief Applies histogram equalization to enhance contrast.
 */
class HistogramEqualization : public Operation {
 public:
  HistogramEqualization();
  void apply(Image& img, std::mt19937& rng) const override;
  std::string name() const override;
};

/**
 * @class WhiteBalance
 * @brief Adjusts image colors using gray-world white balance.
 */
class WhiteBalance : public Operation {
 public:
  WhiteBalance();
  void apply(Image& img, std::mt19937& rng) const override;
  std::string name() const override;
};

/**
 * @class ToGrayscale
 * @brief Converts an image to grayscale.
 */
class ToGrayscale : public Operation {
 public:
  ToGrayscale();
  void apply(Image& img, std::mt19937& rng) const override;
  std::string name() const override;
};

/**
 * @class AdjustBrightness
 * @brief Randomly adjusts image brightness.
 */
class AdjustBrightness : public Operation {
 public:
  AdjustBrightness(double min_val, double max_val);
  void apply(Image& img, std::mt19937& rng) const override;
  std::string name() const override;

 private:
  double min_val_, max_val_;
};

/**
 * @class AdjustContrast
 * @brief Randomly adjusts image contrast.
 */
class AdjustContrast : public Operation {
 public:
  AdjustContrast(double min_val, double max_val);
  void apply(Image& img, std::mt19937& rng) const override;
  std::string name() const override;

 private:
  double min_val_, max_val_;
};

/**
 * @class AdjustSaturation
 * @brief Randomly adjusts image saturation in HSV color space.
 */
class AdjustSaturation : public Operation {
 public:
  AdjustSaturation(double min_val, double max_val);
  void apply(Image& img, std::mt19937& rng) const override;
  std::string name() const override;

 private:
  double min_val_, max_val_;
};

/**
 * @class AdjustHue
 * @brief Randomly shifts image hue in HSV space.
 */
class AdjustHue : public Operation {
 public:
  AdjustHue(int min_val, int max_val);
  void apply(Image& img, std::mt19937& rng) const override;
  std::string name() const override;

 private:
  int min_val_, max_val_;
};

// ==========================================================
// Noise & Filtering
// ==========================================================

/**
 * @class InjectNoise
 * @brief Adds Gaussian noise to the image.
 */
class InjectNoise : public Operation {
 public:
  InjectNoise();
  InjectNoise(double mean_min, double mean_max, double stdev_min,
              double stdev_max);
  void apply(Image& img, std::mt19937& rng) const override;
  std::string name() const override;

 private:
  double mean_min_, mean_max_, stdev_min_, stdev_max_;
};

/**
 * @class BlurImage
 * @brief Applies a blur filter with a random odd-sized kernel.
 */
class BlurImage : public Operation {
 public:
  BlurImage();
  BlurImage(int min_k, int max_k);
  void apply(Image& img, std::mt19937& rng) const override;
  std::string name() const override;

 private:
  int min_k_, max_k_;
};

/**
 * @class SharpenImage
 * @brief Sharpens the image using Laplacian-based enhancement.
 */
class SharpenImage : public Operation {
 public:
  SharpenImage();
  void apply(Image& img, std::mt19937& rng) const override;
  std::string name() const override;
};

/**
 * @class RandomErase
 * @brief Erases a random rectangular region in the image.
 */
class RandomErase : public Operation {
 public:
  RandomErase();
  RandomErase(int min_h, int max_h, int min_w, int max_w);
  void apply(Image& img, std::mt19937& rng) const override;
  std::string name() const override;

 private:
  int min_h_, max_h_, min_w_, max_w_;
};
