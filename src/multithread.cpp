/**
 * @file multithread.cpp
 * @brief Implements multithreaded producer-consumer image augmentation using a
 * task-pool model.
 * @author Emmanuel Butsana
 * @date Refactored: July 23, 2025
 *
 * This file defines the core logic for parallel image augmentation using
 * producer threads that apply transformations from a shared Pipeline and a
 * consumer thread that writes processed images to disk. Work is distributed
 * using thread-safe queues (SafeQueue<T>), and supports both single-image and
 * paired-image augmentation.
 */

#include "../include/multithread.hpp"

/** Producer pool **/
void producerPool(SafeQueue<fs::path>& pathQueue, SafeQueue<Image>& outputQueue,
                  Pipeline& pipeline, std::atomic<size_t>& processedCount) {
  fs::path path;
  while (pathQueue.pop(path)) {
    try {
      Image img(path);
      pipeline.apply(img);
      outputQueue.push(std::move(img));
      ++processedCount;
    } catch (const std::exception& e) {
      std::cerr << "[WARN] Failed to process " << path << ": " << e.what()
                << "\n";
    }
  }
}

/** Consumer pool */
void consumerThread(SafeQueue<Image>& queue, const std::string& outputDir,
                    std::atomic<size_t>& processedCount) {
  Image img;
  while (queue.pop(img)) {
    try {
      img.save(outputDir);

      if (processedCount % 20 == 0) {
        std::cout << "[INFO] Saved " << processedCount << " images...\n";
      }
    } catch (const std::exception& e) {
      std::cerr << "[ERROR] Failed to save image: " << e.what() << "\n";
    }
  }
}
