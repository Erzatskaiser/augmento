<p align="center">
  <img src="../assets/augmento.png" alt="augmento logo" width="400"/>
</p>

# 📊 Benchmarking augmento

This section provides benchmarking insights into **augmento**'s performance. The benchmark evaluates augmentation pipeline throughput across different configurations, measuring both processing efficiency and end-to-end execution time.

---

## ⚙️ Running the Benchmark

To run the benchmark, navigate to the benchmark directory and execute:

```bash
cd benchmark
python benchmark_sweep.py
```

Make sure to install the required Python packages before running the benchmarks using

```bash
pip install -r requirements.txt
```

The script will automatically:

- Build the benchmarking setup
- Measure processing time across runs
- Output results in organized folders

# 📁 Output Structure

After execution, results will be saved in the following structure:

benchmark/
├── benchmark_sweep.py
├── output/                # Image samples from benchmark runs
└── results/
    ├── benchmark_data.csv # Raw performance data (CSV)
    ├── pipeline_time.png  # Time excluding JSON loading
    └── total_time.png     # Time including JSON loading
