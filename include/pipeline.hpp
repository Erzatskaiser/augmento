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
#include <memory>
#include <random>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "factory.hpp"
#include "image.hpp"
#include "json.hpp"
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
   * @param op OperationEntry object represeting operation.
   */
  void addOperation(const OperationEntry& op);

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
  std::vector<OperationEntry>
      operations_;          ///< Stored transformation operations.
  unsigned int base_seed_;  ///< Seed used for deterministic augmentation.
};

using ParamList = std::vector<double>;

/**
 * @brief Constructs pipeline from a list of operations with explicit parameters
 * @param config A vector of tuples where each tuple contains: operation
 * name(std::string), a list of parameters (ParamList), probability of applying
 * the operation
 * @param seed Optional base seed for deterministic behavior.
 * @return Configured Pipeline object.
 */
Pipeline configurePipeline(
    const std::vector<std::tuple<std::string, ParamList, double>>& config,
    unsigned int seed = std::random_device{}());

/**
 * @brief Constructs pipeline from a list of operations using default parameters
 * @param config A vector of pairs, where each contains: operation name
 * (std::string), probability of applying the operation
 * @param seed Optional base seed for deterministic behavior.
 * @return Configured Pipeline object.
 */
Pipeline configurePipeline(
    const std::vector<std::pair<std::string, double>>& config,
    unsigned int seed = std::random_device{}());
