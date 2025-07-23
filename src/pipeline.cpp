/**
 * @file pipeline.cpp
 * @brief Implementation of the Pipeline class for applying image augmentations.
 * @author Emmanuel Butsana
 * @date Initial release: June 3, 2025
 */

// TODO: Add new operations, addOperationWithName, verbosity, metadata of ops
// applied

#include "pipeline.hpp"

/* Constructor with optional base seed */
Pipeline::Pipeline(unsigned int seed) : base_seed_(seed) {}

/* Add an operation to the pipeline with an associated execution probability */
void Pipeline::addOperation(std::shared_ptr<Operation> op, double prob) {
  operations_.push_back(OperationEntry{op, prob});
}

/* Apply the pipeline to an image using internal seeding based on image ID */
void Pipeline::apply(Image& img) {
  thread_local std::mt19937 rand;
  rand.seed(base_seed_ + static_cast<unsigned int>(img.getId()));
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  // TODO: Iterate over operations, applying operation
}

/* Apply the pipeline to an image using an externally provided seed */
void Pipeline::apply(Image& img, unsigned int seed) {
  thread_local std::mt19937 rand;
  rand.seed(seed);
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  // TODO: Iterate over operations, applying operation
}

/* Configure a pipeline from a map of probabilities */
Pipeline configurePipeline(const std::unordered_map<std::string, double>& probs,
                           unsigned int seed) {
  Pipeline pipeline(seed);

  auto add_if_present = [&](const std::string& key, std::shared_ptr<Operation> op){
    auto it = probs.find(key);
    if (it != probs.end()) {
      pipeline.addOperation(std::move(op), it->second);
    }
  };

  add_if_present("rotate", std::make_shared<RotateImage>(
  
  return pipeline;
}
