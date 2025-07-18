// Emmanuel Butsana, 03 June 2025, image.cpp
// Defines the image class from extra.hpp

#include "extra.hpp"

/* Initialize global Id */
std::atomic<size_t> Image::global_id_{0};

/* Default contructor */
Image::Image() : data_(), name_(""), id_(global_id_++) {}

/* Constructor from existing cv::Mat */
Image::Image(const cv::Mat& in, const std::string& name)
    : data_(in.clone()), name_(name), id_(global_id_++) {}

/* Constructor from file path */
Image::Image(const std::string& path) {
  name_ = path;
  data_ = cv::imread(path);
  id_ = global_id_++;
  if (data_.empty()) {
    std::cerr << "Error: Image could not be loaded from " << path << std::endl;
  }
}

/* Retrieve stored image data (mutable) */
cv::Mat& Image::getData() { return data_; }

/* Retrieve stored image data (non-mutable) */
const cv::Mat& Image::getData() const { return data_; }

/* Replace image data */
void Image::setData(const cv::Mat& newData) { data_ = newData.clone(); }

/* Set the image name */
void Image::setName(const std::string& name) { name_ = name; }

/* Get the image name */
const std::string& Image::getName() const { return name_; }

/* Get the image id */
const size_t Image::getId() const { return id_; }

/* Save image to file */
int Image::save(const std::string& path, const std::string& ext) const {
  if (path.empty()) return -1;
  namespace fs = std::filesystem;

  // If path does exist, create folder
  if (!fs::exists(path)) {
    if (!fs::create_directories(fs::path(path))) return -1;
  }

  // Make default name if name is empty
  std::string temp = name_;
  if (name_.empty()) {
    temp = "image_" + std::to_string(id_) + ext;
  }

  // Extract basename
  fs::path pathObj(temp);
  std::string stem = pathObj.stem().string();

  // Compose filename and full path
  std::string filename = stem + ext;
  fs::path outputPath = fs::path(path) / filename;

  if (!cv::imwrite(outputPath, data_)) return -1;
  return 0;
}
