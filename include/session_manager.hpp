/**
 * @file session_manager.hpp
 * @brief Orchestrates config, parsing, pipeline construction, and parallelization
 * @author Emanuel Butsana
 * @Data Initial release: July 25, 2025
 *
 * Defines the SessionManager class, which serves as the main entry point for managing
 * a complete augmentation session. It supports JSON-based configuration, oversees
 * pipeline instantiation, thread management, and ensures orderly execution flow.
 */

#pragma once

#include <chrono>
#include <iostream>

#include "json.hpp"
#include "pipeline.hpp"
#include "thread_controller.hpp"

namespace fs = std::filesystem;

class SessionManager {
  public: 
    /**
     * @brief Constructs the session manager with command-line arguments.
     * @param argc Argument count from main.
     * @param argv Argument vector from main.
     */
    explicit SessionManager(const int argc, const char* argv[]);

    /**
     * @brief Executes the full augmentation session.
     * @return Exit code (0 on sucess, non-zero on failure).
     */
    int execute();

    private:
      /**
       * @brief Parses any CLI arguments
       *
       * Recognixed flags:
       * - --config <path> or -c <path>: JSON configuration path
       * - --tui: Use TUI mode instead of config file
       * - --dry-run: Perform setup but skip augmentation execution
       * Unrecognized arguments throw an error.
       */
      void parseArguments();

      /**
       * @brief Loads image paths from input directory
       */
      void loadImages();

      /**
       * @brief Loads and validates configuration from JSON file.
       */
      void loadConfiguration();

      /**
       * @brief Prepares the pipeline form parsed configuration data.
       */
      void preparePipeline();

      /**
       * @brief Launches multithreaded producer-consumer system.
       */
      void launchThreads();

      std::string config_path_;               ///< Path to the JSON configuration file.
      ConfigSpec config_;	              ///< Parsed configuration values.
      Pipeline pipeline_;	              ///< Configured augmentation pipeline.
      ThreadController thread_controller_;    ///< Thread pool controller.
      std::vector<fs::path> image_paths;      ///< Input image paths
      int argc_;                              ///< Argument count from main().
      char* argv_[];     		      ///< Argument vector from main().
}
