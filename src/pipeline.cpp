/**
 * @file pipeline.cpp
 * @brief Implementation of the Pipeline class for applying image augmentations.
 * @author Emmanuel Butsana
 * @date Initial release: June 3, 2025
 */

// TODO: Add new operations, addOperationWithName, verbosity, metadata of ops
// applied

#include "pipeline.hpp"

#include <random>
#include <unordered_map>

#include "image.hpp"
#include "manipulations.hpp"

/* Constructor with optional base seed */
Pipeline::Pipeline(unsigned int seed) : base_seed_(seed) {}

/* Add an operation to the pipeline with an associated execution probability */
void Pipeline::addOperation(std::function<void(Image&, std::mt19937&)> op,
                            double prob) {
  operations_.push_back(Operation{op, prob});
}

/* Apply the pipeline to an image using internal seeding based on image ID */
void Pipeline::apply(Image& img) {
  thread_local std::mt19937 rand;
  rand.seed(base_seed_ + static_cast<unsigned int>(img.getId()));
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  for (const auto& op : operations_) {
    if (dist(rand) < op.prob) {
      op.func(img, rand);
    }
  }
}

/* Apply the pipeline to an image using an externally provided seed */
void Pipeline::apply(Image& img, unsigned int seed) {
  thread_local std::mt19937 rand;
  rand.seed(seed);
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  for (const auto& op : operations_) {
    if (dist(rand) < op.prob) {
      op.func(img, rand);
    }
  }
}

/* Configure a pipeline from a map of probabilities */
Pipeline configurePipeline(const std::unordered_map<std::string, double>& probs,
                           unsigned int seed) {
  Pipeline pipeline(seed);

  pipeline.addOperation(
      [](Image& img, std::mt19937& rng) {
        std::uniform_real_distribution<double> angleDist(-30.0, 30.0);
        img.setData(rotateImageCrop(img.getData(), angleDist(rng)));
      },
      probs.at("rotate"));

  pipeline.addOperation(
      [](Image& img, std::mt19937& rng) {
        std::uniform_int_distribution<int> axisDist(0, 1);
        int axis = axisDist(rng);
        if (axis == 0)
          reflectImageVertical(img.getData());
        else
          reflectImageHorizontal(img.getData());
      },
      probs.at("reflect"));

  pipeline.addOperation(
      [](Image& img, std::mt19937&) { histogramEqualization(img.getData()); },
      probs.at("hist_eq"));

  pipeline.addOperation(
      [](Image& img, std::mt19937&) { whiteBalance(img.getData()); },
      probs.at("white_balance"));

  pipeline.addOperation(
      [](Image& img, std::mt19937& rng) {
        std::uniform_real_distribution<double> dist(-50.0, 50.0);
        adjustBrightness(img.getData(), dist(rng));
      },
      probs.at("brightness"));

  pipeline.addOperation(
      [](Image& img, std::mt19937& rng) {
        std::uniform_real_distribution<double> dist(0.8, 1.5);
        adjustContrast(img.getData(), dist(rng));
      },
      probs.at("contrast"));

  pipeline.addOperation(
      [](Image& img, std::mt19937& rng) {
        std::uniform_real_distribution<double> dist(0.7, 1.3);
        adjustSaturation(img.getData(), dist(rng));
      },
      probs.at("saturation"));

  pipeline.addOperation(
      [](Image& img, std::mt19937& rng) {
        std::uniform_int_distribution<int> dist(-10, 10);
        adjustHue(img.getData(), dist(rng));
      },
      probs.at("hue"));

  pipeline.addOperation(
      [](Image& img, std::mt19937& rng) {
        std::normal_distribution<double> noiseMeanDist(0.0, 30.0);
        double mean = noiseMeanDist(rng);
        double stdev = std::abs(noiseMeanDist(rng));
        injectNoise(img.getData(), mean, stdev);
      },
      probs.at("noise"));

  pipeline.addOperation(
      [](Image& img, std::mt19937& rng) {
        std::uniform_int_distribution<int> dist(1, 5);
        int k = dist(rng);
        if (k % 2 == 0) ++k;
        blurImage(img.getData(), k);
      },
      probs.at("blur"));

  pipeline.addOperation(
      [](Image& img, std::mt19937&) { sharpenImage(img.getData()); },
      probs.at("sharpen"));

  return pipeline;
}
