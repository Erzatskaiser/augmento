// Emmanuel Butsana, 03 June 2025, pipeline.cpp
// Implementation of the pipeline class

#include "extra.hpp"

/* Default constructor */
Pipeline::Pipeline(unsigned int seed) : base_seed_(seed) {}

/* Add operation to the pipeline */
void Pipeline::addOperation(std::function<void(Image&, std::mt19937&)> op, double prob) {
  operations_.push_back(Operation{op, prob});
}

/* Pass image through the pipeline, without preset seed */
void Pipeline::apply(Image& img) {
  // Generate random number between 0 and 1
  thread_local std::mt19937 rand;
  rand.seed(base_seed_ + static_cast<unsigned int>(img.getId()));
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  // Check whether operation will be executed
  for (const auto& op : operations_) {
    if (dist(rand) < op.prob) {
      op.func(img, rand);
    }
  }
}

/* Pass image through the piepeline, with preset seed */
void Pipeline::apply(Image& img, unsigned int seed) {
  // Generate random number between 0 and 1
  thread_local std::mt19937 rand;
  rand.seed(seed);
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  // Check whether operation will be executed
  for (const auto& op : operations_) {
    if (dist(rand) < op.prob) {
      op.func(img, rand);
    }
  }
}

/* Configure pipeline upon instantiation */
Pipeline configurePipeline(const std::unordered_map<std::string, double>& probs,
                           unsigned int seed) {
  Pipeline pipeline(seed);

  // Rotation (random angle)
  pipeline.addOperation(
    [](Image& img, std::mt19937& rng) {
      std::uniform_real_distribution<double> angleDist(-30.0, 30.0);
      double angle = angleDist(rng);
      img.setData(rotateImageCrop(img.getData(), angle));
    },
    probs.at("rotate")
  );

  // Reflect (horizontal or vertical)
  pipeline.addOperation(
    [](Image& img, std::mt19937& rng) {
      std::uniform_int_distribution<int> axisDist(0, 1);
      int axis = axisDist(rng);
      if (axis == 0)
        reflectImageVertical(img.getData());
      else
        reflectImageHorizontal(img.getData());
    },
    probs.at("reflect")
  );

  // Histogram equalization
  pipeline.addOperation(
    [](Image& img, std::mt19937&) {
      histogramEqualization(img.getData());
    },
    probs.at("hist_eq")
  );

  // White balance
  pipeline.addOperation(
    [](Image& img, std::mt19937&) {
      whiteBalance(img.getData());
    },
    probs.at("white_balance")
  );

  // Brightness adjustment
  pipeline.addOperation(
    [](Image& img, std::mt19937& rng) {
      std::uniform_real_distribution<double> dist(-50.0, 50.0);
      adjustBrightness(img.getData(), dist(rng));
    },
    probs.at("brightness")
  );

  // Contrast
  pipeline.addOperation(
    [](Image& img, std::mt19937& rng) {
      std::uniform_real_distribution<double> dist(0.8, 1.5);
      adjustContrast(img.getData(), dist(rng));
    },
    probs.at("contrast")
  );

  // Saturation adjustment
  pipeline.addOperation(
    [](Image& img, std::mt19937& rng) {
      std::uniform_real_distribution<double> dist(0.7, 1.3);
      adjustSaturation(img.getData(), dist(rng));
    },
    probs.at("saturation")
  );

  // Hue adjustment
  pipeline.addOperation(
    [](Image& img, std::mt19937& rng) {
      std::uniform_int_distribution<int> dist(-10, 10);
      adjustHue(img.getData(), dist(rng));
    },
    probs.at("hue")
  );

  // Noise injection
  pipeline.addOperation(
    [](Image& img, std::mt19937& rng) {
      std::normal_distribution<double> noiseMeanDist(0.0, 30.0);
      double mean = noiseMeanDist(rng);
      double stdev = std::abs(noiseMeanDist(rng));
      injectNoise(img.getData(), mean, stdev);
    },
    probs.at("noise")
  );

  // Blur image
  pipeline.addOperation(
    [](Image& img, std::mt19937& rng) {
      std::uniform_int_distribution<int> dist(1, 5);
      int k = dist(rng);
      if (k % 2 == 0) ++k;
      blurImage(img.getData(), k);
    },
    probs.at("blur")
  );

  // Sharpen image
  pipeline.addOperation(
    [](Image& img, std::mt19937&) {
      sharpenImage(img.getData());
    },
    probs.at("sharpen")
  );

  // Return pipeline object
  return pipeline;
}

