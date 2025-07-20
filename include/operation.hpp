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
 * @class RotateImageNoCrop
 * @brief Class for image rotation without cropping
 *
 * This operation rotates an image by a randomly selected angle within the
 * range [min_angle, max_angle]. No cropping is implemented. This operation
 * modifies the image in-place and logs the exact rotation applied.
 */
class RotateImageNoCrop : public Operation {
  public:
    /**
     * @brief Construct new RotateImageNoCrop.
     * @param min_angle Minimum rotation angle in degrees (inclusive).
     * @param max_angle Maximum rotation angle in degrees (inclusive).
     */
    RotateImageNoCrop(double min_angle, double max_angle);

    /**
     * @brief Apply the rotation and cropping to the image.
     * @param img The image to modify (data is replaced).
     * @param rng Random number generator used to sample rotation angle.
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
