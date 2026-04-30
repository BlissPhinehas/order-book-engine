#include "market_data_parser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

TickEvent MarketDataParser::parse_line(const std::string &line)
{
    std::stringstream ss(line);
    std::string token;
    TickEvent ev{};

    // timestamp_ns
    std::getline(ss, token, ',');
    ev.timestamp_ns = std::stoull(token);

    // event type
    std::getline(ss, token, ',');
    if (token == "A")
        ev.type = EventType::ADD;
    else if (token == "X")
        ev.type = EventType::CANCEL;
    else if (token == "E")
        ev.type = EventType::EXECUTE;
    else
        throw std::runtime_error("Unknown event type: " + token);

    // order_id
    std::getline(ss, token, ',');
    ev.order_id = std::stoull(token);

    // side
    std::getline(ss, token, ',');
    ev.side = (token == "B") ? Side::BID : Side::ASK;

    // price
    std::getline(ss, token, ',');
    ev.price = std::stod(token);

    // quantity
    std::getline(ss, token, ',');
    ev.quantity = static_cast<uint32_t>(std::stoul(token));

    return ev;
}

std::vector<TickEvent> MarketDataParser::load_csv(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Cannot open: " + path);

    std::vector<TickEvent> events;
    std::string line;
    std::getline(file, line); // skip header

    while (std::getline(file, line))
    {
        if (!line.empty())
            events.push_back(parse_line(line));
    }
    return events;
}

void MarketDataParser::stream_csv(const std::string &path,
                                  std::function<void(const TickEvent &)> cb)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Cannot open: " + path);

    std::string line;
    std::getline(file, line); // skip header

    while (std::getline(file, line))
    {
        if (!line.empty())
            cb(parse_line(line));
    }
}