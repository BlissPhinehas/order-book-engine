#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>
#include "order_book.h"
#include "market_data_parser.h"
#include "memory_pool.h"
#include "benchmark.h"

static constexpr std::size_t POOL_SIZE = 1 << 20; // 1M orders
static constexpr std::size_t BENCH_ORDERS = 500'000;

int main()
{
    // ── live BBO demo ────────────────────────────────────────────
    std::cout << "=== Live BBO from sample_ticks.csv ===\n";
    {
        OrderBook book;
        std::unordered_map<uint64_t, Order> order_store;

        MarketDataParser::stream_csv("data/sample_ticks.csv",
                                     [&](const TickEvent &ev)
                                     {
                                         bool changed = false;
                                         if (ev.type == EventType::ADD)
                                         {
                                             auto &o = order_store[ev.order_id];
                                             o.order_id = ev.order_id;
                                             o.timestamp_ns = ev.timestamp_ns;
                                             o.price = ev.price;
                                             o.quantity = ev.quantity;
                                             o.side = ev.side;
                                             changed = book.add_order(&o);
                                         }
                                         else if (ev.type == EventType::CANCEL)
                                         {
                                             changed = book.cancel_order(ev.order_id);
                                             order_store.erase(ev.order_id);
                                         }
                                         else if (ev.type == EventType::EXECUTE)
                                         {
                                             changed = book.execute_order(ev.order_id, ev.quantity);
                                         }
                                         if (changed)
                                         {
                                             BBO bbo = book.get_bbo();
                                             std::cout << "t=" << ev.timestamp_ns
                                                       << "  BID " << bbo.best_bid << " x" << bbo.best_bid_qty
                                                       << "  ASK " << bbo.best_ask << " x" << bbo.best_ask_qty
                                                       << "\n";
                                         }
                                     });
    }

    // ── benchmark ────────────────────────────────────────────────
    std::cout << "\n=== Benchmark: " << BENCH_ORDERS << " add_order ops ===\n";
    {
        OrderBook book;
        auto pool = std::make_unique<MemoryPool<Order, POOL_SIZE>>();
        std::vector<uint64_t> latencies;
        latencies.reserve(BENCH_ORDERS);

        uint64_t t_start = Benchmark::now_ns();

        for (uint64_t i = 0; i < BENCH_ORDERS; ++i)
        {
            Order *o = pool->allocate();
            o->order_id = i;
            o->timestamp_ns = i;
            o->price = 100.0 + (i % 20) * 0.25; // 20 price levels
            o->quantity = 10 + (i % 50);
            o->side = (i % 2 == 0) ? Side::BID : Side::ASK;

            uint64_t t0 = Benchmark::now_ns();
            book.add_order(o);
            uint64_t t1 = Benchmark::now_ns();
            latencies.push_back(t1 - t0);
        }

        uint64_t t_end = Benchmark::now_ns();
        double elapsed_s = (t_end - t_start) / 1e9;
        double throughput = BENCH_ORDERS / elapsed_s;

        LatencyStats stats = Benchmark::compute_stats(latencies);
        Benchmark::print_stats("add_order", stats, throughput);
    }

    return 0;
}