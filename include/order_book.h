#pragma once
#include "order.h"
#include <map>
#include <unordered_map>
#include <cstdint>

struct PriceLevel
{
    double price;
    uint32_t total_quantity = 0;
    Order *head = nullptr; // front of queue
    Order *tail = nullptr; // back of queue

    void push_back(Order *o);
    void remove(Order *o);
    bool empty() const { return head == nullptr; }
};

struct BBO
{
    double best_bid = 0.0;
    uint32_t best_bid_qty = 0;
    double best_ask = 0.0;
    uint32_t best_ask_qty = 0;
    uint64_t timestamp_ns = 0;
};

class OrderBook
{
public:
    // returns true if BBO changed
    bool add_order(Order *o);
    bool cancel_order(uint64_t order_id);
    bool execute_order(uint64_t order_id, uint32_t qty);

    BBO get_bbo() const;

private:
    // bids: highest price first
    std::map<double, PriceLevel, std::greater<double>> bids_;
    // asks: lowest price first
    std::map<double, PriceLevel> asks_;
    // O(1) order lookup by id
    std::unordered_map<uint64_t, Order *> order_map_;

    bool bbo_changed(const BBO &before) const;
};