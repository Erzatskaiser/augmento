/**
 * @file multithread.hpp
 * @brief Thread-safe queue and multithreading utilities for augmento.
 * @author Emmanuel Butsana
 * @date Initial release: June 4, 2025
 *
 * Provides a SafeQueue<T> class and producer/consumer thread entry points
 * for parallel image augmentation using a thread-safe work queue.
 */

#pragma once

#include <condition_variable>
#include <filesystem>
#include <mutex>
#include <queue>
#include <vector>

#include "image.hpp"
#include "pipeline.hpp"

namespace fs = std::filesystem;

/**
 * @brief Thread-safe queue for producer-consumer workflows.
 * @tparam T Type of elements stored in the queue.
 *
 * Supports safe concurrent push and blocking pop. Use setDone() to
 * signal shutdown and unblock consumers.
 */
template <typename T>
class SafeQueue {
 public:
  /**
   * @brief Push an item into the queue.
   * @param item Item to enqueue.
   */
  void push(T item) {
    std::lock_guard<std::mutex> lock(mtx_);
    queue_.push(std::move(item));
    cv_.notify_one();
  }

  /**
   * @brief Pop an item from the queue. Blocks until item is available or queue is closed.
   * @param item Reference to store the dequeued item.
   * @return true if an item was dequeued, false if the queue is closed and empty.
   */
  bool pop(T& item) {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [&]() { return !queue_.empty() || done_; });
    if (!queue_.empty()) {
      item = std::move(queue_.front());
      queue_.pop();
      return true;
    }
    return false;
  }

  /**
   * @brief Signal that no more items will be pushed.
   */
  void setDone() {
    std::lock_guard<std::mutex> lock(mtx_);
    done_ = true;
    cv_.notify_all();
  }

 private:
  std::queue<T> queue_;              ///< Underlying STL queue
  std::mutex mtx_;                   ///< Mutex for thread safety
  std::condition_variable cv_;       ///< Condition variable for blocking pop
  bool done_ = false;                ///< Flag to signal shutdown
};

/**
 * @brief Producer thread function that loads and pushes images to the queue.
 * @param image_paths List of image file paths to process.
 * @param queue Shared thread-safe queue to receive processed images.
 * @param pipeline Augmentation pipeline to apply to each image.
 * @param thread_id Unique thread identifier (used for logging/debugging).
 */
void producerThread(const std::vector<fs::path>& image_paths,
                    SafeQueue<Image>& queue,
                    Pipeline& pipeline,
                    int thread_id);

/**
 * @brief Consumer thread function that saves processed images from the queue.
 * @param queue Shared thread-safe queue to consume images from.
 * @param output_dir Output directory to save augmented images.
 */
void consumerThread(SafeQueue<Image>& queue, const std::string& output_dir);

