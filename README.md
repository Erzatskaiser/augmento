## augmento

**augmento** is a high-performance, modular image augmentation toolkit built for flexibility and speed. Designed with a multithreaded pipeline and clean CLI interface, it supports fast, consistent transformations for large datasets in machine learning, computer vision, and synthetic data generation workflows.

---

## 🚀 Features

- **Modular Augmentation Pipeline** – Compose complex augmentation sequences easily
- **Multithreaded Execution** – Speed up augmentation with producer-consumer threading
- **Configurable via CLI** – Run augmentations without writing any code
- **Built for Speed** – Engineered for high-throughput augmentation on large datasets

---

## 📦 Installation

Before building, make sure the following libraries are installed on your system:

- OpenCV (version 4 or higher)
- simdjson
- CMake (version 3.10 or higher)
- A C++17-capable compiler

Upon installation of these dependencies, **augmento** can be built as follows:

```bash
git clone https://github.com/Erzatskaiser/augmento.git
cd augmento
mkdir build && cd build
cmake ..
make
```

---

## 🛠️ Usage

After building, you can rum augmento from the command line:

```bash
./augmento --config path/to/config.json [OPTIONS]
```

with additional optional flags:

```bash
--drun-run     # Perform a dry run without writing files
--tui          # Launch TUI mode (not yet implemented)
--help, -h     # Display help information and exit
```

---

## 🧾 Configuration file

**augmento** requires a JSON config file that defines the augmentation pipeline and parameters. Example configs are included under **example/**. Here's an overview of the required fields:

```json
{
  "input_dir": "images/input",
  "output_dir": "images/output",
  "num_threads": "number of threads (int)",
  "iterations": "number of iterations per image (uint, default 1)",
  "queue_capacity": "size of internal buffer (uint, default 128)",
  "seed": "seed for internal random number generator (uint)",
  "verbose": "verbosity of programe (bool, default true)",
  "pipeline_specs": [
    {
      "operation": "operation name",
      "param": operation parameter
    }
  ]
}
```
The implemented operations are the following:

| Operation              | Description                                          | Key Parameters                                                                 |
|------------------------|------------------------------------------------------|--------------------------------------------------------------------------------|
| `RotateImage`          | Rotates image randomly                              | `min_angle`, `max_angle`, `rot_type` (0 = no crop, 1 = crop, 2 = fill)        |
| `ReflectImage`         | Flips image vertically or horizontally              | *(none)*                                                                      |
| `ResizeImage`          | Scales image or resizes to random dimensions        | EITHER: `min_scale`, `max_scale` OR `min_w`, `max_w`, `min_h`, `max_h`        |
| `CropImage`            | Crops a fixed or random region                      | `width`, `height` [optional: `x`, `y`]                                         |
| `AffineTransform`      | Applies affine transform (random or fixed matrix)   | *(none)* – uses internally generated 2×3 matrix                               |
| `ColorJitter`          | Adjusts brightness, contrast, saturation, hue       | `brightness_range`, `contrast_range`, `saturation_range`, `hue_range`         |
| `AdjustBrightness`     | Adjusts brightness                                   | `min_val`, `max_val`                                                          |
| `AdjustContrast`       | Adjusts contrast                                     | `min_val`, `max_val`                                                          |
| `AdjustSaturation`     | Adjusts saturation                                   | `min_val`, `max_val`                                                          |
| `AdjustHue`            | Adjusts hue                                          | `min_val`, `max_val`                                                          |
| `InjectNoise`          | Adds Gaussian noise                                  | `mean_min`, `mean_max`, `stdev_min`, `stdev_max`                              |
| `BlurImage`            | Applies blur with square averaging kernel            | `min_k`, `max_k` (kernel size, odd integers)                                  |
| `SharpenImage`         | Sharpens image using Laplacian                       | *(none)*                                                                      |
| `HistogramEqualization`| Improves contrast via histogram equalization         | *(none)*                                                                      |
| `WhiteBalance`         | Performs simple white balance correction             | *(none)*                                                                      |
| `ToGrayscale`          | Converts image to grayscale                          | *(none)*                                                                      |
| `RandomErase`          | Randomly masks rectangular patches                   | `min_h`, `max_h`, `min_w`, `max_w`  
