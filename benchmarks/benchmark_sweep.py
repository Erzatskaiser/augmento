##
# @file benchmark_sweep_plot_device_progress.py
# @brief Run benchmark sweeps with progress bar and plot results with device info
# @author Emmanuel Butsana
# @date August 14, 2025
#
# Runs a multithreaded benchmark across configurations and plots timing results by iteration,
# including device info in the plot title. Shows a progress bar during sweep.
#

import os
import multiprocessing
import subprocess
import json
import re
import shutil
import csv
from collections import defaultdict
import matplotlib.pyplot as plt
import platform
import psutil
import albumentations as A
import cv2
import random
import time
import numpy as np
from tqdm import tqdm  # Progress bar

CSV_FILE = "./results/benchmark_results.csv"
CONFIG_FILE = "./test/config_template.json"
BENCHMARK_EXE = "./build/benchmark"


##
# @brief Builds Albumentations pipeline for comparison
# @param config JSON config dict
# @return pipeline Albumentations pipeline object
def build_albumentations_pipeline(config):
    aug_list = []
    for step in config["pipeline"]:
        name = step["name"].lower()
        prob = step.get("prob", 1.0)
        params = step.get("params", [])

        if name == "rotate":
            limit = tuple(params[:2]) if len(params) >= 2 else (-30, 30)
            aug_list.append(A.Rotate(limit=limit, p=prob))

        elif name == "reflect":
            aug_list.append(
                A.OneOf([A.HorizontalFlip(p=1),
                         A.VerticalFlip(p=1)], p=prob))

        elif name == "resize":
            scale = tuple(params[:2]) if len(params) >= 2 else (0.8, 1.0)
            aug_list.append(
                A.RandomResizedCrop(size=(512, 512), scale=scale, p=prob))

        elif name == "color jitter":
            # Albumentations doesn't have ColorJitter → emulate
            brightness = params[0] if len(params) > 0 else 0.2
            contrast = params[1] if len(params) > 1 else 0.2
            saturation = params[2] if len(params) > 2 else 0.2
            hue = params[3] if len(params) > 3 else 0.2
            aug_list.append(
                A.Compose([
                    A.RandomBrightnessContrast(brightness_limit=brightness,
                                               contrast_limit=contrast,
                                               p=prob),
                    A.HueSaturationValue(hue_shift_limit=int(hue * 50),
                                         sat_shift_limit=int(saturation * 50),
                                         val_shift_limit=0,
                                         p=prob)
                ]))

        elif name == "inject noise":
            var_low = params[0] if len(params) > 0 else 10.0
            var_high = params[1] if len(params) > 1 else 50.0
            aug_list.append(A.GaussNoise(var_limit=(var_low, var_high), p=prob))

        elif name == "blur image":
            blur_low = params[0] if len(params) > 0 else 3
            blur_high = params[1] if len(params) > 1 else 7
            aug_list.append(A.Blur(blur_limit=(blur_low, blur_high), p=prob))

        elif name == "sharpen image":
            aug_list.append(A.Sharpen(p=prob))

        elif name == "histogram equalization":
            aug_list.append(A.Equalize(p=prob))

        elif name == "white balance":
            # No ColorTemperature in Albumentations → approximate
            aug_list.append(
                A.HueSaturationValue(hue_shift_limit=20,
                                     sat_shift_limit=30,
                                     val_shift_limit=10,
                                     p=prob))

    return A.Compose(aug_list)


##
# @brief Runs Albumentations pipeline
def run_albumentations_pipeline(config_path):
    with open(config_path, "r") as f:
        config = json.load(f)

    pipeline = build_albumentations_pipeline(config)
    input_dir = config["input_dir"]
    output_dir = config["output_dir"]
    os.makedirs(output_dir, exist_ok=True)

    t_start_total = time.perf_counter_ns()
    for i in range(config["iterations"]):
        for img_name in os.listdir(input_dir):
            img_path = os.path.join(input_dir, img_name)
            image = cv2.imread(img_path)
            if image is None:
                continue
            pipeline(image=image)
    t_end_total = time.perf_counter_ns()

    total_time = (t_end_total - t_start_total) / 1000
    return total_time


##
# @brief Builds benchmarking application
def build_scripts() -> None:
    if os.path.exists(BENCHMARK_EXE):
        return
    else:
        if platform.system() in ("Darwin", "Linux"):
            try:
                os.makedirs("build", exist_ok=True)
                subprocess.run(["cmake", ".."], cwd="./build", check=True)
                subprocess.run(["make"], cwd="./build", check=True)
            except subprocess.CalledProcessError as e:
                raise RuntimeError(
                    f"Error while building executable : {e.stderr}")
        else:
            return


##
# @brief Returns processor details
def get_processor_name():
    if platform.system() == "Windows":
        return platform.processor()
    elif platform.system() == "Darwin":
        os.environ['PATH'] += os.pathsep + '/usr/sbin'
        command = "sysctl -n machdep.cpu.brand_string"
        return subprocess.check_output(command, shell=True).decode().strip()
    elif platform.system() == "Linux":
        with open("/proc/cpuinfo") as f:
            for line in f:
                if "model name" in line:
                    return re.sub(".*model name.*:", "", line, count=1).strip()
    return ""


##
# @brief Run sweep benchmarks with a progress bar and save results to CSV
def run_sweep() -> None:
    try:
        with open(CSV_FILE, mode="w", newline="") as csvfile:
            writer = csv.writer(csvfile)
            writer.writerow([
                "framework", "iteration", "num_threads", "pipeline time",
                "total time"
            ])

            with open(CONFIG_FILE, "r") as f:
                config = json.load(f)

            max_threads = multiprocessing.cpu_count() or 8

            # Albumentations sweep → 15 runs total
            with tqdm(total=15,
                      desc="Running augmentations with Albumentations") as pbar:
                for iteration in range(1, 16):
                    config["iterations"] = iteration
                    with open(CONFIG_FILE, "w") as f:
                        json.dump(config, f, indent=2)

                    total_time = run_albumentations_pipeline(CONFIG_FILE)
                    writer.writerow(
                        ["albumentations", iteration, None, None, total_time])
                    pbar.update(1)

            # Augmento sweep → 15 × num_threads runs
            total_runs = 15 * max_threads
            with tqdm(total=total_runs,
                      desc="Running augmentations with augmento") as pbar:
                for iteration in range(1, 16):
                    for num_threads in range(1, max_threads + 1):
                        config["iterations"] = iteration
                        config["num_threads"] = num_threads

                        with open(CONFIG_FILE, "w") as f:
                            json.dump(config, f, indent=2)

                        result = subprocess.run(
                            [BENCHMARK_EXE, "--config", CONFIG_FILE],
                            capture_output=True,
                            text=True)
                        output = result.stdout

                        if "ERROR" in output:
                            match = re.search(r"\[ERROR\]\s+(.*)", output)
                            raise RuntimeError(
                                match.group(1) if match else "Unknown error")

                        pipeline_match = re.search(
                            r"\[TIMING\].*?in\s+(\d+)\s*us", output)
                        total_match = re.search(
                            r"\[TIMING\]\s+Total\(us\):\s*(\d+)", output)

                        if not pipeline_match or not total_match:
                            raise RuntimeError(
                                "Failed to extract both pipeline and total timing from output."
                            )

                        pipeline_time = int(pipeline_match.group(1))
                        total_time = int(total_match.group(1))

                        writer.writerow([
                            "augmento", iteration, num_threads, pipeline_time,
                            total_time
                        ])

                        pbar.update(1)
                        pbar.set_postfix(iter=iteration, threads=num_threads)

        print(f"Benchmark results saved to {CSV_FILE}")

    except subprocess.CalledProcessError as e:
        raise RuntimeError(f"Benchmark execution failed: {e.stderr}")
    except Exception as e:
        raise RuntimeError(f"Unexpected error during benchmark: {str(e)}")


##
# @brief Reads benchmark CSV and plots results
def plot_benchmark(filename: str) -> None:
    augmento_pipeline = defaultdict(list)
    augmento_total = defaultdict(list)
    albumentations_total = {}

    with open(filename, newline="") as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            fw = row["framework"]
            iteration = int(row["iteration"])
            total_time = float(row["total time"])

            if fw == "augmento":
                num_threads = int(row["num_threads"])
                pipeline_time = float(row["pipeline time"])
                augmento_pipeline[iteration].append(
                    (num_threads, pipeline_time))
                augmento_total[iteration].append((num_threads, total_time))
            elif fw == "albumentations":
                albumentations_total[iteration] = total_time

    def plot(augmento_data, albumentations_data, title, ylabel, filename):
        plt.figure(figsize=(10, 6))

        for iteration, values in sorted(augmento_data.items()):
            values.sort(key=lambda x: x[0])
            x = [v[0] for v in values]
            y = [v[1] for v in values]
            plt.plot(x, y, marker="o", label=f"Augmento Iter {iteration}")

        for iteration, y_val in sorted(albumentations_data.items()):
            x_min = min(
                min(v[0] for v in vals) for vals in augmento_data.values())
            x_max = max(
                max(v[0] for v in vals) for vals in augmento_data.values())
            plt.hlines(y=y_val,
                       xmin=x_min,
                       xmax=x_max,
                       colors="red",
                       linestyles="--",
                       label=f"Albumentations Iter {iteration}")

        plt.xlabel("Number of Threads")
        plt.ylabel(f"{ylabel} (µs)")
        plt.title(title)

        cpu_info = get_processor_name()
        ram_gb = round(psutil.virtual_memory().total / (1024**3), 1)
        plt.suptitle(f"Device: {cpu_info}, RAM: {ram_gb} GB",
                     fontsize=10,
                     y=0.98)

        plt.legend(loc="upper right", fontsize="small", ncol=2)
        plt.grid(True)
        plt.tight_layout()
        plt.savefig(filename)
        plt.show()

    plot(augmento_pipeline, {}, "Pipeline Time by Thread Count and Iteration",
         "Pipeline Time", "./results/pipeline_time.png")

    plot(augmento_total, albumentations_total,
         "Total Time by Thread Count and Iteration", "Total Time",
         "./results/total_time.png")


##
# @brief Entry point
def main() -> None:
    try:
        os.makedirs("./results", exist_ok=True)
        for item in os.listdir("./results"):
            item_path = os.path.join("./results", item)
            if os.path.isfile(item_path):
                os.remove(item_path)
            elif os.path.isdir(item_path):
                shutil.rmtree(item_path)
        build_scripts()
        run_sweep()
        plot_benchmark(CSV_FILE)
    except Exception as e:
        raise RuntimeError(f"Unexpected failure in main(): {str(e)}")


if __name__ == "__main__":
    main()
