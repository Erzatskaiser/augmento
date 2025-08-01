/**
 * @name benchmark.cpp
 * @brief Benchmark main file, to allow for performance monitoring
 * @author Emmanuel Butsana
 * @date July 31, 2025
 */

#include <chrono>

#include "../include/session_manager.hpp"

int main(int argc, char* argv[]) {
  try {
    auto start = std::chrono::high_resolution_clock::now();
    SessionManager session(argc, argv);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "[TIMING] Total: " << duration_ms << std::endl;
    return 0;
  } catch (const std::exception& e) {
    std::cout << "[ERROR] " << e.what() << std::endl;
    return -1;
  } 
}
