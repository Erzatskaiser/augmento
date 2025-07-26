/**
 * @file multithread.hpp
 * @brief Thread-safe queue and multithreading utilities for augmento.
 * @author Emmanuel Butsana
 * @date Refactored: July 23, 2025
 *
 * Provides SafeQueue<T> for communication, and thread entry points for
 * producer-consumer parallelism using a shared task-pool model.
 */

#pragma once

#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <queue>
#include <utility>
#include <vector>

#include "image.hpp"
#include "pipeline.hpp"

namespace fs = std::filesystem;

/**
 * @brief Thread-safe bounded queue for producer-consumer workflows.
 * @tparam T Type of elements stored in the queue.
 */
template <typename T>
class SafeQueue {
 public:
  explicit SafeQueue(size_t max_size = 128) : max_size_(max_size) {}

  void push(T item) {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_not_full_.wait(lock,
                      [&]() { return queue_.size() < max_size_ || done_; });
    if (done_) return;
    queue_.push(std::move(item));
    cv_not_empty_.notify_one();
  }

  bool pop(T& item) {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_not_empty_.wait(lock, [&]() { return !queue_.empty() || done_; });
    if (!queue_.empty()) {
      item = std::move(queue_.front());
      queue_.pop();
      cv_not_full_.notify_one();
      return true;
    }
    return false;
  }

  void setDone() {
    std::lock_guard<std::mutex> lock(mtx_);
    done_ = true;
    cv_not_empty_.notify_all();
    cv_not_full_.notify_all();
  }

 private:
  std::queue<T> queue_;
  std::mutex mtx_;
  std::condition_variable cv_not_empty_;
  std::condition_variable cv_not_full_;
  size_t max_size_;
  bool done_ = false;
};

/**
 * @brief Generic image producer using a shared path queue (task-pool model).
 */
void producerPool(SafeQueue<fs::path>& pathQueue, SafeQueue<Image>& outputQueue,
                  Pipeline& pipeline, std::atomic<size_t>& processedCount);

/**
 * @brief Consumer thread that saves augmented images and updates progress.
 */
void consumerThread(SafeQueue<Image>& queue, const std::string& output_dir,
                    std::atomic<size_t>& processedCount);
