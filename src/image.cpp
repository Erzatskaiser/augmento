/**
 * @file image.cpp
 * @brief Implementation of the Image class defined in image.hpp.
 * @author Emmanuel Butsana
 * @date Initial release: June 3, 2025
 */

#include "../include/image.hpp"

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

/* Initialize global ID counter */
std::atomic<size_t> Image::global_id_{0};

/* Default constructor */
Image::Image() : data_(), name_(""), id_(global_id_++) {}

/* Constructor from existing cv::Mat */
Image::Image(const cv::Mat& in, const std::string& name)
    : data_(in.clone()), name_(name), id_(global_id_++) {}

/* Constructor from file path */
Image::Image(const std::string& path) : name_(path), id_(global_id_++) {
  data_ = cv::imread(path);
  if (data_.empty()) {
    std::cerr << "Error: Failed to load image from " << path << std::endl;
  }
}

/* Retrieve mutable reference to image data */
cv::Mat& Image::getData() { return data_; }

/* Retrieve const reference to image data */
const cv::Mat& Image::getData() const { return data_; }

/* Replace image data with a copy of the input */
void Image::setData(const cv::Mat& newData) { data_ = newData.clone(); }

/* Set image name */
void Image::setName(const std::string& name) { name_ = name; }

/* Get image name */
const std::string& Image::getName() const { return name_; }

/* Get globally unique image ID */
const size_t Image::getId() const { return id_; }

/* Save image to specified path and extension */
int Image::save(const std::string& path, const std::string& ext) const {
  if (path.empty()) return -1;

  // Create output directory if it doesn't exist
  if (!fs::exists(path)) {
    if (!fs::create_directories(fs::path(path))) return -1;
  }

  // Determine filename
  std::string base = name_.empty() ? "image_" + std::to_string(id_)
                                   : fs::path(name_).stem().string();
  fs::path outputPath = fs::path(path) / (base + ext);

  // Write image to file
  if (!cv::imwrite(outputPath.string(), data_)) return -1;

  return 0;
}
