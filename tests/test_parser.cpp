#include <gtest/gtest.h>
#include "market_data_parser.h"
#include <fstream>

static void write_test_csv(const std::string &path)
{
    std::ofstream f(path);
    f << "timestamp_ns,event,order_id,side,price,quantity\n"
      << "1000,A,1,B,99.50,100\n"
      << "2000,A,2,S,100.00,50\n"
      << "3000,X,1,B,99.50,0\n"
      << "4000,E,2,S,100.00,25\n";
}

TEST(Parser, LoadsCorrectEventCount)
{
    write_test_csv("/tmp/test_ticks.csv");
    auto events = MarketDataParser::load_csv("/tmp/test_ticks.csv");
    EXPECT_EQ(events.size(), 4u);
}

TEST(Parser, ParsesAddEvent)
{
    write_test_csv("/tmp/test_ticks.csv");
    auto events = MarketDataParser::load_csv("/tmp/test_ticks.csv");
    EXPECT_EQ(events[0].type, EventType::ADD);
    EXPECT_EQ(events[0].order_id, 1u);
    EXPECT_EQ(events[0].side, Side::BID);
    EXPECT_DOUBLE_EQ(events[0].price, 99.50);
    EXPECT_EQ(events[0].quantity, 100u);
}

TEST(Parser, ParsesCancelEvent)
{
    write_test_csv("/tmp/test_ticks.csv");
    auto events = MarketDataParser::load_csv("/tmp/test_ticks.csv");
    EXPECT_EQ(events[2].type, EventType::CANCEL);
}

TEST(Parser, ParsesExecuteEvent)
{
    write_test_csv("/tmp/test_ticks.csv");
    auto events = MarketDataParser::load_csv("/tmp/test_ticks.csv");
    EXPECT_EQ(events[3].type, EventType::EXECUTE);
    EXPECT_EQ(events[3].quantity, 25u);
}