/**
 * @file main.cpp
 * @brief Entry point for the augmento image augmentation toolkit.
 * @author Emmanuel Butsana
 * @date July 2025
 */

#include <iostream>

#include "../include/session_manager.hpp"

int main(int argc, char* argv[]) {
  try {
    SessionManager session(argc, argv);
    return session.execute();
  } catch (const std::exception& e) {
    std::cout << "[FATAL] " << e.what() << std::endl;
    return -1;
  }
}
