/** operation.cpp
 * @brief Implementation of concrete augmentation operations for augmento
 * @author Emmanuel Butsana
 * @date Initial release: July, 20, 2025
 *
 * Implements geometric, color, noise, annd filtering transformations for use in
 * an data augmentation pipeline.
 */

#include "../include/operation.hpp"

/** Constructor for RotateImage class **/
RotateImage::RotateImage(double min_angle, double max_angle, size_t rot_type) : min_angle_ (min_angle), max_angle_(max_angle), rot_type_(rot_type) {
  if (min_angle_ > max_angle_) throw std::invalid_argument("RotateImage: min angle cannot be greater than max angle");
}

/** RotateImage apply function **/
void RotateImage::apply(Image& img, std::mt19937& rng) const {
  std::uniform_real_distribution<double> angleDist(min_angle_, max_angle_);
  double angle = angleDist(rng);

  if (rot_type_ == 0) {
    img.setData(rotateImageNocrop(img.getData(), angle));
    img.logOperation("Rotate image (no crop): " + std::to_string(angle));
  }
  else if (rot_type_ == 1) { 
    img.setData(rotateImageCrop(img.getData(), angle));
    img.logOperation("Rotate image (crop): " + std::to_string(angle));
  }
  else {
    img.setData(rotateImage(img.getData(), angle));
    img.logOperation("Rotate image: " + std::to_string(angle));
  }
}

/** RotateImage name function **/
std::string RotateImage::name() const {
  return "Rotate Image : rotates image with crop, no crop, or fill-in";
}

/** Constructor for ReflectImage class **/
ReflectImage::ReflectImage() = default;

/** ReflectImage apply function **/
void ReflectImage::apply(Image& img, std::mt19937& rng) const {
  std::uniform_int_distribution<int> axisDist(0, 1);
  int axis = axisDist(rng);

  if (axis == 0) {
    img.setData(reflectImageVertical(img.getData());
    img.logOperation("Reflect image: Vertical");
  }
  else if (axis == 1) {
    img.setData(reflectImageHorizontal(img.getData());
    img.logOperation("Reflect image: Horizontal");
  }
}

/** ReflectImage name function **/
std::string ReflectImage::name() const {
  return "Reflect Image : reflects image along a horizontal or vertical axis";
}

/** Constructor for ResizeImage, scale range */
ResizeImage::ResizeImage(double min_scale, double max_scale) : min_scale_ (min_scale), max_scale_ (max_scale), min_w_ (-1), max_w_ (-1), min_h_ (-1), max_h_ (-1) {
  if (min_scale_ > max_scale_) throw std::invalid_argument("ResizeImage: min scale cannot be greater than max scale");
}

/** Constructor for ResizeImage, using absolute scale */
ResizeImage::ResizeImage(int min_w, int max_w, int min_h, int max_h) : min_w_ (min_w), max_w_ (max_w), min_h_(min_h), max_h_(max_h), min_scale_(-1), max_scale_(-1) {
  if (min_w_ > max_w_ || min_h_ > max_h_) throw std::invalid_argument("ResizeImage: min dimension cannot be greater than max dimension");
}

/** ResizeImage apply function **/
void ResizeImage::apply(Image& img, std::mt19937& rng) const {
  if (min_w_ == -1) {
    std::uniform_real_distribution<double> scaleDist(min_scale_, max_scale_);
    double scale = scaleDist(rng);

    img.setData(resizeImage(img.getData(), scale);
    img.logOperation("Resized image (with scale): " + std::to_string(angle)); 
  }

  else if (min_scale_ == -1) {
    std::uniform_int_distribution<int> wDist(min_w_, max_w_);
    std::uniform_int_distribution<int> hDist(min_h_, max_h_);
    int w = wDist(rng);
    int h = hDist(rng);

    img.setData(resizeImage(img.getData(), w, h);
    img.logOperation("resized image (absolute): " + std::to_string(w) + ", " + std::to_string(h));
  }
}

/** Constructor for CropImage, random **/
