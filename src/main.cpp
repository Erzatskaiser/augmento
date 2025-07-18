// Emmanuel Butsana, 02 June 2025, main.cpp
// Main file for image manipulation pipeline

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <vector>

#include "extra.hpp"
#include "manipulations.hpp"
#include "multithread.hpp"

// Useful namespaces
namespace fs = std::filesystem;

/* Parses command run by user */
int parseCommand(int argc, char* argv[], std::string& iPath,
                 std::string& oPath) {
  iPath.clear();
  oPath.clear();
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "-i" && i + 1 < argc) {
      iPath = argv[i + 1];
    }

    if (arg == "-o" && i + 1 < argc) {
      oPath = argv[i + 1];
    }
  }
  if (iPath.empty() || oPath.empty()) {
    std::cerr << "Could not extract path" << std::endl;
    return -1;
  }
  return 0;
}

/* Split files in directory among workers */
std::vector<std::vector<fs::path>> splitFilesIntoPiles(const std::string& dir,
                                                       size_t workers = 6) {
  // Collect all files, shuffle in directory
  std::vector<fs::path> allFiles;
  for (const auto& file : fs::directory_iterator(dir)) {
    if (fs::is_regular_file(file.path())) allFiles.push_back(file.path());
  }
  std::sort(allFiles.begin(), allFiles.end());

  // Prepare to split files between workers
  std::vector<std::vector<fs::path>> piles(workers);
  size_t total = allFiles.size();
  size_t baseSize = total / workers;
  size_t remainder = total % workers;

  // Split files between workers
  size_t index = 0;
  for (size_t i = 0; i < workers; ++i) {
    size_t pileSize = baseSize + (i < remainder ? 1 : 0);
    for (size_t j = 0; j < pileSize; ++j) {
      piles[i].push_back(allFiles[index++]);
    }
  }
  return piles;
}

/* Split files in directory among workers, in pairs */
std::vector<std::vector<std::pair<fs::path, fs::path>>> splitPairsIntoPiles(
    const std::string& dir, size_t workers = 6) {
  std::unordered_map<std::string, std::pair<fs::path, fs::path>> paired;
  namespace fs = std::filesystem;

  // Scan files and group by ID and VIEW
  for (const auto& file : fs::directory_iterator(dir)) {
    if (!fs::is_regular_file(file.path())) continue;

    std::string filename = file.path().filename().string();
    size_t pos = filename.find("FRONT");
    if (pos == std::string::npos) pos = filename.find("SIDE");
    if (pos == std::string::npos) continue;

    std::string id = filename.substr(0, pos - 1);
    std::string view = filename.substr(pos);

    if (view.find("FRONT") != std::string::npos)
      paired[id].first = file.path();
    else if (view.find("SIDE") != std::string::npos)
      paired[id].second = file.path();
  }

  // Collect only fully matched pairs
  std::vector<std::pair<fs::path, fs::path>> fullPairs;
  for (const auto& [id, p] : paired) {
    if (!p.first.empty() && !p.second.empty()) {
      fullPairs.emplace_back(p.first, p.second);
    }
  }

  // Shuffle and split
  std::sort(fullPairs.begin(),
            fullPairs.end());  // Optional: deterministic order
  std::vector<std::vector<std::pair<fs::path, fs::path>>> piles(workers);
  size_t index = 0;
  for (const auto& pair : fullPairs) {
    piles[index % workers].push_back(pair);
    ++index;
  }

  return piles;
}

int main(int argc, char* argv[]) {
  // Attempt to read folder path with -i flag and outpt with -o
  std::string iPath;
  std::string oPath;
  if (parseCommand(argc, argv, iPath, oPath) == -1) return -1;
  if (!fs::exists(iPath)) {
    std::cerr << "The directory does not exist" << std::endl;
    return -1;
  }
  if (!fs::is_directory(iPath)) {
    std::cerr << "Please enter a directory" << std::endl;
    return -1;
  }
  std::cout << "Processed inputs..." << std::endl;

  // Initialize and configure pipeline
  std::unordered_map<std::string, double> probs = {
      {"rotate", 0},          {"reflect", 0.6},    {"hist_eq", 0.6},
      {"white_balance", 0.6}, {"brightness", 0.6}, {"contrast", 0.6},
      {"saturation", 0.6},    {"hue", 0.6},        {"noise", 0.9},
      {"blur", 0.6},          {"sharpen", 0.4}};
  Pipeline pipeline = configurePipeline(probs, 1945);
  std::cout << "Initialized pipeline..." << std::endl;

  // Split the files between 6 worker threads
  // auto piles = splitFilesIntoPiles(iPath, 6);
  auto paired_piles = splitPairsIntoPiles(iPath, 6);
  std::cout << "Split files between workers, starting..." << std::endl;

  // Initialize queue and threads
  SafeQueue<Image> queue;
  std::vector<std::thread> producers;
  for (int i = 0; i < 6; ++i) {
    producers.emplace_back(pairedProducerThread, std::ref(paired_piles[i]),
                           std::ref(queue), std::ref(pipeline), 5);
  }
  std::thread consumer(consumerThread, std::ref(queue), oPath);

  // Join all producers
  for (auto& t : producers) t.join();

  // Signal consumer to terminate
  queue.setDone();
  consumer.join();

  // Sucess message
  std::cout << "Sucessfully augmented dataset" << std::endl;

  return 0;
}
