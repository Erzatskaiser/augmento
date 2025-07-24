/**
 * @file factory.hpp
 * @brief Defines the OperationFactory class that builds OperationEntry objects.
 * @author Emmanuel Butsana
 * @date July 23, 2025
 *
 * The OperationFactory may takes as input a string denoting the operation to be
 * implemented together with a vector of doubles identifying the parameters
 * being specified and a double specifying probabilities. Parameters may also be
 * selected at random by the program, deterministically or not.
 */

#include <algorithm>
#include <cctype>
#include <opencv2/core.hpp>
#include <stdexcept>

#include "operation.hpp"

/**
 * @struct OperationEntry
 * @brief Represents a single operation and its associated probability in the
 * pipeline.
 *
 * This structure is used to store an instance of an image augmentation
 * operation along with the probability that the operation should be applied
 * during pipeline execution. It is typically used internally by the Pipeline
 * class.
 */
struct OperationEntry {
  std::shared_ptr<Operation> op;
  double prob;
};

using ParamList = std::vector<double>;

/**
 * @class OperationFactory
 * @brief Static factory class for constructing image augmentation operations.
 *
 * Provides static methods to instantiate concrete image manipulation operations
 * based on operation name and parameter list. This class is intended to be used
 * as a utility and cannot be instantiated.
 */
class OperationFactory {
 public:
  OperationFactory() = delete;

  /**
   * @brief Creates an operation entry from a given name, parameters, and
   * probability.
   * @param name Name of the operation (case-insensitive).
   * @param params List of numeric parameters specific to the operation.
   * @param prob Probability of applying the operation (default: 1.0).
   * @return An OperationEntry object containing the operation and its
   * probability.
   * @throws std::invalid_argument if the parameters are invalid or unsupported.
   */
  static OperationEntry create(const std::string& name, const ParamList& params,
                               double prob = 1.0);

  /**
   * @brief Creates an operation entry from a given name using random/default
   * parameters.
   * @param name Name of the operation (case-insensitive).
   * @param prob Probability of applying the operation (default: 1.0).
   * @return An OperationEntry object with randomly selected parameters.
   * @throws std::invalid_argument if the operation is unsupported or requires
   * parameters.
   */
  static OperationEntry create(const std::string& name, double prob = 1.0);
};
