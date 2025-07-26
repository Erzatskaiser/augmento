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
#include <thread>
#include <stdexcept>
#include <filesystem>
#include <simdjson.h>

namespace fs = std::filesystem;
using namespace simdjson;

/**
 * @brief Struct to hold parsed configuration from a JSON file.
 */
struct ConfigSpec {
  std::string output_dir;
  std::string input_dir;
  int iterations = 1;
  size_t num_threads = std::thread::hardware_concurrency();
  size_t queue_capacity = 128;
  bool verbose = true;
  unsigned int seed = std::random_device{}();

  std::vector<fs::path> image_paths;
  std::vector<std::tuple<std::string, std::vector<double>, double>> pipeline_specs;
};

/**
 * @brief Parses a JSON config and returns spec info.
 * @param json_path Path to config file
 * @return Struct with paths
 */
ConfigSpec parseConfigFile(const std::string& json_path);
