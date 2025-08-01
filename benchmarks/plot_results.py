##
# @file plot_benchmark.py
# @brief Plots benchmark timing results from CSV file.
# @details Plots number of threads on x-axis and time taken on y-axis,
#          with each iteration represented by a distinct color line.
# @author Emmanuel Butsana
# @date August 1, 2025
#

import csv
import matplotlib.pyplot as plt
from collections import defaultdict

##
# @brief Reads benchmark CSV and plots timing results.
# @param filename Path to CSV file with columns: iteration, num_threads, pipeline time, total time
# @return None
def plot_benchmark(filename: str) -> None:
    data = defaultdict(list)  # iteration -> list of (num_threads, total_time)

    with open(filename, newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            iteration = int(row['iteration'])
            num_threads = int(row['num_threads'])

            # Sum all timing columns starting with 'timing'
            timing_cols = [col for col in row if col.startswith('timing')]
            total_time = sum(float(row[col]) for col in timing_cols)

            data[iteration].append((num_threads, total_time))

    plt.figure(figsize=(10, 6))

    for iteration, points in data.items():
        points.sort(key=lambda x: x[0])  # Sort by num_threads
        x_vals = [p[0] for p in points]
        y_vals = [p[1] for p in points]
        plt.plot(x_vals, y_vals, marker='o', label=f'Iteration {iteration}')

    plt.xlabel('Number of Threads')
    plt.ylabel('Time (microseconds)')
    plt.title('Benchmark Timing by Thread Count and Iteration')
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    plot_benchmark("benchmark_results.csv")

