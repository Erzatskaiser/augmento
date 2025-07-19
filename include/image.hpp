/**
 * @file image.hpp
 * @brief Image class for handling image data and metadata in augmento.
 * @author Emmanuel Butsana
 * @date Initial release: June 3, 2025
 *
 * Provides an `Image` class that encapsulates image data, filenames,
 * and unique IDs for consistent handling throughout the augmentation pipeline.
 */

#pragma once

#include <atomic>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <string>
#include <filesystem>
#include <iostream>

/**
 * @class Image
 * @brief Represents an image with associated metadata such as name and ID.
 */
class Image {
 public:
  /// @brief Default constructor.
  Image();

  /**
   * @brief Construct an Image from raw OpenCV matrix and optional name.
   * @param in Input image data.
   * @param name Optional image identifier.
   */
  Image(const cv::Mat& in, const std::string& name = "");

  /**
   * @brief Construct an Image from a file path.
   * @param path Path to image file.
   */
  Image(const std::string& path);

  /// @brief Move constructor.
  Image(Image&&) noexcept = default;

  /// @brief Move assignment operator.
  Image& operator=(Image&&) noexcept = default;

  /// @return Mutable access to image data.
  cv::Mat& getData();

  /// @return Const access to image data.
  const cv::Mat& getData() const;

  /// @return Image name or identifier string.
  const std::string& getName() const;

  /// @return Globally unique image ID.
  const size_t getId() const;

  /**
   * @brief Set image data.
   * @param in New image matrix to set.
   */
  void setData(const cv::Mat& in);

  /**
   * @brief Set image name or ID.
   * @param name New name string.
   */
  void setName(const std::string& name);

  /**
   * @brief Save image to disk.
   * @param path Output directory or full file path.
   * @param ext File extension (default ".png").
   * @return 0 on success, -1 on failure.
   */
  int save(const std::string& path = "", const std::string& ext = ".png") const;

 private:
  cv::Mat data_;              ///< Raw image matrix.
  std::string name_;          ///< Optional image name or identifier.
  size_t id_;                 ///< Unique image ID.
  static std::atomic<size_t> global_id_; ///< Global counter for assigning unique IDs.
};

