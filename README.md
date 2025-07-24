#augmento

**augmento** is a high-performance, modular image augmentation toolkit built for flexibility and speed. Designed with a multithreaded pipeline and clean CLI interface, it supports fast, consistent transformations for large datasets in machine learning, computer vision, and synthetic data generation workflows.

---

## 🚀 Features

- 🔁 **Modular Augmentation Pipeline** – Compose complex augmentation sequences easily
- 🧵 **Multithreaded Execution** – Speed up augmentation with producer-consumer threading
- 🔧 **Configurable via CLI** – Run augmentations without writing any code
- ⚡ **Built for Speed** – Engineered for high-throughput augmentation on large datasets

---

## 📦 Installation

> **Note**: Prebuilt binaries and pip install support will be added soon.

For now, clone and build manually:

```bash
git clone https://github.com/yourusername/augmento.git
cd augmento
mkdir build && cd build
cmake ..
make
