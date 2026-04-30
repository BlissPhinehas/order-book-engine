#include <iostream>
#include <unordered_map>
#include "order_book.h"
#include "market_data_parser.h"

int main()
{
    OrderBook book;
    std::unordered_map<uint64_t, Order> order_store;

    auto print_bbo = [&](uint64_t ts)
    {
        BBO bbo = book.get_bbo();
        std::cout << "t=" << ts
                  << "  BID " << bbo.best_bid << " x" << bbo.best_bid_qty
                  << "  ASK " << bbo.best_ask << " x" << bbo.best_ask_qty
                  << "\n";
    };

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
                                         if (order_store.count(ev.order_id) &&
                                             order_store[ev.order_id].quantity == 0)
                                             order_store.erase(ev.order_id);
                                     }
                                     if (changed)
                                         print_bbo(ev.timestamp_ns);
                                 });

    return 0;
}