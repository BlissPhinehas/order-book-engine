#include <gtest/gtest.h>
#include "order_book.h"

static Order make_order(uint64_t id, Side side, double price, uint32_t qty)
{
    Order o;
    o.order_id = id;
    o.timestamp_ns = 0;
    o.price = price;
    o.quantity = qty;
    o.side = side;
    return o;
}

TEST(OrderBook, AddBidUpdatesBBO)
{
    OrderBook book;
    Order o = make_order(1, Side::BID, 100.0, 10);
    bool changed = book.add_order(&o);
    EXPECT_TRUE(changed);
    BBO bbo = book.get_bbo();
    EXPECT_DOUBLE_EQ(bbo.best_bid, 100.0);
    EXPECT_EQ(bbo.best_bid_qty, 10u);
}

TEST(OrderBook, AddAskUpdatesBBO)
{
    OrderBook book;
    Order o = make_order(2, Side::ASK, 101.0, 5);
    book.add_order(&o);
    BBO bbo = book.get_bbo();
    EXPECT_DOUBLE_EQ(bbo.best_ask, 101.0);
    EXPECT_EQ(bbo.best_ask_qty, 5u);
}

TEST(OrderBook, CancelOrderRemovesLevel)
{
    OrderBook book;
    Order o = make_order(3, Side::BID, 99.0, 20);
    book.add_order(&o);
    bool changed = book.cancel_order(3);
    EXPECT_TRUE(changed);
    BBO bbo = book.get_bbo();
    EXPECT_DOUBLE_EQ(bbo.best_bid, 0.0);
}

TEST(OrderBook, BestBidIsHighestPrice)
{
    OrderBook book;
    Order o1 = make_order(4, Side::BID, 99.0, 10);
    Order o2 = make_order(5, Side::BID, 100.0, 10);
    book.add_order(&o1);
    book.add_order(&o2);
    EXPECT_DOUBLE_EQ(book.get_bbo().best_bid, 100.0);
}

TEST(OrderBook, ExecutePartialReducesQty)
{
    OrderBook book;
    Order o = make_order(6, Side::ASK, 101.0, 10);
    book.add_order(&o);
    book.execute_order(6, 4);
    EXPECT_EQ(book.get_bbo().best_ask_qty, 6u);
}