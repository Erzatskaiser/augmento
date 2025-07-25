/**
 * @file json.cpp
 * @brief Implementation of JSON configuration parsing for augmento using
 * simdjson.
 * @author Emmanuel Butsana
 * @date July 24, 2025
 */

#include "../include/json.hpp"

#include <simdjson.h>

using simdjson::padded_string;
using simdjson::simdjson_error;
using simdjson::ondemand::document;
using simdjson::ondemand::object;
using simdjson::ondemand::parser;

ConfigSpec parseConfigFile(const std::string& json_path) {
  ConfigSpec config;

  // Load the JSON file into padded_string
  padded_string json_data;
  try {
    json_data = simdjson::padded_string::load(json_path);
  } catch (const simdjson_error& e) {
    throw std::runtime_error("[ERROR] Failed to load JSON configuration file.");
  }

  // Parse the document
  parser json_parser;
  document doc;
  try {
    doc = json_parser.iterate(json_data);
  } catch (const simdjson_error& e) {
    throw std::runtime_error(
        "[ERROR] Failed to parse JSON configuration file.");
  }

  try {
    // output_dir (required)
    auto output_dir_val = doc["output_dir"];
    if (!output_dir_val.is_null()) {
      config.output_dir = std::string(output_dir_val.get_string().value());
    } else {
      throw std::runtime_error("[ERROR] Missing required field output_dir.");
    }

    // iterations (optional, default 1)
    if (auto iter_val = doc["iterations"]; !iter_val.is_null()) {
      config.iterations = static_cast<int>(iter_val.get_uint64().value());
      if (config.iterations < 1) config.iterations = 1;
    }

    // num_threads (optional)
    if (auto nt_val = doc["num_threads"]; !nt_val.is_null()) {
      config.num_threads = static_cast<size_t>(nt_val.get_uint64().value());
      if (config.num_threads < 1) config.num_threads = 1;
    }

    // queue_capacity (optional)
    if (auto qc_val = doc["queue_capacity"]; !qc_val.is_null()) {
      config.queue_capacity = static_cast<size_t>(qc_val.get_uint64().value());
      if (config.queue_capacity < 1) config.queue_capacity = 128;
    }

    // verbose (optional)
    if (auto vb_val = doc["verbose"]; !vb_val.is_null()) {
      config.verbose = vb_val.get_bool().value();
    }

    // deterministic (optional)
    if (auto det_val = doc["deterministic"]; !det_val.is_null()) {
      config.deterministic = det_val.get_bool().value();
    }

    // seed (optional)
    if (auto seed_val = doc["seed"]; !seed_val.is_null()) {
      config.seed = static_cast<unsigned int>(seed_val.get_uint64().value());
    }

    // image_paths (required)
    auto paths_val = doc["image_paths"];
    if (!paths_val.is_null()) {
      for (auto path_val : paths_val.get_array().value()) {
        std::string p = std::string(path_val.get_string().value());
        config.image_paths.push_back(fs::path(p));
      }
    } else {
      throw std::runtime_error("[ERROR] Missing required field image_paths.");
    }

    // pipeline - detect which style is used
    auto pipeline_val = doc["pipeline"];
    if (!pipeline_val.is_null()) {
      for (auto op_val : pipeline_val.get_array().value()) {
        object op_obj = op_val.get_object().value();

        std::string name;
        double prob = 1.0;
        std::vector<double> params;

        // Parse name
        auto name_val = op_obj["name"];
        if (!name_val.is_null()) {
          name = std::string(name_val.get_string().value());
        } else {
          throw std::runtime_error(
              "[ERROR] Pipeline operation missing 'name' field.");
        }

        // Parse prob (optional)
        if (auto prob_val = op_obj["prob"]; !prob_val.is_null()) {
          prob = prob_val.get_double().value();
          if (prob < 0.0 || prob > 1.0) {
            throw std::runtime_error(
                "[ERROR] Probability must be between 0 and 1.");
          }
        }

        // Parse params (if present)
        if (auto params_val = op_obj["params"]; !params_val.is_null()) {
          for (auto p : params_val.get_array().value()) {
            params.push_back(p.get_double().value());
          }
          config.pipeline_specs.emplace_back(name, std::move(params), prob);
        } else {
          config.pipeline_specs.emplace_back(name, std::vector<double>{}, prob);
        }
      }
    } else {
      throw std::runtime_error("[ERROR] Missing required field pipeline.");
    }

  } catch (const simdjson_error& e) {
    throw std::runtime_error(
        std::string("[ERROR] While accessing JSON fields: ") + e.what());
  }

  return config;
}
