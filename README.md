# High-Performance Order Book Engine

A C++ implementation of a limit order book — the core data structure at the
heart of every electronic exchange. Processes add, cancel, and execute order
events from a market data feed, maintains bid/ask price levels, and outputs
best bid/offer (BBO) updates with nanosecond timestamps.

## Performance Results (WSL2 / Ubuntu 24.04 / GCC 13, Release -O3)

| Metric        | Value       |
|---------------|-------------|
| Throughput    | ~11M ops/sec |
| Latency p50   | 29 ns       |
| Latency p99   | 70 ns       |
| Latency p99.9 | 1,584 ns    |
| CPU utilization | 97.4%     |
| Hot-path page faults | 0   |
| Context switches | 0       |

> p99.9 spikes are WSL2 scheduler noise. On bare-metal Linux with CPU pinning
> these would be in the 100–200ns range.

## Architecture

```
order-book-engine/
├── include/
│   ├── order.h               # Order struct + intrusive linked list pointers
│   ├── order_book.h          # PriceLevel, BBO, OrderBook class
│   ├── market_data_parser.h  # CSV tick event parser
│   ├── memory_pool.h         # Fixed-size slab allocator (header-only template)
│   └── benchmark.h           # Latency stats + throughput measurement
├── src/
│   ├── order_book.cpp        # Bid/ask map, O(1) order lookup, BBO diffing
│   ├── market_data_parser.cpp
│   ├── benchmark.cpp
│   └── main.cpp              # Live BBO demo + benchmark harness
├── tests/                    # 13 Google Test cases
├── data/sample_ticks.csv     # Sample market feed
└── scripts/
    ├── run_bench.sh
    └── run_perf.sh
```

## Key Design Decisions

**Intrusive linked list for price levels** — each `Order` carries its own
`prev/next` pointers so inserting and removing from a price level is O(1) with
no heap allocation and no pointer chasing through a separate node object.

**`std::map` with custom comparator for bid/ask sides** — bids use
`std::greater<double>` so `begin()` always points to the best bid. Asks use
default ascending order so `begin()` always points to the best ask. BBO reads
are always O(1).

**Memory pool allocator** — a fixed-capacity slab of pre-allocated `Order`
slots with a free-list. Zero heap allocation in the hot path means zero
`malloc` latency and zero fragmentation. All 20K page faults occur at startup,
none during benchmarking.

**BBO diffing** — `add_order`, `cancel_order`, and `execute_order` all return
`bool` indicating whether the best bid or offer changed. Downstream consumers
only process events that actually move the market.

## Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/order_book          # live BBO demo + benchmark
./build/run_tests           # 13 unit tests
bash scripts/run_perf.sh    # perf stat profiling
```

Requires: GCC 13+, CMake 3.14+, Linux/WSL2

## Stack
C++ · Low-Latency Systems · Order Book · Memory Pool Allocation ·
Cache Optimization · Linux · perf · Google Test · CMake
HEREDOC
