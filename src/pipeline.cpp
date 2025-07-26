/**
 * @file pipeline.cpp
 * @brief Implementation of the Pipeline class for applying image augmentations.
 * @author Emmanuel Butsana
 * @date Initial release: June 3, 2025
 */

#include "../include/pipeline.hpp"

/* Constructor with optional base seed */
Pipeline::Pipeline(unsigned int seed) : base_seed_(seed) {}

/* add an operation to the pipeline using an OperationEntry object */
void Pipeline::addOperation(const OperationEntry& op) {
  operations_.push_back(op);
}

/* Apply the pipeline to an image using internal seeding based on image ID */
void Pipeline::apply(Image& img) {
  std::mt19937 rand(base_seed_ + static_cast<unsigned int>(
                                     std::hash<std::string>{}(img.getName())));
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  // Iterate over operations, applying operation
  for (const OperationEntry& op : operations_) {
    double probs = dist(rand);
    if (probs <= op.prob) op.op->apply(img, rand);
  }
}

/* Apply the pipeline to an image using an externally provided seed */
void Pipeline::apply(Image& img, unsigned int seed) {
  std::mt19937 rand(seed);
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  // Iterate over operations, applying operation
  for (const OperationEntry& op : operations_) {
    double probs = dist(rand);
    if (probs <= op.prob) op.op->apply(img, rand);
  }
}

/* Configure a pipeline from a map of probabilities */
Pipeline configurePipeline(
    const std::vector<std::tuple<std::string, ParamList, double>>& config,
    unsigned int seed) {
  Pipeline pipeline(seed);

  for (auto& configLine : config) {
    ParamList params = std::get<1>(configLine);
    if (!params.empty()) {
      OperationEntry op = OperationFactory::create(std::get<0>(configLine),
                                                   std::get<1>(configLine),
                                                   std::get<2>(configLine));
      pipeline.addOperation(op);
    }

    else {
      OperationEntry op = OperationFactory::create(std::get<0>(configLine),
                                                   std::get<1>(configLine));
      pipeline.addOperation(op);
    }
  }

  return pipeline;
}

/* Configure a pipeline without setting any parameters */
Pipeline configurePipeline(
    const std::vector<std::pair<std::string, double>>& config,
    unsigned int seed) {
  Pipeline pipeline(seed);

  for (auto& configLine : config) {
    OperationEntry op = OperationFactory::create(std::get<0>(configLine),
                                                 std::get<1>(configLine));
    pipeline.addOperation(op);
  }

  return pipeline;
}
