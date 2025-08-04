##
# @file run_sweep.py
# @brief Run sweeps for benchmarking purposes
# @author Emmanuel Butsana
# @date July 31, 2025
#
# Sweeps the .cpp benchmark file with various configs, saving performance in a .csv file.
#

import multiprocessing
import subprocess
import json
import re
import csv

##
# @brief Program main function
# @throws RuntimeError if benchmark fails
def main():
    try:
        # Open CSV file for writing results
        with open("benchmark_results.csv", mode="w", newline="") as csvfile:
            writer = csv.writer(csvfile)
            writer.writerow(["iteration", "num_threads", "pipeline time", "total time"])

            with open("config_template.json", "r") as f:
                config = json.load(f)

            max_threads = multiprocessing.cpu_count() or 8

            for i in range(1, 21):  # 20 iterations
                for j in range(1, max_threads + 1):  # 1 to max_threads inclusive
                    config["iterations"] = i
                    config["num_threads"] = j

                    # Write config to file
                    with open("config_template.json", "w") as f:
                        json.dump(config, f, indent=2)

                    # Run benchmark and capture output
                    result = subprocess.run(["./build/benchmark", "--config", "config_template.json"],
                                            capture_output=True, text=True)

                    output = result.stdout

                    # Check for errors
                    if "ERROR" in output:
                        match = re.search(r"\[ERROR\]\s+(.*)", output)
                        raise RuntimeError(match.group(1) if match else "Unknown error")

                    # Extract all TIMING values (numbers only)
                    timing_vals = [int(v) for v in re.findall(r"\[TIMING\].*?:\s*(\d+)", output)]

                    # Write row: iteration, threads, pipeline timing, total timing
                    writer.writerow([i, j] + timing_vals)

    except subprocess.CalledProcessError as e:
        raise RuntimeError(f"Benchmark execution failed: {e.stderr}")

    except Exception as e:
        raise RuntimeError(f"Unexpected error during benchmark: {str(e)}")

if __name__ == "__main__":
    main()

