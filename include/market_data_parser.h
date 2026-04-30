#pragma once
#include "order.h"
#include <string>
#include <vector>
#include <functional>

enum class EventType : uint8_t
{
    ADD,
    CANCEL,
    EXECUTE
};

struct TickEvent
{
    uint64_t timestamp_ns;
    EventType type;
    uint64_t order_id;
    Side side;
    double price;
    uint32_t quantity;
};

class MarketDataParser
{
public:
    // Load all events from a CSV file into memory
    static std::vector<TickEvent> load_csv(const std::string &path);

    // Stream events one at a time — calls cb for each parsed row
    static void stream_csv(const std::string &path,
                           std::function<void(const TickEvent &)> cb);

private:
    static TickEvent parse_line(const std::string &line);
};