<p align="center">
  <img src="./assets/augmento.png" alt="augmento logo" width="400"/>
</p>

**augmento** is a high-performance, modular image augmentation toolkit built for flexibility and speed. Designed with a multithreaded pipeline and clean CLI interface, it supports fast, consistent transformations for large datasets in machine learning, computer vision, and synthetic data generation workflows.

## 🚀 Features

- **Modular Augmentation Pipeline** – Compose complex augmentation sequences easily
- **Multithreaded Execution** – Speed up augmentation with producer-consumer threading
- **Configurable via CLI** – Run augmentations without writing any code
- **Built for Speed** – Engineered for high-throughput augmentation on large datasets

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
  "verbose": "verbosity of program (bool, default true)",
  "pipeline_specs": [
    {
      "operation": "operation name",
      "param": "operation parameter"
    }
  ]
}
```
Below are the operations you can define so far in the `pipeline_specs` array of your JSON config:

| Operation Name         | Description                                          | Parameters                                                                    |
|------------------------|------------------------------------------------------|-------------------------------------------------------------------------------|
| `rotate`               | Rotates image randomly                               | `min_angle`, `max_angle`, `rot_type` (0 = no crop, 1 = crop, 2 = fill)        |
| `reflect`              | Flips image vertically or horizontally               | *(none)*                                                                      |
| `resize`               | Scales or resizes image                              | EITHER: `min_scale`, `max_scale` OR `min_w`, `max_w`, `min_h`, `max_h`        |
| `crop`                 | Crops a region (random or fixed)                     | EITHER: `width`, `height` OR `x`, `y`, `width`, `height`                      |
| `affine transform`     | Applies affine transform                             | *(none)* OR 6 matrix values (row-major 2×3)                                   |
| `color jitter`         | Jitters brightness, contrast, saturation, hue        | `brightness_range`, `contrast_range`, `saturation_range`, `hue_range`         |
| `adjust brightness`    | Adjusts brightness                                   | `min_val`, `max_val`                                                          |
| `adjust contrast`      | Adjusts contrast                                     | `min_val`, `max_val`                                                          |
| `adjust saturation`    | Adjusts saturation                                   | `min_val`, `max_val`                                                          |
| `adjust hue`           | Adjusts hue                                          | `min_val`, `max_val`                                                          |
| `inject noise`         | Adds Gaussian noise                                  | *(none)* OR `mean_min`, `mean_max`, `stdev_min`, `stdev_max`                  |
| `blur image`           | Applies box blur                                     | *(none)* OR `min_k`, `max_k` (odd integers recommended)                       |
| `sharpen image`        | Sharpens image using Laplacian                       | *(none)*                                                                      |
| `histogram equalization` | Equalizes image histogram                          | *(none)*                                                                      |
| `white balance`        | Applies white balance correction                     | *(none)*                                                                      |
| `to grayscale`         | Converts image to grayscale                          | *(none)*                                                                      |
| `random erase`         | Randomly erases rectangular regions                  | *(none)* OR `min_h`, `max_h`, `min_w`, `max_w`                                |

When parameters are not provided, the pipeline will randomly select values within reasonable defaults to ensure variability and robustness of augmentation. Refer to the source code or documentation for specific default ranges used by each operation.

---

## 📚 Developer Documentation

This project uses [Doxygen](https://www.doxygen.nl/) to generate API documentation from inline comments. To generate and view the docs locally, install doxygen and run

```bash
doxygen Doxyfile
```

The newly created documentation can be accessed under `doc/html/annotated.html`.
