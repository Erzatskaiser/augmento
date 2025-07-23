/**
 * @file factory.cpp
 * @brief Implementation of the OperationFactory class defined in image.hpp.
 * @author Emmanuel Butsana
 * @date Initial release: June 3, 2025
 */

#include "../include/factory.hpp"

/** Create operation entry with params **/
OperationEntry OperationFactory::create(const std::string& name,
                                        const ParamList& params, double prob) {
  std::string key = name;
  std::transform(key.begin(), key.end(), key.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  if (key == "rotate") {
    if (params.size() != 3)
      throw std::invalid_argument("rotation operation takes 3 arguments");
    return OperationEntry{
        std::make_shared<RotateImage>(params[0], params[1],
                                      static_cast<size_t>(params[2])),
        prob};
  }

  else if (key == "reflect") {
    if (params.size() != 0)
      throw std::invalid_argument("reflect operation takes no arguments");
    return OperationEntry{std::make_shared<ReflectImage>(), prob};
  }

  else if (key == "resize") {
    if (params.size() == 2)
      return OperationEntry{std::make_shared<ResizeImage>(params[0], params[1]),
                            prob};
    else if (params.size() == 4)
      return OperationEntry{std::make_shared<ResizeImage>(params[0], params[1],
                                                          params[2], params[3]),
                            prob};
    throw std::invalid_argument("resize operation takes 2 or 4 arguments");
  }

  else if (key == "crop") {
    if (params.size() == 2)
      return OperationEntry{std::make_shared<CropImage>(params[0], params[1]),
                            prob};
    else if (params.size() == 4)
      return OperationEntry{std::make_shared<CropImage>(params[0], params[1],
                                                        params[2], params[3]),
                            prob};
    throw std::invalid_argument("crop operation takes 2 or 4 arguments");
  }

  else if (key == "affine transform") {
    if (params.empty()) {
      std::random_device rd;
      std::mt19937 rng(rd());
      return OperationEntry{std::make_shared<AffineTransform>(rng), prob};
    } else if (params.size() == 6) {
      cv::Mat matrix = (cv::Mat_<double>(2, 3) << params[0], params[1],
                        params[2], params[3], params[4], params[5]);
      return OperationEntry{std::make_shared<AffineTransform>(matrix), prob};
    } else
      throw std::invalid_argument(
          "affine transform operation takes 0 or 6 arguments");
  }

  else if (key == "color jitter") {
    if (params.size() != 4)
      throw std::invalid_argument("color jitter operation takes 4 arguments");
    return OperationEntry{
        std::make_shared<ColorJitter>(params[0], params[1], params[2],
                                      static_cast<int>(params[3])),
        prob};
  }

  else if (key == "histogram equalization") {
    if (params.size() != 0)
      throw std::invalid_argument("histogram equalization takes 0 arguments");
    return OperationEntry{std::make_shared<HistogramEqualization>(), prob};
  }

  else if (key == "white balance") {
    if (params.size() != 0)
      throw std::invalid_argument("white balance takes 0 arguments");
    return OperationEntry{std::make_shared<WhiteBalance>(), prob};
  }

  else if (key == "to grayscale") {
    if (params.size() != 0)
      throw std::invalid_argument("to grayscale takes 0 arguments");
    return OperationEntry{std::make_shared<ToGrayscale>(), prob};
  }

  else if (key == "adjust brightness") {
    if (params.size() != 2)
      throw std::invalid_argument("adjust brightness takes 2 arguments");
    return OperationEntry{
        std::make_shared<AdjustBrightness>(params[0], params[1]), prob};
  }

  else if (key == "adjust contrast") {
    if (params.size() != 2)
      throw std::invalid_argument("adjust contrast takes 2 arguments");
    return OperationEntry{
        std::make_shared<AdjustContrast>(params[0], params[1]), prob};
  }

  else if (key == "adjust saturation") {
    if (params.size() != 2)
      throw std::invalid_argument("adjust saturation takes 2 arguments");
    return OperationEntry{
        std::make_shared<AdjustSaturation>(params[0], params[1]), prob};
  }

  else if (key == "adjust hue") {
    if (params.size() != 2)
      throw std::invalid_argument("adjust hue takes 2 arguments");
    return OperationEntry{std::make_shared<AdjustHue>(params[0], params[1]),
                          prob};
  }

  else if (key == "inject noise") {
    if (params.size() == 0)
      return OperationEntry{std::make_shared<InjectNoise>(), prob};
    if (params.size() == 4)
      return OperationEntry{std::make_shared<InjectNoise>(params[0], params[1],
                                                          params[2], params[3]),
                            prob};
    else
      throw std::invalid_argument("inject noise must take 0 or 4 arguments");
  }

  else if (key == "blur image") {
    if (params.size() == 0)
      return OperationEntry{std::make_shared<BlurImage>(), prob};
    if (params.size() == 2)
      return OperationEntry{std::make_shared<BlurImage>(params[0], params[1]),
                            prob};
    else
      throw std::invalid_argument("blur image must take 0 or 2 arguments");
  }

  else if (key == "sharpen image") {
    if (params.size() == 0)
      return OperationEntry{std::make_shared<SharpenImage>(), prob};
    else
      throw std::invalid_argument("sharpen image take 0 arguments");
  }

  else if (key == "random erase") {
    if (params.size() == 0)
      return OperationEntry{std::make_shared<RandomErase>(), prob};
    else if (params.size() == 4)
      return OperationEntry{std::make_shared<RandomErase>(params[0], params[1],
                                                          params[2], params[3]),
                            prob};
    else
      throw std::invalid_argument("random erase must take 0 or 4 arguments");
  }

  else
    throw std::invalid_argument("operation \"" + name + "\" is not recognized");
}
