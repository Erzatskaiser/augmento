/**
 * @name session_manager.cpp
 * @brief Implementation of the SessionManager class defined in
 * session_manager.hpp.
 * @author Emmanuel Butsana
 * @data Initial release: June 3, 2025
 */

#include "../include/session_manager.hpp"

/* Class constructor for SessionManager */
SessionManager::SessionManager(const int argc, const char* argv[])
    : argc_(argc), argv_(argv) {}

/* Execution function for Session Manager */
int SessionManager::execute() {
  parseArguments();
  loadConfiguration();
  preparePipeline();
  std::cout << "[INFO] Completed pipeline configuration, starting...\n";
  auto start = std::chrono::high_resolution_clock::now();
  launchThreads();
  auto end = std::chrono::high_resolution_clock::now();
  auto duration_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
          .count();
  std::cout << "[INFO] Completed augmentation process in " << duration_ms
            << ".\n";
  return 0;
}

/* Parse function for Session Manager */
void SessionManager::parseArguments() {
  bool config_provided = false;

  for (int i = 1; i < argc_; ++i) {
    std::string arg = argv_[i];

    if ((arg == "--config" || arg == "-c") && i+1 < argc_) {
      config_path_ = argv_[++i];
      config_provided = true;
    }

    else if (arg == "--tui") {
      std::cout << "[INFO] TUI mode not yet implemented.\n";
    }
    else if (arg == "--dry-run") {
      config_.verbose = true;
      config_.iterations = 0;
      std::cout << "[INFO] Dry-run mode enabled.\n";
    }
    else {
      throw std::invalid_argument("[ERROR] Unrecognized flag " + arg + ".");
    }

    if (!config_provided) {
      throw std::runtime_error("[ERROR] No --config <path> provided.);
    }
  }
  std::cout << "[INFO] Parsed arguments, loaded configuration...\n";
}
