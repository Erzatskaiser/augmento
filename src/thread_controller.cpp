/**
 * @file thread_controller.cpp
 * @brief Implementation of the ThreadController class for progress tracking
 * @author Emmanuel Butsana
 * @date July 24, 2024
 */

#include "../include/thread_controller.hpp"

/** ThreadController constructor **/
ThreadController::ThreadController(size_t numThreads, size_t queueCapacity)
    : numThreads_(numThreads), imageQueue_(queueCapacity) {
  if (numThreads_ == 0)
    throw std::invalid_argument(
        "ThreadController: number of threads must be at least 1.");
}

/** ThreadController run function **/
void ThreadController::run(const std::vector<fs::path>& image_paths,
                           int iterations, Pipeline& pipeline,
                           const std::string& output_dir, bool verbose,
                           bool save_specs) {
  if (image_paths.empty()) {
    if (verbose) std::cout << "[WARNING] No image paths provided." << std::endl;
    return;
  }
  if (iterations <= 0)
    throw std::invalid_argument(
        "ThreadController: iterations must be at least 1.");

  totalTasks_ = image_paths.size() * iterations;

  if (verbose) {
    std::cout << "[INFO] Launching " << numThreads_ << " producer threads."
              << std::endl;
    std::cout << "[INFO] Total tasks to process: " << totalTasks_ << std::endl;
  }

  launchProducers(pipeline);
  launchConsumer(output_dir, save_specs);
  for (const auto& path : image_paths) {
    for (int i = 0; i < iterations; ++i) {
      pathQueue_.push(path);
    }
  }
  pathQueue_.setDone();
  waitForCompletion();

  if (verbose) std::cout << "[INFO] Augmentation complete." << std::endl;
}

/** ThreadController launch producers function **/
void ThreadController::launchProducers(Pipeline& pipeline) {
  for (size_t i = 0; i < numThreads_; ++i) {
    producers_.emplace_back(
        [&, i] { producerPool(pathQueue_, imageQueue_, pipeline); });
  }
}

/** ThreadController launch consumer function **/
void ThreadController::launchConsumer(const std::string& output_dir,
                                      bool save_specs) {
  consumer_ = std::thread([&, output_dir, save_specs] {
    consumerThread(imageQueue_, output_dir, save_specs);
  });
}

/** ThreadController wait for completion function **/
void ThreadController::waitForCompletion() {
  for (auto& t : producers_) {
    if (t.joinable()) t.join();
  }

  imageQueue_.setDone();

  if (consumer_.joinable()) consumer_.join();
}
