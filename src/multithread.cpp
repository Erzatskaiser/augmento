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
                  Pipeline& pipeline) {
  fs::path path;
  while (pathQueue.pop(path)) {
    try {
      Image img(path);
      pipeline.apply(img);
      outputQueue.push(std::move(img));
    } catch (const std::exception& e) {
      std::cerr << "[WARN] Failed to process " << path << ": " << e.what()
                << std::endl;
    }
  }
}

/** Consumer pool */
void consumerThread(SafeQueue<Image>& queue, const std::string& outputDir, bool save_specs) {
  Image img;
  size_t batchSize = 12;
  size_t localSaveCount = 0;
  std::vector<Image> image_batch;

  while (queue.pop(img)) {
    try {
      image_batch.push_back(std::move(img));
      if (image_batch.size() >= batchSize) {
        for (auto& image : image_batch) {
	  if (save_specs) image.saveWithHistory(outputDir);
	  else image.save(outputDir);
          ++localSaveCount;
        }
        image_batch.clear();
      }

      if (localSaveCount % 7 == 0 && localSaveCount != 0) {
        std::cout << "[INFO] Saved " << localSaveCount << " images..."
                  << std::endl;
      }
    } catch (const std::exception& e) {
      std::cerr << "[ERROR] Failed to save image: " << e.what() << std::endl;
    }
  }

  if (!image_batch.empty()) {
    for (auto& image : image_batch) {
      image.save(outputDir);
    }
    image_batch.clear();
  }
}
