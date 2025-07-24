/**
 * @file thread_controller.hpp
 * @brief Thread pool manager for image augmentation with progress tracking.
 * @author Emmanuel Butsana
 * @date July 24, 2025
 *
 * Manages multiple producer threads and a single consumer thread using
 * thread-safe queues to perform parallel image augmentation.
 * Handles task distribution, queueing, and lifecycle management.
 */

#include <atom>
#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "multithread.hpp"
#include "pipeline.hpp"

namespace fs = std::filesystem;

/**
 * @class ThreadController
 * @brief Manages a thread pool for parallel image augmentation tasks.
 *
 * Launches multiple producer threads to apply augmentations on images
 * and a consumer thread to save augmented images to disk.
 * Supports configurable number of threads and queue capacities.
 */
class ThreadController {
 public:
  /**
   * @brief Construct a new AugmentationController.
   * @param numThreads Number of producer threads to spawn. Defaults to hardware
   * concurrency.
   * @param queueCapacity Maximum capacity of internal queues to apply
   * backpressure.
   */
  explicit ThreadController(
      size_t numThreads = std::thread::hardware_concurrency(),
      size_t queueCapacity = 128);

  /**
   * @brief Runs the augmentation pipeline on a set of images.
   * @param image_paths Vector of image file paths to augment.
   * @param pipeline The augmentation pipeline to apply to each image.
   * @param output_dir Directory to save augmented images.
   * @param iterations Number of augmentations to perform per image.
   */
  void run(const std::vector<fs::path>& image_paths, Pipeline& pipeline,
           const std::string& output_dir, int iterations, bool verbose = false);

 private:
  /**
   * @brief Launches producer threads that pull tasks from pathQueue_ and push
   * augmented images.
   * @param image_paths Input image paths.
   * @param iterations Number of augmentations per image.
   * @param pipeline Augmentation pipeline.
   */
  void launchProducers(const std::vector<fs::path>& image_paths, int iterations,
                       Pipeline& pipeline);

  /**
   * @brief Launches consumer thread that saves augmented images from
   * imageQueue_ to disk.
   * @param output_dir Directory to save images.
   */
  void launchConsumer(const std::string& output_dir);

  /**
   * @brief Waits for all producer threads to finish and signals consumer to
   * stop.
   */
  void waitForCompletion();

  size_t numThreads_;  ///< Number of producer threads.
  SafeQueue<fs::path>
      pathQueue_;  ///< Queue holding image paths to be processed.
  SafeQueue<Image>
      imageQueue_;  ///< Queue holding augmented images ready to save.
  std::vector<std::thread> producers_;     ///< Vector of producer threads.
  std::thread consumer_;                   ///< Consumer thread.
  std::atomic<size_t> totalTasks_{0};      ///< Total tasks available
  std::atomic<size_t> processedCount_{0};  ///< Count of tasks processed
};
