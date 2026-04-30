#include "order_book.h"
#include <stdexcept>

// ── PriceLevel ───────────────────────────────────────────────────

void PriceLevel::push_back(Order *o)
{
    o->prev = tail;
    o->next = nullptr;
    if (tail)
        tail->next = o;
    else
        head = o;
    tail = o;
    total_quantity += o->quantity;
}

void PriceLevel::remove(Order *o)
{
    if (o->prev)
        o->prev->next = o->next;
    else
        head = o->next;
    if (o->next)
        o->next->prev = o->prev;
    else
        tail = o->prev;
    o->prev = o->next = nullptr;
    total_quantity -= o->quantity;
}

// ── OrderBook ────────────────────────────────────────────────────

bool OrderBook::add_order(Order *o)
{
    BBO before = get_bbo();
    if (o->side == Side::BID)
    {
        auto &level = bids_[o->price];
        level.price = o->price;
        level.push_back(o);
    }
    else
    {
        auto &level = asks_[o->price];
        level.price = o->price;
        level.push_back(o);
    }
    order_map_[o->order_id] = o;
    return bbo_changed(before);
}

bool OrderBook::cancel_order(uint64_t order_id)
{
    auto it = order_map_.find(order_id);
    if (it == order_map_.end())
        return false;

    BBO before = get_bbo();
    Order *o = it->second;

    if (o->side == Side::BID)
    {
        auto &level = bids_[o->price];
        level.remove(o);
        if (level.empty())
            bids_.erase(o->price);
    }
    else
    {
        auto &level = asks_[o->price];
        level.remove(o);
        if (level.empty())
            asks_.erase(o->price);
    }
    order_map_.erase(it);
    return bbo_changed(before);
}

bool OrderBook::execute_order(uint64_t order_id, uint32_t qty)
{
    auto it = order_map_.find(order_id);
    if (it == order_map_.end())
        return false;

    BBO before = get_bbo();
    Order *o = it->second;
    o->quantity -= qty;

    if (o->side == Side::BID)
    {
        bids_[o->price].total_quantity -= qty;
        if (o->quantity == 0)
        {
            bids_[o->price].remove(o);
            if (bids_[o->price].empty())
                bids_.erase(o->price);
            order_map_.erase(it);
        }
    }
    else
    {
        asks_[o->price].total_quantity -= qty;
        if (o->quantity == 0)
        {
            asks_[o->price].remove(o);
            if (asks_[o->price].empty())
                asks_.erase(o->price);
            order_map_.erase(it);
        }
    }
    return bbo_changed(before);
}

BBO OrderBook::get_bbo() const
{
    BBO bbo;
    if (!bids_.empty())
    {
        const auto &lvl = bids_.begin()->second;
        bbo.best_bid = lvl.price;
        bbo.best_bid_qty = lvl.total_quantity;
    }
    if (!asks_.empty())
    {
        const auto &lvl = asks_.begin()->second;
        bbo.best_ask = lvl.price;
        bbo.best_ask_qty = lvl.total_quantity;
    }
    return bbo;
}

bool OrderBook::bbo_changed(const BBO &before) const
{
    BBO after = get_bbo();
    return before.best_bid != after.best_bid ||
           before.best_bid_qty != after.best_bid_qty ||
           before.best_ask != after.best_ask ||
           before.best_ask_qty != after.best_ask_qty;
}