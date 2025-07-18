// Emmanuel Butsana, 04 June 2025, multithread.cpp
// Implements producer and consumer functions for the process

#include "multithread.hpp"

/* Producer thread function, individual image */
void producerThread(const std::vector<fs::path>& files, SafeQueue<Image>& queue,
                    Pipeline& pipeline, int iter) {
  for (const auto& path : files) {
    for (int i = 0; i < iter; ++i) {
      Image img(path);
      pipeline.apply(img);
      queue.push(std::move(img));
    }
  }
}

/* Producer thread function, paired augmentation */
void pairedProducerThread(const std::vector<std::pair<fs::path, fs::path>>& pairs,
                          SafeQueue<Image>& queue, Pipeline& pipeline,
                          int iter) {

  // Apply the same augmentation to images in pairs
  for (const auto& [front, side] : pairs) {
    if (front.empty() || side.empty()) continue;
    std::string id = front.stem().string().substr(0,front.stem().string().find("_"));

    // Create as many augmentations as are required
    for (int i = 0; i < iter; ++i) {
      // Load images, set image seed
      unsigned int shared_seed = static_cast<unsigned int>(
          std::hash<std::string>{}(id + std::to_string(i)));
      Image img1(front);
      Image img2(side);

      // Apply pipeline, set image name
      pipeline.apply(img1, shared_seed);
      pipeline.apply(img2, shared_seed);
      img1.setName(id + "_aug" + std::to_string(i) + "_FRONT");
      img2.setName(id + "_aug" + std::to_string(i) + "_SIDE");

      // Push to queue
      queue.push(std::move(img1));
      queue.push(std::move(img2));
    }
  }
}

/* Consumer thread function */
void consumerThread(SafeQueue<Image>& queue, const std::string& outputDir) {
  Image img;
  while (queue.pop(img)) {
    img.save(outputDir);
  }
}
