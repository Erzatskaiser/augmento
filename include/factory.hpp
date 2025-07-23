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
#include "pipeline.hpp"

using ParamList = std::vector<double>;

class OperationFactory {
 public:
  OperationFactory();
  static OperationEntry create(const std::string& name, const ParamList& params,
                               double prob = 1.0);
  static OperationEntry create(const std::string& name, double prob = 1.0);
};
