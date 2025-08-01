/**
 * @name session_manager.cpp
 * @brief Implementation of the SessionManager class defined in
 * session_manager.hpp.
 * @author Emmanuel Butsana
 * @data Initial release: June 3, 2025
 */

#include "../include/session_manager.hpp"

/* Class constructor for SessionManager */
SessionManager::SessionManager(int argc, char* argv[])
    : argc_(argc), argv_(argv) {}

/* Execution function for Session Manager */
int SessionManager::execute() {
  parseArguments();
  loadConfiguration();
  loadImages();
  preparePipeline();
  std::cout << "[INFO] Completed pipeline configuration, starting...\n";
  auto start = std::chrono::high_resolution_clock::now();
  launchThreads();
  auto end = std::chrono::high_resolution_clock::now();
  auto duration_ms =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start)
          .count();
  std::cout << "[TIMING] Completed augmentation process in " << duration_ms
            << " us.\n";
  return 0;
}

/* Parse function for Session Manager */
void SessionManager::parseArguments() {
  bool config_provided = false;

  for (int i = 1; i < argc_; ++i) {
    std::string arg = argv_[i];

    if ((arg == "--config" || arg == "-c") && i + 1 < argc_) {
      config_path_ = argv_[++i];
      config_provided = true;
    } else if (arg == "--tui") {
      std::cout << "[INFO] TUI mode not yet implemented.\n";
    } else if (arg == "--dry-run") {
      dry_run_ = true;
      std::cout << "[INFO] Dry-run mode enabled.\n";
    } else if ((arg == "--help") || (arg == "-h")) {
      std::cout << R"(
Usage: augmento [OPTIONS]

Required:
  --config <path>       Path to JSON configuration file

Optional:
  --tui                 Launch TUI mode (not yet implemented)
  --dry-run             Perform a dry run without writing any files
  --help, -h            Show this help message and exit
)";
      std::exit(0);
    } else {
      throw std::invalid_argument("[ERROR] Unrecognized flag " + arg + ".");
    }

    if (!config_provided) {
      throw std::runtime_error("[ERROR] No --config <path> provided.");
    }
  }
  std::cout << "[INFO] Parsed arguments, loaded configuration...\n";
}

/* Reads in images from input folder */
void SessionManager::loadImages() {
  image_paths_.clear();
  for (const auto& entry : fs::directory_iterator(config_.input_dir)) {
    if (entry.is_regular_file()) {
      image_paths_.push_back(entry.path());
    }
  }
}

/* Read user configuration */
void SessionManager::loadConfiguration() {
  if (config_.iterations = 0) {
    config_ = parseConfigFile(config_path_);
    config_.iterations = 0;
  }
  config_ = parseConfigFile(config_path_);
}

/* Prepare pipeline based on config */
void SessionManager::preparePipeline() {
  pipeline_ = configurePipeline(config_.pipeline_specs, config_.seed);
}

/* Executes augmentation over specified number of threads */
void SessionManager::launchThreads() {
  if (dry_run_) {
    std::cout << "[INFO] Successfully completed dry run.\n";
    std::exit(0);
  }
  ThreadController thread_controller(config_.num_threads,
                                     config_.queue_capacity);
  thread_controller.run(image_paths_, config_.iterations, pipeline_,
                        config_.output_dir, config_.verbose);
}
