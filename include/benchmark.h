#pragma once
#include <cstdint>
#include <vector>
#include <string>

struct LatencyStats
{
    double min_ns;
    double max_ns;
    double mean_ns;
    double p50_ns;
    double p99_ns;
    double p999_ns;
    uint64_t sample_count;
};

class Benchmark
{
public:
    // returns current time in nanoseconds
    static uint64_t now_ns();

    // compute stats from a vector of nanosecond latency samples
    static LatencyStats compute_stats(std::vector<uint64_t> &samples);

    // print a stats report to stdout
    static void print_stats(const std::string &label,
                            const LatencyStats &stats,
                            double throughput_ops_per_sec);
};