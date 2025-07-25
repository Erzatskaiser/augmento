/**
 * @file json.hpp
 * @brief Parses configuration files for augmento using simdjson.
 * @author Emmanuel Butsana
 * @date July 24, 2025
 *
 * Defines AugmentoConfig struct and functions to parse it from a JSON file.
 */

#pragma once

#include <string>
#include <tuple>
#include <vector>
#include <random>
#include <utility>
#include <stdexcept>
#include <simdjson.h>

using fs = std::filesystem;
using namespace simdjson;

/**
 * @brief Struct to hold parsed configuration from a JSON file.
 */
struct ConfigSpec {
  std::string output_dir;
  int iterations = 1;
  size_t num_threads = std::thread::hardware_concurrency();
  size_t queue_capacity = 128;
  bool verbose = true;
  bool deterministic = true;
  unsigned int seed = std::random_device{}();

  std::vector<fs::paths> image_paths;
  std::vector<std::pair<std::string, double>> pipeline_specs_no_params;
  std::vector<std::tuple<std::string, ParamList, double>> pipeline_specs_params;
};

/**
 * @brief Parses a JSON config and returns spec info.
 * @param json_path Path to config file
 * @return Struct with paths
 */
ConfigSpec parseConfigFile(const std::string& json_path);
