#include "benchmark.h"
#include <ctime>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <iomanip>

uint64_t Benchmark::now_ns()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<uint64_t>(ts.tv_sec) * 1'000'000'000ULL + ts.tv_nsec;
}

LatencyStats Benchmark::compute_stats(std::vector<uint64_t> &samples)
{
    std::sort(samples.begin(), samples.end());

    LatencyStats s{};
    s.sample_count = samples.size();
    s.min_ns = static_cast<double>(samples.front());
    s.max_ns = static_cast<double>(samples.back());
    s.mean_ns = static_cast<double>(
                    std::accumulate(samples.begin(), samples.end(), 0ULL)) /
                s.sample_count;

    auto percentile = [&](double p) -> double
    {
        std::size_t idx = static_cast<std::size_t>(p / 100.0 * s.sample_count);
        if (idx >= s.sample_count)
            idx = s.sample_count - 1;
        return static_cast<double>(samples[idx]);
    };

    s.p50_ns = percentile(50.0);
    s.p99_ns = percentile(99.0);
    s.p999_ns = percentile(99.9);
    return s;
}

void Benchmark::print_stats(const std::string &label,
                            const LatencyStats &s,
                            double throughput)
{
    std::cout << "\n=== " << label << " ===\n"
              << std::fixed << std::setprecision(1)
              << "  samples   : " << s.sample_count << "\n"
              << "  throughput: " << throughput / 1e6 << " M ops/sec\n"
              << "  min       : " << s.min_ns << " ns\n"
              << "  mean      : " << s.mean_ns << " ns\n"
              << "  p50       : " << s.p50_ns << " ns\n"
              << "  p99       : " << s.p99_ns << " ns\n"
              << "  p99.9     : " << s.p999_ns << " ns\n"
              << "  max       : " << s.max_ns << " ns\n";
}