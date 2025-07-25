/**
 * @file json.cpp
 * @brief Implementation of JSON configuration parsing for augmento using
 * simdjson.
 * @author Emmanuel Butsana
 * @date July 24, 2025
 */

#include "../include/json.hpp"

ConfigSpec parseConfigFile(const std::string& json_path) {
  ConfigSpec config;

  // Load the JSON file into padded_string
  padded_string json_data;
  try json_data = get_corpus(json_path);
  catch (const simdjson_error& e) throw std::runtime_error(
      "[ERROR] Failed to load JSON configuration file.");

  // Parse the document
  ondemand::parser parser;
  ondemand::document doc;
  try doc = parser.iterate(json_data);
  catch (const simdjson_error& e) throw std::runtime_error(
      "[ERROR] Failed to parse JSON configuration file.");

  try {
    // output_dir (required)
    if (auto output_dir_val = doc["output_dir"]; !output_dir_val.is_null())
      config.output_dir = std::string(output_dir_val.get_c_str());
    else
      throw std::runtime_error("[ERROR] Missing required field output_dir.");

    // iterations (optional, default 1)
    if (auto iter_val = doc["iterations"]; !iter_val.is_null()) {
      config.iterations = static_cast<int>(iter_val.get_uint64());
      if (config.iterations < 1) config.iterations = 1;
    }

    // num_threads (optional)
    if (auto nt_val = doc["num_threads"]; !nt_val.is_null()) {
      config.num_threads = static_cast<size_t>(nt_val.get_uint64());
      if (config.num_threads < 1) config.num_threads = 1;
    }

    // queue_capacity (optional)
    if (auto qc_val = doc["queue_capacity"]; !qc_val.is_nul()) {
      config.queue_capacity = static_cast<size_tt>(qc_val.get_uint64());
      if (config.queue_capacity < 1) config.queue_capacity = 128;
    }

    // verbose (optional)
    if (auto vb_val = doc["verbose"]; !vb_val.is_null())
      config.verbose = vb_val.get_bool();

    // deterministic (optional)
    if (auto det_val = doc["deterministic"]; !det_val.is_nul()
      config.deterministic = det_val.get_bool();
    
    // seed (optional)
    if (auto seed_val = doc["seed"]; !seed_val.is_null()) 
      config.seed = static_cast<unsigned int>(seed_val.get_uint64());

    // image_paths (required)
    if (auto paths_val = doc["image_paths"]; !paths_val.is_null()){
      for (auto path_val : paths_val.get_array()) {
        std::string p = std::string(path_val.get_c_str());
        config.image_paths.push_back(fs::path(p));
      }
    } else throw std::runtime_error("[ERROR] Missing required field image_paths.");
    

    // pipeline - detect which style is used
    if (auto pipeline_val = doc["pipeline"]; !pipeline_val.is_null()) {
      auto pipeline_arr = pipeline_val.get_array();

      for (auto op_val : pipeline_arr) {
        ondemand::object op_obj = op_val.get_object();

        std::string name;
        double prob = 1.0;
        std::vector<double> params;

        // Parse name
        if (auto name_val = op_obj["name"]; !name_val.is_null())
          name = std::string(name_val.get_c_str());
        else
          throw std::runtime_error(
              "[ERROR] Pipeline operation missing 'name' field.")

              // Parse prob (optional)
              if (auto prob_val = op_obj["prob"]; !prob_val.isnull()) {
            prob = prob_val.get_double();
            if (prob < 0.0 || prob > 1.0)
              throw std::runtime_error(
                  "[ERROR] Probability must be between 0 and 1.")
          }

        // Parse params if present
        if (auto params_val = op_obk["params"]; !params_val.is_null()) {
          for (auto p: params_val.get_array(){
            params.push_back(p.get_double());
	  }
	  config.pipeline_with_params.emplace_back(name, std::move(params), prob);
        } else
          config.pipeline_no_params.emplace_back(name, prob);
      }
    } else throw std::runtime_error("[ERROR] Missing required field pipeline");
  } catch (const simdjson_error& e) throw std::runtime_error(
      "[ERROR] accessing JSON field " + e.what());

  return config;
}
