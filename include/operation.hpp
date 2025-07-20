/**
 * @file operation.hpp
 * @brief Abstract base class for all image augmentation operations in augmento.
 * @author Emmanuel Butsana
 * @date Initial release: July 18, 2025
 *
 * Defines the Operation interface and a registry of built-in operations.
 */

#pragma once

#include <opencv2/core.hpp>
#include <random>
#include <string>
#include <memory>

#include "image.hpp"
#include "manipulations.hpp"

/**
 * @class Operation
 * @brief Abstract base class for all image augmentation operations.
 */
class Operation {
  public:
    ///@brief Virtual destructor.
    virtual ~Operation() = default;

    /**
     * @brief Apply the operation to the given image using provided RNG.
     * @param img Image object to modify.
     * @param rng Random number generator.
     */
    virtual void apply(Image& img, std::mt19937& rng) const = 0;

    /**
     * @brief Get human-readable name of the operation
     * @return String representation of the operation
     */
    virtual std::string name() const = 0;
};

/// Type alias for shared pointer to Operation
using OperationPtr = std::shared_ptr<Operation>;

/**
 * @class RotateImage
 * @brief Class for image rotation operations.
 *
 * This operation rotates an image by a randomly selected angle within the
 * range [min_angle, max_angle]. Cropping may or may not be implemented. 
 * This operation modifies the image in-place and logs the exact rotation applied.
 */
class RotateImageNoCrop : public Operation {
  public:
    /**
     * @brief Construct new RotateImageNoCrop.
     * @param min_angle Minimum rotation angle in degrees (inclusive).
     * @param max_angle Maximum rotation angle in degrees (inclusive).
     * @param rot_type Rotation type to be implemented (default = no crop).
     */
    RotateImageNoCrop(double min_angle, double max_angle, size_t rot_type);

    /**
     * @brief Apply the rotation to the image.
     * @param img The image to modify (data is replaced).
     * @param rng Random number generator.
     */
    void apply(Image& img, std::mt19937& rng) const override;

    /**
     * @brief Get the name of the operation
     */
    std::string name() const override;

  private:
    double min_angle_; ///< Minimum angle in degrees.
    double max_angle_; ///< Maximum angle in degrees.
};

/**
 * @class ReflectImage
 * @brief Class for image reflection operations.
 *
 * This operation reflects an image vertically or horizontally. This operation
 * modifies the image in-place and logs the reflection applied.
 */
class ReflectImage : public Operation {
  public:
    /**
     * @brief Construct new ReflectImage.
     */
     ReflectImage();

     /**
      * @brieg Apply the reflection to the image.
      * @param img the Image to modify (data is replaced).
      * @param rng Random number generator.
      */
     void apply(Image& im, std::mt19937& rng) const override;

     /**
      * @brief Get the name of the operation
      */
     std::string name() const override;
};

/**
 * @class ResizeImage
 * @brief Class for image resizing operatoins.
 *
 * This operation resizes an image, using specific height/width or scale. This 
 * operation modifies the image in-place and logs the reflection applied.
 */
class ResizeImage : public Operation {
  public:
    /**
     * @brief Construct new ResizeImage.
     * @param min_scale Minimum scale level (inclusive).
     * @param max_scale Maximum scale level (inclusive).
     */
    ResizeImage(double min_scale, double max_scale);

    /**
     * @brief Construct new ResizeImage.
     * @param min_width Minimum width (inclusive).
     * @param max_width Maximum width (inclusive).
     * @param min_height Minimum height (inclusive).
     * @param max_height Maximum height (inclusive).
     */
    ResizeImage(int min_width, int max_width, int min_height, int max_height)
    
    /**
     * @brief Apply the resizing to the image.
     * @param img The image to modiy (data is replaced).
     * @param rng Random number generator.
     */
    void apply(Img& img, std::mt19937& rng) const override;

    /**
     * @brief Get the name of the operation
     */
    std::string name() const override;

  private:
    bool scale_type;
    double min_scale_;
    double max_scale_;
    double min_width_;
    double max_width_;
};

/**
 * @class CropImage
 * @brief Class for image cropping
 *
 * This operation crops an image, allowing for both random and deterministic
 * crops. This operation modifies the image in-place and logs the exact crop applied.
 */
class CropImage : public Operation {
  public:
    /**
     * @brief Construct new CropImage.
     * @param width Width of the crop region
     * @param height Height of the crop region
    */
    CropImage(int width, int height);

    /**
     * @brief Construct new CropImage.
     * @param x X-coordinate of top left point
     * @param y Y-coordinate of top left point
     * width width Width of the crop region
     * height height Height of the crop region
     */
    CropImage(int x, int y, int width, int height);

    /**
     * @brief Apply the crop to the image.
     * @param img The image to modify (data is replaced).
     * @param rng Random number generator.
     */
    void apply(Image& img, std::mt19937& rng) const override;

    /**
     * @brief Get the name of the operation
     */
    std::string name() const override;

  private:
    double x_;
    double y_;
    double width_;
    double height_;
    bool israndom;
};
