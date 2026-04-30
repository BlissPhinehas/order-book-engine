#pragma once
#include <cstdint>

enum class Side : uint8_t
{
    BID,
    ASK
};

struct Order
{
    uint64_t order_id;
    uint64_t timestamp_ns;
    double price;
    uint32_t quantity;
    Side side;

    // intrusive linked list pointers — no heap allocation needed
    Order *prev = nullptr;
    Order *next = nullptr;
};