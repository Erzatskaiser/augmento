/**
 * @file pipeline.hpp
 * @brief Defines the Pipeline class for managing probabilistic image
 * transformations in augmento. Also declares helper functions for building
 * configured pipelines from operation maps.
 * @author Emmanuel Butsana
 * @date Initial release: June 3, 2025
 *
 * The Pipeline class holds a sequence of augmentation operations with
 * associated probabilities and applies them consistently using a seeded random
 * engine.
 */

#pragma once

#include <functional>
#include <random>
#include <unordered_map>
#include <vector>

#include "image.hpp"
#include "operation.hpp"

/**
 * @class Pipeline
 * @brief Manages a sequence of probabilistic image transformations.
 *
 * Each transformation is a callable that takes an Image and a seeded random
 * generator. Operations are applied with a given probability, allowing for
 * randomized augmentation pipelines.
 */
class Pipeline {
 public:
  /**
   * @brief Construct a pipeline with a given base seed.
   * @param seed Seed for deterministic random behavior (default:
   * random_device).
   */
  explicit Pipeline(unsigned int seed = std::random_device{}());

  /**
   * @brief Add a transformation operation to the pipeline.
   * @param op Shared pointer to an Operation instance.
   * @param prob Probability [0.0, 1.0] that this operation is applied.
   */
  void addOperation(std::shared_ptr<Operation> op,double prob = 1.0);

  /**
   * @brief Apply the pipeline to a given image using internal base seed.
   * @param img Image to transform in-place.
   */
  void apply(Image& img);

  /**
   * @brief Apply the pipeline to a given image using an explicit seed.
   * @param img Image to transform in-place.
   * @param seed Seed for deterministic behavior.
   */
  void apply(Image& img, unsigned int seed);

 private:
  struct OperationEntry {
    std::shared_ptr<Operation> op;;
    double prob;
  };

  std::vector<OperationEntry> operations_;  ///< Stored transformation operations.
  unsigned int base_seed_;  ///< Seed used for deterministic augmentation.
};

/**
 * @brief Utility function to configure a pipeline from a map of operation names
 * to probabilities.
 * @param probs Map from operation name (e.g. "rotate", "blur") to probability
 * [0.0, 1.0].
 * @param seed Optional base seed for deterministic behavior.
 * @return Configured Pipeline object.
 */
Pipeline configurePipeline(const std::unordered_map<std::string, double>& probs,
                           unsigned int seed = std::random_device{}());
