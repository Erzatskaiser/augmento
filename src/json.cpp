/**
 * @file json.cpp
 * @brief Implementation of JSON configuration parsing for augmento using
 * simdjson.
 * @author Emmanuel Butsana
 * @date July 24, 2025
 */

#include "../include/json.hpp"

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
    json_data = padded_string::load(json_path);
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

  object parse_object = doc.get_object();

  // Tracker variables to ensure required fields are set
  bool output_dir_set = false;
  bool input_dir_set = false;
  bool pipeline_set = false;

  try {
    for (auto field : parse_object) {
      std::string_view key = field.escaped_key();

      if (key == "output_dir") {
        config.output_dir = std::string(field.value().get_string().value());
        if (config.output_dir.empty())
          throw std::runtime_error(
              "[ERROR] Missing required field output_dir.");
        output_dir_set = true;
      }

      if (key == "input_dir") {
        config.input_dir = std::string(field.value().get_string().value());
        if (config.input_dir.empty())
          throw std::runtime_error("[ERROR] Missing required field input_dir.");
        input_dir_set = true;
      }

      if (key == "iterations") {
        config.iterations = static_cast<int>(uint64_t(field.value()));
        if (!config.iterations || config.iterations < 1) config.iterations = 1;
      }

      if (key == "num_threads") {
        config.num_threads = static_cast<int>(uint64_t(field.value()));
        if (!config.num_threads || config.num_threads < 1)
          config.num_threads = 1;
      }

      if (key == "queue_capacity") {
        config.queue_capacity = static_cast<size_t>(uint64_t(field.value()));
        if (!config.queue_capacity || config.queue_capacity < 1)
          config.queue_capacity = 50;
      }

      if (key == "verbose") {
        config.verbose = bool(field.value());
      }

      if (key == "seed") {
        config.seed = static_cast<size_t>(uint64_t(field.value()));
      }

      if (key == "pipeline") {
        auto pipeline_array = field.value().get_array().value();

        for (auto op_val : pipeline_array) {
          object op_obj = op_val.get_object().value();

          std::string name;
          double prob = 1.0;
          std::vector<double> params;

          for (auto op_field : op_obj) {
            std::string_view op_key = op_field.escaped_key();

            if (op_key == "name") {
              name = std::string(op_field.value().get_string().value());
              if (name.empty())
                throw std::runtime_error(
                    "[ERROR] Pipeline operation missing 'name' field.");
            }

            if (op_key == "prob") {
              prob = op_field.value().get_double().value();
              if (prob < 0.0 || prob > 1.0)
                throw std::runtime_error(
                    "[ERROR] Probability must be between 0 and 1.");
            }

            if (op_key == "params") {
              for (auto param : op_field.value().get_array().value()) {
                params.push_back(param.get_double().value());
              }
            }
          }

          if (name.empty())
            throw std::runtime_error(
                "[ERROR] Pipeline operation missing 'name' field.");

          config.pipeline_specs.emplace_back(std::move(name), std::move(params),
                                             prob);
        }

        pipeline_set = true;
      }
    }
  } catch (const simdjson_error& e) {
    throw std::runtime_error(
        std::string("[ERROR] While accessing JSON fields: ") + e.what());
  }

  if (!output_dir_set || !input_dir_set || !pipeline_set) {
    throw std::runtime_error(
        "[ERROR] Missing one or more required fields in config.");
  }

  return config;
}
